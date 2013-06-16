var ffi   = require('ffi');
var createVT = require('./build/Release/vtchanges');
var vtchanges = createVT(80, 24);

var p = vtchanges.process("one\r\n\033[0;32mtwo\033[m\r\nt\033[1;34mhre\033[me");
console.log(require('util').inspect(p, false, null));

vtchanges.finish();
