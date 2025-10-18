cmake_minimum_required(VERSION 3.15)
project(dirpacker VERSION 1.3.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# === SHA256/SHA3 implementation ===
add_library(sha_impl STATIC
    src/sha256.cpp
    src/sha3.cpp
)

target_include_directories(sha_impl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# === Base64 implementation ===
add_library(base64 STATIC
    src/base64.cpp
)

target_include_directories(base64 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# === DirPacker library ===
add_library(dirpacker_lib STATIC
    src/dirpacker.cpp
)

target_include_directories(dirpacker_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(dirpacker_lib PUBLIC
    sha_impl
    base64
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(dirpacker_lib PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()

# === Executable ===
add_executable(dirpacker src/main.cpp)

target_link_libraries(dirpacker PRIVATE dirpacker_lib)

# === Installation ===
install(TARGETS dirpacker DESTINATION bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md DESTINATION share/doc/dirpacker)

# === Compiler flags ===
if(MSVC)
    target_compile_options(dirpacker_lib PRIVATE /W4)
    target_compile_options(dirpacker PRIVATE /W4)
else()
    target_compile_options(dirpacker_lib PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(dirpacker PRIVATE -Wall -Wextra -pedantic)

    # Link with filesystem library if needed (GCC < 9)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
        target_link_libraries(dirpacker_lib PUBLIC stdc++fs)
    endif()
endif()

# === Summary ===
message(STATUS "=== DirPacker Build Configuration ===")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "=====================================")