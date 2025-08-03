project('dirpacker', 'c', 'cpp',
  version: '1.7.3',
  default_options: [
    'cpp_std=c++17',
    'default_library=static',
    'b_staticpic=true'
  ])

add_project_arguments('-fPIC', language: ['c', 'cpp'])
add_project_link_arguments('-fPIC', language: ['c', 'cpp'])
add_project_link_arguments(
  '-Wl,-no_compact_unwind',
  '-Wl,-no_function_starts',
  language: ['c', 'cpp']
)

# --- Zależności Podstawowe ---
cppcodec_dep = dependency('cppcodec', fallback: ['cppcodec', 'cppcodec_dep'])
blake3_dep = dependency('blake3', fallback: ['blake3', 'blake3_dep'])
hash_library_dep = dependency('hash_library', fallback: ['hash_library', 'hash_library_dep'])

# --- Zależności GUI ---
# 1. Zależność SDL2 - warunkowa konfiguracja
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
  # Konfiguracja dla Linux i innych systemów
  sdl_source_path = meson.current_source_dir() + '/subprojects/SDL'
  sdl_install_path = meson.current_build_dir() + '/sdl2_install'

  message('Konfigurowanie SDL2 dla Linux...')
  run_command(
    'sh', '-c',
    'cd "@0@" && ./autogen.sh && ./configure --disable-shared --enable-static --prefix="@1@"'.format(sdl_source_path, sdl_install_path),
    check: true
  )

  message('Kompilowanie i instalowanie SDL2 dla Linux...')
  run_command(
    'sh', '-c',
    'cd "@0@" && make && make install'.format(sdl_source_path),
    check: true
  )
  message('SDL2 skonfigurowane i zainstalowane lokalnie na Linux.')

  # Pobranie flag z SDL2-config
  sdl_cflags_str = run_command(sdl_install_path + '/bin/sdl2-config', '--cflags', check: true).stdout().strip()
  sdl_libs_str = run_command(sdl_install_path + '/bin/sdl2-config', '--libs', check: true).stdout().strip()

  # Konfiguracja zależności dla Linux
  sdl_inc = include_directories(sdl_install_path + '/include/SDL2')
  sdl_lib = meson.get_compiler('cpp').find_library('SDL2', dirs: sdl_install_path + '/lib')
  sdl_main_lib = meson.get_compiler('cpp').find_library('SDL2main', dirs: sdl_install_path + '/lib')

  sdl_dep = declare_dependency(
    include_directories: sdl_inc,
    dependencies: [sdl_lib, sdl_main_lib],
    compile_args: sdl_cflags_str.split(),
    link_args: sdl_libs_str.split()
  )
endif

# 2. Zależność ImGui Core
imgui_dep = dependency('imgui', fallback: ['imgui', 'imgui_dep'])

# 3. Zależność OpenGL
opengl_dep = dependency('opengl')

# --- Definicja biblioteki DirPacker ---
dirpacker_inc = include_directories('include')
dirpacker_lib = library('dirpacker',
  'src/dirpacker.cpp',
  include_directories: dirpacker_inc,
  dependencies: [cppcodec_dep, blake3_dep, hash_library_dep],
  install: true,
  pic: true,
)
dirpacker_dep = declare_dependency(
  link_with: dirpacker_lib,
  include_directories: dirpacker_inc
)

# --- Aplikacja konsolowa ---
executable('dirpacker-cli',
  'app/main.cpp',
  dependencies: [dirpacker_dep],
  install: true
)

# --- Aplikacja GUI "AI Prompter" ---
imgui_backend_sources = files(
  'src/backends/imgui_impl_sdl2.cpp',
  'src/backends/imgui_impl_opengl3.cpp'
)
ai_prompter_args = [
  '-DIMGUI_IMPL_OPENGL_LOADER_GLAD2',
  '-DFORCE_ENDIAN_H=1',
  '-D__APPLE_COMMON_H=1'
]

executable('ai_prompter',
  'gui/main.cpp',
  imgui_backend_sources,
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
