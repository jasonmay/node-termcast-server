{
  'targets': [
    {
      'target_name': 'vtchanges',
      'sources': [
        'src/addon.cc',
        'src/vtchanges.cc'
      ],
      'dependencies': [
        'deps/libvterm/libvterm.gyp:vterm'
      ],
    }
  ]
}
