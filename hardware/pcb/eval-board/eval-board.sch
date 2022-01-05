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
L Connector_Generic:Conn_01x24 J2
U 1 1 61C3CA0E
P 10100 2050
F 0 "J2" H 10180 2042 50  0000 L CNN
F 1 "Conn_01x24" H 10180 1951 50  0001 L CNN
F 2 "Connector_FFC-FPC:Hirose_FH12-24S-0.5SH_1x24-1MP_P0.50mm_Horizontal" H 10100 2050 50  0001 C CNN
F 3 "~" H 10100 2050 50  0001 C CNN
	1    10100 2050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 61C3ECA6
P 1000 1450
F 0 "J1" H 918 1125 50  0000 C CNN
F 1 "Conn_01x02" H 918 1216 50  0001 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1000 1450 50  0001 C CNN
F 3 "~" H 1000 1450 50  0001 C CNN
	1    1000 1450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	9900 1050 9350 1050
Wire Wire Line
	9900 1750 9350 1750
Wire Wire Line
	9900 1850 9350 1850
Wire Wire Line
	9900 1950 9350 1950
Wire Wire Line
	9900 2050 9350 2050
Wire Wire Line
	9900 2150 9350 2150
Wire Wire Line
	9900 2250 9350 2250
Text Label 9350 1050 0    50   ~ 0
EPD_GDR
Text Label 9350 1150 0    50   ~ 0
EPD_RESET
Text Label 9350 1250 0    50   ~ 0
EPD_VGL
Text Label 9350 1350 0    50   ~ 0
EPD_VGH
Text Label 9350 1650 0    50   ~ 0
EPD_BS
Text Label 9350 1750 0    50   ~ 0
EPD_BUSY
Text Label 9350 1850 0    50   ~ 0
~EPD_RST
Text Label 9350 1950 0    50   ~ 0
EPD_DC
Text Label 9350 2050 0    50   ~ 0
~EPD_CS
Text Label 9350 2150 0    50   ~ 0
EPD_CLK
Text Label 9350 2250 0    50   ~ 0
EPD_DIN
Text Label 9350 2350 0    50   ~ 0
EPD_VDDIO
Text Label 9350 2450 0    50   ~ 0
EPD_VCI
Text Label 9350 2550 0    50   ~ 0
EPD_VSS
Text Label 9350 2650 0    50   ~ 0
EPD_VDD
Text Label 9350 2750 0    50   ~ 0
EPD_VPP
Text Label 9350 2850 0    50   ~ 0
EPD_VSH
Text Label 9350 2950 0    50   ~ 0
EPD_PWRVGH
Text Label 9350 3050 0    50   ~ 0
EPD_VSL
Text Label 9350 3150 0    50   ~ 0
EPD_PWRVGL
Text Label 9350 3250 0    50   ~ 0
EPD_VCOM
NoConn ~ 9900 1450
NoConn ~ 9900 950 
NoConn ~ 9900 1550
$Comp
L power:+BATT #PWR0101
U 1 1 61C50BDC
P 1950 1350
F 0 "#PWR0101" H 1950 1200 50  0001 C CNN
F 1 "+BATT" H 1965 1523 50  0000 C CNN
F 2 "" H 1950 1350 50  0001 C CNN
F 3 "" H 1950 1350 50  0001 C CNN
	1    1950 1350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 61C51D48
P 1550 1600
F 0 "#PWR0102" H 1550 1350 50  0001 C CNN
F 1 "GND" H 1555 1427 50  0000 C CNN
F 2 "" H 1550 1600 50  0001 C CNN
F 3 "" H 1550 1600 50  0001 C CNN
	1    1550 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 1550 1550 1550
Wire Wire Line
	1550 1550 1550 1600
$Comp
L Device:C C10
U 1 1 61C5E5F6
P 9200 3500
F 0 "C10" H 9315 3546 50  0000 L CNN
F 1 "1µF" H 9315 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 9238 3350 50  0001 C CNN
F 3 "~" H 9200 3500 50  0001 C CNN
	1    9200 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:L_Core_Iron L1
U 1 1 61C5EFC2
P 4750 2050
F 0 "L1" H 4838 2096 50  0000 L CNN
F 1 "68µH" H 4838 2005 50  0000 L CNN
F 2 "Inductor_SMD:L_1210_3225Metric_Pad1.42x2.65mm_HandSolder" H 4750 2050 50  0001 C CNN
F 3 "~" H 4750 2050 50  0001 C CNN
	1    4750 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 61C5F60E
P 9250 2550
F 0 "#PWR0104" H 9250 2300 50  0001 C CNN
F 1 "GND" H 9255 2377 50  0000 C CNN
F 2 "" H 9250 2550 50  0001 C CNN
F 3 "" H 9250 2550 50  0001 C CNN
	1    9250 2550
	0    1    1    0   
$EndComp
$Comp
L Diode:MBR0530 D1
U 1 1 61C5FAFD
P 4000 2700
F 0 "D1" H 4000 2917 50  0000 C CNN
F 1 "MBR0530" H 4000 2826 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-123" H 4000 2525 50  0001 C CNN
F 3 "http://www.mccsemi.com/up_pdf/MBR0520~MBR0580(SOD123).pdf" H 4000 2700 50  0001 C CNN
	1    4000 2700
	0    -1   -1   0   
$EndComp
$Comp
L Device:Q_NMOS_GSD Q1
U 1 1 61C602AB
P 4900 1450
F 0 "Q1" H 5104 1496 50  0000 L CNN
F 1 "Si1308EDL" H 5104 1405 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-323_SC-70_Handsoldering" H 5100 1550 50  0001 C CNN
F 3 "~" H 4900 1450 50  0001 C CNN
	1    4900 1450
	1    0    0    1   
$EndComp
$Comp
L Diode:MBR0530 D2
U 1 1 61C6BE3D
P 4350 2700
F 0 "D2" V 4304 2780 50  0000 L CNN
F 1 "MBR0530" V 4395 2780 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-123" H 4350 2525 50  0001 C CNN
F 3 "http://www.mccsemi.com/up_pdf/MBR0520~MBR0580(SOD123).pdf" H 4350 2700 50  0001 C CNN
	1    4350 2700
	0    1    1    0   
$EndComp
$Comp
L Diode:MBR0530 D3
U 1 1 61C6CA62
P 5000 2700
F 0 "D3" V 5046 2620 50  0000 R CNN
F 1 "MBR0530" V 4955 2620 50  0000 R CNN
F 2 "Diode_SMD:D_SOD-123" H 5000 2525 50  0001 C CNN
F 3 "http://www.mccsemi.com/up_pdf/MBR0520~MBR0580(SOD123).pdf" H 5000 2700 50  0001 C CNN
	1    5000 2700
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C9
U 1 1 61C784C0
P 8800 3500
F 0 "C9" H 8915 3546 50  0000 L CNN
F 1 "1µF" H 8915 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 8838 3350 50  0001 C CNN
F 3 "~" H 8800 3500 50  0001 C CNN
	1    8800 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 61C78BE1
P 8400 3500
F 0 "C8" H 8515 3546 50  0000 L CNN
F 1 "1µF" H 8515 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 8438 3350 50  0001 C CNN
F 3 "~" H 8400 3500 50  0001 C CNN
	1    8400 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C7
U 1 1 61C793C3
P 8000 3500
F 0 "C7" H 8115 3546 50  0000 L CNN
F 1 "1µF" H 8115 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 8038 3350 50  0001 C CNN
F 3 "~" H 8000 3500 50  0001 C CNN
	1    8000 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 61C79C96
P 7600 3500
F 0 "C6" H 7715 3546 50  0000 L CNN
F 1 "1µF" H 7715 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 7638 3350 50  0001 C CNN
F 3 "~" H 7600 3500 50  0001 C CNN
	1    7600 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 61C7ABE9
P 7600 3800
F 0 "#PWR0105" H 7600 3550 50  0001 C CNN
F 1 "GND" H 7605 3627 50  0000 C CNN
F 2 "" H 7600 3800 50  0001 C CNN
F 3 "" H 7600 3800 50  0001 C CNN
	1    7600 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 3650 8800 3650
Connection ~ 8000 3650
Wire Wire Line
	8000 3650 7600 3650
Connection ~ 8400 3650
Wire Wire Line
	8400 3650 8000 3650
Connection ~ 8800 3650
Wire Wire Line
	8800 3650 8400 3650
$Comp
L Device:C C5
U 1 1 61C7F28F
P 7200 3500
F 0 "C5" H 7315 3546 50  0000 L CNN
F 1 "1µF" H 7315 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 7238 3350 50  0001 C CNN
F 3 "~" H 7200 3500 50  0001 C CNN
	1    7200 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 61C7FB6C
P 6750 3500
F 0 "C4" H 6865 3546 50  0000 L CNN
F 1 "1µF" H 6865 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 6788 3350 50  0001 C CNN
F 3 "~" H 6750 3500 50  0001 C CNN
	1    6750 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 61C80454
P 6350 3500
F 0 "C3" H 6465 3546 50  0000 L CNN
F 1 "1µF" H 6465 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 6388 3350 50  0001 C CNN
F 3 "~" H 6350 3500 50  0001 C CNN
	1    6350 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C2
U 1 1 61C80DE3
P 5950 3500
F 0 "C2" H 6065 3546 50  0000 L CNN
F 1 "1µF" H 6065 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5988 3350 50  0001 C CNN
F 3 "~" H 5950 3500 50  0001 C CNN
	1    5950 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 61C817A1
P 5550 3500
F 0 "C1" H 5665 3546 50  0000 L CNN
F 1 "1µF" H 5665 3455 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 5588 3350 50  0001 C CNN
F 3 "~" H 5550 3500 50  0001 C CNN
	1    5550 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	7600 3650 7200 3650
Connection ~ 7600 3650
Connection ~ 5950 3650
Wire Wire Line
	5950 3650 5550 3650
Connection ~ 6350 3650
Wire Wire Line
	6350 3650 5950 3650
Connection ~ 6750 3650
Wire Wire Line
	6750 3650 6350 3650
Connection ~ 7200 3650
Wire Wire Line
	7200 3650 6750 3650
Wire Wire Line
	7600 3650 7600 3800
Wire Wire Line
	9200 3250 9200 3350
Wire Wire Line
	9200 3250 9900 3250
Wire Wire Line
	8800 3150 8800 3350
Wire Wire Line
	8800 3150 9900 3150
Wire Wire Line
	8400 3050 8400 3350
Wire Wire Line
	8400 3050 9900 3050
Wire Wire Line
	8000 2950 8000 3350
Wire Wire Line
	8000 2950 9900 2950
Wire Wire Line
	7600 2850 7600 3350
Wire Wire Line
	7600 2850 9900 2850
Wire Wire Line
	7200 2750 7200 3350
Wire Wire Line
	7200 2750 9900 2750
Wire Wire Line
	6750 2650 6750 3350
Wire Wire Line
	6750 2650 9900 2650
Wire Wire Line
	6350 2350 6350 3350
Wire Wire Line
	6350 2350 8950 2350
Wire Wire Line
	5950 1350 5950 3350
Wire Wire Line
	5950 1350 9900 1350
Wire Wire Line
	5550 1250 5550 3350
Wire Wire Line
	5550 1250 9900 1250
Wire Wire Line
	9250 2450 9250 2350
Wire Wire Line
	9250 2450 9900 2450
Connection ~ 9250 2350
Wire Wire Line
	9250 2350 9900 2350
Wire Wire Line
	9250 2550 9900 2550
Connection ~ 8950 2350
Wire Wire Line
	8950 2350 9250 2350
$Comp
L power:GND #PWR0106
U 1 1 61CC2487
P 4000 3050
F 0 "#PWR0106" H 4000 2800 50  0001 C CNN
F 1 "GND" H 4005 2877 50  0000 C CNN
F 2 "" H 4000 3050 50  0001 C CNN
F 3 "" H 4000 3050 50  0001 C CNN
	1    4000 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 2950 5000 2950
Wire Wire Line
	5000 2950 5000 2850
Connection ~ 8000 2950
Wire Wire Line
	8800 3150 4350 3150
Wire Wire Line
	4350 3150 4350 2850
Connection ~ 8800 3150
Wire Wire Line
	4000 2850 4000 3050
$Comp
L Device:C C11
U 1 1 61CCF064
P 4700 2450
F 0 "C11" V 4952 2450 50  0000 C CNN
F 1 "4.7µF" V 4861 2450 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 4738 2300 50  0001 C CNN
F 3 "~" H 4700 2450 50  0001 C CNN
	1    4700 2450
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C12
U 1 1 61CDCE88
P 4250 2050
F 0 "C12" V 4502 2050 50  0000 C CNN
F 1 "4.7µF" V 4411 2050 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 4288 1900 50  0001 C CNN
F 3 "~" H 4250 2050 50  0001 C CNN
	1    4250 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0107
U 1 1 61CE2D02
P 3850 2100
F 0 "#PWR0107" H 3850 1850 50  0001 C CNN
F 1 "GND" H 3855 1927 50  0000 C CNN
F 2 "" H 3850 2100 50  0001 C CNN
F 3 "" H 3850 2100 50  0001 C CNN
	1    3850 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 61CF52D1
P 4300 1450
F 0 "R2" V 4507 1450 50  0000 C CNN
F 1 "10K" V 4416 1450 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 4230 1450 50  0001 C CNN
F 3 "~" H 4300 1450 50  0001 C CNN
	1    4300 1450
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 61CF6859
P 4750 1050
F 0 "R1" V 4950 1050 50  0000 C CNN
F 1 "3R" V 4850 1050 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 4680 1050 50  0001 C CNN
F 3 "~" H 4750 1050 50  0001 C CNN
	1    4750 1050
	0    1    1    0   
$EndComp
Wire Wire Line
	5000 1050 5000 1150
Wire Wire Line
	4850 2450 5000 2450
Connection ~ 5000 2450
Wire Wire Line
	5000 2450 5000 2550
Wire Wire Line
	4100 2050 3850 2050
Wire Wire Line
	3850 2050 3850 2100
Wire Wire Line
	4700 1450 4500 1450
$Comp
L power:GND #PWR0108
U 1 1 61D066F6
P 3850 1550
F 0 "#PWR0108" H 3850 1300 50  0001 C CNN
F 1 "GND" H 3855 1377 50  0000 C CNN
F 2 "" H 3850 1550 50  0001 C CNN
F 3 "" H 3850 1550 50  0001 C CNN
	1    3850 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 1450 3850 1450
Wire Wire Line
	3850 1450 3850 1550
$Comp
L power:GND #PWR0109
U 1 1 61D14DB2
P 4600 1200
F 0 "#PWR0109" H 4600 950 50  0001 C CNN
F 1 "GND" H 4605 1027 50  0000 C CNN
F 2 "" H 4600 1200 50  0001 C CNN
F 3 "" H 4600 1200 50  0001 C CNN
	1    4600 1200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 2550 4000 2450
Wire Wire Line
	4000 2450 4350 2450
Wire Wire Line
	4350 2550 4350 2450
Connection ~ 4350 2450
Wire Wire Line
	4350 2450 4550 2450
Wire Wire Line
	5000 1650 5000 2050
Wire Wire Line
	4400 2050 4500 2050
Wire Wire Line
	4900 2050 5000 2050
Connection ~ 5000 2050
Wire Wire Line
	5000 2050 5000 2450
Wire Wire Line
	5000 1150 9900 1150
Connection ~ 5000 1150
Wire Wire Line
	5000 1150 5000 1250
Wire Wire Line
	4600 1200 4600 1050
Wire Wire Line
	4900 1050 5000 1050
Wire Wire Line
	4500 1450 4500 950 
Wire Wire Line
	4500 950  9350 950 
Wire Wire Line
	9350 950  9350 1050
Connection ~ 4500 1450
Wire Wire Line
	4500 1450 4450 1450
$Comp
L power:+2V5 #PWR0110
U 1 1 61D52E15
P 2950 1750
F 0 "#PWR0110" H 2950 1600 50  0001 C CNN
F 1 "+2V5" H 2965 1923 50  0000 C CNN
F 2 "" H 2950 1750 50  0001 C CNN
F 3 "" H 2950 1750 50  0001 C CNN
	1    2950 1750
	1    0    0    -1  
$EndComp
Connection ~ 4500 2050
Wire Wire Line
	4500 2050 4600 2050
$Comp
L Device:L L2
U 1 1 61D7E553
P 2850 4000
F 0 "L2" H 2902 4046 50  0000 L CNN
F 1 "2,2µH" H 2902 3955 50  0000 L CNN
F 2 "Inductor_SMD:L_Wuerth_MAPI-1610" H 2850 4000 50  0001 C CNN
F 3 "~" H 2850 4000 50  0001 C CNN
	1    2850 4000
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C13
U 1 1 61D8037F
P 1200 5200
F 0 "C13" H 1315 5246 50  0000 L CNN
F 1 "10µF" H 1315 5155 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 1238 5050 50  0001 C CNN
F 3 "~" H 1200 5200 50  0001 C CNN
	1    1200 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C14
U 1 1 61D8296D
P 3550 5300
F 0 "C14" H 3665 5346 50  0000 L CNN
F 1 "10µF" H 3665 5255 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 3588 5150 50  0001 C CNN
F 3 "~" H 3550 5300 50  0001 C CNN
	1    3550 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 61D861EB
P 2050 5300
F 0 "R3" V 2257 5300 50  0000 C CNN
F 1 "R" V 2166 5300 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 1980 5300 50  0001 C CNN
F 3 "~" H 2050 5300 50  0001 C CNN
	1    2050 5300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0111
U 1 1 61DBB021
P 1650 5600
F 0 "#PWR0111" H 1650 5350 50  0001 C CNN
F 1 "GND" H 1655 5427 50  0000 C CNN
F 2 "" H 1650 5600 50  0001 C CNN
F 3 "" H 1650 5600 50  0001 C CNN
	1    1650 5600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0112
U 1 1 61DBD4A3
P 3550 5650
F 0 "#PWR0112" H 3550 5400 50  0001 C CNN
F 1 "GND" H 3555 5477 50  0000 C CNN
F 2 "" H 3550 5650 50  0001 C CNN
F 3 "" H 3550 5650 50  0001 C CNN
	1    3550 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 5550 2050 5450
Wire Wire Line
	1650 5600 1650 5550
Connection ~ 1650 5550
Wire Wire Line
	1650 5550 2050 5550
Wire Wire Line
	3550 5450 3550 5550
Wire Wire Line
	3000 5550 3550 5550
Connection ~ 3550 5550
Wire Wire Line
	3550 5550 3550 5650
Wire Wire Line
	3000 4750 3550 4750
Wire Wire Line
	3550 4750 3550 5150
$Comp
L power:+2V5 #PWR0113
U 1 1 61DD5B1C
P 3850 4650
F 0 "#PWR0113" H 3850 4500 50  0001 C CNN
F 1 "+2V5" H 3865 4823 50  0000 C CNN
F 2 "" H 3850 4650 50  0001 C CNN
F 3 "" H 3850 4650 50  0001 C CNN
	1    3850 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 4750 1200 5050
Wire Wire Line
	1850 4750 1850 4000
Wire Wire Line
	1850 4000 2700 4000
Connection ~ 1850 4750
Wire Wire Line
	1850 4750 1200 4750
Wire Wire Line
	1200 4650 1200 4750
Connection ~ 1200 4750
$Comp
L power:+BATT #PWR0114
U 1 1 61DF86F1
P 1200 4650
F 0 "#PWR0114" H 1200 4500 50  0001 C CNN
F 1 "+BATT" H 1215 4823 50  0000 C CNN
F 2 "" H 1200 4650 50  0001 C CNN
F 3 "" H 1200 4650 50  0001 C CNN
	1    1200 4650
	1    0    0    -1  
$EndComp
$Comp
L power:+BATT #PWR0115
U 1 1 61E06AF4
P 1500 3400
F 0 "#PWR0115" H 1500 3250 50  0001 C CNN
F 1 "+BATT" H 1515 3573 50  0000 C CNN
F 2 "" H 1500 3400 50  0001 C CNN
F 3 "" H 1500 3400 50  0001 C CNN
	1    1500 3400
	1    0    0    -1  
$EndComp
$Comp
L power:+2V5 #PWR0116
U 1 1 61E2D3EE
P 4250 6000
F 0 "#PWR0116" H 4250 5850 50  0001 C CNN
F 1 "+2V5" H 4265 6173 50  0000 C CNN
F 2 "" H 4250 6000 50  0001 C CNN
F 3 "" H 4250 6000 50  0001 C CNN
	1    4250 6000
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0117
U 1 1 61E32005
P 6650 6350
F 0 "#PWR0117" H 6650 6100 50  0001 C CNN
F 1 "GND" H 6655 6177 50  0000 C CNN
F 2 "" H 6650 6350 50  0001 C CNN
F 3 "" H 6650 6350 50  0001 C CNN
	1    6650 6350
	1    0    0    -1  
$EndComp
$Comp
L power:+2V5 #PWR0119
U 1 1 61E3E8F5
P 8500 4150
F 0 "#PWR0119" H 8500 4000 50  0001 C CNN
F 1 "+2V5" H 8515 4323 50  0000 C CNN
F 2 "" H 8500 4150 50  0001 C CNN
F 3 "" H 8500 4150 50  0001 C CNN
	1    8500 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0120
U 1 1 61E447BB
P 9050 6100
F 0 "#PWR0120" H 9050 5850 50  0001 C CNN
F 1 "GND" H 9055 5927 50  0000 C CNN
F 2 "" H 9050 6100 50  0001 C CNN
F 3 "" H 9050 6100 50  0001 C CNN
	1    9050 6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 5200 7150 5200
Wire Wire Line
	5400 4600 4800 4600
Wire Wire Line
	6500 5100 7150 5100
Wire Wire Line
	6500 5000 7150 5000
Wire Wire Line
	6500 4900 7150 4900
Wire Wire Line
	6500 4700 7150 4700
Wire Wire Line
	5400 4700 4800 4700
Wire Wire Line
	5400 5200 4750 5200
Text Label 7150 5200 2    50   ~ 0
EPD_CLK
Text Label 4800 4600 0    50   ~ 0
USART1_RX
Text Label 4800 4700 0    50   ~ 0
USART1_TX
Text Label 7150 5000 2    50   ~ 0
EPD_DC
Text Label 7150 4900 2    50   ~ 0
~EPD_CS
Text Label 7150 5100 2    50   ~ 0
~EPD_RST
Text Label 7150 4700 2    50   ~ 0
EPD_DIN
Text Label 4750 5200 0    50   ~ 0
EPD_BUSY
$Comp
L power:+BATT #PWR0103
U 1 1 61CCF2E5
P 7350 5300
F 0 "#PWR0103" H 7350 5150 50  0001 C CNN
F 1 "+BATT" H 7365 5473 50  0000 C CNN
F 2 "" H 7350 5300 50  0001 C CNN
F 3 "" H 7350 5300 50  0001 C CNN
	1    7350 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9150 5150 8500 5150
Wire Wire Line
	9150 5050 8500 5050
Wire Wire Line
	9150 5750 8500 5750
Wire Wire Line
	10300 5650 10950 5650
Wire Wire Line
	9150 5350 8500 5350
Text Label 8500 5150 0    50   ~ 0
USART1_TX
Text Label 8500 5050 0    50   ~ 0
USART1_RX
Text Label 8500 5350 0    50   ~ 0
~XBEE_RESET
Text Label 8500 5750 0    50   ~ 0
XBEE_SLEEP_RQ
Text Label 10950 5650 2    50   ~ 0
~XBEE_SLEEP
Wire Wire Line
	5400 5900 4750 5900
Wire Wire Line
	5400 5600 4750 5600
Wire Wire Line
	5400 5500 4750 5500
Text Label 4750 5900 0    50   ~ 0
~XBEE_SLEEP
Text Label 4750 5600 0    50   ~ 0
XBEE_SLEEP_RQ
Text Label 4750 5500 0    50   ~ 0
~XBEE_RESET
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 61D0B46A
P 3750 4950
F 0 "J4" V 3668 4762 50  0000 R CNN
F 1 "Conn_01x02" V 3713 4762 50  0001 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3750 4950 50  0001 C CNN
F 3 "~" H 3750 4950 50  0001 C CNN
	1    3750 4950
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 61D0C6A9
P 1850 1650
F 0 "J3" V 1768 1462 50  0000 R CNN
F 1 "Conn_01x02" V 1813 1462 50  0001 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 1850 1650 50  0001 C CNN
F 3 "~" H 1850 1650 50  0001 C CNN
	1    1850 1650
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 61D11F5F
P 4350 6250
F 0 "J5" V 4268 6062 50  0000 R CNN
F 1 "Conn_01x02" V 4313 6062 50  0001 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 4350 6250 50  0001 C CNN
F 3 "~" H 4350 6250 50  0001 C CNN
	1    4350 6250
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 61D136EE
P 8600 4400
F 0 "J6" V 8518 4212 50  0000 R CNN
F 1 "Conn_01x02" V 8563 4212 50  0001 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 8600 4400 50  0001 C CNN
F 3 "~" H 8600 4400 50  0001 C CNN
	1    8600 4400
	0    -1   1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J7
U 1 1 61D1722F
P 3050 2050
F 0 "J7" V 2968 1862 50  0000 R CNN
F 1 "Conn_01x02" V 3013 1862 50  0001 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3050 2050 50  0001 C CNN
F 3 "~" H 3050 2050 50  0001 C CNN
	1    3050 2050
	0    -1   1    0   
$EndComp
Wire Wire Line
	1200 1450 1850 1450
Wire Wire Line
	1950 1350 1950 1450
$Comp
L power:VD #PWR0121
U 1 1 61D6CC92
P 3250 1750
F 0 "#PWR0121" H 3250 1600 50  0001 C CNN
F 1 "VD" H 3265 1923 50  0000 C CNN
F 2 "" H 3250 1750 50  0001 C CNN
F 3 "" H 3250 1750 50  0001 C CNN
	1    3250 1750
	1    0    0    -1  
$EndComp
$Comp
L power:VD #PWR0122
U 1 1 61D6D4C7
P 4500 1850
F 0 "#PWR0122" H 4500 1700 50  0001 C CNN
F 1 "VD" H 4515 2023 50  0000 C CNN
F 2 "" H 4500 1850 50  0001 C CNN
F 3 "" H 4500 1850 50  0001 C CNN
	1    4500 1850
	1    0    0    -1  
$EndComp
$Comp
L power:VD #PWR0123
U 1 1 61D6FB63
P 8950 2100
F 0 "#PWR0123" H 8950 1950 50  0001 C CNN
F 1 "VD" H 8965 2273 50  0000 C CNN
F 2 "" H 8950 2100 50  0001 C CNN
F 3 "" H 8950 2100 50  0001 C CNN
	1    8950 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 2100 8950 2350
Wire Wire Line
	4500 1850 4500 2050
Wire Wire Line
	3250 1750 3150 1750
Wire Wire Line
	3150 1750 3150 1850
Wire Wire Line
	2950 1750 3050 1750
Wire Wire Line
	3050 1750 3050 1850
Wire Wire Line
	3550 4750 3750 4750
Connection ~ 3550 4750
Wire Wire Line
	3850 4750 3850 4650
Wire Wire Line
	4450 6050 5400 6050
Wire Wire Line
	4250 6000 4350 6000
Wire Wire Line
	4350 6000 4350 6050
Wire Wire Line
	8500 4150 8600 4150
Wire Wire Line
	8600 4150 8600 4200
$Comp
L Device:C C15
U 1 1 61DF4753
P 9200 4350
F 0 "C15" H 9315 4396 50  0000 L CNN
F 1 "1µF" H 9315 4305 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 9238 4200 50  0001 C CNN
F 3 "~" H 9200 4350 50  0001 C CNN
	1    9200 4350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0124
U 1 1 61E00176
P 9200 4550
F 0 "#PWR0124" H 9200 4300 50  0001 C CNN
F 1 "GND" H 9205 4377 50  0000 C CNN
F 2 "" H 9200 4550 50  0001 C CNN
F 3 "" H 9200 4550 50  0001 C CNN
	1    9200 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 4200 9000 4200
Wire Wire Line
	9200 4500 9200 4550
Connection ~ 9000 4200
Wire Wire Line
	9000 4200 9200 4200
$Comp
L Switch:SW_SPDT SW1
U 1 1 61E47136
P 1850 3550
F 0 "SW1" H 1850 3743 50  0000 C CNN
F 1 "SW_SPDT" H 1850 3744 50  0001 C CNN
F 2 "Button_Switch_THT:SW_Slide_1P2T_CK_OS102011MS2Q" H 1850 3550 50  0001 C CNN
F 3 "~" H 1850 3550 50  0001 C CNN
	1    1850 3550
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0125
U 1 1 61E6FA64
P 1500 3750
F 0 "#PWR0125" H 1500 3500 50  0001 C CNN
F 1 "GND" H 1505 3577 50  0000 C CNN
F 2 "" H 1500 3750 50  0001 C CNN
F 3 "" H 1500 3750 50  0001 C CNN
	1    1500 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 3650 1500 3650
Wire Wire Line
	1500 3650 1500 3750
Wire Wire Line
	1650 3450 1500 3450
Wire Wire Line
	1500 3450 1500 3400
Wire Wire Line
	9100 1650 9900 1650
$Comp
L power:GND #PWR0126
U 1 1 61CAE1BC
P 9100 1650
F 0 "#PWR0126" H 9100 1400 50  0001 C CNN
F 1 "GND" H 9105 1477 50  0000 C CNN
F 2 "" H 9100 1650 50  0001 C CNN
F 3 "" H 9100 1650 50  0001 C CNN
	1    9100 1650
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 61CB55B6
P 1500 7250
F 0 "#FLG0101" H 1500 7325 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 7423 50  0000 C CNN
F 2 "" H 1500 7250 50  0001 C CNN
F 3 "~" H 1500 7250 50  0001 C CNN
	1    1500 7250
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 61CBBB98
P 1500 7600
F 0 "#FLG0102" H 1500 7675 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 7773 50  0000 C CNN
F 2 "" H 1500 7600 50  0001 C CNN
F 3 "~" H 1500 7600 50  0001 C CNN
	1    1500 7600
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 61CC9254
P 1500 6850
F 0 "#FLG0103" H 1500 6925 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 7023 50  0000 C CNN
F 2 "" H 1500 6850 50  0001 C CNN
F 3 "~" H 1500 6850 50  0001 C CNN
	1    1500 6850
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 61CCFF5E
P 2400 7300
F 0 "#FLG0104" H 2400 7375 50  0001 C CNN
F 1 "PWR_FLAG" H 2400 7473 50  0000 C CNN
F 2 "" H 2400 7300 50  0001 C CNN
F 3 "~" H 2400 7300 50  0001 C CNN
	1    2400 7300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0127
U 1 1 61CD76D1
P 2400 7450
F 0 "#PWR0127" H 2400 7200 50  0001 C CNN
F 1 "GND" H 2405 7277 50  0000 C CNN
F 2 "" H 2400 7450 50  0001 C CNN
F 3 "" H 2400 7450 50  0001 C CNN
	1    2400 7450
	1    0    0    -1  
$EndComp
$Comp
L power:+2V5 #PWR0128
U 1 1 61CDE85E
P 1100 6850
F 0 "#PWR0128" H 1100 6700 50  0001 C CNN
F 1 "+2V5" H 1115 7023 50  0000 C CNN
F 2 "" H 1100 6850 50  0001 C CNN
F 3 "" H 1100 6850 50  0001 C CNN
	1    1100 6850
	1    0    0    -1  
$EndComp
$Comp
L power:+BATT #PWR0129
U 1 1 61CEA414
P 1100 7250
F 0 "#PWR0129" H 1100 7100 50  0001 C CNN
F 1 "+BATT" H 1115 7423 50  0000 C CNN
F 2 "" H 1100 7250 50  0001 C CNN
F 3 "" H 1100 7250 50  0001 C CNN
	1    1100 7250
	1    0    0    -1  
$EndComp
$Comp
L power:VD #PWR0130
U 1 1 61CFB78C
P 1100 7600
F 0 "#PWR0130" H 1100 7450 50  0001 C CNN
F 1 "VD" H 1115 7773 50  0000 C CNN
F 2 "" H 1100 7600 50  0001 C CNN
F 3 "" H 1100 7600 50  0001 C CNN
	1    1100 7600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 7300 2400 7450
Wire Wire Line
	1100 7600 1500 7600
Wire Wire Line
	1500 7250 1100 7250
Wire Wire Line
	1100 6850 1500 6850
Wire Wire Line
	5400 4900 4750 4900
Text Label 4750 4900 0    50   ~ 0
BUTTON
$Comp
L Switch:SW_SPST SW2
U 1 1 61E04627
P 3450 6650
F 0 "SW2" V 3450 6748 50  0000 L CNN
F 1 "SW_SPST" H 3450 6794 50  0001 C CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 3450 6650 50  0001 C CNN
F 3 "~" H 3450 6650 50  0001 C CNN
	1    3450 6650
	0    1    1    0   
$EndComp
$Comp
L power:+2V5 #PWR0131
U 1 1 61E0F5E3
P 3450 6400
F 0 "#PWR0131" H 3450 6250 50  0001 C CNN
F 1 "+2V5" H 3465 6573 50  0000 C CNN
F 2 "" H 3450 6400 50  0001 C CNN
F 3 "" H 3450 6400 50  0001 C CNN
	1    3450 6400
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 61E19C4D
P 3450 7050
F 0 "R4" V 3657 7050 50  0000 C CNN
F 1 "1M" V 3566 7050 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3380 7050 50  0001 C CNN
F 3 "~" H 3450 7050 50  0001 C CNN
	1    3450 7050
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR0132
U 1 1 61E232E2
P 3450 7250
F 0 "#PWR0132" H 3450 7000 50  0001 C CNN
F 1 "GND" H 3455 7077 50  0000 C CNN
F 2 "" H 3450 7250 50  0001 C CNN
F 3 "" H 3450 7250 50  0001 C CNN
	1    3450 7250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 6400 3450 6450
Wire Wire Line
	3450 6850 3450 6900
Wire Wire Line
	3450 7200 3450 7250
Wire Wire Line
	3450 6900 4100 6900
Connection ~ 3450 6900
Text Label 4100 6900 2    50   ~ 0
BUTTON
$Comp
L power:PWR_FLAG #FLG0105
U 1 1 61E63721
P 9000 4150
F 0 "#FLG0105" H 9000 4225 50  0001 C CNN
F 1 "PWR_FLAG" H 9000 4323 50  0000 C CNN
F 2 "" H 9000 4150 50  0001 C CNN
F 3 "~" H 9000 4150 50  0001 C CNN
	1    9000 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9000 4150 9000 4200
$Comp
L Insa:STM32L031 U1
U 1 1 61D5E960
P 6000 5400
F 0 "U1" H 5950 6465 50  0000 C CNN
F 1 "STM32L031" H 5950 6374 50  0000 C CNN
F 2 "Insa:STM32-Nucleo-32" H 6000 6400 50  0001 C CNN
F 3 "" H 6000 6400 50  0001 C CNN
	1    6000 5400
	1    0    0    -1  
$EndComp
$Comp
L Insa:XBee3 U2
U 1 1 61D62A97
P 9700 5850
F 0 "U2" H 9725 7015 50  0000 C CNN
F 1 "XBee3" H 9725 6924 50  0000 C CNN
F 2 "Insa:Xbee3" H 9700 5650 50  0001 C CNN
F 3 "" H 9700 5650 50  0001 C CNN
	1    9700 5850
	1    0    0    -1  
$EndComp
$Comp
L Insa:MAX17220 U3
U 1 1 61E782BE
P 2600 5050
F 0 "U3" H 2667 5615 50  0000 C CNN
F 1 "MAX17220ELT" H 2667 5524 50  0000 C CNN
F 2 "Package_DFN_QFN:DFN-6-1EP_2x2mm_P0.5mm_EP0.61x1.42mm" H 2600 5550 50  0001 C CNN
F 3 "" H 2600 5550 50  0001 C CNN
	1    2600 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1200 5350 1200 5550
Wire Wire Line
	1200 5550 1650 5550
Wire Wire Line
	1850 4750 2200 4750
Wire Wire Line
	2200 4950 2050 4950
Wire Wire Line
	2050 3550 2050 4950
Wire Wire Line
	2200 5150 2050 5150
Wire Wire Line
	3000 5150 3000 5550
Wire Wire Line
	3000 4950 3150 4950
Wire Wire Line
	3150 4950 3150 4000
Wire Wire Line
	3150 4000 3000 4000
Wire Wire Line
	6500 6150 6650 6150
Wire Wire Line
	6650 6150 6650 6250
Wire Wire Line
	6500 6250 6650 6250
Connection ~ 6650 6250
Wire Wire Line
	6650 6250 6650 6350
NoConn ~ 5400 6150
NoConn ~ 5400 6250
$Comp
L Device:R R5
U 1 1 6205F801
P 7000 5300
F 0 "R5" V 7207 5300 50  0000 C CNN
F 1 "100R" V 7116 5300 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 6930 5300 50  0001 C CNN
F 3 "~" H 7000 5300 50  0001 C CNN
	1    7000 5300
	0    1    1    0   
$EndComp
Wire Wire Line
	6500 5300 6850 5300
Wire Wire Line
	7150 5300 7350 5300
$Comp
L power:+2V5 #PWR?
U 1 1 6213A7B2
P 6800 5850
F 0 "#PWR?" H 6800 5700 50  0001 C CNN
F 1 "+2V5" H 6815 6023 50  0000 C CNN
F 2 "" H 6800 5850 50  0001 C CNN
F 3 "" H 6800 5850 50  0001 C CNN
	1    6800 5850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6500 5850 6800 5850
NoConn ~ 5400 4800
NoConn ~ 5400 5000
NoConn ~ 5400 5100
NoConn ~ 5400 5300
NoConn ~ 5400 5400
NoConn ~ 5400 5700
NoConn ~ 5400 5800
NoConn ~ 6500 4600
NoConn ~ 6500 4800
NoConn ~ 6500 5550
NoConn ~ 6500 5650
Wire Wire Line
	9150 5850 9050 5850
Wire Wire Line
	9050 5850 9050 6100
Wire Wire Line
	9150 4950 9000 4950
Wire Wire Line
	9000 4200 9000 4950
NoConn ~ 9150 5250
NoConn ~ 9150 5450
NoConn ~ 9150 5550
NoConn ~ 10300 4950
NoConn ~ 10300 5050
NoConn ~ 10300 5150
NoConn ~ 10300 5250
NoConn ~ 10300 5350
NoConn ~ 10300 5450
NoConn ~ 10300 5750
NoConn ~ 10300 5850
$EndSCHEMATC
