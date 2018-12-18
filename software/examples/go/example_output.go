package main

import (
	"fmt"
	"github.com/tinkerforge/go-api-bindings/io4_v2_bricklet"
	"github.com/tinkerforge/go-api-bindings/ipconnection"
	"time"
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

	// Configure channel 3 as output low
	io.SetConfiguration(3, 'o', false)

	// Set channel 3 alternating high/low 10 times with 100 ms delay
	for i := 0; i < 10; i++ {
		time.Sleep(100 * time.Millisecond)
		io.SetSelectedValue(3, true)
		time.Sleep(100 * time.Millisecond)
		io.SetSelectedValue(3, false)
	}

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
