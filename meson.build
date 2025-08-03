# meson.build - WERSJA PRZENOŚNA (Linux + Windows)

project('dirpacker', 'c', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++17'])

cppcodec_dep = dependency('cppcodec', fallback: ['cppcodec', 'cppcodec_dep'])
blake3_dep = dependency('blake3', fallback: ['blake3', 'blake3_dep'])
hash_library_dep = dependency('hash_library', fallback: ['hash_library', 'hash_library_dep'])

executable('dirpacker',
  'src/main.cpp',
  dependencies: [cppcodec_dep, blake3_dep, hash_library_dep],
  install: true
)