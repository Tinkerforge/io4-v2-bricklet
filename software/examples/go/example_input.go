package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/io4_v2_bricklet"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your IO-4 Bricklet 2.0.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	io, _ := io4_v2_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	// Get current value.
	value, _ := io.GetValue()

	fmt.Printf("Channel 0: \n", value[0])
	fmt.Printf("Channel 1: \n", value[1])
	fmt.Printf("Channel 2: \n", value[2])
	fmt.Printf("Channel 3: \n", value[3])

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
