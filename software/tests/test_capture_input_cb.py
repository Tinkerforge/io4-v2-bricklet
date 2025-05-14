#!/usr/bin/env python
# -*- coding: utf-8 -*-

HOST = "localhost"
PORT = 4223
UID = "27kd" # Change XYZ to the UID of your IO-4 Bricklet

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_io4_v2 import BrickletIO4V2
import time
import matplotlib.pyplot as plt

data_channel = [[], [], [], []]

def cb_capture_input(data):
    # Save data.
    # The IO-4 input capture is encoded as 2 values with 4 bits each.
    for d in data:
        for ch in range(4):
            data_channel[ch].append((d >> (ch + 0)) & 1)
            data_channel[ch].append((d >> (ch + 4)) & 1)

if __name__ == "__main__":
    ipcon = IPConnection() # Create IP connection
    io = BrickletIO4V2(UID, ipcon) # Create device object

    ipcon.connect(HOST, PORT) # Connect to brickd
    # Don't use device before ipcon is connected

    # Register callbacks
    io.register_callback(io.CALLBACK_CAPTURE_INPUT, cb_capture_input)

    # Configure capture input callback
    # and capture data for 50ms with a frequency of 40kHz (time between capture = 25us)
    io.set_capture_input_callback_configuration(True, 25)
    time.sleep(0.05)
    io.set_capture_input_callback_configuration(False, 25)

    print("Captures {} samples".format(len(data_channel[0])))

    # Plot channel 0 and 1
    plt.figure(figsize=(10, 6))

    for ch in range(2):
        plt.step(range(len(data_channel[ch])), data_channel[ch], where='post', label=f'Channel {ch}')

    plt.xlabel('Sample')
    plt.ylabel('State')
    plt.title('IO-4 Input Channels')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()