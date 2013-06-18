var express = require('express')
  , app     = express()
  , server  = require('http').createServer(app)
  , io      = require('socket.io').listen(server)
  , net     = require('net')
  , jstream = require('JSONStream')
  , config  = require('../config');

// Terminal stream tracking stuff
var streams = {};
var manager_parser = new jstream.parse();
var socket = new net.Socket({type: 'unix'});
socket.on('data', function (message) {
  manager_parser.write(message);
});
manager_parser.on('data', function (data) {

  // TODO make a class for this stuff
  if (data.notice) {
    if (data.notice == 'connect') {
      var cdata = data.connection;
      var sid = cdata.session_id;

      if (!streams[sid]) streams[sid] = {};
      streams[sid].ready = false;
      streams[sid].id = sid;
      streams[sid].user = cdata.user;
      streams[sid].last_active = cdata.last_active;
    }
    else if (data.notice == 'metadata') {
      var metadata = data.metadata;
      var sid = data.session_id;
      streams[sid].geometry = metadata.geometry;
      streams[sid].ready = true;
      // TODO announce resizes here
    }
    else if (data.notice == 'disconnect') {
      var sid = data.session_id;
      delete streams[sid];
    }
  }
  else if (data.response) {
    if (data.response == 'sessions') {
      var sessions = data.sessions;
      for (var i = 0; i < sessions.length; ++i) {
        var session = sessions[i];
        var sid = session.session_id;
        if (!streams[sid]) streams[sid] = {};
        streams[sid].id          = sid;
        streams[sid].user        = session.user;
        streams[sid].last_active = session.last_active;
        streams[sid].geometry    = session.geometry;
        streams[sid].ready       = true;
      }
    }
  }
});

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
  res.send(streams);
});

socket.connect(config.manager, function() {
  socket.write('{"request":"sessions"}');
  server.listen(4000);
  console.log('Web server listening on port 4000');
});
