#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your IO-4 Bricklet 2.0

import time

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_io4_v2 import BrickletIO4V2

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    io = BrickletIO4V2(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Configure channel 3 as output low
    io.set_configuration(3, "o", False)

    # Set channel 3 alternating high/low 10 times with 100 ms delay
    for i in range(10):
        time.sleep(0.1)
        io.set_selected_value(3, True)
        time.sleep(0.1)
        io.set_selected_value(3, False)

    input("Press key to exit\n") # Use raw_input() in Python 2
    ipcon.disconnect()
