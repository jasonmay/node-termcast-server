{
  'targets': [
    {
      'target_name': 'vterm',
      'product_prefix': 'lib',
      'type': 'static_library',
      'standalone_static_library': 1,
      'sources': [
        'src/encoding.c',
        'src/input.c',
        'src/parser.c',
        'src/pen.c',
        'src/screen.c',
        'src/state.c',
        'src/unicode.c',
        'src/vterm.c'
      ],
      'include_dirs': [
        'include',
        # platform and arch-specific headers
        'config/<(OS)/<(target_arch)'
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'include',
          'config/<(OS)/<(target_arch)'
        ]
      },
      'cflags': ['-std=c99']
    }
  ]
}
