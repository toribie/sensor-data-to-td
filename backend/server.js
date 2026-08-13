const WebSocket = require("ws");
const PORT = 8080;
const wss = new WebSocket.Server({ port: PORT });

wss.on("connection", (ws, req) => {
    console.log("Client connected:", req.socket.remoteAddress);

    ws.on("message", (data) => {
        console.log("Received:", data.toString());

        try {
            const json = JSON.parse(data.toString());

            console.log("Parsed JSON:", json);

            ws.send(JSON.stringify({
                type: "response",
                message: "Hello from Node.js!"
            }));
        } catch (err) {
            console.error("Invalid JSON:", err.message);

            ws.send(JSON.stringify({
                type: "error",
                message: "Invalid JSON"
            }));
        }
    });

    ws.on("close", () => {
        console.log("Client disconnected");
    });

    ws.on("error", (err) => {
        console.error("Error:", err);
    });
});

console.log(`WebSocket server listening on port ${PORT}`);