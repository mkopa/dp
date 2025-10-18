#include "base64.hpp"
#include <cstring>

const char Base64::encoding_table[65] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const uint8_t Base64::decoding_table[256] = {
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

bool Base64::is_base64(uint8_t c) {
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' || c == '/' || c == '=';
}

size_t Base64::encoded_size(size_t input_size) {
    return ((input_size + 2) / 3) * 4;
}

size_t Base64::decoded_size(const std::string& encoded) {
    size_t len = encoded.length();
    size_t padding = 0;
    
    if (len >= 2) {
        if (encoded[len - 1] == '=') padding++;
        if (encoded[len - 2] == '=') padding++;
    }
    
    return (len / 4) * 3 - padding;
}

std::string Base64::encode(const uint8_t* data, size_t length) {
    if (length == 0) return "";
    
    size_t output_length = encoded_size(length);
    std::string encoded;
    encoded.reserve(output_length);
    
    size_t i = 0;
    uint8_t array_3[3];
    uint8_t array_4[4];
    
    while (length--) {
        array_3[i++] = *(data++);
        if (i == 3) {
            array_4[0] = (array_3[0] & 0xfc) >> 2;
            array_4[1] = ((array_3[0] & 0x03) << 4) + ((array_3[1] & 0xf0) >> 4);
            array_4[2] = ((array_3[1] & 0x0f) << 2) + ((array_3[2] & 0xc0) >> 6);
            array_4[3] = array_3[2] & 0x3f;
            
            for (i = 0; i < 4; i++) {
                encoded += encoding_table[array_4[i]];
            }
            i = 0;
        }
    }
    
    if (i > 0) {
        for (size_t j = i; j < 3; j++) {
            array_3[j] = 0;
        }
        
        array_4[0] = (array_3[0] & 0xfc) >> 2;
        array_4[1] = ((array_3[0] & 0x03) << 4) + ((array_3[1] & 0xf0) >> 4);
        array_4[2] = ((array_3[1] & 0x0f) << 2) + ((array_3[2] & 0xc0) >> 6);
        
        for (size_t j = 0; j <= i; j++) {
            encoded += encoding_table[array_4[j]];
        }
        
        while (i++ < 3) {
            encoded += '=';
        }
    }
    
    return encoded;
}

std::string Base64::encode(const std::vector<uint8_t>& data) {
    return encode(data.data(), data.size());
}

std::string Base64::encode(const std::string& data) {
    return encode(reinterpret_cast<const uint8_t*>(data.data()), data.size());
}

std::vector<uint8_t> Base64::decode(const std::string& encoded) {
    size_t in_len = encoded.size();
    if (in_len == 0) return {};
    if (in_len % 4 != 0) {
        throw std::invalid_argument("Invalid Base64 string length");
    }
    
    size_t out_len = decoded_size(encoded);
    std::vector<uint8_t> decoded;
    decoded.reserve(out_len);
    
    size_t i = 0;
    uint8_t array_4[4];
    uint8_t array_3[3];
    
    for (char c : encoded) {
        if (!is_base64(static_cast<uint8_t>(c))) {
            throw std::invalid_argument("Invalid Base64 character");
        }
        
        if (c == '=') break;
        
        array_4[i++] = c;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                array_4[i] = decoding_table[static_cast<uint8_t>(array_4[i])];
            }
            
            array_3[0] = (array_4[0] << 2) + ((array_4[1] & 0x30) >> 4);
            array_3[1] = ((array_4[1] & 0x0f) << 4) + ((array_4[2] & 0x3c) >> 2);
            array_3[2] = ((array_4[2] & 0x03) << 6) + array_4[3];
            
            for (i = 0; i < 3; i++) {
                decoded.push_back(array_3[i]);
            }
            i = 0;
        }
    }
    
    if (i > 0) {
        for (size_t j = i; j < 4; j++) {
            array_4[j] = 0;
        }
        
        for (size_t j = 0; j < i; j++) {
            array_4[j] = decoding_table[static_cast<uint8_t>(array_4[j])];
        }
        
        array_3[0] = (array_4[0] << 2) + ((array_4[1] & 0x30) >> 4);
        array_3[1] = ((array_4[1] & 0x0f) << 4) + ((array_4[2] & 0x3c) >> 2);
        
        for (size_t j = 0; j < i - 1; j++) {
            decoded.push_back(array_3[j]);
        }
    }
    
    return decoded;
}