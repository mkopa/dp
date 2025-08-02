#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <regex>
#include <map>
#include <cppcodec/base64_rfc4648.hpp>

namespace fs = std::filesystem;

// Definition of the separator constant
const std::string END_OF_RECORD = "\n--- END_OF_FILE ---\n";

// Program version
constexpr const char* VERSION = "1.0.0";

/**
 * @brief Convert path to forward slash format (POSIX style)
 */
std::string to_forward_slash(const fs::path& p) {
    std::string str = p.generic_string();
    std::replace(str.begin(), str.end(), '\\', '/');
    return str;
}

/**
 * @brief Parses size string like 100k, 1m, 10b into bytes.
 */
uintmax_t parse_size(const std::string& size_str) {
    std::regex re(R"((\d+)([bkm])?)", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_match(size_str, match, re)) {
        uintmax_t value = std::stoull(match[1].str());
        if (match.size() > 1) {
            char unit = std::tolower(match[2].str()[0]);
            if (unit == 'k') value *= 1024;
            else if (unit == 'm') value *= 1024 * 1024;
        }
        return value;
    }
    throw std::invalid_argument("Invalid size format: " + size_str);
}

/**
 * @brief Checks if a file is binary by searching for a NUL byte.
 */
bool is_binary(const fs::path& file_path) {
    constexpr size_t check_size = 8000;
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file: " + file_path.string());
    }
    std::vector<char> buffer(check_size);
    file.read(buffer.data(), buffer.size());
    std::streamsize bytes_read = file.gcount();
    return std::find(buffer.begin(), buffer.begin() + bytes_read, '\0') != buffer.begin() + bytes_read;
}

/**
 * @brief Struct to represent a single ignore rule.
 */
struct IgnoreRule {
    std::regex regex;
    bool is_negated;
};

/**
 * @brief Loads .dpignore rules from a given directory or program's dir.
 */
std::vector<IgnoreRule> load_dpignore_rules(const fs::path& base_dir, const fs::path& exec_path) {
    std::vector<IgnoreRule> rules;
    fs::path ignore_file = base_dir / ".dpignore";

    std::cerr << "🔍 Looking for .dpignore in: " << base_dir << std::endl;

    if (fs::exists(ignore_file) && fs::is_regular_file(ignore_file)) {
        std::cerr << "✅ .dpignore found and opened." << std::endl;
        std::ifstream file(ignore_file);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not read .dpignore in " << base_dir << std::endl;
            return rules;
        }
        std::string line;
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            if (line.empty() || line[0] == '#') continue;

            bool is_negated = false;
            if (line[0] == '!') {
                is_negated = true;
                line = line.substr(1);
            }
            if (line.empty()) continue;

            std::string regex_pattern = "^";
            for (size_t i = 0; i < line.length(); ++i) {
                char c = line[i];
                if (c == '*') {
                    regex_pattern += ".*";
                } else if (c == '?') {
                    regex_pattern += ".";
                } else if (c == '/') {
                    regex_pattern += "/";
                    if (i == line.length() - 1) {
                        regex_pattern += ".*";
                    }
                } else if (std::string(".+()[]{}|^$").find(c) != std::string::npos) {
                    regex_pattern += '\\';
                    regex_pattern += c;
                } else {
                    regex_pattern += c;
                }
            }
            regex_pattern += "$";

            try {
                auto rule = IgnoreRule{ std::regex(regex_pattern, std::regex_constants::icase), is_negated };
                rules.push_back(rule);
                std::cerr << "📋 Loaded rule: " << (is_negated ? "EXCEPT " : "IGNORE ") << regex_pattern << std::endl;
            } catch (const std::regex_error& e) {
                std::cerr << "Invalid regex in .dpignore: " << regex_pattern << " (" << e.what() << ")" << std::endl;
            }
        }
    } else {
        std::cerr << "❌ .dpignore NOT found in: " << base_dir << std::endl;
    }

    return rules;
}

/**
 * @brief Checks if a path should be ignored based on .dpignore rules.
 */
bool should_ignore(const fs::path& path, const std::vector<IgnoreRule>& rules) {
    std::string path_str = to_forward_slash(path);
    std::cerr << "🔍 Checking path: " << path_str << std::endl;

    bool matched_any = false;
    bool is_excluded = false;

    for (const auto& rule : rules) {
        if (std::regex_match(path_str, rule.regex)) {
            std::cerr << "🎯 MATCH: " << path_str << " → " << (rule.is_negated ? "EXCEPT" : "IGNORE") << std::endl;
            matched_any = true;
            if (rule.is_negated) {
                is_excluded = false;
            } else {
                is_excluded = true;
            }
        }
    }

    return matched_any && is_excluded;
}

/**
 * @brief Statistics collector.
 */
struct Stats {
    uintmax_t processed = 0, ignored = 0, too_large = 0, binary_skipped = 0, text = 0, binary = 0, total_size = 0;

    void print() const {
        std::cerr << "\n--- STATISTICS ---\n"
                  << "Processed files:     " << processed << "\n"
                  << "  - Text files:      " << text << "\n"
                  << "  - Binary files:    " << binary << "\n"
                  << "Total data size:     " << total_size << " bytes\n"
                  << "Skipped (ignored):   " << ignored << "\n"
                  << "Skipped (too large): " << too_large << "\n"
                  << "Skipped (binary):    " << binary_skipped << "\n"
                  << "------------------\n";
    }
};

/**
 * @brief Serializes directory with filters and stats.
 */
void serialize_directory(
    const fs::path& input_dir,
    std::ostream& output_stream,
    const fs::path& exec_path,
    bool text_only,
    uintmax_t size_limit,
    bool has_limit,
    bool dry_run,
    Stats& stats
) {
    auto rules = load_dpignore_rules(input_dir, exec_path);
    if (!fs::is_directory(input_dir)) {
        throw std::runtime_error("Input path is not a directory: " + input_dir.string());
    }

    for (const auto& entry : fs::recursive_directory_iterator(input_dir)) {
        if (!entry.is_regular_file()) continue;

        const fs::path& path = entry.path();
        fs::path rel = fs::relative(path, input_dir);
        uintmax_t size = fs::file_size(path);

        // Skip if ignored by .dpignore
        if (should_ignore(rel, rules)) {
            std::cerr << "🚫 Ignored: " << rel << std::endl;
            stats.ignored++;
            continue;
        }

        // Skip if too large
        if (has_limit && size > size_limit) {
            std::cerr << "📏 Skipped (too large): " << rel << " (" << size << " bytes)" << std::endl;
            stats.too_large++;
            continue;
        }

        // Skip binary if --text-only
        if (text_only && is_binary(path)) {
            std::cerr << "🗎 Skipped (binary, --text-only): " << rel << std::endl;
            stats.binary_skipped++;
            continue;
        }

        std::cerr << "📄 Processing: " << rel << std::endl;
        stats.processed++;
        stats.total_size += size;

        if (is_binary(path)) {
            stats.binary++;
            if (!dry_run) {
                output_stream << "--- START BINARY FILE " << rel << " " << size << " ---\n";
                std::ifstream file(path, std::ios::binary);
                std::vector<uint8_t> data{
                    std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>()
                };
                std::string encoded = cppcodec::base64_rfc4648::encode(data);
                output_stream << encoded;
                output_stream << END_OF_RECORD;
            }
        } else {
            stats.text++;
            if (!dry_run) {
                output_stream << "--- START TEXT FILE " << rel << " " << size << " ---\n";
                std::ifstream file(path);
                output_stream << file.rdbuf();
                output_stream << END_OF_RECORD;
            }
        }
    }
}

/**
 * @brief Parse command-line arguments.
 */
struct Args {
    fs::path input_dir;
    fs::path output_file;
    bool show_help = false;
    bool show_version = false;
    bool text_only = false;
    bool dry_run = false;
    bool show_stats = false;
    uintmax_t size_limit = 0;
    bool has_limit = false;
};

Args parse_args(int argc, char** argv) {
    Args args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            args.show_help = true;
        } else if (arg == "-v" || arg == "--version") {
            args.show_version = true;
        } else if (arg == "-d" || arg == "--dir" || arg == "-i" || arg == "--input") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            args.input_dir = argv[++i];
        } else if (arg == "-o" || arg == "--out") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            args.output_file = argv[++i];
        } else if (arg == "-t" || arg == "--text-only") {
            args.text_only = true;
        } else if (arg == "--dry-run") {
            args.dry_run = true;
        } else if (arg == "--stats") {
            args.show_stats = true;
        } else if (arg == "-l" || arg == "--limit") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            try {
                args.size_limit = parse_size(argv[++i]);
                args.has_limit = true;
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid size format: " << e.what() << "\n";
                std::exit(1);
            }
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cerr << "Use -h or --help for usage.\n";
            std::exit(1);
        }
    }

    if (!args.show_help && !args.show_version && args.input_dir.empty()) {
        std::cerr << "Error: Input directory (-d/--dir) is required.\n";
        std::cerr << "Use -h or --help for usage.\n";
        std::exit(1);
    }

    return args;
}

/**
 * @brief Main function.
 */
int main(int argc, char** argv) {
    auto args = parse_args(argc, argv);

    if (args.show_version) {
        std::cout << "DirPacker v" << VERSION << std::endl;
        return 0;
    }
    if (args.show_help) {
        std::cout << "DirPacker v" << VERSION << "\n"
                  << "Usage: " << argv[0] << " [OPTIONS]\n"
                  << "Options:\n"
                  << "  -d, --dir DIR         Input directory to pack (required)\n"
                  << "  -i, --input DIR       Alias for --dir\n"
                  << "  -o, --out FILE        Output file (optional)\n"
                  << "  -t, --text-only       Only include text files\n"
                  << "  -l, --limit SIZE      Skip files larger than SIZE (e.g. 100k, 1m)\n"
                  << "  --dry-run             Simulate without writing\n"
                  << "  --stats               Show statistics\n"
                  << "  -v, --version         Show version\n"
                  << "  -h, --help            Show this help\n"
                  << "Example:\n"
                  << "  " << argv[0] << " -d . -l 100k --stats -o out.pack\n"
                  << std::endl;
        return 0;
    }

    fs::path exec_path = argv[0];
    Stats stats;

    try {
        if (!args.output_file.empty()) {
            if (args.dry_run) {
                std::cerr << "Dry run: No output file will be written.\n";
            } else {
                std::ofstream out(args.output_file, std::ios::binary);
                if (!out) {
                    throw std::runtime_error("Cannot create output file: " + args.output_file.string());
                }
                serialize_directory(args.input_dir, out, exec_path, args.text_only, args.size_limit, args.has_limit, args.dry_run, stats);
                out.close();
            }
        } else {
            if (!args.dry_run) {
                serialize_directory(args.input_dir, std::cout, exec_path, args.text_only, args.size_limit, args.has_limit, args.dry_run, stats);
            } else {
                std::cerr << "Dry run: Output would go to stdout.\n";
            }
        }

        if (args.show_stats) {
            stats.print();
        }

        if (args.dry_run) {
            std::cerr << "Dry run completed. No data was written.\n";
        } else {
            std::cerr << "Serialization completed successfully.\n";
            if (!args.output_file.empty()) {
                std::cerr << "Output saved to: " << args.output_file << std::endl;
            } else {
                std::cerr << "Output written to stdout.\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}