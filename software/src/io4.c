/* io4-v2-bricklet
 * Copyright (C) 2018-2019 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 * Copyright (C) 2025 Olaf Lüke <olaf@tinkerforge.com>
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

#include "bricklib2/utility/util_definitions.h"
#include "bricklib2/logging/logging.h"
#include "bricklib2/hal/system_timer/system_timer.h"

#include "communication.h"
#include "timer.h"

#include "xmc_ccu4.h"

#define io_timer_handler IRQ_Hdlr_21

uint8_t io4_capture_input_data[IO4_CAPTURE_INPUT_SIZE];
uint8_t io4_capture_input_data_last_index = 0;
volatile uint8_t io4_capture_input_data_index = 0;

void __attribute__((optimize("-O3"))) __attribute__ ((section (".ram_code"))) io_timer_handler(void) {
	// io4_capture_input_data has size of 256, we use the uint8_t overflow here to our advantage
	io4_capture_input_data[io4_capture_input_data_index] = PORT1->IN & 0x0F;
	io4_capture_input_data_index++;
}

IO4 io4;

// GPIO modes
XMC_GPIO_MODE_t ch_pin_out_mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
XMC_GPIO_MODE_t ch_pin_pwm_mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT2;
XMC_GPIO_MODE_t ch_pin_in_pull_up_mode = XMC_GPIO_MODE_INPUT_PULL_UP;
XMC_GPIO_MODE_t ch_pin_in_no_pull_up_mode = XMC_GPIO_MODE_INPUT_TRISTATE;

XMC_CCU4_SLICE_t *const io4_slice[NUMBER_OF_CHANNELS] = {
	CCU40_CC40,
	CCU40_CC41,
	CCU40_CC42,
	CCU40_CC43,
};

void io4_init(void) {
	logd("[+] IO4-V2: io4_init()\n\r");

	XMC_GPIO_CONFIG_t ch_pin_in_pull_up_config;

	// Channel in pull-up config
	ch_pin_in_pull_up_config.mode = XMC_GPIO_MODE_INPUT_PULL_UP;
	ch_pin_in_pull_up_config.input_hysteresis = XMC_GPIO_INPUT_HYSTERESIS_STANDARD;

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
		io4.channels[i].monoflop.running = false;

		// Channel input value callback config
		io4.channels[i].input_value_cb.period = 0;
		io4.channels[i].input_value_cb.period_start = 0;
		io4.channels[i].input_value_cb.last_value = false;
		io4.channels[i].input_value_cb.value_has_to_change = false;

		// Channel edge count config
		io4.channels[i].edge_count.debounce = 100;
		io4.channels[i].edge_count.cnt_edge_rising = 0;
		io4.channels[i].edge_count.cnt_edge_falling = 0;
		io4.channels[i].edge_count.edge_type = IO4_V2_EDGE_TYPE_RISING;

		// Initialised as pull-up input
		XMC_GPIO_Init(io4.channels[i].port_base, io4.channels[i].pin, &ch_pin_in_pull_up_config);

		if(XMC_GPIO_GetInput(io4.channels[i].port_base, io4.channels[i].pin) == 1) {
			io4.channels[i].value = true;
		}
		else {
			io4.channels[i].value = false;
		}

		io4.channels[i].init_value = true;

		// Channel edge count config
		io4.channels[i].edge_count.last_value = io4.channels[i].value;
		io4.channels[i].edge_count.debounce_start = system_timer_get_ms();
	}

	// All input value callback
	io4.all_input_value_cb.period = 0;
	io4.all_input_value_cb.period_start = 0;
	io4.all_input_value_cb.value_has_to_change = false;

	// Capture input callback
	io4.capture_input_callback_enabled = false;
	io4.capture_input_buffer_index = 0;
	memset(io4.capture_input_buffer, 0, sizeof(io4.capture_input_buffer));

	// Input value callback ringbuffer init
	ringbuffer_init(&io4.input_value_cb_rb, INPUT_VALUE_CB_BUFFER_SIZE, &io4.input_value_cb_buffer[0]);
	ringbuffer_init(&io4.all_input_value_cb.cb_rb, ALL_INPUT_VALUE_CB_BUFFER_SIZE, &io4.all_input_value_cb.cb_buffer[0]);

	// Monopflop callback ringbuffer init
	ringbuffer_init(&io4.monoflop_cb_rb, MONOFLOP_CB_BUFFER_SIZE, &io4.monoflop_cb_buffer[0]);
}

void io4_tick(void) {
	if(io4.capture_input_callback_enabled) {
		// We need to wait for full input buffer to be send
		if(io4.capture_input_buffer_index == 64) {
			return;
		}

		uint8_t new_index = io4_capture_input_data_index;
		if(new_index != io4_capture_input_data_last_index) {
			uint8_t new_data_length = new_index - io4_capture_input_data_last_index;
			new_data_length = new_data_length & 0xFE; // Make sure we have even number of bytes
			for (uint8_t i = 0; i < new_data_length; i += 2) {
				// Two io4 captures per byte
				io4.capture_input_buffer[io4.capture_input_buffer_index] =
					(io4_capture_input_data[io4_capture_input_data_last_index + i + 0] << 0) |
					(io4_capture_input_data[io4_capture_input_data_last_index + i + 1] << 4);

				io4.capture_input_buffer_index++;
				if(io4.capture_input_buffer_index == 64) {
					break;
				}
			}

			io4_capture_input_data_last_index += new_data_length;
		}

		// Ignore other IO4 functions if capture input callback is enabled
		return;
	}

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
			if(io4.channels[i].monoflop.running) {
				if(system_timer_is_time_elapsed_ms(io4.channels[i].monoflop.time_start, io4.channels[i].monoflop.time)) {
					// Monoflop time expired
					io4.channels[i].monoflop.time_start = 0;
					io4.channels[i].monoflop.time_remaining = 0;
					io4.channels[i].monoflop.running = false;

					io4.channels[i].value = !io4.channels[i].value;

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
						io4.channels[i].monoflop.time - (system_timer_get_ms() - io4.channels[i].monoflop.time_start);
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

void io4_pwm_init(const uint8_t channel) {
	const XMC_CCU4_SLICE_COMPARE_CONFIG_t compare_config = {
		.timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot            = false,
		.shadow_xfer_clear   = 0,
		.dither_timer_period = 0,
		.dither_duty_cycle   = 0,
		.prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable          = 0,
		.prescaler_initval   = 0,
		.float_limit         = 0,
		.dither_limit        = 0,
		.passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = 0
	};

	XMC_CCU4_StartPrescaler(CCU40);
	XMC_CCU4_Init(CCU40, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
	XMC_CCU4_SLICE_CompareInit(io4_slice[channel], &compare_config);

	// Set the period and compare register values
	XMC_CCU4_SLICE_SetTimerCompareMatch(io4_slice[channel], 0);
	XMC_CCU4_SLICE_SetTimerPeriodMatch(io4_slice[channel], 32000);

	XMC_CCU4_EnableShadowTransfer(CCU40,
	                              (XMC_CCU4_SHADOW_TRANSFER_SLICE_0 << (channel*4)) |
	                              (XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0 << (channel*4)));

	XMC_GPIO_SetMode(io4.channels[channel].port_base, io4.channels[channel].pin, ch_pin_pwm_mode);

	XMC_CCU4_EnableClock(CCU40, channel);
}

void io4_pwm_update(const uint8_t channel, const uint32_t frequency, const uint16_t duty_cycle) {
	uint32_t divisor = 1;
	bool new_start = false;
	uint32_t prescaler = 0;
	uint32_t period_value = 0;
	uint32_t compare_value = 0;

	if(frequency == 0) {
		io4_pwm_stop(channel);

		return;
	}

	new_start = (io4.channels[channel].pwm.frequency == 0);

	if(new_start) {
		io4_pwm_init(channel);
	}

	io4.channels[channel].pwm.duty_cycle = MIN(10000, duty_cycle);
	io4.channels[channel].pwm.frequency  = MIN(320000000, frequency);

	divisor = 1;
	prescaler = 0;
	period_value = (640000000 / io4.channels[channel].pwm.frequency) - 1;

	while(period_value > 0xFFFF) {
		prescaler++;
		divisor *= 2;
		period_value = (640000000 / (io4.channels[channel].pwm.frequency * divisor)) - 1;
	}

	compare_value = (period_value * (10000 - duty_cycle) + 10000 / 2) / 10000;

	XMC_CCU4_SLICE_SetPrescaler(io4_slice[channel], prescaler);
	XMC_CCU4_SLICE_SetTimerPeriodMatch(io4_slice[channel], period_value);
	XMC_CCU4_SLICE_SetTimerCompareMatch(io4_slice[channel], compare_value);
	XMC_CCU4_EnableShadowTransfer(CCU40,
	                              (XMC_CCU4_SHADOW_TRANSFER_SLICE_0 << (channel * 4)) |
	                              (XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0 << (channel * 4)));

	if(new_start) {
		XMC_CCU4_SLICE_StartTimer(io4_slice[channel]);
	}
}

void io4_pwm_stop(const uint8_t channel) {
	if(io4.channels[channel].pwm.frequency != 0) {
		io4.channels[channel].pwm.duty_cycle = 0;
		io4.channels[channel].pwm.frequency  = 0;

		XMC_CCU4_SLICE_StopTimer(io4_slice[channel]);
		// Channel was in PWM mode put it back to output mode
		XMC_GPIO_SetMode(io4.channels[channel].port_base,
		                 io4.channels[channel].pin,
		                 ch_pin_out_mode);
	}
}

void io4_monoflop_update(const uint8_t channel, const bool value, const uint32_t time) {
	io4.channels[channel].value = value;
	io4.channels[channel].monoflop.time = time;
	io4.channels[channel].monoflop.time_remaining = time;
	io4.channels[channel].monoflop.running = true;

	if(io4.channels[channel].value) {
		XMC_GPIO_SetOutputHigh(io4.channels[channel].port_base,
		                       io4.channels[channel].pin);
	}
	else {
		XMC_GPIO_SetOutputLow(io4.channels[channel].port_base,
		                      io4.channels[channel].pin);
	}

	io4.channels[channel].monoflop.time_start = system_timer_get_ms();
}

void io4_monoflop_stop(const uint8_t channel) {
	io4.channels[channel].monoflop.time_start = 0;
	io4.channels[channel].monoflop.time_remaining = 0;
	io4.channels[channel].monoflop.running = false;
}

void io4_edge_count_update(const uint8_t channel,
                           const uint8_t debounce,
                           const uint8_t edge_type) {
	io4.channels[channel].edge_count.cnt_edge_rising = 0;
	io4.channels[channel].edge_count.cnt_edge_falling = 0;
	io4.channels[channel].edge_count.debounce = debounce;
	io4.channels[channel].edge_count.edge_type = edge_type;
	io4.channels[channel].edge_count.last_value = \
		(bool)XMC_GPIO_GetInput(io4.channels[channel].port_base,
		                        io4.channels[channel].pin);
	io4.channels[channel].edge_count.debounce_start = system_timer_get_ms();
}

void io4_edge_count_stop(const uint8_t channel) {
	io4.channels[channel].edge_count.debounce = 0;
	io4.channels[channel].edge_count.debounce_start = 0;
	io4.channels[channel].edge_count.cnt_edge_rising = 0;
	io4.channels[channel].edge_count.cnt_edge_falling = 0;
}

void io4_input_value_cb_update(const uint8_t channel,
                               const uint32_t period,
                               const bool value_has_to_change) {
	io4.channels[channel].input_value_cb.period = period;
	io4.channels[channel].input_value_cb.period_start = 0;
	io4.channels[channel].input_value_cb.value_has_to_change = value_has_to_change;

	if(period > 0) {
		io4.channels[channel].input_value_cb.last_value = \
			(bool)XMC_GPIO_GetInput(io4.channels[channel].port_base,
			                        io4.channels[channel].pin);
		io4.channels[channel].input_value_cb.period_start = system_timer_get_ms();
	}
}

void io4_input_value_cb_stop(const uint8_t channel) {
	io4.channels[channel].input_value_cb.period = 0;
	io4.channels[channel].input_value_cb.period_start = 0;
}

void io4_all_input_value_cb_update(const uint32_t period,
                                   const bool value_has_to_change) {
	io4.all_input_value_cb.period_start = 0;
	io4.all_input_value_cb.period = period;
	io4.all_input_value_cb.value_has_to_change = value_has_to_change;

	if(io4.all_input_value_cb.period > 0) {
		for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
			io4.all_input_value_cb.last_values[i] = \
				(bool)XMC_GPIO_GetInput(io4.channels[i].port_base, io4.channels[i].pin);
		}

		io4.all_input_value_cb.period_start = system_timer_get_ms();
	}
}

void io4_all_input_value_cb_stop(void) {
	io4.all_input_value_cb.period = 0;
	io4.all_input_value_cb.period_start = 0;
}
