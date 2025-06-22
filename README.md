# DirPacker

DirPacker is a cross-platform command-line utility written in C++ for serializing the contents of a directory into a single archive file. It is designed to be self-contained, meaning the final executable has no runtime dependencies on external tools like `git`, `grep`, or `base64`.

## Features

  - **Directory Serialization**: Packs an entire directory tree into a single, portable file.
  - **Binary File Handling**: Automatically detects binary files and encodes them using Base64. Text files are stored as-is.
  - **Cross-Platform**: Compiles and runs on Linux, macOS, and Windows without code changes.
  - **Self-Contained**: The final executable works out-of-the-box without needing other programs installed.
  - **Modern Build System**: Uses Meson and Ninja for a fast, modern, and reliable build process.

## Prerequisites

To build `DirPacker` from source, you will need a C++17 compliant compiler and the following tools installed on your system:

  - A C++ Compiler (GCC, Clang, or MSVC)
  - Meson Build System
  - Ninja

### Installation of Prerequisites

  - **Debian/Ubuntu:**

    ```bash
    sudo apt update
    sudo apt install build-essential meson ninja-build    
    ```

  - **Fedora/RHEL:**

    ```bash
    sudo dnf install gcc-c++ meson ninja-build
    ```

  - **macOS (using Homebrew):**

    ```bash
    xcode-select --install
    brew install meson ninja
    ```

  - **Windows (using Visual Studio):**

    1.  Install(https://visualstudio.microsoft.com/vs/community/) with the **"Desktop development with C++"** workload.
    2.  Install Meson and Ninja. The easiest way is via an MSI installer from the [Meson GitHub releases page](https://github.com/mesonbuild/meson/releases).
    3.  All build commands must be run from a **Developer Command Prompt for VS**.

## Building the Project

Once the prerequisites are installed, you can compile the project by running the following commands from the project's root directory:

1.  **Set up the build directory (only needs to be done once):**

    ```bash
    meson setup builddir
    ```

2.  **Compile the project:**

    ```bash
    meson compile -C builddir
    ```

The compiled executable, `dirpacker` (or `dirpacker.exe` on Windows), will be located in the `builddir` directory.

## Usage

The program takes two command-line arguments: the path to the input directory you want to serialize and the path for the output archive file.

### Syntax

```
dirpacker <input_directory> <output_file>
```

### Examples

- **On Linux / macOS:**
    ```bash
    ./builddir/dirpacker ./my_source_code my_archive.pack
    ````

- **On Windows:**
    ```bash
    builddir\dirpacker.exe C:\Users\Me\MySourceCode my_archive.pack
    ````

The program will print the relative path of each file as it is being processed. Upon completion, `my_archive.pack` will contain the serialized contents of the source directory.
