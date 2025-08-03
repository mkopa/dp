#pragma once

#include <string>
#include <filesystem>
#include <ostream>
#include <cstdint>
#include <vector>

namespace dp {

namespace fs = std::filesystem;

enum class HashAlgorithm {
    BLAKE3,
    SHA256,
    SHA3_256
};

struct Stats {
    uintmax_t processed = 0, ignored = 0, too_large = 0, 
              binary_skipped = 0, text = 0, binary = 0, 
              errors = 0, total_size = 0;
};

struct PackerConfig {
    fs::path input_dir;
    std::ostream& output_stream;
    HashAlgorithm algo = HashAlgorithm::BLAKE3;
    bool text_only = false;
    uintmax_t size_limit = 0;
    bool has_limit = false;
    bool dry_run = false;
    bool verbose = false;
};

Stats pack(const PackerConfig& config);

} // namespace dp