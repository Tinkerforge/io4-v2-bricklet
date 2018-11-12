function matlab_example_output()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletIO4V2;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your IO-4 Bricklet 2.0

    ipcon = IPConnection(); % Create IP connection
    io = handle(BrickletIO4V2(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Configure channel 3 as output low
    io.setConfiguration(3, 'o', false);

    % Set channel 3 alternating high/low 10 times with 100 ms delay
    for i = 0:9
        pause(0.1);
        io.setSelectedValue(3, true);
        pause(0.1);
        io.setSelectedValue(3, false);
    end

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end
