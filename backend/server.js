const WebSocket = require("ws");
const os = require("os");

const PORT = 8080;
const wss = new WebSocket.Server({ port: PORT });

const clients = new Set();

wss.on("connection", (ws, req) => {
    console.log("Client connected:", req.socket.remoteAddress);
    clients.add(ws);

    ws.on("message", (data) => {
        try {
            const json = JSON.parse(data.toString());

            console.log("Received:", json);

            // Send Arduino data to all connected clients
            clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify({
                        type: "data",
                        data: json
                    }));
                }
            });

            // Response to the sender
            ws.send(JSON.stringify({
                type: "response",
                message: "Pong!"
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
        clients.delete(ws);
        console.log("Client disconnected");
    });

    ws.on("error", (err) => {
        console.error("Error:", err);
        clients.delete(ws);
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
addrs.forEach((a) => {
    console.log(`  ${a.interface}: ${a.ip} (${a.cidr}) mac=${a.mac}`);
});

console.log(`WebSocket server listening on port ${PORT}`);
