EESchema Schematic File Version 2
LIBS:tinkerforge
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "IO4 Bricklet 2.0"
Date "2018-01-16"
Rev "1.0"
Comp "Tinkerforge GmbH"
Comment1 "Licensed under CERN OHL v.1.1"
Comment2 "Copyright (©) 2018, L.Lauer <lukas@tinkerforge.com>"
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	8250 3750 8350 3750
Wire Wire Line
	8250 3550 8500 3550
Wire Wire Line
	7600 3450 7850 3450
Wire Wire Line
	7600 3650 7850 3650
Connection ~ 8850 3550
Wire Wire Line
	9050 3350 8850 3350
Wire Wire Line
	8850 3350 8850 4050
Connection ~ 8850 3850
Wire Wire Line
	8850 3650 9050 3650
Wire Wire Line
	8500 3750 8500 3650
Wire Wire Line
	8500 3750 9050 3750
Wire Wire Line
	9050 3450 8500 3450
Wire Wire Line
	8500 3450 8500 3550
Wire Wire Line
	10150 3750 10150 3700
Wire Wire Line
	10150 3600 10150 3500
Wire Wire Line
	8350 3250 8350 3450
Wire Wire Line
	8350 3250 9050 3250
Wire Wire Line
	8350 3750 8350 3950
Wire Wire Line
	8350 3950 9050 3950
Wire Wire Line
	8850 3850 9050 3850
Wire Wire Line
	8850 3550 9050 3550
Connection ~ 8850 3650
Wire Wire Line
	7600 3750 7850 3750
Wire Wire Line
	7850 3550 7600 3550
Wire Wire Line
	8350 3450 8250 3450
Wire Wire Line
	8500 3650 8250 3650
$Comp
L R_PACK4 RP1
U 1 1 4EFC46A3
P 8050 3800
F 0 "RP1" H 8050 4250 40  0000 C CNN
F 1 "82" H 8050 3750 40  0000 C CNN
F 2 "4X0402" H 8050 3800 60  0001 C CNN
F 3 "" H 8050 3800 60  0001 C CNN
	1    8050 3800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 4EFC205A
P 8850 4050
F 0 "#PWR01" H 8850 4050 30  0001 C CNN
F 1 "GND" H 8850 3980 30  0001 C CNN
F 2 "" H 8850 4050 60  0001 C CNN
F 3 "" H 8850 4050 60  0001 C CNN
	1    8850 4050
	1    0    0    -1  
$EndComp
$Comp
L AKL_5_2 J3
U 1 1 4EFC2031
P 9250 3700
F 0 "J3" H 9350 3695 60  0000 C CNN
F 1 "AKL_5_2" H 9600 3700 60  0000 C CNN
F 2 "AKL_5_2" H 9250 3700 60  0001 C CNN
F 3 "" H 9250 3700 60  0001 C CNN
	1    9250 3700
	1    0    0    -1  
$EndComp
$Comp
L AKL_5_2 J4
U 1 1 4EFC2030
P 9250 3900
F 0 "J4" H 9350 3895 60  0000 C CNN
F 1 "AKL_5_2" H 9600 3900 60  0000 C CNN
F 2 "AKL_5_2" H 9250 3900 60  0001 C CNN
F 3 "" H 9250 3900 60  0001 C CNN
	1    9250 3900
	1    0    0    -1  
$EndComp
Text Notes 550  7700 0    40   ~ 0
Copyright Tinkerforge GmbH 2018.\nThis documentation describes Open Hardware and is licensed under the\nCERN OHL v. 1.1.\nYou may redistribute and modify this documentation under the terms of the\nCERN OHL v.1.1. (http://ohwr.org/cernohl). This documentation is distributed\nWITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF\nMERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A\nPARTICULAR PURPOSE. Please see the CERN OHL v.1.1 for applicable\nconditions\n
$Comp
L GND #PWR02
U 1 1 4D00FFDA
P 10150 3750
F 0 "#PWR02" H 10150 3750 30  0001 C CNN
F 1 "GND" H 10150 3680 30  0001 C CNN
F 2 "" H 10150 3750 60  0001 C CNN
F 3 "" H 10150 3750 60  0001 C CNN
	1    10150 3750
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR03
U 1 1 4D00FFD7
P 10150 3500
F 0 "#PWR03" H 10150 3600 30  0001 C CNN
F 1 "VCC" H 10150 3600 30  0000 C CNN
F 2 "" H 10150 3500 60  0001 C CNN
F 3 "" H 10150 3500 60  0001 C CNN
	1    10150 3500
	1    0    0    -1  
$EndComp
$Comp
L AKL_5_2 J5
U 1 1 4D00FFD4
P 10350 3650
F 0 "J5" H 10450 3645 60  0000 C CNN
F 1 "AKL_5_2" H 10700 3650 60  0000 C CNN
F 2 "AKL_5_2" H 10350 3650 60  0001 C CNN
F 3 "" H 10350 3650 60  0001 C CNN
	1    10350 3650
	1    0    0    1   
$EndComp
$Comp
L AKL_5_2 J2
U 1 1 4D00FFBF
P 9250 3500
F 0 "J2" H 9350 3495 60  0000 C CNN
F 1 "AKL_5_2" H 9600 3500 60  0000 C CNN
F 2 "AKL_5_2" H 9250 3500 60  0001 C CNN
F 3 "" H 9250 3500 60  0001 C CNN
	1    9250 3500
	1    0    0    -1  
$EndComp
$Comp
L AKL_5_2 J1
U 1 1 4D00FFB7
P 9250 3300
F 0 "J1" H 9350 3295 60  0000 C CNN
F 1 "AKL_5_2" H 9600 3300 60  0000 C CNN
F 2 "AKL_5_2" H 9250 3300 60  0001 C CNN
F 3 "" H 9250 3300 60  0001 C CNN
	1    9250 3300
	1    0    0    -1  
$EndComp
$Comp
L DRILL U2
U 1 1 4C6050A5
P 10650 6150
F 0 "U2" H 10700 6200 60  0001 C CNN
F 1 "DRILL" H 10650 6150 60  0000 C CNN
F 2 "DRILL_NP" H 10650 6150 60  0001 C CNN
F 3 "" H 10650 6150 60  0001 C CNN
	1    10650 6150
	1    0    0    -1  
$EndComp
$Comp
L DRILL U3
U 1 1 4C6050A2
P 10650 6350
F 0 "U3" H 10700 6400 60  0001 C CNN
F 1 "DRILL" H 10650 6350 60  0000 C CNN
F 2 "DRILL_NP" H 10650 6350 60  0001 C CNN
F 3 "" H 10650 6350 60  0001 C CNN
	1    10650 6350
	1    0    0    -1  
$EndComp
$Comp
L DRILL U5
U 1 1 4C60509F
P 11000 6350
F 0 "U5" H 11050 6400 60  0001 C CNN
F 1 "DRILL" H 11000 6350 60  0000 C CNN
F 2 "DRILL_NP" H 11000 6350 60  0001 C CNN
F 3 "" H 11000 6350 60  0001 C CNN
	1    11000 6350
	1    0    0    -1  
$EndComp
$Comp
L DRILL U4
U 1 1 4C605099
P 11000 6150
F 0 "U4" H 11050 6200 60  0001 C CNN
F 1 "DRILL" H 11000 6150 60  0000 C CNN
F 2 "DRILL_NP" H 11000 6150 60  0001 C CNN
F 3 "" H 11000 6150 60  0001 C CNN
	1    11000 6150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR04
U 1 1 4C5FCFB4
P 2050 700
F 0 "#PWR04" H 2050 800 30  0001 C CNN
F 1 "VCC" H 2050 800 30  0000 C CNN
F 2 "" H 2050 700 60  0001 C CNN
F 3 "" H 2050 700 60  0001 C CNN
	1    2050 700 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 4C5FCF4F
P 1450 2300
F 0 "#PWR05" H 1450 2300 30  0001 C CNN
F 1 "GND" H 1450 2230 30  0001 C CNN
F 2 "" H 1450 2300 60  0001 C CNN
F 3 "" H 1450 2300 60  0001 C CNN
	1    1450 2300
	1    0    0    -1  
$EndComp
$Comp
L CON-SENSOR2 P1
U 1 1 4C5FCF27
P 950 1500
F 0 "P1" H 1100 1900 60  0000 C CNN
F 1 "CON-SENSOR2" V 1100 1500 60  0000 C CNN
F 2 "CON-SENSOR2" H 950 1500 60  0001 C CNN
F 3 "" H 950 1500 60  0001 C CNN
	1    950  1500
	-1   0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 5A5E1F91
P 1600 1000
F 0 "C2" H 1650 1100 50  0000 L CNN
F 1 "10uF" H 1650 900 50  0000 L CNN
F 2 "C0805" H 1600 1000 60  0001 C CNN
F 3 "" H 1600 1000 60  0000 C CNN
	1    1600 1000
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 5A5E21B0
P 2050 1000
F 0 "C3" H 2100 1100 50  0000 L CNN
F 1 "1uF" H 2100 900 50  0000 L CNN
F 2 "C0603F" H 2050 1000 60  0001 C CNN
F 3 "" H 2050 1000 60  0000 C CNN
	1    2050 1000
	1    0    0    -1  
$EndComp
$Comp
L R_PACK4 RP2
U 1 1 5A5E23BB
P 1800 1850
F 0 "RP2" H 1800 2300 50  0000 C CNN
F 1 "82" H 1800 1800 50  0000 C CNN
F 2 "4X0402" H 1800 1850 50  0001 C CNN
F 3 "" H 1800 1850 50  0000 C CNN
	1    1800 1850
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 5A5E2444
P 1450 2050
F 0 "C1" H 1500 2150 50  0000 L CNN
F 1 "220pF" H 1500 1950 50  0000 L CNN
F 2 "C0402E" H 1450 2050 60  0001 C CNN
F 3 "" H 1450 2050 60  0000 C CNN
	1    1450 2050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 5A5E272B
P 950 2300
F 0 "#PWR06" H 950 2300 30  0001 C CNN
F 1 "GND" H 950 2230 30  0001 C CNN
F 2 "" H 950 2300 60  0001 C CNN
F 3 "" H 950 2300 60  0001 C CNN
	1    950  2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  1950 950  2300
Wire Wire Line
	1450 2300 1450 2250
Wire Wire Line
	1300 1800 1600 1800
Wire Wire Line
	1600 1700 1300 1700
Wire Wire Line
	1300 1600 1600 1600
Wire Wire Line
	1600 1500 1300 1500
Wire Wire Line
	1450 1850 1450 1800
Connection ~ 1450 1800
Wire Wire Line
	1300 1300 2050 1300
Wire Wire Line
	2050 1200 2050 1350
Wire Wire Line
	1300 1400 1400 1400
Wire Wire Line
	1400 1400 1400 750 
Wire Wire Line
	1400 750  2050 750 
Wire Wire Line
	2050 700  2050 800 
Connection ~ 2050 750 
Wire Wire Line
	1600 800  1600 750 
Connection ~ 1600 750 
Wire Wire Line
	1600 1200 1600 1300
Connection ~ 1600 1300
Connection ~ 2050 1300
$Comp
L GND #PWR07
U 1 1 5A5E2C77
P 2050 1350
F 0 "#PWR07" H 2050 1350 30  0001 C CNN
F 1 "GND" H 2050 1280 30  0001 C CNN
F 2 "" H 2050 1350 60  0001 C CNN
F 3 "" H 2050 1350 60  0001 C CNN
	1    2050 1350
	1    0    0    -1  
$EndComp
NoConn ~ 1300 1200
Text GLabel 2250 1500 2    47   Output ~ 0
S-CS
Text GLabel 2250 1600 2    47   Output ~ 0
S-CLK
Text GLabel 2250 1700 2    47   Output ~ 0
S-MOSI
Text GLabel 2250 1800 2    47   Input ~ 0
S-MISO
Wire Wire Line
	2000 1500 2250 1500
Wire Wire Line
	2250 1600 2000 1600
Wire Wire Line
	2000 1700 2250 1700
Wire Wire Line
	2250 1800 2000 1800
Text GLabel 7600 3450 0    47   BiDi ~ 0
IO-1
Text GLabel 7600 3550 0    47   BiDi ~ 0
IO-2
Text GLabel 7600 3650 0    47   BiDi ~ 0
IO-3
Text GLabel 7600 3750 0    47   BiDi ~ 0
IO-4
$Comp
L XMC1XXX24 U1
U 3 1 5A5E3F55
P 5100 3850
F 0 "U1" H 4950 4100 60  0000 C CNN
F 1 "XMC1100" H 5100 3600 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5250 4600 60  0001 C CNN
F 3 "" H 5250 4600 60  0000 C CNN
	3    5100 3850
	1    0    0    -1  
$EndComp
$Comp
L XMC1XXX24 U1
U 2 1 5A5E404E
P 5100 2550
F 0 "U1" H 4950 3100 60  0000 C CNN
F 1 "XMC1100" H 5100 2000 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5250 3300 60  0001 C CNN
F 3 "" H 5250 3300 60  0000 C CNN
	2    5100 2550
	1    0    0    -1  
$EndComp
$Comp
L XMC1XXX24 U1
U 4 1 5A5E4099
P 5100 5150
F 0 "U1" H 4950 5600 60  0000 C CNN
F 1 "XMC1100" H 5100 4700 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5250 5900 60  0001 C CNN
F 3 "" H 5250 5900 60  0000 C CNN
	4    5100 5150
	1    0    0    -1  
$EndComp
$Comp
L XMC1XXX24 U1
U 1 1 5A5E4104
P 5100 6550
F 0 "U1" H 4950 6950 60  0000 C CNN
F 1 "XMC1100" H 5100 6150 60  0000 C CNN
F 2 "kicad-libraries:QFN24-4x4mm-0.5mm" H 5250 7300 60  0001 C CNN
F 3 "" H 5250 7300 60  0000 C CNN
	1    5100 6550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P2
U 1 1 5A5E523D
P 4250 2100
F 0 "P2" H 4250 2200 50  0000 C CNN
F 1 "DEBUG" V 4350 2100 50  0000 C CNN
F 2 "kicad-libraries:DEBUG_PAD" H 4250 2100 50  0001 C CNN
F 3 "" H 4250 2100 50  0000 C CNN
	1    4250 2100
	-1   0    0    1   
$EndComp
Text Notes 3050 3050 0    39   ~ 0
SPI Slave/CH0\nP0.13 : USIC0_CH0-DX2F : SEL\nP0.14 : USIC0_CH0-DX1A : CLK\nP0.15 : USIC0_CH0-DX0B : MOSI\nP2.0 : USIC0_CH0-DOUT0 : MISO
Text GLabel 4550 2800 0    47   Input ~ 0
S-CS
Text GLabel 4550 2900 0    47   Input ~ 0
S-CLK
Text GLabel 4550 3000 0    47   Input ~ 0
S-MOSI
Text GLabel 4550 4800 0    47   Output ~ 0
S-MISO
Wire Wire Line
	4450 2100 4750 2100
Wire Wire Line
	4550 2800 4750 2800
Wire Wire Line
	4750 2900 4550 2900
Wire Wire Line
	4550 3000 4750 3000
Wire Wire Line
	4750 4800 4550 4800
Text GLabel 4550 3700 0    47   BiDi ~ 0
IO-1
Text GLabel 4550 3800 0    47   BiDi ~ 0
IO-2
Text GLabel 4550 3900 0    47   BiDi ~ 0
IO-3
Text GLabel 4550 4000 0    47   BiDi ~ 0
IO-4
Wire Wire Line
	4550 3700 4750 3700
Wire Wire Line
	4750 3800 4550 3800
Wire Wire Line
	4550 3900 4750 3900
Wire Wire Line
	4750 4000 4550 4000
$Comp
L C C4
U 1 1 5A5E7A6C
P 4600 6500
F 0 "C4" H 4600 6600 50  0000 L CNN
F 1 "100nF" H 4600 6400 50  0000 L CNN
F 2 "C0603F" H 4600 6500 60  0001 C CNN
F 3 "" H 4600 6500 60  0000 C CNN
	1    4600 6500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 6200 4600 6300
Wire Wire Line
	4600 6250 4750 6250
Wire Wire Line
	4600 6700 4600 6900
Wire Wire Line
	4600 6850 4750 6850
Wire Wire Line
	4750 6750 4600 6750
Connection ~ 4600 6750
Connection ~ 4600 6250
Connection ~ 4600 6850
$Comp
L GND #PWR08
U 1 1 5A5E865A
P 4600 6900
F 0 "#PWR08" H 4600 6900 30  0001 C CNN
F 1 "GND" H 4600 6830 30  0001 C CNN
F 2 "" H 4600 6900 60  0001 C CNN
F 3 "" H 4600 6900 60  0001 C CNN
	1    4600 6900
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR09
U 1 1 5A5E88C9
P 4600 6200
F 0 "#PWR09" H 4600 6300 30  0001 C CNN
F 1 "VCC" H 4600 6300 30  0000 C CNN
F 2 "" H 4600 6200 60  0001 C CNN
F 3 "" H 4600 6200 60  0001 C CNN
	1    4600 6200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P3
U 1 1 5A5E9354
P 4350 5150
F 0 "P3" H 4350 5300 50  0000 C CNN
F 1 "BOOT" V 4450 5150 50  0000 C CNN
F 2 "SolderJumper" H 4350 5150 50  0001 C CNN
F 3 "" H 4350 5150 50  0000 C CNN
	1    4350 5150
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR010
U 1 1 5A5E9551
P 4600 5250
F 0 "#PWR010" H 4600 5250 30  0001 C CNN
F 1 "GND" H 4600 5180 30  0001 C CNN
F 2 "" H 4600 5250 60  0001 C CNN
F 3 "" H 4600 5250 60  0001 C CNN
	1    4600 5250
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 5100 4550 5100
Wire Wire Line
	4550 5200 4600 5200
Wire Wire Line
	4600 5200 4600 5250
$Comp
L R R1
U 1 1 5A5E9B66
P 4400 5400
F 0 "R1" V 4400 5300 50  0000 C CNN
F 1 "1k" V 4400 5500 50  0000 C CNN
F 2 "R0603F" H 4400 5400 60  0001 C CNN
F 3 "" H 4400 5400 60  0000 C CNN
	1    4400 5400
	0    1    1    0   
$EndComp
$Comp
L LED D1
U 1 1 5A5E9D01
P 3850 5400
F 0 "D1" H 3850 5500 50  0000 C CNN
F 1 "blue" H 3850 5300 50  0000 C CNN
F 2 "D0603F" H 3850 5400 50  0001 C CNN
F 3 "" H 3850 5400 50  0000 C CNN
	1    3850 5400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR011
U 1 1 5A5E9EEE
P 3550 5300
F 0 "#PWR011" H 3550 5400 30  0001 C CNN
F 1 "VCC" H 3550 5400 30  0000 C CNN
F 2 "" H 3550 5300 60  0001 C CNN
F 3 "" H 3550 5300 60  0001 C CNN
	1    3550 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 5400 4650 5400
Wire Wire Line
	4150 5400 4050 5400
Wire Wire Line
	3650 5400 3550 5400
Wire Wire Line
	3550 5400 3550 5300
NoConn ~ 4750 5500
NoConn ~ 4750 5300
NoConn ~ 4750 5200
NoConn ~ 4750 5000
NoConn ~ 4750 4900
NoConn ~ 4750 2700
NoConn ~ 4750 2600
NoConn ~ 4750 2500
NoConn ~ 4750 2400
NoConn ~ 4750 2300
NoConn ~ 4750 2200
$EndSCHEMATC