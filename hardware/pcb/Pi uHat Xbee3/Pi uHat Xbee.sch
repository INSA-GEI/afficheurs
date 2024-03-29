EESchema Schematic File Version 4
EELAYER 30 0
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
L Connector-ML:RPi_GPIO J2
U 1 1 5516AE26
P 7500 2700
F 0 "J2" H 8250 2950 60  0000 C CNN
F 1 "RPi_GPIO" H 8250 2850 60  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_2x20_P2.54mm_Vertical" H 7500 2700 60  0001 C CNN
F 3 "" H 7500 2700 60  0000 C CNN
	1    7500 2700
	1    0    0    -1  
$EndComp
Text Notes 7450 5000 0    100  Italic 0
Thru-Hole Connector
$Comp
L Insa:XBee3 U1
U 1 1 61B342D4
P 3900 3900
F 0 "U1" H 3925 5065 50  0000 C CNN
F 1 "XBee3" H 3925 4974 50  0000 C CNN
F 2 "Insa:Xbee3" H 3900 3700 50  0001 C CNN
F 3 "" H 3900 3700 50  0001 C CNN
	1    3900 3900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 61B34F64
P 9600 4800
F 0 "#PWR0101" H 9600 4550 50  0001 C CNN
F 1 "GND" H 9605 4627 50  0000 C CNN
F 2 "" H 9600 4800 50  0001 C CNN
F 3 "" H 9600 4800 50  0001 C CNN
	1    9600 4800
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR0102
U 1 1 61B35267
P 7100 2650
F 0 "#PWR0102" H 7100 2500 50  0001 C CNN
F 1 "VDD" H 7115 2823 50  0000 C CNN
F 2 "" H 7100 2650 50  0001 C CNN
F 3 "" H 7100 2650 50  0001 C CNN
	1    7100 2650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 61B3E78D
P 7100 4800
F 0 "#PWR0103" H 7100 4550 50  0001 C CNN
F 1 "GND" H 7105 4627 50  0000 C CNN
F 2 "" H 7100 4800 50  0001 C CNN
F 3 "" H 7100 4800 50  0001 C CNN
	1    7100 4800
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 61B3F06D
P 3200 4050
F 0 "#PWR0104" H 3200 3800 50  0001 C CNN
F 1 "GND" H 3205 3877 50  0000 C CNN
F 2 "" H 3200 4050 50  0001 C CNN
F 3 "" H 3200 4050 50  0001 C CNN
	1    3200 4050
	1    0    0    -1  
$EndComp
$Comp
L power:VDD #PWR0105
U 1 1 61B3FD9C
P 3200 2700
F 0 "#PWR0105" H 3200 2550 50  0001 C CNN
F 1 "VDD" H 3215 2873 50  0000 C CNN
F 2 "" H 3200 2700 50  0001 C CNN
F 3 "" H 3200 2700 50  0001 C CNN
	1    3200 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 2900 3200 3000
Wire Wire Line
	3200 3000 3350 3000
Wire Wire Line
	3350 3900 3200 3900
Wire Wire Line
	3200 3900 3200 4050
Wire Wire Line
	7300 2700 7100 2700
Wire Wire Line
	7100 2700 7100 2650
Wire Wire Line
	9200 3000 9950 3000
Wire Wire Line
	9200 3100 9950 3100
Wire Wire Line
	9200 4600 9600 4600
Wire Wire Line
	9600 4600 9600 4800
Wire Wire Line
	7300 4600 7100 4600
Wire Wire Line
	7100 4600 7100 4800
Wire Wire Line
	7300 3200 6750 3200
Wire Wire Line
	7300 3300 6750 3300
Wire Wire Line
	7300 3400 6750 3400
Wire Wire Line
	9200 3200 9950 3200
Wire Wire Line
	9200 3400 9950 3400
Wire Wire Line
	3350 3100 2750 3100
Wire Wire Line
	3350 3200 2750 3200
Wire Wire Line
	3350 3400 2750 3400
Wire Wire Line
	3350 3800 2750 3800
Wire Wire Line
	4500 3500 5200 3500
Wire Wire Line
	4500 3700 5200 3700
Wire Wire Line
	3350 3500 2750 3500
Text Label 2750 3100 0    50   ~ 0
DOUT
Text Label 2750 3200 0    50   ~ 0
DIN
Text Label 2750 3400 0    50   ~ 0
~RESET
Text Label 2750 3500 0    50   ~ 0
RSSI_PWM
Text Label 2750 3800 0    50   ~ 0
SLEEP_REQ
Text Label 5200 3700 2    50   ~ 0
~SLEEP
Text Label 5200 3500 2    50   ~ 0
ASSOCIATE
Text Label 9950 3100 2    50   ~ 0
DOUT
Text Label 9950 3000 2    50   ~ 0
DIN
Text Label 6750 3200 0    50   ~ 0
SLEEP_REQ
Text Label 6750 3300 0    50   ~ 0
~SLEEP
Text Label 6750 3400 0    50   ~ 0
~RESET
Text Label 9950 3200 2    50   ~ 0
RSSI_PWM
Text Label 9950 3400 2    50   ~ 0
ASSOCIATE
Wire Wire Line
	7300 3100 7100 3100
Wire Wire Line
	7100 3100 7100 3900
Connection ~ 7100 4600
Wire Wire Line
	7300 3900 7100 3900
Connection ~ 7100 3900
Wire Wire Line
	7100 3900 7100 4600
Wire Wire Line
	9200 4100 9600 4100
Wire Wire Line
	9600 4100 9600 4300
Connection ~ 9600 4600
Wire Wire Line
	9200 4300 9600 4300
Connection ~ 9600 4300
Wire Wire Line
	9600 4300 9600 4600
Wire Wire Line
	9200 3600 9600 3600
Wire Wire Line
	9600 3600 9600 4100
Connection ~ 9600 4100
Wire Wire Line
	9200 3300 9600 3300
Connection ~ 9600 3600
Wire Wire Line
	9200 2900 9600 2900
Wire Wire Line
	9600 2900 9600 3300
Connection ~ 9600 3300
Wire Wire Line
	9600 3300 9600 3600
Wire Wire Line
	4500 3000 4650 3000
$Comp
L power:GND #PWR0106
U 1 1 61B4FC71
P 4650 4050
F 0 "#PWR0106" H 4650 3800 50  0001 C CNN
F 1 "GND" H 4655 3877 50  0000 C CNN
F 2 "" H 4650 4050 50  0001 C CNN
F 3 "" H 4650 4050 50  0001 C CNN
	1    4650 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4650 3000 4650 3100
Wire Wire Line
	4500 3100 4650 3100
Connection ~ 4650 3100
Wire Wire Line
	4650 3100 4650 3200
Wire Wire Line
	4500 3200 4650 3200
Connection ~ 4650 3200
Wire Wire Line
	4650 3200 4650 3300
Wire Wire Line
	4500 3300 4650 3300
Connection ~ 4650 3300
Wire Wire Line
	4650 3300 4650 4050
$Comp
L Device:LED D2
U 1 1 61B533C3
P 6150 5350
F 0 "D2" V 6189 5232 50  0000 R CNN
F 1 "LED" V 6098 5232 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 6150 5350 50  0001 C CNN
F 3 "~" H 6150 5350 50  0001 C CNN
	1    6150 5350
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 61B53DEB
P 5750 4950
F 0 "R1" H 5820 4996 50  0000 L CNN
F 1 "R" H 5820 4905 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 5680 4950 50  0001 C CNN
F 3 "~" H 5750 4950 50  0001 C CNN
	1    5750 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 61B542CC
P 2300 3350
F 0 "C1" H 2415 3396 50  0000 L CNN
F 1 "100nF" H 2415 3305 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 2338 3200 50  0001 C CNN
F 3 "~" H 2300 3350 50  0001 C CNN
	1    2300 3350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 61B5506B
P 3000 2900
F 0 "J1" H 3080 2846 50  0000 L CNN
F 1 "Conn_01x02" H 3080 2801 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3000 2900 50  0001 C CNN
F 3 "~" H 3000 2900 50  0001 C CNN
	1    3000 2900
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 61B556A3
P 3150 5550
F 0 "J3" V 3068 5630 50  0000 L CNN
F 1 "Conn_01x01" H 3230 5501 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 3150 5550 50  0001 C CNN
F 3 "~" H 3150 5550 50  0001 C CNN
	1    3150 5550
	0    1    1    0   
$EndComp
Wire Wire Line
	3200 2700 3200 2800
Wire Wire Line
	3200 3000 2300 3000
Wire Wire Line
	2300 3000 2300 3200
Connection ~ 3200 3000
Wire Wire Line
	3200 3900 2300 3900
Wire Wire Line
	2300 3900 2300 3500
Connection ~ 3200 3900
$Comp
L Device:LED D3
U 1 1 61B7432D
P 6550 5350
F 0 "D3" V 6589 5232 50  0000 R CNN
F 1 "LED" V 6498 5232 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 6550 5350 50  0001 C CNN
F 3 "~" H 6550 5350 50  0001 C CNN
	1    6550 5350
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D1
U 1 1 61B756AF
P 5750 5350
F 0 "D1" V 5789 5232 50  0000 R CNN
F 1 "LED" V 5698 5232 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 5750 5350 50  0001 C CNN
F 3 "~" H 5750 5350 50  0001 C CNN
	1    5750 5350
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 61B7BE20
P 6150 4950
F 0 "R2" H 6220 4996 50  0000 L CNN
F 1 "R" H 6220 4905 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 6080 4950 50  0001 C CNN
F 3 "~" H 6150 4950 50  0001 C CNN
	1    6150 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 61B7CD3C
P 6550 4950
F 0 "R3" H 6620 4996 50  0000 L CNN
F 1 "R" H 6620 4905 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 6480 4950 50  0001 C CNN
F 3 "~" H 6550 4950 50  0001 C CNN
	1    6550 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 4100 5750 4100
Wire Wire Line
	7300 4200 6150 4200
Wire Wire Line
	7300 4300 6550 4300
Wire Wire Line
	6550 4300 6550 4800
Wire Wire Line
	6150 4200 6150 4800
Wire Wire Line
	5750 4100 5750 4800
Wire Wire Line
	5750 5100 5750 5200
Wire Wire Line
	6150 5100 6150 5200
Wire Wire Line
	6550 5100 6550 5200
Wire Wire Line
	6550 5500 6550 5700
Wire Wire Line
	6550 5700 6150 5700
Wire Wire Line
	5750 5700 5750 5500
Wire Wire Line
	6150 5500 6150 5700
Connection ~ 6150 5700
Wire Wire Line
	6150 5700 5750 5700
Wire Wire Line
	6150 5700 6150 5800
$Comp
L power:GND #PWR0107
U 1 1 61B90BC2
P 6150 5800
F 0 "#PWR0107" H 6150 5550 50  0001 C CNN
F 1 "GND" H 6155 5627 50  0000 C CNN
F 2 "" H 6150 5800 50  0001 C CNN
F 3 "" H 6150 5800 50  0001 C CNN
	1    6150 5800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 61B9900B
P 3400 5550
F 0 "J4" V 3318 5630 50  0000 L CNN
F 1 "Conn_01x01" H 3480 5501 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 3400 5550 50  0001 C CNN
F 3 "~" H 3400 5550 50  0001 C CNN
	1    3400 5550
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 61B9A695
P 3650 5550
F 0 "J5" V 3568 5630 50  0000 L CNN
F 1 "Conn_01x01" H 3730 5501 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 3650 5550 50  0001 C CNN
F 3 "~" H 3650 5550 50  0001 C CNN
	1    3650 5550
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J6
U 1 1 61B9BE07
P 3900 5550
F 0 "J6" V 3818 5630 50  0000 L CNN
F 1 "Conn_01x01" H 3980 5501 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 3900 5550 50  0001 C CNN
F 3 "~" H 3900 5550 50  0001 C CNN
	1    3900 5550
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 61B9D437
P 4150 5550
F 0 "J7" V 4068 5630 50  0000 L CNN
F 1 "Conn_01x01" H 4230 5501 50  0001 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 4150 5550 50  0001 C CNN
F 3 "~" H 4150 5550 50  0001 C CNN
	1    4150 5550
	0    1    1    0   
$EndComp
Wire Wire Line
	3150 5350 3150 4650
Wire Wire Line
	3400 5350 3400 4650
Wire Wire Line
	3650 5350 3650 4650
Wire Wire Line
	3900 5350 3900 4650
Wire Wire Line
	4150 5350 4150 4650
Text Label 3150 4650 3    50   ~ 0
~SLEEP
Text Label 3400 4650 3    50   ~ 0
SLEEP_REQ
Text Label 3650 4650 3    50   ~ 0
RSSI_PWM
Text Label 3900 4650 3    50   ~ 0
ASSOCIATE
Text Label 4150 4650 3    50   ~ 0
~RESET
$EndSCHEMATC
