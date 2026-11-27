import { useEffect, useState } from "react";
import "./App.css";

function SensorValue({ name, value }) {
  // Sensor contains multiple properties
  if (value !== null && typeof value === "object") {
    return (
      <div className="sensor">
        <strong>{name}:</strong>{" "}
        {Object.entries(value)
          .map(([key, val]) => `${key}: ${val}`)
          .join(", ")}
      </div>
    );
  }

  // Sensor is a simple value
  return (
    <div className="sensor">
      <strong>{name}:</strong> {String(value)}
    </div>
  );
}

function App() {
  const [connected, setConnected] = useState(false);
  const [sensorData, setSensorData] = useState({});

  useEffect(() => {
    const ws = new WebSocket("ws://192.168.30.29:8080");      // Put server IP here.

    ws.onopen = () => {
      console.log("WebSocket connected");
      setConnected(true);
    };

    ws.onmessage = (event) => {
      try {
        const message = JSON.parse(event.data);

        console.log("WebSocket message:", message);

        let sensorMessage = null;

        if (message.type === "sensor_data") {
          sensorMessage = message;
        } else if (
          message.type === "data" &&
          message.data?.type === "sensor_data"
        ) {
          sensorMessage = message.data;
        }

        if (!sensorMessage) {
          return;
        } 

        // Remove "type" from the sensor data
        const { type, ...sensors } = sensorMessage;

        console.log("Sensor data:", sensors);

        setSensorData(sensors);
      } catch (error) {
        console.error("Could not parse message:", error);
      }
    };

    ws.onclose = () => {
      console.log("WebSocket disconnected");
      setConnected(false);
    };

    ws.onerror = (error) => {
      console.error("WebSocket error:", error);
      setConnected(false);
    };

    return () => {
      ws.close();
    };
  }, []);

  return (
    <div className="app">
      <div className={`status ${connected ? "connected" : "disconnected"}`}>
        {connected ? "Connected" : "Disconnected"}
      </div>

      <div className="card">
        {Object.keys(sensorData).length === 0 ? (
          <p>Waiting for data...</p>
        ) : (
          Object.entries(sensorData).map(([name, value]) => (
            <SensorValue
              key={name}
              name={name}
              value={value}
            />
          ))
        )}
      </div>
    </div>
  );
}

export default App;