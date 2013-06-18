var app = require('express')()
  , server = require('http').createServer(app)
  , io = require('socket.io').listen(server)
  , net = require('net')
  , config = require('./config');

function error(status, msg) {
  var err = new Error(msg);
  err.status = status;
  return err;
}

app.use('/data', function (req, res, next) {
  next();
});

app.use(app.router);

app.use(function(err, req, res, next) {
  res.send(err.status || 500, { error: err.message });
});

app.use(function(req, res) {
  res.send(404, { error: "File not found" });
});

var streams = {};
var socket = new net.Socket({type: 'unix'});
socket.on('data', function (message) {
  data = JSON.parse(message);
  streams = data;
  if (data.notice) {
    if (data.notice == 'connect') {
      console.log("CONNECT!");
    }
    else if (data.notice == 'metadata') {
      console.log("METADATA!");
    }
    else if (data.notice == 'disconnect') {
      console.log("DISCONNECT!");
    }
  }
  else {
    console.log('response ehhhh?');
  }
});

app.get('/data/streams', function(req, res, next) {
  res.send(streams);
});

socket.connect(config.manager, function() {
  socket.write('{"request":"sessions"}');
  server.listen(4000);
  console.log('Web server listening on port 4000');
});
