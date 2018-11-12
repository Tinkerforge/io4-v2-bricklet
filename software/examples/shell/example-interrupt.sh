#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your IO-4 Bricklet 2.0

# Handle incoming input value callbacks
tinkerforge dispatch io4-v2-bricklet $uid input-value &

# Set period for input value (channel 1) callback to 0.5s (500ms)
tinkerforge call io4-v2-bricklet $uid set-input-value-callback-configuration 1 500 false

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
