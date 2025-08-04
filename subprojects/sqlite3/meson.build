project('sqlite3', 'c')

sqlite3_lib = static_library('sqlite3',
  'sqlite3.c',
  c_args: ['-DSQLITE_ENABLE_COLUMN_METADATA=1'], # Kluczowa flaga dla SQLiteCpp
  install: false
)

sqlite3_dep = declare_dependency(
  link_with: sqlite3_lib,
  include_directories: include_directories('.')
)