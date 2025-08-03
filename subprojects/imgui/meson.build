# meson.build dla Dear ImGui (minimalistyczny i poprawny)

project('imgui', 'cpp', default_options: ['warning_level=0'])

# Źródła rdzenia biblioteki. Nic więcej.
imgui_sources = files(
  'imgui.cpp',
  'imgui_draw.cpp',
  'imgui_tables.cpp',
  'imgui_widgets.cpp',
  'imgui_demo.cpp'
)

# Budujemy bibliotekę statyczną z samego rdzenia
imgui_lib = static_library('imgui', imgui_sources)

# Deklarujemy zależność, która udostępnia tylko rdzeń ImGui i jego nagłówki
imgui_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with: imgui_lib
)