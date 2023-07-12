EESchema Schematic File Version 4
LIBS:fuel_economy_meter-cache
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
L ESP32-DEVKITC-32D:ESP32-DevkitV1 U3
U 1 1 63DED8E4
P 5750 2700
F 0 "U3" H 5750 2767 50  0000 C CNN
F 1 "ESP32-DevkitV1" H 5750 2676 50  0000 C CNN
F 2 "" H 5150 2650 50  0001 C CNN
F 3 "" H 5150 2650 50  0001 C CNN
	1    5750 2700
	1    0    0    -1  
$EndComp
Text GLabel 4850 2900 0    50   Input ~ 0
3v3
Wire Wire Line
	4850 2900 4950 2900
$Comp
L Device:R R1
U 1 1 63DF1C79
P 1900 1350
F 0 "R1" H 1970 1396 50  0000 L CNN
F 1 "10k" H 1970 1305 50  0000 L CNN
F 2 "Resistor_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1830 1350 50  0001 C CNN
F 3 "~" H 1900 1350 50  0001 C CNN
	1    1900 1350
	0    -1   -1   0   
$EndComp
Text GLabel 1650 1350 0    50   Input ~ 0
3v3
Wire Wire Line
	1650 1350 1750 1350
$Comp
L Device:R R3
U 1 1 63DF334E
P 1700 3400
F 0 "R3" V 1907 3400 50  0000 C CNN
F 1 "1k5" V 1816 3400 50  0000 C CNN
F 2 "Resistor_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1630 3400 50  0001 C CNN
F 3 "~" H 1700 3400 50  0001 C CNN
	1    1700 3400
	0    -1   -1   0   
$EndComp
Text GLabel 6550 3000 2    50   Input ~ 0
GND
Text GLabel 3800 4000 2    50   Input ~ 0
GND
Text GLabel 1550 3400 0    50   Input ~ 0
GND
Wire Wire Line
	2400 3400 1850 3400
Text GLabel 2400 3300 0    50   Input ~ 0
VSS_IN
$Comp
L Device:R R5
U 1 1 63DF4016
P 1650 3800
F 0 "R5" V 1857 3800 50  0000 C CNN
F 1 "10k" V 1766 3800 50  0000 C CNN
F 2 "Resistor_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 1580 3800 50  0001 C CNN
F 3 "~" H 1650 3800 50  0001 C CNN
	1    1650 3800
	0    -1   -1   0   
$EndComp
Text GLabel 1500 3800 0    50   Input ~ 0
3v3
$Comp
L MAX9921AUB_V_T:MAX9921AUB_V+T U5
U 1 1 63DEED01
P 3100 3500
F 0 "U5" H 3100 4270 50  0000 C CNN
F 1 "MAX9921AUB_V+T" H 3100 4179 50  0000 C CNN
F 2 "" H 3100 3500 50  0001 L BNN
F 3 "" H 3100 3500 50  0001 L BNN
	1    3100 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1800 3800 2400 3800
Text GLabel 2400 3600 0    50   Input ~ 0
VSS_OE
Text GLabel 2400 3200 0    50   Input ~ 0
VSS_DIAG
Text GLabel 6550 3500 2    50   Input ~ 0
VSS_DIAG
Text GLabel 6550 3400 2    50   Input ~ 0
VSS_OE
Text GLabel 3800 3000 2    50   Input ~ 0
VBAT
Text GLabel 850  3500 0    50   Input ~ 0
VBAT
$Comp
L Device:R R4
U 1 1 63DF66EF
P 1000 3500
F 0 "R4" V 793 3500 50  0000 C CNN
F 1 "63k4" V 884 3500 50  0000 C CNN
F 2 "Resistor_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 930 3500 50  0001 C CNN
F 3 "~" H 1000 3500 50  0001 C CNN
	1    1000 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	1150 3500 2400 3500
$Comp
L 74HC4040D_653:74HC4040D,653 U1
U 1 1 63DF8327
P 3100 1450
F 0 "U1" H 3100 2220 50  0000 C CNN
F 1 "74HC4040D,653" H 3100 2129 50  0000 C CNN
F 2 "Package_SO:SOIC-16_3.9x9.9mm_P1.27mm" H 3100 1450 50  0001 L BNN
F 3 "" H 3100 1450 50  0001 L BNN
F 4 "74HC4040D,653" H 3100 1450 50  0001 L BNN "MPN"
F 5 "71R2477" H 3100 1450 50  0001 L BNN "OC_NEWARK"
F 6 "1826575" H 3100 1450 50  0001 L BNN "OC_FARNELL"
F 7 "NXP" H 3100 1450 50  0001 L BNN "SUPPLIER"
F 8 "SO-16" H 3100 1450 50  0001 L BNN "PACKAGE"
	1    3100 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 1350 2400 1350
Text GLabel 2400 1050 0    50   Input ~ 0
3v3
Text GLabel 2400 1550 0    50   Input ~ 0
GND
Text GLabel 2400 1250 0    50   Input ~ 0
CNT_RST
Text GLabel 3800 3200 2    50   Input ~ 0
VSS_OUT
Text GLabel 2050 1500 3    50   Input ~ 0
VSS_OUT
Wire Wire Line
	2050 1350 2050 1500
Connection ~ 2050 1350
$Comp
L 74HC4050D:74HC4050D U2
U 1 1 63DFE325
P 7850 1600
F 0 "U2" H 7850 2217 50  0000 C CNN
F 1 "74HC4050D" H 7850 2126 50  0000 C CNN
F 2 "" H 7850 1600 50  0001 L BNN
F 3 "" H 7850 1600 50  0001 L BNN
	1    7850 1600
	1    0    0    -1  
$EndComp
Text GLabel 7350 1950 0    50   Input ~ 0
GND
Text GLabel 7350 1250 0    50   Input ~ 0
3v3
Text GLabel 6900 1350 0    50   Input ~ 0
INJ_IN
Text GLabel 8350 1350 2    50   Input ~ 0
INJ_SIG
Text Notes 4350 1100 0    50   ~ 0
2
Text Notes 4350 1300 0    50   ~ 0
4
Text Notes 4350 1200 0    50   ~ 0
8
Text Notes 4350 1400 0    50   ~ 0
16
Text Notes 4350 1500 0    50   ~ 0
32
Text Notes 4350 1600 0    50   ~ 0
64
Text Notes 4350 1700 0    50   ~ 0
128
Text Notes 4350 1800 0    50   ~ 0
256
Text Notes 4350 1900 0    50   ~ 0
512
Text GLabel 6550 3100 2    50   Input ~ 0
GPIO13
Text GLabel 6550 3300 2    50   Input ~ 0
GPIO14
Text GLabel 4950 3100 0    50   Input ~ 0
GPIO15
Text GLabel 4950 3500 0    50   Input ~ 0
GPIO17
Text GLabel 4950 3700 0    50   Input ~ 0
GPIO18
Text GLabel 4950 3800 0    50   Input ~ 0
GPIO19
Text GLabel 4950 3400 0    50   Input ~ 0
GPIO16
Text GLabel 3800 1650 2    50   Input ~ 0
GPIO19
Text GLabel 3800 1550 2    50   Input ~ 0
GPIO18
Text GLabel 3800 1450 2    50   Input ~ 0
GPIO17
Text GLabel 3800 1350 2    50   Input ~ 0
GPIO16
Text GLabel 3800 1150 2    50   Input ~ 0
GPIO15
Text GLabel 3800 1250 2    50   Input ~ 0
GPIO14
Text GLabel 3800 1050 2    50   Input ~ 0
GPIO13
Text Notes 4350 1000 0    50   ~ 0
Signal divided by:
Text GLabel 4950 3900 0    50   Input ~ 0
GPIO21
Text GLabel 3800 1750 2    50   Input ~ 0
GPIO21
Text GLabel 4950 4200 0    50   Input ~ 0
GPIO22
Text GLabel 3800 1850 2    50   Input ~ 0
GPIO22
$Comp
L Device:R R2
U 1 1 63E054CB
P 7050 1500
F 0 "R2" V 6843 1500 50  0000 C CNN
F 1 "10k" V 6934 1500 50  0000 C CNN
F 2 "Resistor_THT:R_Box_L8.4mm_W2.5mm_P5.08mm" V 6980 1500 50  0001 C CNN
F 3 "~" H 7050 1500 50  0001 C CNN
	1    7050 1500
	1    0    0    -1  
$EndComp
Text GLabel 7050 1650 3    50   Input ~ 0
GND
Wire Wire Line
	7350 1350 7050 1350
Connection ~ 7050 1350
Wire Wire Line
	7050 1350 6900 1350
Text GLabel 6550 3600 2    50   Input ~ 0
INJ_SIG
$Comp
L Regulator_Switching:LM2596S-5 U4
U 1 1 63E0789A
P 8300 2800
F 0 "U4" H 8300 3167 50  0000 C CNN
F 1 "LM2596S-5" H 8300 3076 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TO-263-5_TabPin3" H 8350 2550 50  0001 L CIN
F 3 "http://www.ti.com/lit/ds/symlink/lm2596.pdf" H 8300 2800 50  0001 C CNN
	1    8300 2800
	1    0    0    -1  
$EndComp
Text GLabel 7800 2700 0    50   Input ~ 0
VBAT
Text GLabel 8300 3100 3    50   Input ~ 0
GND
Text GLabel 8800 2900 2    50   Input ~ 0
5V
Text GLabel 6550 2900 2    50   Input ~ 0
5V
$EndSCHEMATC
