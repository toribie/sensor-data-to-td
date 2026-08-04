const WebSocket = require('ws');

const wss = new WebSocket.Server({ port: 8080 });
console.log('WebSocket server is running on ws://localhost:8080');

// Connection event handler
wss.on('connection', (ws) => {
  console.log('New client connected');
  
  ws.send('Hello World!');

  ws.on('message', (message) => {
    console.log(`Received: ${message}`);
    // Echo the message back to the client
    ws.send(`Server received: ${message}`);
  });

  ws.on('close', () => {
    console.log('Client disconnected');
  });
}); 