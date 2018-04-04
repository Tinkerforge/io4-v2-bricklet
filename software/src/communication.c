/* io4-v2-bricklet
 * Copyright (C) 2018 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/logging/logging.h"

#include "io4.h"

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_SET_VALUE: return set_value(message);
		case FID_GET_VALUE: return get_value(message, response);
		case FID_SET_SELECTED_VALUE: return set_selected_value(message);
		case FID_GET_SELECTED_VALUE: return get_selected_value(message, response);
		case FID_SET_CONFIGURATION: return set_configuration(message);
		case FID_GET_CONFIGURATION: return get_configuration(message, response);
		case FID_SET_INPUT_VALUE_CALLBACK_CONFIGURATION: return set_input_value_callback_configuration(message);
		case FID_GET_INPUT_VALUE_CALLBACK_CONFIGURATION: return get_input_value_callback_configuration(message, response);
		case FID_SET_ALL_INPUT_VALUE_CALLBACK_CONFIGURATION: return set_all_input_value_callback_configuration(message);
		case FID_GET_ALL_INPUT_VALUE_CALLBACK_CONFIGURATION: return get_all_input_value_callback_configuration(message, response);
		case FID_SET_MONOFLOP: return set_monoflop(message);
		case FID_GET_MONOFLOP: return get_monoflop(message, response);
		case FID_GET_EDGE_COUNT: return get_edge_count(message, response);
		case FID_SET_EDGE_COUNT_CONFIGURATION: return set_edge_count_configuration(message);
		case FID_GET_EDGE_COUNT_CONFIGURATION: return get_edge_count_configuration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse set_value(const SetValue *data) {
	logd("[+] IO4-V2: set_value()\n\r");

	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		if(io4.channels[i].direction != IO4_V2_DIRECTION_OUT) {
			// Channel not an output
			continue;
		}

		if(data->value & (1 << i)) {
			io4.channels[i].value = true;
			XMC_GPIO_SetOutputHigh(io4.channels[i].port_base, io4.channels[i].pin);
		}
		else {
			io4.channels[i].value = false;
			XMC_GPIO_SetOutputLow(io4.channels[i].port_base, io4.channels[i].pin);
		}
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_value(const GetValue *data,
                                          GetValue_Response *response) {
	logd("[+] IO4-V2: get_value()\n\r");

	uint8_t bit_encoded_values = 0;
	response->header.length = sizeof(GetValue_Response);

	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		if(io4.channels[i].value) {
			bit_encoded_values |= (1 << i);
		}
		else {
			bit_encoded_values &= ~(1 << i);
		}
	}

	response->value = bit_encoded_values;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_selected_value(const SetSelectedValue *data) {
	logd("[+] IO4-V2: set_selected_value()\n\r");

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_OUT) {
		return HANDLE_MESSAGE_RESPONSE_EMPTY;
	}

	io4.channels[data->channel].value = data->value;

	if(io4.channels[data->channel].value) {
		XMC_GPIO_SetOutputHigh(io4.channels[data->channel].port_base,
		                       io4.channels[data->channel].pin);
	}
	else {
		XMC_GPIO_SetOutputLow(io4.channels[data->channel].port_base,
		                      io4.channels[data->channel].pin);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_selected_value(const GetSelectedValue *data, GetSelectedValue_Response *response) {
	logd("[+] IO4-V2: get_selected_value()\n\r");

	response->header.length = sizeof(GetSelectedValue_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->value = io4.channels[data->channel].value;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	logd("[+] IO4-V2: set_configuration()\n\r");

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->direction == 'i') {
		io4.channels[data->channel].init_value = data->value;
		io4.channels[data->channel].direction = IO4_V2_DIRECTION_IN;

		if(io4.channels[data->channel].init_value) {
			XMC_GPIO_Init(io4.channels[data->channel].port_base,
			              io4.channels[data->channel].pin,
			              &ch_pin_in_pull_up_config);
		}
		else {
			XMC_GPIO_Init(io4.channels[data->channel].port_base,
			              io4.channels[data->channel].pin,
			              &ch_pin_in_no_pull_up_config);
		}

		io4.channels[data->channel].value = \
			(bool)XMC_GPIO_GetInput(io4.channels[data->channel].port_base,
			                        io4.channels[data->channel].pin);

		// Reset monoflop
		io4.channels[data->channel].monoflop.time = 0;
		io4.channels[data->channel].monoflop.time_start = 0;
		io4.channels[data->channel].monoflop.time_remaining = 0;
	}
	else if(data->direction == 'o') {
		io4.channels[data->channel].init_value = data->value;
		io4.channels[data->channel].direction = IO4_V2_DIRECTION_OUT;
		io4.channels[data->channel].value = io4.channels[data->channel].init_value;

		XMC_GPIO_Init(io4.channels[data->channel].port_base,
		              io4.channels[data->channel].pin,
		              &ch_pin_out_config);

		if(io4.channels[data->channel].init_value) {
			XMC_GPIO_SetOutputHigh(io4.channels[data->channel].port_base,
			                       io4.channels[data->channel].pin);
		}
		else {
			XMC_GPIO_SetOutputLow(io4.channels[data->channel].port_base,
			                      io4.channels[data->channel].pin);
		}

		// Reset monoflop
		io4.channels[data->channel].monoflop.time = 0;
		io4.channels[data->channel].monoflop.time_start = 0;
		io4.channels[data->channel].monoflop.time_remaining = 0;
	}
	else {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_configuration(const GetConfiguration *data, GetConfiguration_Response *response) {
	logd("[+] IO4-V2: get_configuration()\n\r");

	response->header.length = sizeof(GetConfiguration_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->value = io4.channels[data->channel].init_value;
	response->direction = io4.channels[data->channel].direction;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_input_value_callback_configuration(const SetInputValueCallbackConfiguration *data) {
	logd("[+] IO4-V2: set_input_value_callback_configuration()\n\r");

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_IN) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	io4.channels[data->channel].input_value_cb.period_start = 0;
	io4.channels[data->channel].input_value_cb.period = data->period;
	io4.channels[data->channel].input_value_cb.value_has_to_change = data->value_has_to_change;

	if(io4.channels[data->channel].input_value_cb.period > 0) {
		io4.channels[data->channel].input_value_cb.last_value = \
			(bool)XMC_GPIO_GetInput(io4.channels[data->channel].port_base,
			                        io4.channels[data->channel].pin);
		io4.channels[data->channel].input_value_cb.period_start = system_timer_get_ms();
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_input_value_callback_configuration(const GetInputValueCallbackConfiguration *data,
                                                                       GetInputValueCallbackConfiguration_Response *response) {
	logd("[+] IO4-V2: get_input_value_callback_configuration()\n\r");

	response->header.length = sizeof(GetInputValueCallbackConfiguration_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->period = io4.channels[data->channel].input_value_cb.period;
	response->value_has_to_change = io4.channels[data->channel].input_value_cb.value_has_to_change;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_all_input_value_callback_configuration(const SetAllInputValueCallbackConfiguration *data) {
	logd("[+] IO4-V2: set_all_input_value_callback_configuration()\n\r");

	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		if(io4.channels[i].direction != IO4_V2_DIRECTION_IN) {
			return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
		}
	}

	io4.all_input_value_cb.period_start = 0;
	io4.all_input_value_cb.period = data->period;
	io4.all_input_value_cb.value_has_to_change = data->value_has_to_change;

	if(io4.all_input_value_cb.period > 0) {		
		// Store current channel states
		for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
			io4.all_input_value_cb.last_values[i] = \
				(bool)XMC_GPIO_GetInput(io4.channels[i].port_base, io4.channels[i].pin);
		}

		io4.all_input_value_cb.period_start = system_timer_get_ms();
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_all_input_value_callback_configuration(const GetAllInputValueCallbackConfiguration *data,
                                                                           GetAllInputValueCallbackConfiguration_Response *response) {
	logd("[+] IO4-V2: get_all_input_value_callback_configuration()\n\r");

	response->header.length = sizeof(GetInputValueCallbackConfiguration_Response);
	response->period = io4.all_input_value_cb.period;
	response->value_has_to_change = io4.all_input_value_cb.value_has_to_change;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_monoflop(const SetMonoflop *data) {
	logd("[+] IO4-V2: set_monoflop()\n\r");

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_OUT) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	io4.channels[data->channel].value = data->value;
	io4.channels[data->channel].monoflop.time = data->time;
  io4.channels[data->channel].monoflop.time_remaining = data->time;

	if(io4.channels[data->channel].value) {
		XMC_GPIO_SetOutputHigh(io4.channels[data->channel].port_base,
		                       io4.channels[data->channel].pin);
	}
	else {
		XMC_GPIO_SetOutputLow(io4.channels[data->channel].port_base,
		                      io4.channels[data->channel].pin);
	}

	io4.channels[data->channel].monoflop.time_start = system_timer_get_ms();

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_monoflop(const GetMonoflop *data,
                                             GetMonoflop_Response *response) {
	logd("[+] IO4-V2: get_monoflop()\n\r");

	response->header.length = sizeof(GetMonoflop_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_OUT) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->time = io4.channels[data->channel].monoflop.time;
	response->value = io4.channels[data->channel].value;
	response->time_remaining = io4.channels[data->channel].monoflop.time_remaining;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_edge_count(const GetEdgeCount *data, GetEdgeCount_Response *response) {
	logd("[+] IO4-V2: get_edge_count()\n\r");

	response->header.length = sizeof(GetEdgeCount_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_IN) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].edge_count.edge_type == IO4_V2_EDGE_TYPE_RISING) {
		response->count = io4.channels[data->channel].edge_count.cnt_edge_rising;
	}
	else if(io4.channels[data->channel].edge_count.edge_type == IO4_V2_EDGE_TYPE_FALLING) {
		response->count = io4.channels[data->channel].edge_count.cnt_edge_falling;
	}
	else if(io4.channels[data->channel].edge_count.edge_type == IO4_V2_EDGE_TYPE_BOTH) {
		response->count = \
			io4.channels[data->channel].edge_count.cnt_edge_rising + \
			io4.channels[data->channel].edge_count.cnt_edge_falling;
	}

	if(data->reset_counter) {
		io4.channels[data->channel].edge_count.cnt_edge_rising = 0;
		io4.channels[data->channel].edge_count.cnt_edge_falling = 0;
	}

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_edge_count_configuration(const SetEdgeCountConfiguration *data) {
	logd("[+] IO4-V2: set_edge_count_configuration()\n\r");

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_IN) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	io4.channels[data->channel].edge_count.cnt_edge_rising = 0;
	io4.channels[data->channel].edge_count.cnt_edge_falling = 0;
	io4.channels[data->channel].edge_count.debounce = data->debounce;
	io4.channels[data->channel].edge_count.edge_type = data->edge_type;
	io4.channels[data->channel].edge_count.last_value = \
		(bool)XMC_GPIO_GetInput(io4.channels[data->channel].port_base,
		                        io4.channels[data->channel].pin);
	io4.channels[data->channel].edge_count.debounce_start = system_timer_get_ms();

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_edge_count_configuration(const GetEdgeCountConfiguration *data,
                                                             GetEdgeCountConfiguration_Response *response) {
	logd("[+] IO4-V2: get_edge_count_configuration()\n\r");

	response->header.length = sizeof(GetEdgeCountConfiguration_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_IN) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->debounce = io4.channels[data->channel].edge_count.debounce;
	response->edge_type = io4.channels[data->channel].edge_count.edge_type;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

bool handle_input_value_callback(void) {
	static bool is_buffered = false;
	static InputValue_Callback cb;

	if(!is_buffered) {
		if(ringbuffer_is_empty(&io4.input_value_cb_rb)) {
			// Nothing to send
			return false;
		}

		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(InputValue_Callback),
		                        FID_CALLBACK_INPUT_VALUE);

		ringbuffer_get(&io4.input_value_cb_rb, &cb.channel);
		ringbuffer_get(&io4.input_value_cb_rb, (uint8_t *)&cb.changed);
		ringbuffer_get(&io4.input_value_cb_rb, (uint8_t *)&cb.value);
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(InputValue_Callback));
		is_buffered = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

bool handle_all_input_value_callback(void) {
	static bool is_buffered = false;
	static AllInputValue_Callback cb;

	if(!is_buffered) {
		if(ringbuffer_is_empty(&io4.all_input_value_cb.cb_rb)) {
			// Nothing to send
			return false;
		}

		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(AllInputValue_Callback),
		                        FID_CALLBACK_ALL_INPUT_VALUE);

		ringbuffer_get(&io4.all_input_value_cb.cb_rb, &cb.changed);
		ringbuffer_get(&io4.all_input_value_cb.cb_rb, &cb.value);
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(AllInputValue_Callback));

		is_buffered = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

bool handle_monoflop_done_callback(void) {
	static bool is_buffered = false;
	static MonoflopDone_Callback cb;

	if(!is_buffered) {
		if(ringbuffer_is_empty(&io4.monoflop_cb_rb)) {
			// Nothing to send
			return false;
		}

		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(MonoflopDone_Callback),
		                        FID_CALLBACK_MONOFLOP_DONE);

		ringbuffer_get(&io4.monoflop_cb_rb, &cb.channel);
		ringbuffer_get(&io4.monoflop_cb_rb, (uint8_t *)&cb.value);
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(MonoflopDone_Callback));

		is_buffered = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	communication_callback_init();
}
