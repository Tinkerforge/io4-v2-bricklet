/* io4-v2-bricklet
 * Copyright (C) 2018-2019 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 * Copyright (C) 2025 Olaf Lüke <olaf@tinkerforge.com>
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
#include "bricklib2/utility/util_definitions.h"

#include "io4.h"
#include "timer.h"

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	const uint8_t length = ((TFPMessageHeader*)message)->length;
	switch(tfp_get_fid_from_message(message)) {
		case FID_SET_VALUE:                                  return length != sizeof(SetValue)                              ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_value(message);
		case FID_GET_VALUE:                                  return length != sizeof(GetValue)                              ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_value(message, response);
		case FID_SET_SELECTED_VALUE:                         return length != sizeof(SetSelectedValue)                      ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_selected_value(message);
		case FID_SET_CONFIGURATION:                          return length != sizeof(SetConfiguration)                      ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_configuration(message);
		case FID_GET_CONFIGURATION:                          return length != sizeof(GetConfiguration)                      ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_configuration(message, response);
		case FID_SET_INPUT_VALUE_CALLBACK_CONFIGURATION:     return length != sizeof(SetInputValueCallbackConfiguration)    ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_input_value_callback_configuration(message);
		case FID_GET_INPUT_VALUE_CALLBACK_CONFIGURATION:     return length != sizeof(GetInputValueCallbackConfiguration)    ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_input_value_callback_configuration(message, response);
		case FID_SET_ALL_INPUT_VALUE_CALLBACK_CONFIGURATION: return length != sizeof(SetAllInputValueCallbackConfiguration) ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_all_input_value_callback_configuration(message);
		case FID_GET_ALL_INPUT_VALUE_CALLBACK_CONFIGURATION: return length != sizeof(GetAllInputValueCallbackConfiguration) ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_all_input_value_callback_configuration(message, response);
		case FID_SET_MONOFLOP:                               return length != sizeof(SetMonoflop)                           ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_monoflop(message);
		case FID_GET_MONOFLOP:                               return length != sizeof(GetMonoflop)                           ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_monoflop(message, response);
		case FID_GET_EDGE_COUNT:                             return length != sizeof(GetEdgeCount)                          ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_edge_count(message, response);
		case FID_SET_EDGE_COUNT_CONFIGURATION:               return length != sizeof(SetEdgeCountConfiguration)             ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_edge_count_configuration(message);
		case FID_GET_EDGE_COUNT_CONFIGURATION:               return length != sizeof(GetEdgeCountConfiguration)             ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_edge_count_configuration(message, response);
		case FID_SET_PWM_CONFIGURATION:                      return length != sizeof(SetPWMConfiguration)                   ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_pwm_configuration(message);
		case FID_GET_PWM_CONFIGURATION:                      return length != sizeof(GetPWMConfiguration)                   ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_pwm_configuration(message, response);
		case FID_SET_CAPTURE_INPUT_CALLBACK_CONFIGURATION:   return length != sizeof(SetCaptureInputCallbackConfiguration)  ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : set_capture_input_callback_configuration(message);
		case FID_GET_CAPTURE_INPUT_CALLBACK_CONFIGURATION:   return length != sizeof(GetCaptureInputCallbackConfiguration)  ? HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER : get_capture_input_callback_configuration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse set_value(const SetValue *data) {
	logd("[+] IO4-V2: set_value()\n\r");

	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		// Channel not an output
		if(io4.channels[i].direction != IO4_V2_DIRECTION_OUT) {
			continue;
		}

		// Stop PWM if active
		io4_pwm_stop(i);

		// Stop and reset monoflop if active
		io4_monoflop_stop(i);

		if(data->value & (1 << i)) {
			XMC_GPIO_SetOutputHigh(io4.channels[i].port_base, io4.channels[i].pin);

			// Update state
			io4.channels[i].value = true;
		}
		else {
			XMC_GPIO_SetOutputLow(io4.channels[i].port_base, io4.channels[i].pin);

			// Update state
			io4.channels[i].value = false;
		}
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_value(const GetValue *data,
                                          GetValue_Response *response) {
	logd("[+] IO4-V2: get_value()\n\r");

	uint8_t bit_encoded_values = 0;
	response->header.length = sizeof(GetValue_Response);

	// FIXME: What to do if the channel has an active PWM?
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

	// Stop PWM if active
	io4_pwm_stop(data->channel);

	// Stop and reset monoflop if active
	io4_monoflop_stop(data->channel);

	if(data->value) {
		XMC_GPIO_SetOutputHigh(io4.channels[data->channel].port_base,
		                       io4.channels[data->channel].pin);
	}
	else {
		XMC_GPIO_SetOutputLow(io4.channels[data->channel].port_base,
		                      io4.channels[data->channel].pin);
	}

	// Update state
	io4.channels[data->channel].value = data->value;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_configuration(const SetConfiguration *data) {
	logd("[+] IO4-V2: set_configuration()\n\r");

	if(data->channel > (NUMBER_OF_CHANNELS - 1)) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(data->direction == IO4_V2_DIRECTION_IN) {
		// Nothing to do
		if((io4.channels[data->channel].direction == data->direction) && (io4.channels[data->channel].init_value == data->value)) {
			return HANDLE_MESSAGE_RESPONSE_EMPTY;
		}

		// Direction changing (OUT -> IN)
		if(io4.channels[data->channel].direction != data->direction) {
			// Stop PWM if active
			io4_pwm_stop(data->channel);

			// Stop and reset monoflop if active
			io4_monoflop_stop(data->channel);

			// Reset and stop edge counter
			io4_edge_count_stop(data->channel);

			// Reset and stop input value cb
			io4_input_value_cb_stop(data->channel);
			io4_all_input_value_cb_stop();
		}

		// Do input configuration
		if(data->value) {
			XMC_GPIO_SetMode(io4.channels[data->channel].port_base,
			                 io4.channels[data->channel].pin,
			                 ch_pin_in_pull_up_mode);
		}
		else {
			XMC_GPIO_SetMode(io4.channels[data->channel].port_base,
			                 io4.channels[data->channel].pin,
			                 ch_pin_in_no_pull_up_mode);
		}

		// Update state
		io4.channels[data->channel].init_value = data->value;
		io4.channels[data->channel].direction = data->direction;
		io4.channels[data->channel].value = \
			(bool)XMC_GPIO_GetInput(io4.channels[data->channel].port_base,
			                        io4.channels[data->channel].pin);
	}
	else if(data->direction == IO4_V2_DIRECTION_OUT) {
		// Do output configuration (IN -> OUT)
		if(io4.channels[data->channel].direction != data->direction) {
			// Reset and stop edge counter
			io4_edge_count_stop(data->channel);

			// Reset and stop input value cb
			io4_input_value_cb_stop(data->channel);
			io4_all_input_value_cb_stop();

			XMC_GPIO_SetMode(io4.channels[data->channel].port_base,
			                 io4.channels[data->channel].pin,
			                 ch_pin_out_mode);
		} else {
			// Stop PWM if active
			io4_pwm_stop(data->channel);

			// Stop and reset monoflop if active
			io4_monoflop_stop(data->channel);
		}

		// Set output value
		if(data->value) {
			XMC_GPIO_SetOutputHigh(io4.channels[data->channel].port_base,
			                       io4.channels[data->channel].pin);
		}
		else {
			XMC_GPIO_SetOutputLow(io4.channels[data->channel].port_base,
			                      io4.channels[data->channel].pin);
		}

		// Update state
		io4.channels[data->channel].value = data->value;
		io4.channels[data->channel].init_value = data->value;
		io4.channels[data->channel].direction = data->direction;
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

	io4_input_value_cb_update(data->channel, data->period, data->value_has_to_change);

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

	io4_all_input_value_cb_update(data->period, data->value_has_to_change);

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

	// Reset and stop PWM
	io4_pwm_stop(data->channel);

	io4_monoflop_update(data->channel, data->value, data->time);

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

	io4_edge_count_update(data->channel, data->debounce, data->edge_type);

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

BootloaderHandleMessageResponse set_pwm_configuration(const SetPWMConfiguration *data) {
	if(data->channel >= NUMBER_OF_CHANNELS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	if(io4.channels[data->channel].direction != IO4_V2_DIRECTION_OUT) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// Stop and reset monoflop
	io4_monoflop_stop(data->channel);

	io4_pwm_update(data->channel, data->frequency, data->duty_cycle);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_pwm_configuration(const GetPWMConfiguration *data, GetPWMConfiguration_Response *response) {
	if(data->channel >= NUMBER_OF_CHANNELS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(GetPWMConfiguration_Response);
	response->duty_cycle    = io4.channels[data->channel].pwm.duty_cycle;
	response->frequency     = io4.channels[data->channel].pwm.frequency;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_capture_input_callback_configuration(const SetCaptureInputCallbackConfiguration *data) {
	io4.capture_input_callback_enabled              = data->enable;
	io4.capture_input_callback_time_between_capture = BETWEEN(20, data->time_between_capture, 0xFFFF);

	if(data->enable) {
		timer_init();
	} else {
		timer_stop();
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_capture_input_callback_configuration(const GetCaptureInputCallbackConfiguration *data, GetCaptureInputCallbackConfiguration_Response *response) {
	response->header.length        = sizeof(GetCaptureInputCallbackConfiguration_Response);
	response->enable               = io4.capture_input_callback_enabled;
	response->time_between_capture = io4.capture_input_callback_time_between_capture;

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

bool handle_capture_input_callback(void) {
	static bool is_buffered = false;
	static CaptureInput_Callback cb;

	if(!is_buffered) {
		// Nothing to send
		if(io4.capture_input_buffer_index != 64) {
			return false;
		}

		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(CaptureInput_Callback), FID_CALLBACK_CAPTURE_INPUT);
		memcpy(cb.data, io4.capture_input_buffer, sizeof(io4.capture_input_buffer));
		io4.capture_input_buffer_index = 0;
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(CaptureInput_Callback));
		is_buffered = false;
		return true;
	} else {
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
