/* io4-v2-bricklet
 * Copyright (C) 2018 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * io4.c: Implementation of IO4 V2 Bricklet
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "io4.h"

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/logging/logging.h"
#include "bricklib2/hal/system_timer/system_timer.h"

#include "communication.h"

IO4 io4;
XMC_GPIO_CONFIG_t ch_pin_out_config;
XMC_GPIO_CONFIG_t ch_pin_in_pull_up_config;
XMC_GPIO_CONFIG_t ch_pin_in_no_pull_up_config;

void io4_init(void) {
	logd("[+] IO4-V2: io4_init()\n\r");

	// Channel out config
	ch_pin_out_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
	ch_pin_out_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;

	// Channel in pull-up config
	ch_pin_in_pull_up_config.mode = XMC_GPIO_MODE_INPUT_PULL_UP;
	ch_pin_in_pull_up_config.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD;

	// Channel in no pull-up config
	ch_pin_in_no_pull_up_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
	ch_pin_in_no_pull_up_config.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD;

	// Initialise all the channels
	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		// Generic channel config
		io4.channels[i].pin = i;
		io4.channels[i].direction = IO4_V2_DIRECTION_IN;
		io4.channels[i].port_base = (XMC_GPIO_PORT_t *)PORT1_BASE;

		// Channel monoflop config
		io4.channels[i].monoflop.time = 0;
		io4.channels[i].monoflop.time_start = 0;
		io4.channels[i].monoflop.time_remaining = 0;

		// Channel edge count config
		io4.channels[i].edge_count.debounce = 100;
		io4.channels[i].edge_count.cnt_edge_rising = 0;
		io4.channels[i].edge_count.cnt_edge_falling = 0;
		io4.channels[i].edge_count.edge_type = IO4_V2_EDGE_TYPE_RISING;

		// Channel input value callback config
		io4.channels[i].input_value_cb.period = 0;
		io4.channels[i].input_value_cb.last_value = false;
		io4.channels[i].input_value_cb.period_start = 0;
		io4.channels[i].input_value_cb.value_has_to_change = false;

		// Initialised as pull-up input
		XMC_GPIO_Init(io4.channels[i].port_base, io4.channels[i].pin, &ch_pin_in_pull_up_config);

		io4.channels[i].init_value = true;

		if(XMC_GPIO_GetInput(io4.channels[i].port_base, io4.channels[i].pin) == 1) {
			io4.channels[i].value = true;
		}
		else {
			io4.channels[i].value = false;
		}

		// Channel edge count config
		io4.channels[i].edge_count.last_value = io4.channels[i].value;
		io4.channels[i].edge_count.debounce_start = system_timer_get_ms();
	}

	// All input value callback
	io4.all_input_value_cb.period = 0;
	io4.all_input_value_cb.period_start = 0;
	io4.all_input_value_cb.value_has_to_change = false;

	// Input value callback ringbuffer init
	ringbuffer_init(&io4.all_input_value_cb.cb_rb, ALL_INPUT_VALUE_CB_BUFFER_SIZE, &io4.all_input_value_cb.cb_buffer[0]);

	ringbuffer_init(&io4.input_value_cb_rb, INPUT_VALUE_CB_BUFFER_SIZE, &io4.input_value_cb_buffer[0]);

	// Monopflop callback ringbuffer init
	ringbuffer_init(&io4.monoflop_cb_rb, MONOFLOP_CB_BUFFER_SIZE, &io4.monoflop_cb_buffer[0]);
}

void io4_tick(void) {
	uint8_t all_channel_values = 0;
	uint8_t all_channel_changed = 0;
	bool all_ch_in_value_changed = false;
	bool all_ch_in_period_expired = false;

	// Check if all channel input value callback is enabled and if period expired
	if((io4.all_input_value_cb.period > 0) && (system_timer_is_time_elapsed_ms(io4.all_input_value_cb.period_start, io4.all_input_value_cb.period))) {
		all_ch_in_period_expired = true;
	}

	// Iterate all channels
	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		if(io4.channels[i].direction == IO4_V2_DIRECTION_IN) {
			// Channel is input

			// Update current input channel value
			io4.channels[i].value = \
				(bool)XMC_GPIO_GetInput(io4.channels[i].port_base, io4.channels[i].pin);

			// Manage channel specific input value callback
			if((io4.channels[i].input_value_cb.period > 0)) {
				if(system_timer_is_time_elapsed_ms(io4.channels[i].input_value_cb.period_start, io4.channels[i].input_value_cb.period)) {
					// Period expired

					if(io4.channels[i].input_value_cb.value_has_to_change) {
						// Enqueue callback if value changed otherwise not
						if(io4.channels[i].value != io4.channels[i].input_value_cb.last_value) {
							if(ringbuffer_get_used(&io4.input_value_cb_rb) < INPUT_VALUE_CB_BUFFER_SIZE) {
								ringbuffer_add(&io4.input_value_cb_rb, i); // Channel
								ringbuffer_add(&io4.input_value_cb_rb, (uint8_t)true); // Changed
								ringbuffer_add(&io4.input_value_cb_rb, (uint8_t)io4.channels[i].value); // Value
							}
						}
					}
					else {
						// Enqueue callback regardless of change
						if(ringbuffer_get_used(&io4.input_value_cb_rb) < INPUT_VALUE_CB_BUFFER_SIZE) {
							// Channel
							ringbuffer_add(&io4.input_value_cb_rb, i);

							// Changed
							if(io4.channels[i].value != io4.channels[i].input_value_cb.last_value) {
								ringbuffer_add(&io4.input_value_cb_rb, (uint8_t)true);
							}
							else {
								ringbuffer_add(&io4.input_value_cb_rb, (uint8_t)false);
							}

							// Value
							ringbuffer_add(&io4.input_value_cb_rb, (uint8_t)io4.channels[i].value);
						}
					}

					// Update last value
					io4.channels[i].input_value_cb.last_value = io4.channels[i].value;
					io4.channels[i].input_value_cb.period_start = system_timer_get_ms();
				}
			}

			// Manage all input value callback
			if(all_ch_in_period_expired) {
				if(io4.channels[i].value != io4.all_input_value_cb.last_values[i]) {
					all_ch_in_value_changed = true;
					all_channel_changed |= (1 << i);
				}
				else {
					all_channel_changed &= ~(1 << i);
				}

				if(io4.channels[i].value) {
					all_channel_values |= (1 << i);
				}
				else {
					all_channel_values &= ~(1 << i);
				}

				// Update last value
				io4.all_input_value_cb.last_values[i] = io4.channels[i].value;
			}

			// Manage edge count
			if(system_timer_is_time_elapsed_ms(io4.channels[i].edge_count.debounce_start, io4.channels[i].edge_count.debounce)) {
				// Debounce period expired, update edge count

				if(!io4.channels[i].edge_count.last_value && io4.channels[i].value) {
					// Rising edge
					io4.channels[i].edge_count.cnt_edge_rising++;
				}
				else if(io4.channels[i].edge_count.last_value && !io4.channels[i].value) {
					// Falling edge
					io4.channels[i].edge_count.cnt_edge_falling++;
				}

				// Update last value
				io4.channels[i].edge_count.last_value = io4.channels[i].value;
				io4.channels[i].edge_count.debounce_start = system_timer_get_ms();
			}
		}
		else {
			// Channel is output

			// Manage monoflop
			if(io4.channels[i].monoflop.time > 0) {
				if(system_timer_is_time_elapsed_ms(io4.channels[i].monoflop.time_start, io4.channels[i].monoflop.time)) {
					// Monoflop time expired

					io4.channels[i].monoflop.time = 0;
					io4.channels[i].monoflop.time_start = 0;
					io4.channels[i].monoflop.time_remaining = 0;

					if(io4.channels[i].value) {
						io4.channels[i].value = false;
					}
					else {
						io4.channels[i].value = true;
					}

					if(io4.channels[i].value) {
						XMC_GPIO_SetOutputHigh(io4.channels[i].port_base, io4.channels[i].pin);
					}
					else {
						XMC_GPIO_SetOutputLow(io4.channels[i].port_base, io4.channels[i].pin);
					}

					// Enqueue monoflop callback for the channel
					if(ringbuffer_get_used(&io4.monoflop_cb_rb) < MONOFLOP_CB_BUFFER_SIZE) {
						// Channel
						ringbuffer_add(&io4.monoflop_cb_rb, i);
						// Value
						ringbuffer_add(&io4.monoflop_cb_rb, (uint8_t)io4.channels[i].value);
					}
				}
				else {
					io4.channels[i].monoflop.time_remaining = \
						system_timer_get_ms() - io4.channels[i].monoflop.time_start;
				}
			}
		}
	}

	// Manage all input value callback
	if(all_ch_in_period_expired) {
		// Period expired

		if(io4.all_input_value_cb.value_has_to_change) {
			// Enqueue CB if value changed otherwise not
			if(all_ch_in_value_changed) {
				if(ringbuffer_get_used(&io4.all_input_value_cb.cb_rb) < ALL_INPUT_VALUE_CB_BUFFER_SIZE) {
					// Changed
					ringbuffer_add(&io4.all_input_value_cb.cb_rb, all_channel_changed);
					// Value
					ringbuffer_add(&io4.all_input_value_cb.cb_rb, all_channel_values);
				}
			}
		}
		else {
			// Enqueue CB regardless of change
			if(ringbuffer_get_used(&io4.all_input_value_cb.cb_rb) < ALL_INPUT_VALUE_CB_BUFFER_SIZE) {
				// Changed
				ringbuffer_add(&io4.all_input_value_cb.cb_rb, all_channel_changed);
				// Value
				ringbuffer_add(&io4.all_input_value_cb.cb_rb, all_channel_values);
			}
		}

		io4.all_input_value_cb.period_start = system_timer_get_ms();
	}
}
