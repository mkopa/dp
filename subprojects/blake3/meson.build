# subprojects/blake3/meson.build

project('blake3', 'c', 'cpp',
  version: '1.3.1', # Używam oficjalnej wersji jako przykład
  license: 'CC0-1.0 OR Apache-2.0'
)

# --- Logika budowania BLAKE3 przeniesiona z głównego pliku ---

c_compiler = meson.get_compiler('c')

# 1. Zbuduj małe biblioteki SIMD w C z odpowiednimi flagami
blake3_simd_libs = []
simd_files_c = {
  'sse2': '-msse2',
  'sse41': '-msse4.1',
  'avx2': '-mavx2',
  'avx512': '-mavx512f -mavx512vl -mavx512bw -mavx512dq'
}

foreach name, flag : simd_files_c
  # Meson automatycznie tłumaczy flagi GCC na flagi MSVC (np. -mavx2 na /arch:AVX2)
  if c_compiler.has_argument(flag)
    lib = static_library(
      'blake3-' + name,
      'blake3_' + name + '.c',
      c_args: flag.split()
    )
    blake3_simd_libs += lib
  endif
endforeach

# 2. Wybierz odpowiednie pliki asemblera w zależności od systemu i kompilatora
blake3_asm_sources = []
host_system = host_machine.system()

if host_system == 'windows'
  if c_compiler.get_id() == 'msvc'
    # Używamy plików .asm dla kompilatora Microsoftu
    blake3_asm_sources = [
      'blake3_sse2_x86-64_windows_msvc.asm',
      'blake3_sse41_x86-64_windows_msvc.asm',
      'blake3_avx2_x86-64_windows_msvc.asm',
      'blake3_avx512_x86-64_windows_msvc.asm'
    ]
  else
    # Używamy plików .S dla MinGW/GCC na Windows
    blake3_asm_sources = [
      'blake3_sse2_x86-64_windows_gnu.S',
      'blake3_sse41_x86-64_windows_gnu.S',
      'blake3_avx2_x86-64_windows_gnu.S',
      'blake3_avx512_x86-64_windows_gnu.S'
    ]
  endif
elif host_system == 'linux' or host_system == 'darwin'
  # Używamy plików .S dla systemów uniksowych
  blake3_asm_sources = [
    'blake3_sse2_x86-64_unix.S',
    'blake3_sse41_x86-64_unix.S',
    'blake3_avx2_x86-64_unix.S',
    'blake3_avx512_x86-64_unix.S'
  ]
endif

# Lista "zwykłych" plików C, wspólnych dla wszystkich systemów
blake3_normal_sources = [
  'blake3.c',
  'blake3_dispatch.c',
  'blake3_portable.c',
]

# 3. Zbuduj główną bibliotekę BLAKE3
blake3_lib = static_library(
  'blake3',
  blake3_normal_sources + blake3_asm_sources,
  link_with: blake3_simd_libs
)

# Deklarujemy finalną zależność, która będzie używana przez główny projekt
blake3_dep = declare_dependency(
  link_with: blake3_lib,
  include_directories: include_directories('.') # Udostępniamy nagłówki z bieżącego katalogu
)