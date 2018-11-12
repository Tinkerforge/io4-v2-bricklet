#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_io4_v2'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your IO-4 Bricklet 2.0

ipcon = IPConnection.new # Create IP connection
io = BrickletIO4V2.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Configure channel 3 as output low
io.set_configuration 3, 'o', false

# Set channel 3 alternating high/low 10 times with 100 ms delay
for _ in 0..9
  sleep 0.1
  io.set_selected_value 3, true
  sleep 0.1
  io.set_selected_value 3, false
end

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
