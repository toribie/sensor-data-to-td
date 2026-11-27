This project has for purpose to transfer accelerometer and other sensor data from an Arduino to send to
TouchDesigner.
Currently the yaw has a tendency to drift so it is necessary to recalibrate by resetting after some
period of time.

## What you need

- Node.js `^20.19.0` or `>=22.12.0` and npm
- Arduino IDE with the ESP32 board support package
- An ESP32-S3 development board, MPU-6050, and 128x64 SSD1306 OLED
- A server on the same local Wi-Fi as the ESP32.

## Project layout

- `main/` - ESP32 firmware, Wi-Fi setup portal, OLED, and sensor code
- `backend/` - WebSocket server on port `8080`
- `frontend/` - Dashboard made in React, currently only used to see all the data.
- `touch-designer/` - TouchDesigner project files

## Setup

### Wiring

| Device | Signal | ESP32-S3 |
| --- | --- | --- |
| SSD1306 OLED | SDA | GPIO 8 |
| SSD1306 OLED | SCL | GPIO 9 |
| SSD1306 OLED | VCC / GND | 3V3 / GND |
| MPU-6050 | SDA | GPIO 10 |
| MPU-6050 | SCL | GPIO 11 |
| MPU-6050 | VCC / GND | 5V / GND |

### 1. Backend

In a terminal at the project root:

```sh
cd backend
npm ci
node server.js
```

The server listens on port `8080` and prints its local IP address on startup.

### 2. Frontend

In `frontend/src/App.jsx`, replace the WebSocket URL with the server's address:

```js
const ws = new WebSocket("ws://<SERVER-IP>:8080");
```

For example, use `ws://192.168.1.25:8080` if that is the computer's local IP.
Then, in another terminal:

```sh
cd frontend
npm ci
npm run dev
```

Open the local URL printed by Vite (usually `http://localhost:5173`).
To open the dashboard from another device on the network, start Vite with
`npm run dev -- --host 0.0.0.0` and open the network URL it prints.

### 3. TouchDesigner

Open `main.toe` and click on `ws_input`, in the section `Connect`
change the Network Address to the server IP.

### 4. Arduino IDE

Install these libraries through Arduino IDE's Library Manager:

- ArduinoJson
- WebSockets by Markus Sattler
- Adafruit MPU6050
- Adafruit Unified Sensor
- Adafruit SSD1306
- Adafruit GFX Library

Open `main/main.ino`, select **ESP32S3 Dev Module**, and upload the sketch.

On bootup, hold the board's **PRG** button
while powering it on. Within a few seconds, connect to the `ESP32-Setup`
Wi-Fi network (password `76543210`) and open `http://192.168.4.1`. Enter the
Wi-Fi name and password, and the server's local IP as **Server IP**. Save
and let the board restart.

Once connected, the ESP32 sends data to the server. Keep the
backend running while using the dashboard & TouchDesigner.


## Adding new sensors

### ESP32

Write the necessary code to collect the sensor data, preferably in a function
as it will be called in sendSensorData.
Modify sendSensorData as necessary to accomodate for any new input.

### TouchDesigner

Below is the general workflow inside `main.toe`, `sensor_controller` shouldn't need any modifications when implementing
new features.

1. `ws_input`

This is where TD receives the data.

2. `sensor_controller`

Its role is to convert the data in JSON format to one akin to a python dictionary.
Note that if a data is written like so in the code: `doc[key][value]` will be written as `key_value`.

3. `gyro_values` + `sensor_values` + `...`

The sensor data is converted into channels to be used within TD.
To add new channels refer to the documentation inside `sensor_values1_callbacks`.