use std::{error::Error, io, thread};
use tinkerforge::{io4_v2_bricklet::*, ip_connection::IpConnection};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your IO-4 Bricklet 2.0.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let io = Io4V2Bricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    let input_value_receiver = io.get_input_value_callback_receiver();

    // Spawn thread to handle received callback messages.
    // This thread ends when the `io` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for input_value in input_value_receiver {
            println!("Channel: {}", input_value.channel);
            println!("Changed: {}", input_value.changed);
            println!("Value: {}", input_value.value);
            println!();
        }
    });

    // Set period for input value (channel 1) callback to 0.5s (500ms).
    io.set_input_value_callback_configuration(1, 500, false);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
