<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletIO4V2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletIO4V2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your IO-4 Bricklet 2.0

$ipcon = new IPConnection(); // Create IP connection
$io = new BrickletIO4V2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Configure channel 3 as output low
$io->setConfiguration(3, 'o', FALSE);

// Set channel 3 alternating high/low 10 times with 100 ms delay
for($i = 0; $i < 10; $i++) {
    usleep(100*1000);
    $io->setSelectedValue(3, TRUE);
    usleep(100*1000);
    $io->setSelectedValue(3, FALSE);
}

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
