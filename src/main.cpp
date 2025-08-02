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
 * @brief Parses size string like 100k, 1m, 10b into bytes.
 * @param size_str Size string (e.g. 100k, 1m, 10b).
 * @return Size in bytes.
 */
uintmax_t parse_size(const std::string& size_str) {
    std::regex re(R"((\d+)([bkm])?)", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_match(size_str, match, re)) {
        uintmax_t value = std::stoull(match[1].str());
        if (match.size() > 1) {
            char unit = std::tolower(match[2].str()[0]);
            if (unit == 'k') {
                value *= 1024;
            } else if (unit == 'm') {
                value *= 1024 * 1024;
            } // 'b' means bytes
        }
        return value;
    }
    throw std::invalid_argument("Invalid size format: " + size_str);
}

/**
 * @brief Checks if a file is binary by searching for a NUL byte.
 * @param file_path Path to the file.
 * @return true if the file is binary, otherwise false.
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
 * @param base_dir Directory to search for .dpignore.
 * @param exec_path Path to the executable (used if .dpignore not found in input dir).
 * @return Vector of ignore rules.
 */
std::vector<IgnoreRule> load_dpignore_rules(const fs::path& base_dir, const fs::path& exec_path) {
    std::vector<IgnoreRule> rules;
    // Try to find .dpignore in input directory
    fs::path ignore_file = base_dir / ".dpignore";
    if (fs::exists(ignore_file) && fs::is_regular_file(ignore_file)) {
        std::ifstream file(ignore_file);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not read .dpignore in " << base_dir << std::endl;
            return rules;
        }
        std::string line;
        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            bool is_negated = false;
            if (line[0] == '!') {
                is_negated = true;
                line = line.substr(1);
            }
            if (line.empty()) continue;
            std::string regex_pattern = "^";
            bool in_glob = false;
            for (size_t i = 0; i < line.length(); ++i) {
                char c = line[i];
                if (c == '*' && !in_glob) {
                    regex_pattern += ".*";
                    in_glob = true;
                } else if (c == '?' && !in_glob) {
                    regex_pattern += ".";
                    in_glob = true;
                } else if (c == '.' || c == '+' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '|' || c == '^' || c == '$') {
                    regex_pattern += '\\';
                    regex_pattern += c;
                } else {
                    regex_pattern += c;
                }
                in_glob = false;
            }
            regex_pattern += "$";
            try {
                rules.emplace_back(IgnoreRule{
                    .regex = std::regex(regex_pattern, std::regex_constants::icase),
                    .is_negated = is_negated
                });
            } catch (const std::regex_error& e) {
                std::cerr << "Invalid regex in .dpignore: " << regex_pattern << " (" << e.what() << ")\n";
            }
        }
    } else {
        // Try to find .dpignore in the executable's directory
        fs::path exec_dir = exec_path.parent_path();
        ignore_file = exec_dir / ".dpignore";
        if (fs::exists(ignore_file) && fs::is_regular_file(ignore_file)) {
            std::ifstream file(ignore_file);
            if (!file.is_open()) {
                std::cerr << "Warning: Could not read .dpignore in executable directory: " << exec_dir << std::endl;
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
                bool in_glob = false;
                for (size_t i = 0; i < line.length(); ++i) {
                    char c = line[i];
                    if (c == '*' && !in_glob) {
                        regex_pattern += ".*";
                        in_glob = true;
                    } else if (c == '?' && !in_glob) {
                        regex_pattern += ".";
                        in_glob = true;
                    } else if (c == '.' || c == '+' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '|' || c == '^' || c == '$') {
                        regex_pattern += '\\';
                        regex_pattern += c;
                    } else {
                        regex_pattern += c;
                    }
                    in_glob = false;
                }
                regex_pattern += "$";
                try {
                    rules.emplace_back(IgnoreRule{
                        .regex = std::regex(regex_pattern, std::regex_constants::icase),
                        .is_negated = is_negated
                    });
                } catch (const std::regex_error& e) {
                    std::cerr << "Invalid regex in .dpignore: " << regex_pattern << " (" << e.what() << ")\n";
                }
            }
        }
    }
    return rules;
}

/**
 * @brief Checks if a path should be ignored based on .dpignore rules.
 * @param path Relative path from input_dir.
 * @param rules List of rules (with negation flags).
 * @return true if should be ignored, false if should be included.
 */
bool should_ignore(const fs::path& path, const std::vector<IgnoreRule>& rules) {
    std::string path_str = path.generic_string();
    bool matched_any = false;
    bool is_excluded = false;
    for (const auto& rule : rules) {
        if (std::regex_match(path_str, rule.regex)) {
            matched_any = true;
            if (rule.is_negated) {
                // Exception rule → do NOT ignore
                is_excluded = false;
            } else {
                // Regular rule → ignore
                is_excluded = true;
            }
        }
    }
    return matched_any && is_excluded;
}

/**
 * @brief Struct for collecting statistics.
 */
struct Stats {
    uintmax_t processed_files = 0;
    uintmax_t total_size = 0;
    uintmax_t text_files = 0;
    uintmax_t binary_files = 0;
    uintmax_t ignored_files = 0;
    uintmax_t skipped_large = 0;
    uintmax_t skipped_binary = 0;

    void print() const {
        std::cout << "\n--- STATISTICS ---\n"
                  << "Processed files:     " << processed_files << "\n"
                  << "  - Text files:      " << text_files << "\n"
                  << "  - Binary files:    " << binary_files << "\n"
                  << "Total data size:     " << total_size << " bytes\n"
                  << "Skipped (ignored):   " << ignored_files << "\n"
                  << "Skipped (too large): " << skipped_large << "\n"
                  << "Skipped (binary):    " << skipped_binary << "\n"
                  << "------------------\n";
    }
};

/**
 * @brief Serializes an entire directory into a single archive stream.
 * @param input_dir Path to the input directory.
 * @param output_stream Output stream (could be file or stdout).
 * @param exec_path Path to executable (for .dpignore lookup).
 * @param text_only If true, skip binary files.
 * @param size_limit Max allowed file size in bytes.
 * @param has_limit If true, enforce size limit.
 * @param dry_run If true, don't write anything.
 * @param stats Reference to stats object to collect data.
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

    fs::recursive_directory_iterator iter(input_dir);
    for (const auto& entry : iter) {
        if (entry.is_regular_file()) {
            const fs::path& path = entry.path();
            fs::path relative_path = fs::relative(path, input_dir);
            uintmax_t file_size = fs::file_size(path);

            // Skip if ignored
            if (should_ignore(relative_path, rules)) {
                std::cout << "Ignored: " << relative_path.generic_string() << std::endl;
                stats.ignored_files++;
                continue;
            }

            // Skip if exceeds size limit
            if (has_limit && file_size > size_limit) {
                std::cout << "Skipped (too large): " << relative_path.generic_string() << " (" << file_size << " bytes)" << std::endl;
                stats.skipped_large++;
                continue;
            }

            // Skip binary files if --text-only is active
            if (text_only && is_binary(path)) {
                std::cout << "Skipped (binary, --text-only): " << relative_path.generic_string() << std::endl;
                stats.skipped_binary++;
                continue;
            }

            std::cout << "Processing: " << relative_path.generic_string() << std::endl;

            // Update stats
            stats.processed_files++;
            stats.total_size += file_size;

            if (is_binary(path)) {
                stats.binary_files++;
                if (!dry_run) {
                    output_stream << "--- START BINARY FILE " << relative_path.generic_string() << " " << file_size << " ---\n";
                    std::ifstream file(path, std::ios::binary);
                    std::vector<uint8_t> binary_data;
                    binary_data.assign(
                        std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>()
                    );
                    std::string encoded = cppcodec::base64_rfc4648::encode(binary_data);
                    output_stream << encoded;
                    output_stream << END_OF_RECORD;
                }
            } else {
                stats.text_files++;
                if (!dry_run) {
                    output_stream << "--- START TEXT FILE " << relative_path.generic_string() << " " << file_size << " ---\n";
                    std::ifstream file(path);
                    output_stream << file.rdbuf();
                    output_stream << END_OF_RECORD;
                }
            }
        }
    }
}

/**
 * @brief Displays help message.
 * @param program_name Name of the executable.
 */
void show_help(const std::string& program_name) {
    std::cout << "DirPacker v" << VERSION << "\n"
              << "Usage: " << program_name << " [OPTIONS]\n"
              << "Options:\n"
              << "  -d, --dir DIR         Input directory to pack (required)\n"
              << "  -i, --input DIR       Alias for --dir\n"
              << "  -o, --out FILE        Output file (optional). If omitted, output goes to stdout.\n"
              << "  -t, --text-only       Only include text files (skip binary files).\n"
              << "  -l, --limit SIZE      Skip files larger than SIZE (e.g. 100k, 1m, 512b).\n"
              << "  --dry-run             Simulate packing without writing output.\n"
              << "  --stats               Show statistics after processing.\n"
              << "  -v, --version         Show version and exit.\n"
              << "  -h, --help            Show this help message.\n"
              << "Size format: <number>[b|k|m] (default: bytes). Examples: 100k, 1m, 512b.\n"
              << "Ignore Files:\n"
              << "  .dpignore: Supports patterns like:\n"
              << "    *.log     → ignores all .log files\n"
              << "    logs/     → ignores logs directory\n"
              << "    !important.txt → includes important.txt even if ignored\n"
              << "    # comment → ignored line\n"
              << "Example:\n"
              << "  " << program_name << " -d ./src -t -l 100k --dry-run --stats\n"
              << "  " << program_name << " -d ./docs -o docs.pack --stats\n"
              << std::endl;
}

/**
 * @brief Parses command-line arguments.
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
        } else if (arg == "-l" || arg == "--limit") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            try {
                std::string limit_str = argv[++i];
                args.size_limit = parse_size(limit_str);
                args.has_limit = true;
            } catch (const std::exception& e) {
                std::cerr << "Error: Invalid size format: " << e.what() << "\n";
                std::exit(1);
            }
        } else if (arg == "--dry-run") {
            args.dry_run = true;
        } else if (arg == "--stats") {
            args.show_stats = true;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cerr << "Use -h or --help for usage.\n";
            std::exit(1);
        }
    }
    if (args.input_dir.empty()) {
        std::cerr << "Error: Input directory (-d/--dir) is required.\n";
        std::exit(1);
    }
    return args;
}

/**
 * @brief Main function of the program.
 */
int main(int argc, char** argv) {
    auto args = parse_args(argc, argv);
    if (args.show_version) {
        std::cout << "DirPacker v" << VERSION << std::endl;
        return 0;
    }
    if (args.show_help) {
        show_help(argv[0]);
        return 0;
    }

    fs::path exec_path = argv[0];
    Stats stats;

    try {
        if (!args.output_file.empty()) {
            if (args.dry_run) {
                std::cout << "Dry run: No output file will be written.\n";
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
                std::cout << "Dry run: Output would go to stdout.\n";
            }
        }

        // Show stats if requested
        if (args.show_stats) {
            stats.print();
        }

        if (args.dry_run) {
            std::cout << "Dry run completed. No data was written.\n";
        } else {
            std::cout << "Serialization completed successfully.\n";
            if (!args.output_file.empty()) {
                std::cout << "Output saved to: " << args.output_file << std::endl;
            } else {
                std::cout << "Output written to stdout." << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}