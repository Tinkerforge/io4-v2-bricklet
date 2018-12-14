use std::{io, error::Error};
use std::thread;
use std::time::Duration;
use tinkerforge::{ip_connection::IpConnection, 
                  io4_v2_bricklet::*};


const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your IO-4 Bricklet 2.0.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let io = Io4V2Bricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
    // Don't use device before ipcon is connected.

		// Configure channel 3 as output low
		io.set_configuration(3, 'o', false);

		// Set channel 3 alternating high/low 10 times with 100 ms delay
		for i in 0..10{
			thread::sleep(Duration::from_millis(100));
			io.set_selected_value(3, true);
			thread::sleep(Duration::from_millis(100));
			io.set_selected_value(3, false);
		}

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}