function octave_example_input()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your IO-4 Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    io = javaObject("com.tinkerforge.BrickletIO4V2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current value
    value = io.getValue();

    fprintf("Channel 0: %d\n", value(1));
    fprintf("Channel 1: %d\n", value(2));
    fprintf("Channel 2: %d\n", value(3));
    fprintf("Channel 3: %d\n", value(4));

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
