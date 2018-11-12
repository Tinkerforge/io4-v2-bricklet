import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletIO4V2;

public class ExampleOutput {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your IO-4 Bricklet 2.0
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletIO4V2 io = new BrickletIO4V2(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Configure channel 3 as output low
		io.setConfiguration(3, 'o', false);

		// Set channel 3 alternating high/low 10 times with 100 ms delay
		for(int i = 0; i < 10; i++) {
			Thread.sleep(100);
			io.setSelectedValue(3, true);
			Thread.sleep(100);
			io.setSelectedValue(3, false);
		}

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
