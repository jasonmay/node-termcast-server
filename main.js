var ffi   = require('ffi');
var libvt = ffi.Library('./ext/.libs/libtermcastserver', {
  'jason': ['int', []],
});

console.log(libvt.jason());
