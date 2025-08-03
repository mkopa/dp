#include "dirpacker/dirpacker.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

namespace {
// ... cała zawartość `namespace {}` pozostaje bez zmian (Args, parse_args, etc.) ...
constexpr const char *VERSION = "1.3.1";

struct Args {
    fs::path input_dir;
    fs::path output_file;
    bool show_help = false;
    bool show_version = false;
    bool show_stats = false;
    bool text_only = false;
    bool dry_run = false;
    bool verbose = false;
    uintmax_t size_limit = 0;
    bool has_limit = false;
    dp::HashAlgorithm algo = dp::HashAlgorithm::BLAKE3;
};

uintmax_t parse_size(const std::string &size_str) {
    std::regex re(R"((\d+)([bkmg]?)?)", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_match(size_str, match, re)) {
        uintmax_t value = std::stoull(match[1].str());
        if (match[2].matched) {
            char unit = std::tolower(match[2].str()[0]);
            if (unit == 'k') value *= 1024;
            else if (unit == 'm') value *= 1024 * 1024;
            else if (unit == 'g') value *= 1024 * 1024 * 1024;
        }
        return value;
    }
    throw std::invalid_argument("Invalid size format: " + size_str);
}

Args parse_args(int argc, char **argv) {
    Args args;
    std::vector<std::string> pos_args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") args.show_help = true;
        else if (arg == "-v" || arg == "--version") args.show_version = true;
        else if (arg == "-d" || arg == "--dir") {
            if (i + 1 < argc) args.input_dir = argv[++i];
            else { std::cerr << "Error: Missing argument for " << arg << std::endl; exit(1); }
        } else if (arg == "-o" || arg == "--out") {
            if (i + 1 < argc) args.output_file = argv[++i];
            else { std::cerr << "Error: Missing argument for " << arg << std::endl; exit(1); }
        } else if (arg == "-t" || arg == "--text-only") args.text_only = true;
        else if (arg == "--dry-run") args.dry_run = true;
        else if (arg == "--stats") args.show_stats = true;
        else if (arg == "--verbose") args.verbose = true;
        else if (arg == "-l" || arg == "--limit") {
            if (i + 1 < argc) {
                try {
                    args.size_limit = parse_size(argv[++i]);
                    args.has_limit = true;
                } catch (const std::exception& e) {
                    std::cerr << "Error: " << e.what() << std::endl; exit(1);
                }
            } else { std::cerr << "Error: Missing argument for " << arg << std::endl; exit(1); }
        } else if (arg == "-a" || arg == "--algorithm") {
            if (i + 1 < argc) {
                std::string algo_str = argv[++i];
                std::transform(algo_str.begin(), algo_str.end(), algo_str.begin(), ::tolower);
                if (algo_str == "blake3") args.algo = dp::HashAlgorithm::BLAKE3;
                else if (algo_str == "sha256") args.algo = dp::HashAlgorithm::SHA256;
                else if (algo_str == "sha3" || algo_str == "sha3-256") args.algo = dp::HashAlgorithm::SHA3_256;
                else { std::cerr << "Error: Unknown algorithm '" << algo_str << "'" << std::endl; exit(1); }
            } else { std::cerr << "Error: Missing argument for " << arg << std::endl; exit(1); }
        } else if (arg[0] == '-') {
            std::cerr << "Unknown argument: " << arg << std::endl; exit(1);
        } else {
            pos_args.push_back(arg);
        }
    }

    if (!args.show_help && !args.show_version) {
        if (args.input_dir.empty()) {
            if (!pos_args.empty()) args.input_dir = pos_args[0];
            else { std::cerr << "Error: Input directory is required." << std::endl; exit(1); }
        }
    }
    return args;
}

void show_help(const char *exec_name) {
    std::cout << "DirPacker v" << VERSION << " (Application Layer)\n"
              << "Packs a directory structure using the DirPacker library.\n\n"
              << "Usage: " << exec_name << " <input_directory> [OPTIONS]\n"
              << "   or: " << exec_name << " -d <input_directory> [OPTIONS]\n\n"
              << "Arguments:\n"
              << "  <input_directory>   Input directory to pack (required).\n\n"
              << "Options:\n"
              << "  -d, --dir, -i       Alternative way to specify input directory.\n"
              << "  -o, --out FILE      Output file (default: stdout).\n"
              << "  -a, --algorithm ALGO Hash algorithm to use (blake3, sha256, sha3-256). Default: blake3.\n"
              << "  -t, --text-only     Only include text files.\n"
              << "  -l, --limit SIZE    Skip files larger than SIZE (e.g., 100k, 1m, 2g).\n"
              << "  --dry-run           Simulate and show stats, but don't write output.\n"
              << "  --stats             Show statistics on stderr after completion.\n"
              << "  --verbose           Show detailed step-by-step processing logs.\n"
              << "  -v, --version       Show version information.\n"
              << "  -h, --help          Show this help message.\n\n"
              << "Example:\n"
              << "  " << exec_name << " . --algorithm sha3-256 -l 100k --stats -o out.pack\n"
              << std::endl;
}

void print_stats(const dp::Stats& stats) {
    std::cerr << "\n--- STATISTICS ---\n"
              << "Processed files:     " << stats.processed << "\n"
              << "  - Text files:      " << stats.text << "\n"
              << "  - Binary files:    " << stats.binary << "\n"
              << "Total data size:     " << stats.total_size << " bytes\n"
              << "Skipped (ignored):   " << stats.ignored << "\n"
              << "Skipped (too large): " << stats.too_large << "\n"
              << "Skipped (binary):    " << stats.binary_skipped << "\n"
              << "Access errors:       " << stats.errors << "\n"
              << "------------------\n";
}
}

int main(int argc, char **argv) {
    const auto args = parse_args(argc, argv);

    if (args.show_version) {
        std::cout << "DirPacker v" << VERSION << std::endl;
        return 0;
    }
    if (args.show_help) {
        show_help(argv[0]);
        return 0;
    }

    try {
        if (!args.output_file.empty()) {
            std::ofstream out(args.output_file, std::ios::binary);
            if (!out) {
                throw std::runtime_error("Cannot create output file: " + args.output_file.string());
            }
            dp::PackerConfig config = {args.input_dir, out, args.algo, args.text_only, args.size_limit, args.has_limit, args.dry_run, args.verbose};
            dp::Stats final_stats = dp::pack(config);
            if (args.show_stats) {
                print_stats(final_stats);
            }
        } else {
            dp::PackerConfig config = {args.input_dir, std::cout, args.algo, args.text_only, args.size_limit, args.has_limit, args.dry_run, args.verbose};
            dp::Stats final_stats = dp::pack(config);
            if (args.show_stats) {
                print_stats(final_stats);
            }
        }

        if (args.dry_run) {
            std::cerr << "Dry run completed. No data was written.\n";
        } else if (!args.output_file.empty()) {
            std::cerr << "Serialization completed. Output saved to: " << args.output_file << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "A critical error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}