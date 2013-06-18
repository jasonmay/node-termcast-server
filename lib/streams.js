var streams = {};

exports.streams = function() { return streams; }
exports.dispatch = function(data) {
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
}
