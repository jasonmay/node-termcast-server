var express = require('express')
  , app     = express()
  , server  = require('http').createServer(app)
  , io      = require('socket.io').listen(server)
  , net     = require('net')
  , jstream = require('JSONStream')
  , es      = require('event-stream')
  , config  = require('../config')
  , streams = require('./streams');

// Terminal stream tracking stuff
var manager_parser = new jstream.parse();
var socket = new net.Socket({type: 'unix'});
socket.pipe(manager_parser).pipe(es.mapSync(streams.dispatch));

// Express stuff

function error(status, msg) {
  var err = new Error(msg);
  err.status = status;
  return err;
}

app.use(express.logger());

app.use('/', express.static(__dirname + '/../public'));
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

app.get('/data/streams', function(req, res, next) {
  res.send(streams.streams());
});

socket.connect(config.manager, function() {
  socket.write('{"request":"sessions"}');
  server.listen(4000);
  console.log('Web server listening on port 4000');
});
