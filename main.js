var ref = require('ref');
var ffi = require('ffi');

var libvt = ffi.Library('./libvterm/.libs/libvterm', {
  'vterm_new': ['pointer', ['int', 'int']],
  'vterm_free': ['void', ['pointer']],
  'vterm_obtain_screen': ['pointer', ['pointer']],
  'vterm_push_bytes': ['void', ['pointer', 'string', 'size_t']],
});

var vterm_ptr = libvt.vterm_new(80, 24);
libvt.vterm_push_bytes(vterm_ptr, "HElLO!", 6);
var vterm_screen =libvt.vterm_obtain_screen(vterm_ptr);
console.log(vterm_screen);
libvt.vterm_free(vterm_ptr);
