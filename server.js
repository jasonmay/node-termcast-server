var app = require('express')()
  , server = require('http').createServer(app)
  , io = require('socket.io').listen(server);

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

app.get('/data/streams', function(req, res, next) {
  res.send({fortytwo: 42});
});

if (!module.parent) {
  server.listen(4000);
  console.log('Web server listening on port 4000');
}
