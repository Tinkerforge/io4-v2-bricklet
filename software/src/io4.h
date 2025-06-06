/* io4-v2-bricklet
 * Copyright (C) 2018-2019 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 * Copyright (C) 2025 Olaf Lüke <olaf@tinkerforge.com>
 *
 * io4.h: Implementation of IO4 V2 Bricklet
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

#ifndef IO4_H
#define IO4_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/utility/ringbuffer.h"

#include "xmc_gpio.h"

#define IO4_CAPTURE_INPUT_SIZE 256

#define NUMBER_OF_CHANNELS 4

#define INPUT_VALUE_CB_BUFFER_UNIT_SIZE 3
#define INPUT_VALUE_CB_BUFFER_SIZE (64 * INPUT_VALUE_CB_BUFFER_UNIT_SIZE)

#define ALL_INPUT_VALUE_CB_BUFFER_UNIT_SIZE 2
#define ALL_INPUT_VALUE_CB_BUFFER_SIZE (64 * ALL_INPUT_VALUE_CB_BUFFER_UNIT_SIZE)

#define MONOFLOP_CB_BUFFER_UNIT_SIZE 2
#define MONOFLOP_CB_BUFFER_SIZE (64 * MONOFLOP_CB_BUFFER_UNIT_SIZE)

typedef struct {
  uint32_t period;
  bool last_value;
  uint32_t period_start;
  bool value_has_to_change;
} IO_CH_INPUT_VALUE_CB_t;

typedef struct {
  uint32_t period;
  Ringbuffer cb_rb;
  uint32_t period_start;
  bool value_has_to_change;
  bool last_values[NUMBER_OF_CHANNELS];
  uint8_t cb_buffer[ALL_INPUT_VALUE_CB_BUFFER_SIZE];
} ALL_IO_CH_INPUT_VALUE_CB_t;

typedef struct {
  bool last_value;
  uint8_t debounce;
  uint8_t edge_type;
  uint32_t debounce_start;
  uint32_t cnt_edge_rising;
  uint32_t cnt_edge_falling;
} IO_CH_EDGE_COUNT_t;

typedef struct {
  uint32_t time;
  uint32_t time_start;
  uint32_t time_remaining;
  bool running;
} IO_CH_MONOFLOP_t;

typedef struct {
  uint32_t frequency;
  uint16_t duty_cycle;
} IO_CH_PWM_t;

typedef struct {
  // Generic channel config
  bool value;
  uint8_t pin;
  char direction;
  bool init_value;
  XMC_GPIO_PORT_t *port_base;

  // Channel monoflop config
  IO_CH_MONOFLOP_t monoflop;
  // Channel edge count config
  IO_CH_EDGE_COUNT_t edge_count;
  // Channel input value callback config
  IO_CH_INPUT_VALUE_CB_t input_value_cb;
  // Channel PWM config
  IO_CH_PWM_t pwm;
} IO_CH_t;

typedef struct {
  IO_CH_t channels[NUMBER_OF_CHANNELS];

  // Input value change callback
  Ringbuffer input_value_cb_rb;
  ALL_IO_CH_INPUT_VALUE_CB_t all_input_value_cb;
  uint8_t input_value_cb_buffer[INPUT_VALUE_CB_BUFFER_SIZE];

  // Monoflop callback
  Ringbuffer monoflop_cb_rb;
  uint8_t monoflop_cb_buffer[MONOFLOP_CB_BUFFER_SIZE];

  bool capture_input_callback_enabled;
  uint16_t capture_input_callback_time_between_capture;
  uint8_t capture_input_buffer[64];
  uint8_t capture_input_buffer_index;
} IO4;

extern IO4 io4;
extern XMC_GPIO_MODE_t ch_pin_out_mode;
extern XMC_GPIO_MODE_t ch_pin_pwm_mode;
extern XMC_GPIO_MODE_t ch_pin_in_pull_up_mode;
extern XMC_GPIO_MODE_t ch_pin_in_no_pull_up_mode;

void io4_init(void);
void io4_tick(void);
void io4_pwm_update(const uint8_t channel,
                    const uint32_t frequency,
                    const uint16_t duty_cycle);
void io4_pwm_stop(const uint8_t channel);
void io4_monoflop_update(const uint8_t channel,
                         const bool value,
                         const uint32_t time);
void io4_monoflop_stop(const uint8_t channel);
void io4_edge_count_update(const uint8_t channel,
                           const uint8_t debounce,
                           const uint8_t edge_type);
void io4_edge_count_stop(const uint8_t channel);
void io4_input_value_cb_update(const uint8_t channel,
                               const uint32_t period,
                               const bool value_has_to_change);
void io4_input_value_cb_stop(const uint8_t channel);
void io4_all_input_value_cb_update(const uint32_t period,
                                   const bool value_has_to_change);
void io4_all_input_value_cb_stop(void);

#endif
