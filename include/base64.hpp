#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

class Base64 {
public:
    // Encode binary data to Base64 string
    static std::string encode(const uint8_t* data, size_t length);
    static std::string encode(const std::vector<uint8_t>& data);
    static std::string encode(const std::string& data);
    
    // Decode Base64 string to binary data
    static std::vector<uint8_t> decode(const std::string& encoded);
    
    // Calculate encoded size without actually encoding
    static size_t encoded_size(size_t input_size);
    
    // Calculate decoded size without actually decoding
    static size_t decoded_size(const std::string& encoded);

private:
    static const char encoding_table[65];
    static const uint8_t decoding_table[256];
    
    static void init_decoding_table();
    static bool is_base64(uint8_t c);
};