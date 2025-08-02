#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <sstream>

#include <cppcodec/base64_rfc4648.hpp>

namespace fs = std::filesystem;

// Definition of the separator constant
const std::string END_OF_RECORD = "\n--- END_OF_FILE ---\n";

// Program version
constexpr const char* VERSION = "1.0.0";

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
 * @brief Serializes an entire directory into a single archive stream.
 * @param input_dir Path to the input directory.
 * @param output_stream Output stream (could be file or stdout).
 */
void serialize_directory(const fs::path& input_dir, std::ostream& output_stream) {
    if (!fs::is_directory(input_dir)) {
        throw std::runtime_error("Input path is not a directory: " + input_dir.string());
    }

    fs::recursive_directory_iterator iter(input_dir);
    for (const auto& entry : iter) {
        if (entry.is_regular_file()) {
            const fs::path& path = entry.path();
            fs::path relative_path = fs::relative(path, input_dir);
            uintmax_t file_size = fs::file_size(path);

            std::cout << "Processing: " << relative_path.generic_string() << std::endl;

            if (is_binary(path)) {
                output_stream << "--- START BINARY FILE " << relative_path.generic_string() << " " << file_size << " ---\n";
                
                std::ifstream file(path, std::ios::binary);
                std::vector<uint8_t> binary_data;
                binary_data.assign(
                    std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>()
                );
                
                std::string encoded = cppcodec::base64_rfc4648::encode(binary_data);
                output_stream << encoded;
            } else {
                output_stream << "--- START TEXT FILE " << relative_path.generic_string() << " " << file_size << " ---\n";
                
                std::ifstream file(path);
                output_stream << file.rdbuf();
            }
            output_stream << END_OF_RECORD;
        }
    }
}

/**
 * @brief Displays help message.
 */
void show_help(const std::string& program_name) {
    std::cout << "DirPacker v" << VERSION << "\n"
              << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  -d, --dir DIR         Input directory to pack (required)\n"
              << "  -i, --input DIR       Alias for --dir\n"
              << "  -o, --out FILE        Output file (optional). If omitted, output goes to stdout.\n"
              << "  -v, --version         Show version and exit.\n"
              << "  -h, --help            Show this help message.\n\n"
              << "Example:\n"
              << "  " << program_name << " -d ./myproject -o archive.pack\n"
              << "  " << program_name << " -d ./docs | cat > docs.pack\n"
              << std::endl;
}

/**
 * @brief Parses command-line arguments.
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return Parsed values: input_dir, output_file (empty if should go to stdout), show_help, show_version.
 */
struct Args {
    fs::path input_dir;
    fs::path output_file;
    bool show_help = false;
    bool show_version = false;
};

Args parse_args(int argc, char** argv) {
    Args args;
    std::string program_name = argv[0];

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            args.show_help = true;
        } else if (arg == "-v" || arg == "--version") {
            args.show_version = true;
        } else if (arg == "-d" || arg == "--dir") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            args.input_dir = argv[++i];
        } else if (arg == "-i" || arg == "--input") {
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
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cerr << "Use -h or --help for usage.\n";
            std::exit(1);
        }
    }

    // Validate required input dir
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

    try {
        if (!args.output_file.empty()) {
            std::ofstream out(args.output_file, std::ios::binary);
            if (!out) {
                throw std::runtime_error("Cannot create output file: " + args.output_file.string());
            }
            serialize_directory(args.input_dir, out);
            std::cout << "Serialization completed successfully. Output saved to: " << args.output_file << std::endl;
        } else {
            // Output to stdout
            serialize_directory(args.input_dir, std::cout);
            std::cout << "Serialization completed successfully. Output written to stdout." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}