// This example is not self-contained.
// It requres usage of the example driver specific to your platform.
// See the HAL documentation.

#include "bindings/hal_common.h"
#include "bindings/bricklet_io4_v2.h"

#define UID "XYZ" // Change XYZ to the UID of your IO-4 Bricklet 2.0

void check(int rc, const char* msg);

void example_setup(TF_HalContext *hal);
void example_loop(TF_HalContext *hal);


// Callback function for input value callback
static void input_value_handler(TF_IO4V2 *device, uint8_t channel, bool changed,
                                bool value, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Channel: %I8u\n", channel);
	tf_hal_printf("Changed: %s\n", changed ? "true" : "false");
	tf_hal_printf("Value: %s\n", value ? "true" : "false");
	tf_hal_printf("\n");
}

static TF_IO4V2 io;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_io4_v2_create(&io, UID, hal), "create device object");

	// Register input value callback to function input_value_handler
	tf_io4_v2_register_input_value_callback(&io,
	                                        input_value_handler,
	                                        NULL);

	// Set period for input value (channel 1) callback to 0.5s (500ms)
	tf_io4_v2_set_input_value_callback_configuration(&io, 1, 500, false);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
