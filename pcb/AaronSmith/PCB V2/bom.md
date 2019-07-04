# Bill of materials (BOM)
The BOM is split into required and optional sections. Read the use statement to decide if you wish to buy that part.
## Required parts
Below is the minimum required parts list

Part | Reference | Use | link
------------ | ------------- | ------------- | ------------- 
2.54mm header 1x6 | P1 | Programming header | https://www.ebay.co.uk/itm/263091291344
XT60 | P5 | Power connector | 
10k 0805 resistor | R2 | ESP32 enable pull up | 
RX5808| RX1-6 | VTX receiver modules | https://www.ebay.co.uk/itm/272610834936
Push button | SW1-2 | Programming buttons | https://www.ebay.co.uk/itm/111726375227
ESP Micro 32 | U1 | The processor | https://www.banggood.com/LILYGO-TTGO-Micro-32-V2_0-Wifi-Wireless-bluetooth-Module-ESP32-PICO-D4-IPEX-ESP-32-p-1321324.html
MP1584 | U3 | Board power supply | https://www.ebay.co.uk/itm/132921707182
USB To TTL FT232RL FTDI Serial Adapter 3.3V 5V | N/A | Used to program the board | https://www.ebay.co.uk/itm/222727769676
2.54mm pin header | N/A | To attach power board | https://www.ebay.co.uk/itm/263091291344
The PCB | N/A | The PCB | https://www.pcbway.com/project/shareproject/Chorus_32_Lap_Timer_PCB.html

Notes: 
* You don’t need to have all 6 nodes installed
* China post for the PCB's has always taken 2-3 weeks for me

## Optional parts

Part | Reference | Use |  link
------------ | ------------- | ------------- |  -------------
Buzzer | BZ1 | Beeping | https://www.ebay.co.uk/itm/153149159546
100nF 0805 | C1 | Decoupling 
470uF 25V electrolytic capacitor | C2 | bulk storage, <= 13.5mm height  
0805 LED | D1 | Power indicator 
0805 LED | D2 | ESP32 status LED 
NMOS SOT-23 FET GSD AO3400 or similar | Q1 | Buzzer control FET | https://www.ebay.co.uk/itm/223494178507
NMOS SOT-23 FET GSD AO3400 or similar | Q2-4 | LED control FET | https://www.ebay.co.uk/itm/223494178507
10k 0805 resistor | R3 | Buzzer enable pull down 
0805 resistor | R5 | ESP32 status LED resistor
47k 0805 resistor | R7 | Voltage divider pot down resistor
1.8k 0805 resistor | R8 | Voltage divider pot down resistor
1k 0805 resistor | R12 | Power indicator LED resistor 
NLAS4599DTT1G Analogue switch | U2 | switch to view battery voltage | https://www.ebay.co.uk/itm/351036086894
2.4GHz u.fl whip antenna | N/A | External antenna |
3 position switch DPDT ON-OFF-ON | N/A | Selects number of power receivers | https://www.ebay.co.uk/itm/131860761523
2 position switch | N/A | Main power switch | https://www.ebay.co.uk/itm/131860761523
4 PIN JST SM Male + Female | N/A | LED connector | https://www.ebay.co.uk/itm/192636180868
Heatsinks x6 | N/A | To extend RX5808 life | https://www.ebay.co.uk/itm/161834915171
Thermal adhesive | N/A | To attach the heatsinks | https://www.ebay.co.uk/itm/132406581708