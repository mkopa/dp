# meson.build - WERSJA MODUŁOWA (Ostateczna, Gwarantowana)

project('dirpacker', 'c', 'cpp',
  version: '1.7.3', # Wersja z poprawnym declare_dependency
  default_options: [
    'cpp_std=c++17',
    'default_library=static'
  ])

# --- Zależności Podstawowe ---
cppcodec_dep = dependency('cppcodec', fallback: ['cppcodec', 'cppcodec_dep'])
blake3_dep = dependency('blake3', fallback: ['blake3', 'blake3_dep'])
hash_library_dep = dependency('hash_library', fallback: ['hash_library', 'hash_library_dep'])


if host_machine.system() == 'darwin'
  # Konfiguracja dla macOS
  sdl_source_path = meson.current_source_dir() + '/subprojects/SDL'
  sdl_install_path = meson.current_build_dir() + '/sdl2_install'

  message('Konfigurowanie SDL2 dla macOS...')
  run_command(
    'sh', '-c',
    'cd "@0@" && ./autogen.sh && ./configure --disable-shared --enable-static --prefix="@1@"'.format(sdl_source_path, sdl_install_path),
    check: true
  )

  message('Kompilowanie i instalowanie SDL2 dla macOS...')
  run_command(
    'sh', '-c',
    'cd "@0@" && make && make install'.format(sdl_source_path),
    check: true
  )
  message('SDL2 skonfigurowane i zainstalowane lokalnie na macOS.')

  # Użycie pkg-config dla macOS
  sdl_dep = dependency('sdl2', method: 'pkg-config')

else
# --- Zależności GUI ---

# 1. Budowanie i Zależność SDL2
  sdl_source_path = meson.current_source_dir() + '/subprojects/SDL'
  sdl_install_path = meson.current_build_dir() + '/sdl2_install'

# Krok 1: Konfiguracja SDL
  message('Konfigurowanie SDL2...')
  run_command(
    'sh', '-c',
    'cd "@0@" && ./autogen.sh && ./configure --disable-shared --enable-static --prefix="@1@"'.format(sdl_source_path, sdl_install_path),
    check: true
  )

  # Krok 2: Kompilacja i instalacja SDL
  message('Kompilowanie i instalowanie SDL2...')
  run_command(
    'sh', '-c',
    'cd "@0@" && make && make install'.format(sdl_source_path),
    check: true
  )
  message('SDL2 skonfigurowane i zainstalowane lokalnie.')

  # Krok 3: Stworzenie zależności na podstawie zbudowanej biblioteki
  sdl_inc = include_directories('sdl2_install/include/SDL2')
  sdl_lib = meson.get_compiler('cpp').find_library('SDL2', dirs: sdl_install_path + '/lib')
  sdl_main_lib = meson.get_compiler('cpp').find_library('SDL2main', dirs: sdl_install_path + '/lib')

  sdl_cflags_str = run_command(sdl_install_path + '/bin/sdl2-config', '--cflags', check: true).stdout().strip()
  sdl_libs_str = run_command(sdl_install_path + '/bin/sdl2-config', '--libs', check: true).stdout().strip()


  sdl_dep = declare_dependency(
    include_directories: sdl_inc,
    # Znalezione biblioteki muszą być w 'dependencies', a nie 'link_with'
    dependencies: [sdl_lib, sdl_main_lib],
    compile_args: sdl_cflags_str.split(),
    link_args: sdl_libs_str.split()
  )
endif

# 2. Zależność ImGui Core (bez zmian)
imgui_dep = dependency('imgui', fallback: ['imgui', 'imgui_dep'])


# 3. Zależność OpenGL (bez zmian)
opengl_dep = dependency('opengl')


# --- Definicja naszej biblioteki DirPacker (bez zmian) ---
dirpacker_inc = include_directories('include')
dirpacker_lib = library('dirpacker',
  'src/dirpacker.cpp',
  include_directories: dirpacker_inc,
  dependencies: [cppcodec_dep, blake3_dep, hash_library_dep],
  install: true,
)
dirpacker_dep = declare_dependency(
  link_with: dirpacker_lib,
  include_directories: dirpacker_inc
)

executable('dirpacker-cli',
  'app/main.cpp',
  dependencies: [dirpacker_dep],
  install: true
)

# --- CEL: Aplikacja GUI "AI Prompter" ---
imgui_backend_sources = files(
  'src/backends/imgui_impl_sdl2.cpp',
  'src/backends/imgui_impl_opengl3.cpp'
)
ai_prompter_args = ['-DIMGUI_IMPL_OPENGL_LOADER_GLAD2']

font_dest_dir = join_paths(meson.current_build_dir(), 'assets/fonts')
font_source_file = join_paths(meson.current_source_dir(), 'gui/assets/fonts/Roboto-Regular.ttf')

# Definiujemy cel, który kopiuje czcionkę i tworzy plik-stempel jako dowód.
font_copier = custom_target('copy_font_asset',
  # Oficjalnym wynikiem jest prosty plik-stempel. BEZ ŚCIEŻKI.
  output: 'font_copied.stamp',
  
  # To polecenie wykonuje całą pracę: tworzy katalog, kopiuje plik, a na końcu
  # tworzy plik-stempel (@OUTPUT@), aby zasygnalizować Mesonowi sukces.
  command: ['sh', '-c', 'mkdir -p @0@ && cp @1@ @0@/ && touch @OUTPUT@'.format(font_dest_dir, font_source_file)],
  
  # Ten cel musi być zbudowany domyślnie.
  build_by_default: true
)

# Osobna definicja dla instalacji - czysty podział odpowiedzialności.
install_data(
  'gui/assets/fonts/Roboto-Regular.ttf',
  install_dir: get_option('datadir') / 'ai_prompter/assets/fonts'
)

executable('ai_prompter',
  'gui/main.cpp',
  'gui/Themes.cpp', # <- DODANY NOWY PLIK ŹRÓDŁOWY
  imgui_backend_sources,
  font_copier,
  cpp_args: ai_prompter_args,
  include_directories: 'src/backends',
  dependencies: [
    dirpacker_dep,
    imgui_dep,
    sdl_dep,
    opengl_dep
  ],
  install: true
)
