# subprojects/cppcodec/meson.build

project('cppcodec', 'cpp')

# Deklarujemy zależność, którą udostępnimy głównemu projektowi.
# cppcodec jest biblioteką nagłówkową, więc wystarczy
# wskazać jej katalog z plikami .hpp
cppcodec_dep = declare_dependency(
  include_directories: include_directories('.')
)

# Ta zmienna musi być zadeklarowana, aby główny projekt mógł
# ją znaleźć za pomocą dependency('cppcodec'). Nazwa zmiennej
# jest domyślnie taka sama jak nazwa projektu.