# Racing external hud

### Summary

Project that show the real Audi GT3 display in a nextion display with arduino for racing games.

### Compatible games

* RaceRoom Racing Experience
* Assetto corsa competizione

### Needed hardware and software

* Arduino device (I use ESP32)
* Arduino IDE (Download link: https://www.arduino.cc/en/main/software)
* ITEADLIB_Arduino_Nextion library (Download link: https://github.com/itead/ITEADLIB_Arduino_Nextion)
* Nextion display (I use NX8048K070)
* Breadboard and wires
* Micro USB charger (Need a 2A charger for NX8048K070)
* Micro SD card
* (Optionnal) Nextion Editor (Download link: https://nextion.tech/nextion-editor-ver-0-58/)

### Instalation

##### Step 1 Arduino (that step need only to be done once)

Firstly you need to add Arduino Nextion library to your Arduino IDE and configure it for your device.

After that you need to configure the arduino.ino file and put your WIFI SSID and WIFI password, and IP adress of your computer that will run the game.

Then you need to upload ./arduino/arduino.ino into your arduino device with Arduino IDE.

##### Step 2 Nextion (that step need only to be done once)

Copy the display.tft file into an empty Micro SD card and put it into your Nextion device.

Power on your Nextion device and wait until the loading is finished.

Power off your Nextion device and remove the Micro SD card.

##### Step 3 Wires connection (with ESP32)

In order to allow your Arduino device to communicate with your Nextion display you need to insert the yellow wire into the G17 pin and the blue wire into the G16 pin.

##### Step 4 Game software

Now you need to launch the server software that will communicate with your arduino device.

You only need to start the .exe file of the game of your choice. Now start the game and enjoy :)

### Open source

All files have a compiled and an open source version so you can modify them as you want.


