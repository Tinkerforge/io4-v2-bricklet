<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletIO4V2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletIO4V2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your IO-4 Bricklet 2.0

// Callback function for input value callback
function cb_inputValue($channel, $changed, $value)
{
    echo "Channel: $channel\n";
    echo "Changed: $changed\n";
    echo "Value: $value\n";
    echo "\n";
}

$ipcon = new IPConnection(); // Create IP connection
$io = new BrickletIO4V2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register input value callback to function cb_inputValue
$io->registerCallback(BrickletIO4V2::CALLBACK_INPUT_VALUE, 'cb_inputValue');

// Set period for input value (channel 1) callback to 0.5s (500ms)
$io->setInputValueCallbackConfiguration(1, 500, FALSE);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
