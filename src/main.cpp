#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <regex>
#include <iomanip>
#include <map>
#include <system_error> // Potrzebne do przechwytywania błędów systemowych

// Zależności projektu
#include "blake3.h"
#include <cppcodec/base64_rfc4648.hpp>

// Zintegrowana biblioteka dla SHA
#include "sha256.h"
#include "sha3.h"

namespace fs = std::filesystem;

const std::string END_OF_RECORD = "\n--- END_OF_FILE ---\n";
constexpr const char *VERSION = "1.2.1"; // Podniesienie wersji (bugfix/robustness)

enum class HashAlgorithm
{
    BLAKE3,
    SHA256,
    SHA3_256
};

// --- Funkcje pomocnicze ---

std::string to_string(HashAlgorithm algo)
{
    static const std::map<HashAlgorithm, std::string> lookup = {
        {HashAlgorithm::BLAKE3, "blake3"},
        {HashAlgorithm::SHA256, "sha256"},
        {HashAlgorithm::SHA3_256, "sha3-256"}};
    return lookup.at(algo);
}

std::string bytes_to_hex(const uint8_t *bytes, size_t len)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i)
    {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

// NOWA funkcja: haszowanie danych z pamięci (dla komunikatów o błędach)
std::string compute_hash_from_memory(const std::string &data, HashAlgorithm algo)
{
    const char *data_ptr = data.c_str();
    size_t data_len = data.length();

    switch (algo)
    {
    case HashAlgorithm::BLAKE3:
    {
        uint8_t hash[BLAKE3_OUT_LEN];
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, data_ptr, data_len);
        blake3_hasher_finalize(&hasher, hash, BLAKE3_OUT_LEN);
        return bytes_to_hex(hash, BLAKE3_OUT_LEN);
    }
    case HashAlgorithm::SHA256:
    {
        SHA256 sha256;
        sha256.add(data_ptr, data_len);
        return sha256.getHash();
    }
    case HashAlgorithm::SHA3_256:
    {
        SHA3 sha3(SHA3::Bits256);
        sha3.add(data_ptr, data_len);
        return sha3.getHash();
    }
    default:
        throw std::runtime_error("Unknown or unsupported hash algorithm.");
    }
}

std::string compute_hash(const fs::path &path, HashAlgorithm algo)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open file for hashing: " + path.string());
    }
    std::vector<char> buffer(8192);

    switch (algo)
    {
    case HashAlgorithm::BLAKE3:
    {
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0)
        {
            blake3_hasher_update(&hasher, reinterpret_cast<const uint8_t *>(buffer.data()), file.gcount());
        }
        uint8_t hash[BLAKE3_OUT_LEN];
        blake3_hasher_finalize(&hasher, hash, BLAKE3_OUT_LEN);
        return bytes_to_hex(hash, BLAKE3_OUT_LEN);
    }
    case HashAlgorithm::SHA256:
    {
        SHA256 sha256;
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0)
        {
            sha256.add(buffer.data(), file.gcount());
        }
        return sha256.getHash();
    }
    case HashAlgorithm::SHA3_256:
    {
        SHA3 sha3(SHA3::Bits256);
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0)
        {
            sha3.add(buffer.data(), file.gcount());
        }
        return sha3.getHash();
    }
    default:
        throw std::runtime_error("Unknown or unsupported hash algorithm.");
    }
}

// ... (reszta funkcji pomocniczych, które się nie zmieniają)
std::string to_forward_slash(const fs::path &p) { return p.generic_string(); }
uintmax_t parse_size(const std::string &size_str)
{
    std::regex re(R"((\d+)([bkmg]?)?)", std::regex_constants::icase);
    std::smatch match;
    if (std::regex_match(size_str, match, re))
    {
        uintmax_t value = std::stoull(match[1].str());
        if (match[2].matched)
        {
            char unit = std::tolower(match[2].str()[0]);
            if (unit == 'k')
                value *= 1024;
            else if (unit == 'm')
                value *= 1024 * 1024;
            else if (unit == 'g')
                value *= 1024 * 1024 * 1024;
        }
        return value;
    }
    throw std::invalid_argument("Invalid size format: " + size_str);
}
bool is_binary(const fs::path &file_path)
{
    constexpr size_t check_size = 8000;
    std::ifstream file(file_path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open file: " + file_path.string());
    }
    std::vector<char> buffer(check_size);
    file.read(buffer.data(), buffer.size());
    std::streamsize bytes_read = file.gcount();
    return std::find(buffer.begin(), buffer.begin() + bytes_read, '\0') != buffer.begin() + bytes_read;
}
struct IgnoreRule
{
    std::regex regex;
    bool is_negated;
};
std::vector<IgnoreRule> load_dpignore_rules(const fs::path &base_dir, bool verbose)
{
    std::vector<IgnoreRule> rules;
    fs::path ignore_file = base_dir / ".dpignore";
    if (verbose)
        std::cerr << "🔍 Looking for .dpignore in: " << base_dir << std::endl;
    if (fs::exists(ignore_file) && fs::is_regular_file(ignore_file))
    {
        if (verbose)
            std::cerr << "✅ .dpignore found and opened." << std::endl;
        std::ifstream file(ignore_file);
        if (!file.is_open())
        {
            std::cerr << "Warning: Could not read .dpignore in " << base_dir << std::endl;
            return rules;
        }
        std::string line;
        while (std::getline(file, line))
        {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty() || line[0] == '#')
                continue;
            bool is_negated = false;
            if (line[0] == '!')
            {
                is_negated = true;
                line = line.substr(1);
            }
            if (line.empty())
                continue;
            std::string regex_pattern;
            for (char c : line)
            {
                if (c == '*')
                    regex_pattern += ".*";
                else if (c == '?')
                    regex_pattern += ".";
                else if (std::string(".+()[]{}|^$\\").find(c) != std::string::npos)
                    regex_pattern += std::string("\\") + c;
                else
                    regex_pattern += c;
            }
            try
            {
                IgnoreRule rule = {std::regex(regex_pattern, std::regex_constants::icase), is_negated};
                rules.push_back(rule);
                if (verbose)
                    std::cerr << "📋 Loaded rule: " << (is_negated ? "EXCEPT " : "IGNORE ") << regex_pattern << std::endl;
            }
            catch (const std::regex_error &e)
            {
                std::cerr << "Warning: Invalid regex in .dpignore: " << line << " -> " << regex_pattern << " (" << e.what() << ")" << std::endl;
            }
        }
    }
    else
    {
        if (verbose)
            std::cerr << "❌ .dpignore NOT found in: " << base_dir << std::endl;
    }
    return rules;
}
bool should_ignore(const fs::path &path, const std::vector<IgnoreRule> &rules, bool verbose)
{
    std::string path_str = to_forward_slash(path);
    bool is_ignored = false;
    for (const auto &rule : rules)
    {
        if (std::regex_search(path_str, rule.regex))
        {
            is_ignored = !rule.is_negated;
        }
    }
    if (is_ignored && verbose)
    {
        std::cerr << "🚫 Ignored by rule: " << path_str << std::endl;
    }
    return is_ignored;
}

struct Stats
{
    uintmax_t processed = 0, ignored = 0, too_large = 0, binary_skipped = 0, text = 0, binary = 0, errors = 0, total_size = 0; // <-- DODANA KATEGORIA `errors`
    void print() const
    {
        std::cerr << "\n--- STATISTICS ---\n"
                  << "Processed files:     " << processed << "\n"
                  << "  - Text files:      " << text << "\n"
                  << "  - Binary files:    " << binary << "\n"
                  << "Total data size:     " << total_size << " bytes\n"
                  << "Skipped (ignored):   " << ignored << "\n"
                  << "Skipped (too large): " << too_large << "\n"
                  << "Skipped (binary):    " << binary_skipped << "\n"
                  << "Access errors:       " << errors << "\n" // <-- DODANA LINIA W WYDRUKU
                  << "------------------\n";
    }
};

void serialize_directory(const fs::path &input_dir, std::ostream &output_stream, HashAlgorithm algo, bool text_only, uintmax_t size_limit, bool has_limit, bool dry_run, bool verbose, Stats &stats)
{
    auto rules = load_dpignore_rules(input_dir, verbose);
    if (!fs::is_directory(input_dir))
    {
        throw std::runtime_error("Input path is not a directory: " + input_dir.string());
    }
    std::string algo_name = to_string(algo);

    // --- ZMIANA: ZASTĘPUJEMY PĘTLĘ `for` PĘTLĄ `while` Z OBSŁUGĄ BŁĘDÓW ---
    std::error_code ec;
    fs::recursive_directory_iterator it(input_dir, fs::directory_options::skip_permission_denied, ec);
    fs::recursive_directory_iterator end;

    while (it != end)
    {
        // Główna logika przetwarzania pliku
        try
        {
            const fs::path &path = it->path();

            if (!it->is_regular_file())
            {
                // Pomijamy katalogi i inne typy, ale robimy to w sposób bezpieczny
                // (w odróżnieniu od `continue`, które mogłoby przeskoczyć inkrementację)
            }
            else
            {
                fs::path rel = fs::relative(path, input_dir);
                if (should_ignore(rel, rules, verbose))
                {
                    stats.ignored++;
                }
                else
                {
                    uintmax_t size = fs::file_size(path);
                    if (has_limit && size > size_limit)
                    {
                        if (verbose)
                            std::cerr << "📏 Skipped (too large): " << to_forward_slash(rel) << " (" << size << " bytes)" << std::endl;
                        stats.too_large++;
                    }
                    else
                    {
                        bool is_file_binary = is_binary(path);
                        if (text_only && is_file_binary)
                        {
                            if (verbose)
                                std::cerr << "🗎 Skipped (binary, --text-only): " << to_forward_slash(rel) << std::endl;
                            stats.binary_skipped++;
                        }
                        else
                        {
                            if (verbose)
                                std::cerr << "📄 Processing: " << to_forward_slash(rel) << std::endl;
                            stats.processed++;
                            stats.total_size += size;
                            std::string hash = compute_hash(path, algo);
                            std::string rel_str = to_forward_slash(rel);

                            if (is_file_binary)
                            {
                                stats.binary++;
                                if (!dry_run)
                                {
                                    output_stream << "--- START BINARY FILE path: \"" << rel_str << "\" size: " << size << " b " << algo_name << ": " << hash << " ---\n";
                                    if (size > 0)
                                    {
                                        std::ifstream file(path, std::ios::binary);
                                        std::vector<uint8_t> data(size);
                                        file.read(reinterpret_cast<char *>(data.data()), size);
                                        output_stream << cppcodec::base64_rfc4648::encode(data);
                                    }
                                    output_stream << END_OF_RECORD;
                                }
                            }
                            else
                            {
                                stats.text++;
                                if (!dry_run)
                                {
                                    output_stream << "--- START TEXT FILE path: \"" << rel_str << "\" size: " << size << " b " << algo_name << ": " << hash << " ---\n";
                                    if (size > 0)
                                    {
                                        std::ifstream file(path, std::ios::binary);
                                        std::vector<char> data(size);
                                        file.read(data.data(), size);
                                        output_stream.write(data.data(), size);
                                    }
                                    output_stream << END_OF_RECORD;
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (const fs::filesystem_error &e)
        {
            stats.errors++;
            std::string error_path_str = to_forward_slash(e.path1());
            std::string error_message = "ERROR: Failed to process path.\nPath: " + error_path_str + "\nReason: " + e.what();
            if (verbose)
                std::cerr << "🔥 " << error_message << std::endl;

            if (!dry_run)
            {
                std::string error_hash = compute_hash_from_memory(error_message, algo);
                output_stream << "--- START ERROR MESSAGE path: \"" << error_path_str << "\" size: " << error_message.length() << " b " << algo_name << ": " << error_hash << " ---\n";
                output_stream << error_message;
                output_stream << END_OF_RECORD;
            }
        }

        // Inkrementacja iteratora z obsługą błędów
        it.increment(ec);
        if (ec)
        {
            stats.errors++;
            std::string error_path_str = to_forward_slash(it->path());
            std::string error_message = "ERROR: Cannot iterate filesystem.\nPath: " + error_path_str + "\nReason: " + ec.message();
            if (verbose)
                std::cerr << "🔥 " << error_message << std::endl;

            if (!dry_run)
            {
                std::string error_hash = compute_hash_from_memory(error_message, algo);
                output_stream << "--- START ERROR MESSAGE path: \"" << error_path_str << "\" size: " << error_message.length() << " b " << algo_name << ": " << error_hash << " ---\n";
                output_stream << error_message;
                output_stream << END_OF_RECORD;
            }
            // Spróbuj pominąć błędny wpis. Jeśli to się nie uda, pętla się zakończy.
            it.pop(ec);
        }
    }
}

// ... (reszta pliku: Args, parse_args, show_help, main - pozostają bez zmian)
struct Args
{
    fs::path input_dir;
    fs::path output_file;
    bool show_help = false;
    bool show_version = false;
    bool text_only = false;
    bool dry_run = false;
    bool show_stats = false;
    bool verbose = false;
    uintmax_t size_limit = 0;
    bool has_limit = false;
    HashAlgorithm algo = HashAlgorithm::BLAKE3;
};
Args parse_args(int argc, char **argv)
{
    Args args;
    std::vector<std::string> pos_args;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help")
        {
            args.show_help = true;
        }
        else if (arg == "-v" || arg == "--version")
        {
            args.show_version = true;
        }
        else if (arg == "-d" || arg == "--dir" || arg == "-i" || arg == "--input")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            args.input_dir = argv[++i];
        }
        else if (arg == "-o" || arg == "--out")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            args.output_file = argv[++i];
        }
        else if (arg == "-t" || arg == "--text-only")
        {
            args.text_only = true;
        }
        else if (arg == "--dry-run")
        {
            args.dry_run = true;
        }
        else if (arg == "--stats")
        {
            args.show_stats = true;
        }
        else if (arg == "--verbose")
        {
            args.verbose = true;
        }
        else if (arg == "-l" || arg == "--limit")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            try
            {
                args.size_limit = parse_size(argv[++i]);
                args.has_limit = true;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error: Invalid size format: " << argv[i] << " (" << e.what() << ")\n";
                std::exit(1);
            }
        }
        else if (arg == "-a" || arg == "--algorithm")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: Missing argument after " << arg << "\n";
                std::exit(1);
            }
            std::string algo_str = argv[++i];
            std::transform(algo_str.begin(), algo_str.end(), algo_str.begin(), ::tolower);
            if (algo_str == "blake3")
            {
                args.algo = HashAlgorithm::BLAKE3;
            }
            else if (algo_str == "sha256")
            {
                args.algo = HashAlgorithm::SHA256;
            }
            else if (algo_str == "sha3" || algo_str == "sha3-256")
            {
                args.algo = HashAlgorithm::SHA3_256;
            }
            else
            {
                std::cerr << "Error: Unknown algorithm '" << argv[i] << "'. Available: blake3, sha256, sha3-256.\n";
                std::exit(1);
            }
        }
        else
        {
            if (arg[0] == '-')
            {
                std::cerr << "Unknown argument: " << arg << "\nUse -h or --help for usage.\n";
                std::exit(1);
            }
            pos_args.push_back(arg);
        }
    }
    if (!args.show_help && !args.show_version)
    {
        if (args.input_dir.empty() && !pos_args.empty())
        {
            args.input_dir = pos_args[0];
        }
        else if (args.input_dir.empty())
        {
            std::cerr << "Error: Input directory is required.\nUse -h or --help for usage.\n";
            std::exit(1);
        }
    }
    return args;
}
void show_help(const char *exec_name) { std::cout << "DirPacker v" << VERSION << "\n"
                                                  << "Packs a directory structure into a single text-based archive.\n\n"
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
                                                  << std::endl; }
int main(int argc, char **argv)
{
    auto args = parse_args(argc, argv);
    if (args.show_version)
    {
        std::cout << "DirPacker v" << VERSION << std::endl;
        return 0;
    }
    if (args.show_help)
    {
        show_help(argv[0]);
        return 0;
    }
    Stats stats;
    try
    {
        if (args.dry_run)
        {
            serialize_directory(args.input_dir, std::cout, args.algo, args.text_only, args.size_limit, args.has_limit, true, true, stats);
        }
        else if (!args.output_file.empty())
        {
            std::ofstream out(args.output_file, std::ios::binary);
            if (!out)
            {
                throw std::runtime_error("Cannot create output file: " + args.output_file.string());
            }
            serialize_directory(args.input_dir, out, args.algo, args.text_only, args.size_limit, args.has_limit, false, args.verbose, stats);
        }
        else
        {
            serialize_directory(args.input_dir, std::cout, args.algo, args.text_only, args.size_limit, args.has_limit, false, args.verbose, stats);
        }
        if (args.show_stats)
        {
            stats.print();
        }
        if (args.dry_run)
        {
            std::cerr << "Dry run completed. No data was written.\n";
        }
        else if (!args.output_file.empty())
        {
            std::cerr << "Serialization completed successfully. Output saved to: " << args.output_file << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "A critical error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}