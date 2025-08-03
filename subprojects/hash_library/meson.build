# subprojects/hash-library/meson.build

project('hash_library', 'cpp',
  version: '1.0', # Dobra praktyka
  default_options: ['cpp_std=c++17']
)

# 1. Stwórz bibliotekę statyczną, która KOMPILUJE potrzebne pliki źródłowe.
#    To jest brakujący element, który naprawi błąd linkera.
lib_hashes = static_library('stbrumme_hashes_lib',
  'sha256.cpp',
  'sha3.cpp',
)

# 2. Zadeklaruj zależność, która udostępnia ZARÓWNO katalogi nagłówkowe,
#    JAK I skompilowaną bibliotekę do zlinkowania.
hash_library_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: lib_hashes
)