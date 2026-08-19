const WebSocket = require("ws");
const os = require('os');
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

function getLocalIPv4Addresses() {
    const interfaces = os.networkInterfaces();
    const addresses = [];

    for (const [name, iface] of Object.entries(interfaces)) {
        for (const addr of iface) {
            // Filter for IPv4, non-internal (not loopback) addresses
            if (addr.family === 'IPv4' && !addr.internal) {
                addresses.push({
                    interface: name,
                    ip: addr.address,
                    netmask: addr.netmask,
                    cidr: addr.cidr,
                    mac: addr.mac,
                });
            }
        }
    }
    return addresses;
}

const addrs = getLocalIPv4Addresses();
console.log('Local IPv4 addresses:');
addrs.forEach(a => {
    console.log(`  ${a.interface}: ${a.ip} (${a.cidr}) mac=${a.mac}`);
});

console.log(`WebSocket server listening on port ${PORT}`);