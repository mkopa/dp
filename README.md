# dirpacker

> **dirpacker** – Lightweight, text-based directory archiver for code sharing and AI prompting.

`dirpacker` is a minimal, dependency-focused tool that serializes an entire directory into a single, human-readable text stream. It's ideal for:
- Sharing project structure with AI assistants (e.g. LLMs)
- Archiving codebases without binary formats
- Debugging or auditing file inclusions/exclusions
- Secure, transparent packaging (no compression, no hidden data)

All files are wrapped in clear markers, and binary files are Base64-encoded. Rules from `.dpignore` (like `.gitignore`) control what's included.

![Example output snippet](https://via.placeholder.com/600x200?text=---+START+TEXT+FILE+main.cpp+---\n#include+<iostream>\n...\n---+END_OF_FILE+---)
*Example of serialized output*



## ✨ Features

- ✅ **Text-only output** – safe for AI, logs, diffs
- ✅ **Binary file support** via Base64 encoding
- ✅ **`.dpignore` support** – ignore files/dirs like `.gitignore`
- ✅ **Filter by type**: `--text-only` skips binaries
- ✅ **Limit by size**: `-l 100k` skips large files
- ✅ **Dry-run & stats**: preview what would be packed
- ✅ **No compression** – fully inspectable output
- ✅ **Cross-platform**: Linux, macOS, Windows (MSVC/MinGW)



## 🛠️ Build Instructions

`dirpacker` uses **Meson + Ninja** – fast, modern, and portable.

### Prerequisites

- **C++17 compiler** (GCC 8+, Clang 7+, MSVC 19.14+)
- **Meson** (v0.55+)
- **Ninja** (bundled with Meson on most systems)
- **Git** (to clone)

Install Meson:
```bash
# Linux/macOS (pip)
pip3 install meson ninja

# Ubuntu/Debian
sudo apt install meson ninja-build

# Fedora
sudo dnf install meson ninja-build

# macOS (Homebrew)
brew install meson ninja

# Windows (MSYS2)
pacman -S meson mingw-w64-x86_64-ninja
```



### 🐧 Linux & 🍏 macOS

```bash
# Clone the repo
git clone https://github.com/mkopa/dp.git
cd dp

# Configure build
meson setup build

# Compile
meson compile -C build

# Run
./build/dirpacker --help
```



### 💻 Windows

#### Option 1: MSYS2 / MinGW-w64 (Recommended)

```bash
# Install MSYS2 from https://www.msys2.org/
# Open "MSYS2 MinGW 64-bit"

# Update and install
pacman -Syu
pacman -S git mingw-w64-x86_64-gcc mingw-w64-x86_64-meson mingw-w64-x86_64-ninja

# Clone and build
git clone https://github.com/mkopa/dp.git
cd dp
meson setup build
meson compile -C build

# Run
./build/dirpacker.exe --help
```

#### Option 2: Visual Studio (MSVC)

Ensure you have **Visual Studio 2019+** with C++ tools.

```bash
# Open x64 Native Tools Command Prompt
git clone https://github.com/mkopa/dp.git
cd dp

# Meson will auto-detect MSVC
meson setup build
meson compile -C build

dirpacker.exe --help
```



## 🧰 Usage

```bash
dirpacker -d <DIRECTORY> [OPTIONS]
```

### Required
- `-d, --dir DIR` – input directory to pack

### Optional
- `-o, --out FILE` – output file (default: stdout)
- `-t, --text-only` – skip binary files
- `-l, --limit SIZE` – skip files larger than SIZE (e.g. `100k`, `1m`, `512b`)
- `--dry-run` – simulate without writing
- `--stats` – show summary after completion
- `-v, --version` – show version
- `-h, --help` – show help



### Examples

```bash
# Pack current dir to stdout
dirpacker -d .

# Save to file
dirpacker -d . -o project.pack

# Pipe to another tool
dirpacker -d src -l 100k | wc -c

# Dry run with stats
dirpacker -d . --dry-run --stats

# Only text files, max 50KB
dirpacker -d . -t -l 50k -o code_text.pack
```



## 📁 `.dpignore`

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
--- START TEXT FILE path/to/file.txt 1234 ---
(file content here)
--- END_OF_FILE ---
```

or for binaries:

```
--- START BINARY FILE image.png 5678 ---
(base64-encoded data)
--- END_OF_FILE ---
```

This makes parsing and debugging trivial.



## 📄 License

MIT License – see [LICENSE](LICENSE) for details.

> Copyright © 2025 Marcin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files, to deal in the software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the software, and to permit persons to whom the software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.



## 🙌 Contributing

PRs welcome! Please ensure:
- Code compiles on all platforms
- No new dependencies
- Follows existing style
- Updates README if needed



## 🐞 Bug Reports

Open an issue with:
- OS and compiler
- `meson --version`
- Exact command
- Expected vs actual output



> 🏷️ `dirpacker` – because sometimes you just need to send the code, not the noise.