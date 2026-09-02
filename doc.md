board used: ESP32 S3-N16R8
(select ESP32S3 Dev Module in Arduino IDE)

https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass/
https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.h

L'écran est branché au port suivant:
SDA: GPIO8
SCL: GPIO9
GND: GND
VCC: 3V3

Joystick:
GND: GND
+5V: 3V3
VRx: GPIO11
VRy: GPIO12
SW: GPIO10

For testing server:
wscat -c ws://localhost:8080 

run backend: node server.js
run frontend: npm run dev

example message: 
{"ax":11.3,"volume":32.7}

Average delay between each update: 133.57 ms