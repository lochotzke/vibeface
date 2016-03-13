// https://github.com/websockets/ws

'use strict';

var server = require('http').createServer()
    , WebSocketServer = require('ws').Server
    , wss = new WebSocketServer({ server: server })
    , ws = null
    , express = require('express')
    , app = express()
    , ipaddress = process.env.OPENSHIFT_NODEJS_IP || '127.0.0.1'
    , port = process.env.OPENSHIFT_NODEJS_PORT || 8080
    , vibrate = false;

app.get('/', function (req, res) {
    console.log('app.get /');
    vibrate = !vibrate;
    res.send({ connected: ws !== null, vibrate: vibrate });
    if (ws) {
        ws.send(JSON.stringify({ vibrate: vibrate }));
    }
});

app.get('/on', function (req, res) {
    console.log('app.get /on');
    vibrate = true;
    res.send({ connected: ws !== null, vibrate: vibrate });
    if (ws) {
        ws.send(JSON.stringify({ vibrate: vibrate }));
    }
});

app.get('/off', function (req,res) {
    console.log('app.get /off');
    vibrate = false;
    res.send({ connected: ws !== null, vibrate: vibrate });
    if (ws) {
        ws.send(JSON.stringify({ vibrate: vibrate }));
    }
});

wss.on('connection', function (socket) {
    console.log('wss.on connection');
    socket.on('message', function (message) {
        console.log('socket.on message');
        socket.send(message);
    });
    socket.on('close', function () {
        console.log('socket.on close');
        ws = null;
    });
    socket.send(JSON.stringify({ vibrate: vibrate }));
    ws = socket;
});

server.on('request', app);
server.listen(port, ipaddress, function () {
    console.log('server listening on ' + ipaddress + ':' + port);
});

