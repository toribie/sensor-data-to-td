board used: ESP32 S3-N16R8
(select ESP32S3 Dev Module in Arduino IDE)

https://docs.arduino.cc/language-reference/en/functions/wifi/wificlass/


For testing server:
wscat -c ws://localhost:8080 

run the server: node server.js

example message: 
{"ax":11.3,"volume":32.7}

Average delay between each update: 133.57 ms