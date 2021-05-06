#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your IO-4 Bricklet 2.0

# Get current value
tinkerforge call io4-v2-bricklet $uid get-value
