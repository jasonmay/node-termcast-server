var ffi   = require('ffi');
var createObject = require('./build/Release/vtchanges');
var vtchanges = createObject(10);

console.log(vtchanges.process());
console.log(vtchanges.process());
console.log(vtchanges.process());
console.log(vtchanges.process());

vtchanges.finish();
