// import express from 'express';
// import dotenv from 'dotenv';
// import morgan from 'morgan';
// import { connectDB } from './config/db.js';
// import router from './routes/router.js';
// dotenv.config();
// const app = express();

// app.use(morgan("dev"));
// app.use(express.json());
// app.use("/api/v1/", router);
// app.get('/Test', (req, res) => {
//     res.status(200).send("<h1>Hello World</h1>");
// });

// app.listen(process.env.PORT, () => {
//     connectDB.query("SELECT 1").then(()=>{
//         console.log("Database connected successfully");
//     });
//     console.log(`Server is running on port ${process.env.PORT}`);
// });

// import express from 'express';
// import { Server } from 'socket.io';
// import http from 'http';
// import dotenv from 'dotenv';
// import morgan from 'morgan';
// import { connectDB } from './config/db.js';
// import router from './routes/router.js';

// dotenv.config();

// const app = express();
// app.use(morgan('dev'));

// // HTTP Server
// const server = http.createServer(app);
// const io = new Server(server, {
//     cors: {
//         origin: '*', // Điều chỉnh nếu cần để phù hợp với domain của app
//         methods: ['GET', 'POST'],
//     },
// });

// // Room handling logic
// // Các namespaces cho ESP8266 và Flutter
// const espNamespace = io.of('/esp');
// const flutterNamespace = io.of('/flutter');

// // Xử lý kết nối cho ESP8266
// espNamespace.on('connection', (socket) => {
//     console.log('ESP8266 connected:', socket.id);

//     // Nhận dữ liệu từ ESP8266
//     socket.on('data', (data) => {
//         console.log('Data from ESP8266:', data);
//         // Chuyển tiếp dữ liệu tới Flutter
//         flutterNamespace.emit('update_data', data);
//     });

//     // Xử lý khi ESP8266 ngắt kết nối
//     socket.on('disconnect', () => {
//         console.log('ESP8266 disconnected:', socket.id);
//     });
// });

// // Xử lý kết nối cho Flutter
// flutterNamespace.on('connection', (socket) => {
//     console.log('Flutter app connected:', socket.id);

//     // Nhận lệnh từ Flutter và chuyển tiếp tới ESP8266
//     socket.on('command', (command) => {
//         console.log('Command from Flutter:', command);
//         espNamespace.emit('esp_command', command);
//     });
//     // Xử lý khi Flutter ngắt kết nối
//     socket.on('disconnect', () => { 
//         console.log('Flutter app disconnected:', socket.id);
//     });
// });

// app.use(express.json());
// app.use("/api/v1/", router);
// // Route for testing
// app.get('/test', (req, res) => {
//     res.send('<h1>Server is running</h1>');
// });

// // Start server
// const PORT = process.env.PORT || 3000;
// server.listen(PORT, () => {
//     connectDB.query("SELECT 1").then(() => {
//         console.log("Database connected successfully");
//     });
//     console.log(`Server running on http://localhost:${PORT}`);
// });


import express from 'express';
import { WebSocketServer, WebSocket } from 'ws';
import http from 'http';
import dotenv from 'dotenv'; 
import morgan from 'morgan';
import { connectDB } from './config/db.js';
import router from './routes/router.js';
dotenv.config();
const app = express();
app.use(morgan('dev')); 
app.use(express.json());


app.use("/api/v1/", router);
app.get('/Test', (req, res) => {
    res.status(200).send("<h1>Hello World</h1>");
});

// Tạo HTTP Server để dùng chung với WebSocket
const server = http.createServer(app);

// Tạo WebSocket Server
const wss = new WebSocketServer({ server });

// Đăng ký sự kiện khi có client kết nối
wss.on('connection', (ws) => {
    console.log('A new client has connected!'); 
    // Gửi thông báo tới client
    ws.send('connected');
    // Lắng nghe khi client gửi message
    ws.on('message', (message) => {
        console.log('Received message:', message.toString());
        // Gửi thông tin về client đã gửi đến tất cả các client khác
        wss.clients.forEach((client) => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(message.toString());
            }
        });
    });

    // Xử lý khi client ngắt kết nối
    ws.on('close', () => {
        console.log('A client has disconnected.'); 
    });
});


const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
    connectDB.query("SELECT 1").then(() => {
        console.log("Database connected successfully");
    });
    console.log(`Server running on http://localhost:${PORT}`);
});