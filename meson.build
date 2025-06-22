project('dirpacker', 'cpp',
  version : '1.0.0',
  default_options : ['cpp_std=c++17'])

cppcodec_dep = declare_dependency(
  include_directories: include_directories('subprojects/cppcodec')
)

executable('dirpacker', 'src/main.cpp',
  dependencies: [cppcodec_dep]
)
