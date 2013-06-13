var ffi   = require('ffi');
var libvt = ffi.Library('./ext/libtermchanges/.libs/libtermchanges', {
  'jason': ['int', []],
});

console.log(libvt.jason());
