#pragma once

#include <string>
#include <filesystem>
#include <ostream>
#include <cstdint>

namespace dp {

namespace fs = std::filesystem;

enum class HashAlgorithm {
    SHA256,
    SHA3
};

struct Stats {
    uintmax_t processed = 0;
    uintmax_t ignored = 0;
    uintmax_t too_large = 0;
    uintmax_t binary_skipped = 0;
    uintmax_t text = 0;
    uintmax_t binary = 0;
    uintmax_t errors = 0;
    uintmax_t total_size = 0;
};

struct PackerConfig {
    fs::path input_dir;
    std::ostream& output_stream;
    HashAlgorithm algo = HashAlgorithm::SHA3;
    bool text_only = false;
    uintmax_t size_limit = 0;
    bool has_limit = false;
    bool dry_run = false;
    bool verbose = false;
};

// Main packing function
Stats pack(const PackerConfig& config);

} // namespace dp