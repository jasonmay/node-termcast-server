var net = require('net');
var VT = require('./vt');
var stream_status = {};
var streams = {};

exports.streams = function() { return stream_status; }
exports.stream = function(id) { return stream_status[id]; }
exports.dispatch = function(data) {
  if (data.notice) {
    if (data.notice == 'connect') {
      var cdata = data.connection;
      var sid = cdata.session_id;

      if (!streams[sid]) streams[sid] = {};
      if (!stream_status[sid]) stream_status[sid] = {};

      stream_status[sid].ready = false;
      stream_status[sid].id = sid;
      stream_status[sid].user = cdata.user;
      stream_status[sid].last_active = cdata.last_active;

      // TODO DRY this up
      streams[sid].socket      = new net.Socket();
      streams[sid].socket.on('data', function(data) {
        var draw = streams[sid].vt.process(data);
        for (var i in streams[sid].websockets) {
          streams[sid].websockets[i].emit('draw', draw);
        }
      });
      streams[sid].socket.connect(cdata.socket, function() {
        console.log('ansi connect.');
        streams[sid].ready = true;
      });
    }
    else if (data.notice == 'metadata') {
      var metadata = data.metadata;
      var sid = data.session_id;
      var g = metadata.geometry;
      stream_status[sid].geometry = metadata.geometry;
      if (streams[sid].vt) streams[sid].vt.finish();
      streams[sid].vt = new VT(g[0], g[1]);
      stream_status[sid].ready = true;
      // TODO announce resizes here
    }
    else if (data.notice == 'disconnect') {
      var sid = data.session_id;
      streams[sid].socket.end();
      streams[sid].vt.finish();
      for (var i in streams[sid].websockets) {
        streams[sid].websockets[i].emit('disconnect');
      }
      console.log('ansi disconnect.');
      delete streams[sid];
    }
  }
  else if (data.response) {
    console.log(require('util').inspect(data, false, null));
    if (data.response == 'sessions') {
      var sessions = data.sessions;
      for (var i = 0; i < sessions.length; ++i) {
        var session = sessions[i];
        var sid = session.session_id;
        var g = session.geometry;

        if (!streams[sid]) streams[sid] = {};
        if (!stream_status[sid]) stream_status[sid] = {};

        stream_status[sid].id          = sid;
        stream_status[sid].user        = session.user;
        stream_status[sid].last_active = session.last_active;
        stream_status[sid].geometry    = session.geometry;

        // TODO DRY this up
        streams[sid].vt = new VT(g[0], g[1]);
        streams[sid].socket      = new net.Socket();
        streams[sid].socket.on('data', function(data) {
          var draw = streams[sid].vt.process(data);
          if (draw && draw.length) {
            for (var i in streams[sid].websockets) {
              streams[sid].websockets[i].emit('draw', draw);
            }
          }
        });
        streams[sid].socket.connect(session.socket, function() {
          streams[sid].ready = true;
        });
      }
    }
  }
}

exports.addWebsocket = function(id, ws) {
  if (streams[id]) {
    if (!streams[id].websockets) streams[id].websockets = []
    streams[id].websockets.push(ws);
    return true;
  }
  else {
    return false;
  }
}
