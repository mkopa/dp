project('SQLiteCpp', 'cpp')

# SQLiteCpp wymaga biblioteki sqlite3
sqlite3_dep = dependency('sqlite3', fallback: ['sqlite3', 'sqlite3_dep'])

# Lista plików źródłowych biblioteki
sqlitecpp_sources = files(
  'src/Backup.cpp',
  'src/Database.cpp',
  'src/Column.cpp',
  'src/Statement.cpp',
  'src/Transaction.cpp',
  'src/Exception.cpp'
)

# Nagłówki znajdują się w katalogu include
sqlitecpp_inc = include_directories('include')

# Budujemy bibliotekę statyczną
sqlitecpp_lib = static_library('sqlitecpp',
  sqlitecpp_sources,
  include_directories: sqlitecpp_inc,
  dependencies: [sqlite3_dep],
  cpp_args: ['-DSQLITE_ENABLE_COLUMN_METADATA=1'], # Ważna flaga dla pełnej funkcjonalności
  install: false,
)

# Deklarujemy zależność, którą będzie można użyć w głównym projekcie
SQLiteCpp_dep = declare_dependency(
  link_with: sqlitecpp_lib,
  include_directories: sqlitecpp_inc,
  dependencies: [sqlite3_dep]
)