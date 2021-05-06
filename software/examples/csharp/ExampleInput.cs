using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your IO-4 Bricklet 2.0

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletIO4V2 io = new BrickletIO4V2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Get current value
		bool[] value = io.GetValue();

		Console.WriteLine("Channel 0: " + value[0]);
		Console.WriteLine("Channel 1: " + value[1]);
		Console.WriteLine("Channel 2: " + value[2]);
		Console.WriteLine("Channel 3: " + value[3]);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
