board used: ESP32 S3-N16R8
(select ESP32S3 Dev Module in Arduino IDE)

https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass/
https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.h

Screen:
SDA: GPIO8
SCL: GPIO9
GND: GND
VCC: 3V3

Gyro (MPU-6050):
GND: GND
VCC: 5V
SCL: GPIO11
SDA: GPIO10


For testing server:
wscat -c ws://localhost:8080 

run backend: node server.js
run frontend: npm run dev

To connect the frontend and backend: - go to ./frontend/src/App.jsx
                                     - edit const ws to match the backend ip

example message:
{"ax":11.3,"volume":32.7}
