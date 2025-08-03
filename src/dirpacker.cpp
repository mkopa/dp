#include "dirpacker/dirpacker.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <regex>
#include <iomanip>
#include <map>
#include <system_error>

#include "blake3.h"
#include <cppcodec/base64_rfc4648.hpp>
#include "sha256.h"
#include "sha3.h"

namespace dp {

namespace {

const std::string END_OF_RECORD = "\n--- END_OF_FILE ---\n";

std::string to_string(HashAlgorithm algo) {
    static const std::map<HashAlgorithm, std::string> lookup = {
        {HashAlgorithm::BLAKE3, "blake3"},
        {HashAlgorithm::SHA256, "sha256"},
        {HashAlgorithm::SHA3_256, "sha3-256"}};
    return lookup.at(algo);
}

std::string bytes_to_hex(const uint8_t *bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

std::string compute_hash_from_memory(const std::string &data, HashAlgorithm algo) {
    const char *data_ptr = data.c_str();
    size_t data_len = data.length();
    switch (algo) {
    case HashAlgorithm::BLAKE3: {
        uint8_t hash[BLAKE3_OUT_LEN];
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, data_ptr, data_len);
        blake3_hasher_finalize(&hasher, hash, BLAKE3_OUT_LEN);
        return bytes_to_hex(hash, BLAKE3_OUT_LEN);
    }
    case HashAlgorithm::SHA256: {
        SHA256 sha256;
        sha256.add(data_ptr, data_len);
        return sha256.getHash();
    }
    case HashAlgorithm::SHA3_256: {
        SHA3 sha3(SHA3::Bits256);
        sha3.add(data_ptr, data_len);
        return sha3.getHash();
    }
    }
    throw std::runtime_error("Unknown hash algorithm.");
}

std::string compute_hash(const fs::path &path, HashAlgorithm algo) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open file for hashing: " + path.string());
    std::vector<char> buffer(8192);
    switch (algo) {
    case HashAlgorithm::BLAKE3: {
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
            blake3_hasher_update(&hasher, reinterpret_cast<const uint8_t *>(buffer.data()), file.gcount());
        }
        uint8_t hash[BLAKE3_OUT_LEN];
        blake3_hasher_finalize(&hasher, hash, BLAKE3_OUT_LEN);
        return bytes_to_hex(hash, BLAKE3_OUT_LEN);
    }
    case HashAlgorithm::SHA256: {
        SHA256 sha256;
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
            sha256.add(buffer.data(), file.gcount());
        }
        return sha256.getHash();
    }
    case HashAlgorithm::SHA3_256: {
        SHA3 sha3(SHA3::Bits256);
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
            sha3.add(buffer.data(), file.gcount());
        }
        return sha3.getHash();
    }
    }
    throw std::runtime_error("Unknown hash algorithm.");
}

std::string to_forward_slash(const fs::path &p) { return p.generic_string(); }

bool is_binary(const fs::path &file_path) {
    constexpr size_t check_size = 8000;
    std::ifstream file(file_path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open file for binary check: " + file_path.string());
    std::vector<char> buffer(check_size);
    file.read(buffer.data(), buffer.size());
    std::streamsize bytes_read = file.gcount();
    return std::find(buffer.begin(), buffer.begin() + bytes_read, '\0') != buffer.begin() + bytes_read;
}

struct IgnoreRule {
    std::regex regex;
    bool is_negated;
};

std::vector<IgnoreRule> load_dpignore_rules(const fs::path &base_dir, bool verbose) {
    std::vector<IgnoreRule> rules;
    fs::path ignore_file = base_dir / ".dpignore";
    if (verbose) std::cerr << "🔍 Looking for .dpignore in: " << base_dir << std::endl;
    if (fs::exists(ignore_file) && fs::is_regular_file(ignore_file)) {
        if (verbose) std::cerr << "✅ .dpignore found and opened." << std::endl;
        std::ifstream file(ignore_file);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not read .dpignore in " << base_dir << std::endl;
            return rules;
        }
        std::string line;
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            if (line.empty() || line[0] == '#') continue;
            bool is_negated = (line[0] == '!');
            if (is_negated) line = line.substr(1);
            if (line.empty()) continue;
            
            std::string regex_pattern;
            for (char c : line) {
                if (c == '*') regex_pattern += ".*";
                else if (c == '?') regex_pattern += ".";
                else if (std::string(".+()[]{}|^$\\").find(c) != std::string::npos) regex_pattern += std::string("\\") + c;
                else regex_pattern += c;
            }
            try {
                rules.push_back({std::regex(regex_pattern, std::regex_constants::icase), is_negated});
                if (verbose) std::cerr << "📋 Loaded rule: " << (is_negated ? "EXCEPT " : "IGNORE ") << regex_pattern << std::endl;
            } catch (const std::regex_error &e) {
                std::cerr << "Warning: Invalid regex in .dpignore: " << line << " (" << e.what() << ")" << std::endl;
            }
        }
    } else if (verbose) {
        std::cerr << "❌ .dpignore NOT found in: " << base_dir << std::endl;
    }
    return rules;
}

bool should_ignore(const fs::path &path, const std::vector<IgnoreRule> &rules, bool verbose) {
    std::string path_str = to_forward_slash(path);
    bool is_ignored = false;
    for (const auto &rule : rules) {
        if (std::regex_search(path_str, rule.regex)) {
            is_ignored = !rule.is_negated;
        }
    }
    if (is_ignored && verbose) {
        std::cerr << "🚫 Ignored by rule: " << path_str << std::endl;
    }
    return is_ignored;
}


void serialize_directory(const PackerConfig& config, Stats& stats) {
    auto rules = load_dpignore_rules(config.input_dir, config.verbose);
    if (!fs::is_directory(config.input_dir)) {
        throw std::runtime_error("Input path is not a directory: " + config.input_dir.string());
    }
    std::string algo_name = to_string(config.algo);
    
    std::error_code ec;
    fs::recursive_directory_iterator it(config.input_dir, fs::directory_options::skip_permission_denied, ec);
    fs::recursive_directory_iterator end;

    while (it != end) {
        try {
            const fs::path &path = it->path();
            if (it->is_regular_file()) {
                fs::path rel = fs::relative(path, config.input_dir);
                if (should_ignore(rel, rules, config.verbose)) {
                    stats.ignored++;
                } else {
                    uintmax_t size = fs::file_size(path);
                    if (config.has_limit && size > config.size_limit) {
                        if (config.verbose) std::cerr << "📏 Skipped (too large): " << to_forward_slash(rel) << " (" << size << " bytes)" << std::endl;
                        stats.too_large++;
                    } else {
                        bool is_file_binary = is_binary(path);
                        if (config.text_only && is_file_binary) {
                            if (config.verbose) std::cerr << "🗎 Skipped (binary, --text-only): " << to_forward_slash(rel) << std::endl;
                            stats.binary_skipped++;
                        } else {
                            if (config.verbose) std::cerr << "📄 Processing: " << to_forward_slash(rel) << std::endl;
                            stats.processed++;
                            stats.total_size += size;
                            std::string hash = compute_hash(path, config.algo);
                            std::string rel_str = to_forward_slash(rel);

                            if (is_file_binary) {
                                stats.binary++;
                                if (!config.dry_run) {
                                    config.output_stream << "--- START BINARY FILE path: \"" << rel_str << "\" size: " << size << " b " << algo_name << ": " << hash << " ---\n";
                                    if (size > 0) {
                                        std::ifstream file(path, std::ios::binary);
                                        std::vector<uint8_t> data(size);
                                        file.read(reinterpret_cast<char *>(data.data()), size);
                                        config.output_stream << cppcodec::base64_rfc4648::encode(data);
                                    }
                                    config.output_stream << END_OF_RECORD;
                                }
                            } else {
                                stats.text++;
                                if (!config.dry_run) {
                                    config.output_stream << "--- START TEXT FILE path: \"" << rel_str << "\" size: " << size << " b " << algo_name << ": " << hash << " ---\n";
                                    if (size > 0) {
                                        std::ifstream file(path, std::ios::binary);
                                        config.output_stream << file.rdbuf();
                                    }
                                    config.output_stream << END_OF_RECORD;
                                }
                            }
                        }
                    }
                }
            }
        } catch (const fs::filesystem_error &e) {
            stats.errors++;
            std::string error_path_str = to_forward_slash(e.path1());
            std::string error_message = "ERROR: Failed to process path.\nPath: " + error_path_str + "\nReason: " + e.what();
            if (config.verbose) std::cerr << "🔥 " << error_message << std::endl;
            if (!config.dry_run) {
                std::string error_hash = compute_hash_from_memory(error_message, config.algo);
                config.output_stream << "--- START ERROR MESSAGE path: \"" << error_path_str << "\" size: " << error_message.length() << " b " << algo_name << ": " << error_hash << " ---\n";
                config.output_stream << error_message;
                config.output_stream << END_OF_RECORD;
            }
        }
        
        it.increment(ec);
        if (ec) {
            stats.errors++;
            std::string error_path_str = to_forward_slash(it->path());
            std::string error_message = "ERROR: Cannot iterate filesystem.\nPath: " + error_path_str + "\nReason: " + ec.message();
            if (config.verbose) std::cerr << "🔥 " << error_message << std::endl;
            if (!config.dry_run) {
                 std::string error_hash = compute_hash_from_memory(error_message, config.algo);
                config.output_stream << "--- START ERROR MESSAGE path: \"" << error_path_str << "\" size: " << error_message.length() << " b " << algo_name << ": " << error_hash << " ---\n";
                config.output_stream << error_message;
                config.output_stream << END_OF_RECORD;
            }
            it.pop(ec);
        }
    }
}

} 

Stats pack(const PackerConfig& config) {
    Stats stats;
    serialize_directory(config, stats);
    return stats;
}

}