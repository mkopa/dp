# dirpacker

> **dirpacker** — Lightweight, text-based directory archiver with zero external dependencies.

`dirpacker` is a minimal tool that serializes an entire directory into a single, human-readable text stream. Perfect for:
- Sharing project structure with AI assistants (e.g. LLMs)
- Archiving codebases without binary formats
- Debugging or auditing file inclusions/exclusions
- Secure, transparent packaging (no compression, no hidden data)

All files are wrapped in clear markers, and binary files are Base64-encoded. Rules from `.dpignore` (like `.gitignore`) control what's included.

## ✨ Features

- ✅ **Text-only output** — safe for AI, logs, diffs
- ✅ **Binary file support** via custom Base64 encoding
- ✅ **SHA256/SHA3 hashing** — built-in implementations
- ✅ **`.dpignore` support** — ignore files/dirs like `.gitignore`
- ✅ **Filter by type**: `--text-only` skips binaries
- ✅ **Limit by size**: `-l 100k` skips large files
- ✅ **Dry-run & stats**: preview what would be packed
- ✅ **Cross-platform**: Linux, macOS, Windows (MSVC/MinGW)

## 🛠️ Build Instructions

`dirpacker` uses **CMake** — modern, portable, and dependency-free.

### Prerequisites

- **C++17 compiler** (GCC 8+, Clang 7+, MSVC 19.14+)
- **CMake** (v3.15+)
- **Git** (to clone)

### 🐧 Linux & 🍎 macOS

```bash
# Clone the repo
git clone https://github.com/mkopa/dp.git
cd dp

# Build
./build.sh

# Run
./build/dirpacker --help
```

### 💻 Windows

#### Option 1: MSYS2 / MinGW-w64 (Recommended)

```bash
# Install MSYS2 from https://www.msys2.org/
# Open "MSYS2 MinGW 64-bit"

# Install tools
pacman -Syu
pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja

# Build
git clone https://github.com/mkopa/dp.git
cd dp
./build.sh

# Run
./build/dirpacker.exe --help
```

#### Option 2: Visual Studio (MSVC)

```cmd
REM Open x64 Native Tools Command Prompt
git clone https://github.com/mkopa/dp.git
cd dp

REM Build
build.bat

REM Run
build\Release\dirpacker.exe --help
```

## 🧰 Usage

```bash
dirpacker -d <DIRECTORY> [OPTIONS]
```

### Required
- `-d, --dir DIR` — input directory to pack

### Optional
- `-o, --out FILE` — output file (default: stdout)
- `-a, --algorithm ALGO` — hash algorithm: `sha256` (default), `sha3`
- `-t, --text-only` — skip binary files
- `-l, --limit SIZE` — skip files larger than SIZE (e.g. `100k`, `1m`, `512b`)
- `--dry-run` — simulate without writing
- `--stats` — show summary after completion
- `--verbose` — detailed processing logs
- `-v, --version` — show version
- `-h, --help` — show help

### Examples

```bash
# Pack current dir to stdout
dirpacker -d .

# Save to file with SHA3
dirpacker -d . -a sha3 -o project.pack

# Pipe to another tool
dirpacker -d src -l 100k | wc -c

# Dry run with stats
dirpacker -d . --dry-run --stats

# Only text files, max 50KB
dirpacker -d . -t -l 50k -o code_text.pack
```

## 📝 `.dpignore`

Place a `.dpignore` file in your input directory to exclude files. Syntax is `.gitignore`-like:

```gitignore
# Comments
*.log
*.tmp
build/
dist/
node_modules/
*.exe
*.dll

# Exceptions
!important.txt
!docs/README.md
```

Rules are matched recursively. Paths are normalized to forward slashes (`/`).

> 💡 Tip: Use `--dry-run --stats` to verify what will be included.

## 📦 Output Format

Each file is wrapped like this:

```
--- START TEXT FILE path: "path/to/file.txt" size: 1234 bytes sha3: 9dc2f59f074cf22b14bab8cb45b841cd5cc34b9e08387a0532c4c463dc310df5 ---
(file content here)
--- END OF FILE ---
```

or for binaries:

```
--- START BINARY FILE path: "image.png" size: 5678 bytes sha3: 11548bb468eb66240a65e670e0d75c4e357409eafb57d6e1436ba19a003c0507 ---
(base64-encoded data)
--- END OF FILE ---
```

This makes parsing and debugging trivial.

## 🏗️ Project Structure

```
dp/
├── CMakeLists.txt          # Build configuration
├── build.sh                # Linux/macOS build script
├── build.bat               # Windows build script
├── README.md               # This file
├── .dpignore               # Ignore rules
├── include/
│   ├── dirpacker.hpp       # Main library interface
│   ├── sha256.hpp          # SHA256 implementation
│   ├── sha3.hpp            # SHA3 implementation
│   └── base64.hpp          # Base64 encoder/decoder
└── src/
    ├── main.cpp            # CLI application
    ├── dirpacker.cpp       # Core packing logic
    ├── sha256.cpp          # SHA256 implementation
    ├── sha3.cpp            # SHA3 implementation
    └── base64.cpp          # Base64 implementation
```

## 🔧 Technical Details

### Hash Algorithms
- **SHA256**: Secure Hash Algorithm 256-bit
- **SHA3**: SHA-3 (Keccak) 256-bit variant

Both implementations are self-contained, no OpenSSL or external crypto libraries required.

### Base64 Encoding
Custom, optimized Base64 encoder/decoder with:
- RFC 4648 compliance
- Efficient memory usage
- Proper padding handling
- Validation on decode

### Binary Detection
Files are checked for null bytes (`\0`) in the first 8KB to determine if they're binary.
