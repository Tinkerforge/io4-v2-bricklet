/* io4-v2-bricklet
 * Copyright (C) 2025 Olaf Lüke <olaf@tinkerforge.com>
 *
 * timer.c: Timer handling
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

#include "timer.h"

#include "io4.h"

#include "xmc_ccu4.h"
#include "xmc_eru.h"

void timer_init(void) {
	XMC_CCU4_SLICE_COMPARE_CONFIG_t timer0_config = {
		.timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
		.monoshot            = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
		.shadow_xfer_clear   = false,
		.dither_timer_period = false,
		.dither_duty_cycle   = false,
		.prescaler_mode      = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
		.mcm_enable          = false,
		.prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_64, // Use prescaler 2 to get mclk = fccu4
		.float_limit         = 0U,
		.dither_limit        = 0U,
		.passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW,
		.timer_concatenation = false
	};

	// Initialize CCU4
    XMC_CCU4_Init(CCU40, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    XMC_CCU4_StartPrescaler(CCU40);

    XMC_CCU4_EnableClock(CCU40, 0);
    XMC_CCU4_SLICE_CompareInit(CCU40_CC40, &timer0_config);
	// With prescaler 32: XXX = XXXus
    XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, io4.capture_input_callback_time_between_capture);
    XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC40, 0);
    XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0);

    XMC_CCU4_SLICE_SetInterruptNode(CCU40_CC40, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH, XMC_CCU4_SLICE_SR_ID_0);
    XMC_CCU4_SLICE_EnableEvent(CCU40_CC40, XMC_CCU4_SLICE_IRQ_ID_PERIOD_MATCH);
    NVIC_SetPriority(CCU40_0_IRQn, 0);
	NVIC_EnableIRQ(CCU40_0_IRQn);

    XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0 | XMC_CCU4_SHADOW_TRANSFER_PRESCALER_SLICE_0);

    // Start
	XMC_CCU4_SLICE_ClearTimer(CCU40_CC40);
    XMC_CCU4_SLICE_StartTimer(CCU40_CC40);
}

void timer_stop(void) {
	XMC_CCU4_SLICE_StopTimer(CCU40_CC40);
	XMC_CCU4_DisableClock(CCU40, 0);
	NVIC_DisableIRQ(CCU40_0_IRQn);
	XMC_CCU4_StopPrescaler(CCU40);
}