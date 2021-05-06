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

# Get current value
value = io.get_value

puts "Channel 0: #{value[0]}"
puts "Channel 1: #{value[1]}"
puts "Channel 2: #{value[2]}"
puts "Channel 3: #{value[3]}"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
