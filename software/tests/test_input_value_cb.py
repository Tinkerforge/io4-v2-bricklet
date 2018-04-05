#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "XYZ" # Change XYZ to the UID of your IO-4 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_io4_v2 import BrickletIO4V2

def cb_input_value(channel, changed, value):
    print "[+] INPUT VALUE CHANGED CB: CH =", channel, \
          "CHANGED =", changed, \
          "VALUE =", value, "\n"

def cb_all_input_value(changed, value):
    print "[+] ALL INPUT VALUE CHANGED CB: CHANGED =", changed, \
          "VALUE =", value, "\n"

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    io = BrickletIO4V2(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Configure all the channels as input no pull-up
    io.set_configuration(0, io.DIRECTION_IN, False)
    io.set_configuration(1, io.DIRECTION_IN, False)
    io.set_configuration(2, io.DIRECTION_IN, False)
    io.set_configuration(3, io.DIRECTION_IN, False)

    # Set input value callback configuration (value has to change)
    for i in range(0, 4):
        io.set_input_value_callback_configuration(i, 100, True)

    io.set_all_input_value_callback_configuration(100, True)
    
    # Get input value callback configuration
    for i in range(0, 4):
        print "[+] CH =", i, \
              " CONFIG =", io.get_input_value_callback_configuration(i)

    print "[+] ALL CH CONFIG =", io.get_all_input_value_callback_configuration(), "\n"

    # Register callbacks
    io.register_callback(io.CALLBACK_INPUT_VALUE, cb_input_value)
    io.register_callback(io.CALLBACK_ALL_INPUT_VALUE, cb_all_input_value)

    raw_input("Press key to exit\n") # Use input() in Python 3
    ipcon.disconnect()
