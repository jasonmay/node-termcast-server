var ffi   = require('ffi');
var createVT = require('./build/Release/vtchanges');
var vtchanges = createVT(80, 24);

console.log(require('util').inspect(vtchanges.process("one\r\ntwo\r\nthree"), false, null));

vtchanges.finish();
