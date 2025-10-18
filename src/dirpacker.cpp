#include "dirpacker.hpp"

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

#include "base64.hpp"
#include "sha256.hpp"
#include "sha3.hpp"

namespace dp {

namespace {

const std::string END_OF_RECORD = "\n--- END OF FILE ---\n";

std::string to_string(HashAlgorithm algo) {
    static const std::map<HashAlgorithm, std::string> lookup = {
        {HashAlgorithm::SHA256, "sha256"},
        {HashAlgorithm::SHA3, "sha3"}
    };
    auto it = lookup.find(algo);
    if (it != lookup.end()) {
        return it->second;
    }
    throw std::runtime_error("Unknown hash algorithm");
}

std::string compute_hash_from_memory(const std::string &data, HashAlgorithm algo) {
    const char *data_ptr = data.c_str();
    size_t data_len = data.length();
    
    switch (algo) {
    case HashAlgorithm::SHA256: {
        SHA256 sha256;
        sha256.add(data_ptr, data_len);
        return sha256.getHash();
    }
    case HashAlgorithm::SHA3: {
        SHA3 sha3(SHA3::Bits256);
        sha3.add(data_ptr, data_len);
        return sha3.getHash();
    }
    }
    throw std::runtime_error("Unknown hash algorithm.");
}

std::string compute_hash(const fs::path &path, HashAlgorithm algo) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for hashing: " + path.string());
    }
    
    std::vector<char> buffer(8192);
    
    switch (algo) {
    case HashAlgorithm::SHA256: {
        SHA256 sha256;
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
            sha256.add(buffer.data(), file.gcount());
        }
        return sha256.getHash();
    }
    case HashAlgorithm::SHA3: {
        SHA3 sha3(SHA3::Bits256);
        while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
            sha3.add(buffer.data(), file.gcount());
        }
        return sha3.getHash();
    }
    }
    throw std::runtime_error("Unknown hash algorithm.");
}

std::string to_forward_slash(const fs::path &p) {
    return p.generic_string();
}

bool is_binary(const fs::path &file_path) {
    constexpr size_t check_size = 8000;
    std::ifstream file(file_path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for binary check: " + file_path.string());
    }
    
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
    
    if (verbose) {
        std::cerr << "🔍 Looking for .dpignore in: " << base_dir << std::endl;
    }
    
    if (fs::exists(ignore_file) && fs::is_regular_file(ignore_file)) {
        if (verbose) {
            std::cerr << "✅ .dpignore found and opened." << std::endl;
        }
        
        std::ifstream file(ignore_file);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not read .dpignore in " << base_dir << std::endl;
            return rules;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            // Check for negation
            bool is_negated = (line[0] == '!');
            if (is_negated) {
                line = line.substr(1);
            }
            if (line.empty()) continue;
            
            // Convert glob pattern to regex
            std::string regex_pattern;
            for (char c : line) {
                if (c == '*') {
                    regex_pattern += ".*";
                } else if (c == '?') {
                    regex_pattern += ".";
                } else if (std::string(".+()[]{}|^$\\").find(c) != std::string::npos) {
                    regex_pattern += std::string("\\") + c;
                } else {
                    regex_pattern += c;
                }
            }
            
            try {
                rules.push_back({std::regex(regex_pattern, std::regex_constants::icase), is_negated});
                if (verbose) {
                    std::cerr << "📋 Loaded rule: " << (is_negated ? "EXCEPT " : "IGNORE ") 
                             << regex_pattern << std::endl;
                }
            } catch (const std::regex_error &e) {
                std::cerr << "Warning: Invalid regex in .dpignore: " << line 
                         << " (" << e.what() << ")" << std::endl;
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
    
    for (auto it = fs::recursive_directory_iterator(config.input_dir, 
                                                     fs::directory_options::skip_permission_denied, ec);
         it != fs::recursive_directory_iterator(); ) {
        
        if (ec) {
            stats.errors++;
            std::string error_path_str = to_forward_slash(config.input_dir);
            std::string error_message = "ERROR: Cannot start iteration.\nPath: " + error_path_str + 
                                       "\nReason: " + ec.message();
            if (config.verbose) {
                std::cerr << "🔥 " << error_message << std::endl;
            }
            if (!config.dry_run) {
                std::string error_hash = compute_hash_from_memory(error_message, config.algo);
                config.output_stream << "--- START ERROR MESSAGE path: \"" << error_path_str 
                                    << "\" size: " << error_message.length() 
                                    << " bytes " << algo_name << ": " << error_hash << " ---\n";
                config.output_stream << error_message;
                config.output_stream << END_OF_RECORD;
            }
            break;
        }
        
        try {
            const fs::path &path = it->path();
            
            if (it->is_regular_file(ec)) {
                if (ec) {
                    throw fs::filesystem_error("Cannot check if regular file", path, ec);
                }
                
                fs::path rel = fs::relative(path, config.input_dir);
                
                if (should_ignore(rel, rules, config.verbose)) {
                    stats.ignored++;
                } else {
                    uintmax_t size = fs::file_size(path, ec);
                    if (ec) {
                        throw fs::filesystem_error("Cannot get file size", path, ec);
                    }
                    
                    if (config.has_limit && size > config.size_limit) {
                        if (config.verbose) {
                            std::cerr << "📏 Skipped (too large): " << to_forward_slash(rel) 
                                     << " (" << size << " bytes)" << std::endl;
                        }
                        stats.too_large++;
                    } else {
                        bool is_file_binary = is_binary(path);
                        
                        if (config.text_only && is_file_binary) {
                            if (config.verbose) {
                                std::cerr << "🗎 Skipped (binary, --text-only): " 
                                         << to_forward_slash(rel) << std::endl;
                            }
                            stats.binary_skipped++;
                        } else {
                            if (config.verbose) {
                                std::cerr << "📄 Processing: " << to_forward_slash(rel) << std::endl;
                            }
                            
                            stats.processed++;
                            stats.total_size += size;
                            
                            std::string rel_str = to_forward_slash(rel);

                            if (is_file_binary) {
                                stats.binary++;
                                if (!config.dry_run) {
                                    std::string hash = compute_hash_from_memory(path, config.algo);
                                    config.output_stream << "--- START BINARY FILE path: \"" << rel_str 
                                                        << "\" size: " << size << " bytes " << algo_name << ": " << hash << " ---\n";
                                    if (size > 0) {
                                        std::ifstream file(path, std::ios::binary);
                                        std::vector<uint8_t> data(size);
                                        file.read(reinterpret_cast<char *>(data.data()), size);
                                        config.output_stream << Base64::encode(data);
                                    }
                                    config.output_stream << END_OF_RECORD;
                                }
                            } else {
                                stats.text++;
                                if (!config.dry_run) {
                                    std::string hash = compute_hash_from_memory(path, config.algo);
                                    config.output_stream << "--- START TEXT FILE path: \"" << rel_str 
                                                        << "\" size: " << size << " bytes " << algo_name << ": " << hash << " ---\n";
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
            std::string error_message = "ERROR: Failed to process path.\nPath: " + error_path_str + 
                                       "\nReason: " + e.what();
            if (config.verbose) {
                std::cerr << "🔥 " << error_message << std::endl;
            }
            if (!config.dry_run) {
                std::string error_hash = compute_hash_from_memory(error_message, config.algo);
                config.output_stream << "--- START ERROR MESSAGE path: \"" << error_path_str 
                                    << "\" size: " << error_message.length() 
                                    << " bytes " << algo_name << ": " << error_hash << " ---\n";
                config.output_stream << error_message;
                config.output_stream << END_OF_RECORD;
            }
        } catch (const std::exception &e) {
            stats.errors++;
            std::string error_message = std::string("ERROR: Unexpected error: ") + e.what();
            if (config.verbose) {
                std::cerr << "🔥 " << error_message << std::endl;
            }
            if (!config.dry_run) {
                std::string error_hash = compute_hash_from_memory(error_message, config.algo);
                config.output_stream << "--- START ERROR MESSAGE path: \"unknown\" size: " 
                                    << error_message.length() << " bytes " << algo_name 
                                    << ": " << error_hash << " ---\n";
                config.output_stream << error_message;
                config.output_stream << END_OF_RECORD;
            }
        }
        
        // Increment iterator with error checking
        it.increment(ec);
        if (ec) {
            stats.errors++;
            std::string error_message = "ERROR: Cannot advance iterator.\nReason: " + ec.message();
            if (config.verbose) {
                std::cerr << "🔥 " << error_message << std::endl;
            }
            if (!config.dry_run) {
                std::string error_hash = compute_hash_from_memory(error_message, config.algo);
                config.output_stream << "--- START ERROR MESSAGE path: \"iterator\" size: " 
                                    << error_message.length() << " bytes " << algo_name 
                                    << ": " << error_hash << " ---\n";
                config.output_stream << error_message;
                config.output_stream << END_OF_RECORD;
            }
            break;
        }
    }
}

} // anonymous namespace

Stats pack(const PackerConfig& config) {
    Stats stats;
    serialize_directory(config, stats);
    return stats;
}

} // namespace dp