# Chorus32-ESP32Laptimer

Based on the popular Chorus RF Laptimer, this ESP32 port provides many significant advantages, not only does the ESP32 provide much more processing power than AVR based Arduino boards but it's built in Wifi and Bluetooth networking greatly improve connectivity options.

Compared to the original ChorusRF Lamptimer this ESP32 version only requires one RX module per pilot and a single ESP32 (nodemcu or similar) board. This allows you to connect your Lap timer wirelessly with no extra hardware required.

Hardware construction is also simplified as both parts are 3.3v logic and there is no need for level shifting or resistors.  

Updates:
-----

Important Notice for Chorus32 users!!!:

As of last commit the default pinout has been changed to match that of the PCBs currently being tested.

For anyone that has built a Chorus32 with original schematics do not worry, your unit will continue working with future updates. However you must comment out '#define Default_Pins " and un-comnent '//#define Old_Default_Pins' in 'HardwareConfig.h' when compiling.

Added OLED and VBAT measurement support
Auto RSSI threshold setup is also not implemented, just set thresholds manually for now.

Added inital webserver configuration portal
https://www.youtube.com/watch?v=BVd2t0yO_5A/0.jpg

Application Support:
-----
This devices communicates using the Chorus RF Laptimer API and _should_ work with LiveTime (untested) as well as the Chorus RF Lap timer  app
https://play.google.com/store/apps/details?id=app.andrey_voroshkov.chorus_laptimer


Hardware:
-----
Construction is easy and only requires some basic point to point wiring of each module to the ESP32 board.

See HardwareConfig.h for pin assignments, it is possible to change any pin assignments apart from ADC channels. Note that pin assignments are GPIO hardware pin numbers and not pin numbers specific to the particular ESP32 development board you may be using. 

PCB designs are currently being tested

![alt text](img/PCBv1.jpg)

![alt text](pcb/JyeSmith/PCBV2/Schematic_V2.png)

Performance:
-----
The Chorus32 Lap timer was compared to the $600USD ImmersionRC LapRF 8-Way at a local indoor event, arguably the worst conditions due to multipath and reflections. Results are presented below, you can see that the Chorus32 very closely matches the measured lap times of the LapRF.

![alt text](img/Comparison1.png)
![alt text](img/Comparison2.png)

Compiling the Project:
-----
~~Due to the fact that both the Bluetooth and Wifi stack are used quite alot of program memory is required. To compile the project you must choose 'Partition Scheme' -> Minimal SPIFFS in the Arduino IDE. Board should be selected as 'ESP32 Dev Module' in most cases.~~

As we are not supoorting bluetooth for now and are using the SPIFFS partition leave the Partition Scheme as 'default'

Library requirements:
-----
Adafruit_INA219
ESP8266 AND ESP32 OLED DRIVER FOR SSD1306 DISPLAY
