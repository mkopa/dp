# meson.build - WERSJA MODUŁOWA (biblioteka + aplikacja)

project('dirpacker', 'c', 'cpp',
  version: '1.3.0',
  default_options: [
    'cpp_std=c++17',
    'default_library=static'
  ])

# --- Zależności zewnętrzne (w subprojektach) ---
cppcodec_dep = dependency('cppcodec', fallback: ['cppcodec', 'cppcodec_dep'])
blake3_dep = dependency('blake3', fallback: ['blake3', 'blake3_dep'])
hash_library_dep = dependency('hash_library', fallback: ['hash_library', 'hash_library_dep'])

# --- Definicja naszej biblioteki DirPacker ---

# Deklarujemy, gdzie są publiczne nagłówki naszej biblioteki
dirpacker_inc = include_directories('include')

# Budujemy bibliotekę (statyczną lub dynamiczną)
dirpacker_lib = library('dirpacker',
  'src/dirpacker.cpp',
  include_directories: dirpacker_inc,
  dependencies: [cppcodec_dep, blake3_dep, hash_library_dep],
  install: true,
)

# Tworzymy obiekt "zależności" dla naszej biblioteki
dirpacker_dep = declare_dependency(
  link_with: dirpacker_lib,
  include_directories: dirpacker_inc
)


# --- Cel główny (plik wykonywalny) ---

executable('dirpacker',
  'app/main.cpp',
  dependencies: [dirpacker_dep],
  install: true
)