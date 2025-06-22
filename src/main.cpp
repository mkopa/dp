#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <algorithm>

#include <cppcodec/base64_rfc4648.hpp>

namespace fs = std::filesystem;

// Definition of the separator constant
const std::string END_OF_RECORD = "\n---END_OF_FILE---\n";

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
 * @brief Serializes an entire directory into a single archive file.
 * @param input_dir Path to the input directory.
 * @param output_file Path to the output file.
 */
void serialize_directory(const fs::path& input_dir, const fs::path& output_file) {
    std::ofstream out(output_file, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot create output file: " + output_file.string());
    }

    fs::recursive_directory_iterator iter(input_dir);
    for (const auto& entry : iter) {
        if (entry.is_regular_file()) {
            const fs::path& path = entry.path();
            fs::path relative_path = fs::relative(path, input_dir);
            uintmax_t file_size = fs::file_size(path);

            std::cout << "Processing: " << relative_path.generic_string() << std::endl;

            if (is_binary(path)) {
                out << "BINARY " << relative_path.generic_string() << " " << file_size << "\n";
                
                std::ifstream file(path, std::ios::binary);
                std::vector<uint8_t> binary_data(
                    (std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>()
                );
                
                std::string encoded = cppcodec::base64_rfc4648::encode(binary_data);
                out << encoded;
            } else {
                out << "TEXT " << relative_path.generic_string() << " " << file_size << "\n";
                
                std::ifstream file(path);
                out << file.rdbuf();
            }
            out << END_OF_RECORD;
        }
    }
}

/**
 * @brief Main function of the program.
 */
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_directory> <output_file>" << std::endl;
        return 1;
    }

    fs::path input_dir = argv[1];
    fs::path output_file = argv[2];

    if (!fs::is_directory(input_dir)) {
        std::cerr << "Error: Input path is not a directory: " << input_dir << std::endl;
        return 1;
    }

    try {
        serialize_directory(input_dir, output_file);
        std::cout << "Serialization completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}