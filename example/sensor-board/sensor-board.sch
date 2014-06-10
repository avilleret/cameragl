EESchema Schematic File Version 2
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
LIBS:special
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
LIBS:sensor-board-cache
EELAYER 24 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L R R2
U 1 1 537F5C6F
P 5100 4250
F 0 "R2" V 5180 4250 40  0000 C CNN
F 1 "1k" V 5107 4251 40  0000 C CNN
F 2 "" V 5030 4250 30  0000 C CNN
F 3 "" H 5100 4250 30  0000 C CNN
	1    5100 4250
	0    -1   -1   0   
$EndComp
$Comp
L R R1
U 1 1 537F5C7E
P 4750 3900
F 0 "R1" V 4830 3900 40  0000 C CNN
F 1 "PHOTO" V 4757 3901 40  0000 C CNN
F 2 "" V 4680 3900 30  0000 C CNN
F 3 "" H 4750 3900 30  0000 C CNN
	1    4750 3900
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR?
U 1 1 537F5CBB
P 4750 4900
F 0 "#PWR?" H 4750 4900 30  0001 C CNN
F 1 "GND" H 4750 4830 30  0001 C CNN
F 2 "" H 4750 4900 60  0000 C CNN
F 3 "" H 4750 4900 60  0000 C CNN
	1    4750 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 3550 4750 3650
Wire Wire Line
	4750 4150 4750 4350
Wire Wire Line
	4750 4250 4850 4250
Connection ~ 4750 4250
Wire Wire Line
	4750 4850 4750 4900
$Comp
L HE10-26 RPI_GPIO_P1
U 1 1 537F5F6E
P 6650 3400
F 0 "RPI_GPIO_P1" H 6650 4150 70  0000 C CNN
F 1 "HE10-26" H 6650 2750 70  0000 C CNN
F 2 "" H 6650 3400 60  0000 C CNN
F 3 "" H 6650 3400 60  0000 C CNN
	1    6650 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 3550 4750 3550
Wire Wire Line
	6050 3950 5600 3950
Wire Wire Line
	5600 3950 5600 4850
Wire Wire Line
	5600 4850 4750 4850
Wire Wire Line
	6050 3750 5450 3750
Wire Wire Line
	5450 3750 5450 4250
Wire Wire Line
	5450 4250 5350 4250
$Comp
L POT RV1
U 1 1 537F5FF9
P 4750 4600
F 0 "RV1" H 4750 4500 50  0000 C CNN
F 1 "POT" H 4750 4600 50  0000 C CNN
F 2 "" H 4750 4600 60  0000 C CNN
F 3 "" H 4750 4600 60  0000 C CNN
	1    4750 4600
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 4600 4950 4600
Wire Wire Line
	4950 4600 4950 4850
Connection ~ 4950 4850
Text Notes 4000 3900 0    60   ~ 0
photoresistor
Text Notes 3950 4650 0    60   ~ 0
potentiometer
Text Notes 6000 2500 0    60   ~ 0
Raspberry Pi GPIO Pin header\n
Text Label 5800 3550 0    60   ~ 0
3.3V
Text Label 5700 3750 0    60   ~ 0
GPIO 9
Text Label 5800 3950 0    60   ~ 0
GND
$EndSCHEMATC
