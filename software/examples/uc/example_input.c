// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_io4_v2.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

static TF_IO4V2 io;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_io4_v2_create(&io, NULL, hal), "create device object");

	// Get current value
	bool value[4];
	check(tf_io4_v2_get_value(&io, value), "get value");

	tf_hal_printf("Channel 0: %s\n", value[0] ? "true" : "false");
	tf_hal_printf("Channel 1: %s\n", value[1] ? "true" : "false");
	tf_hal_printf("Channel 2: %s\n", value[2] ? "true" : "false");
	tf_hal_printf("Channel 3: %s\n", value[3] ? "true" : "false");
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
