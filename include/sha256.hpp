#pragma once

#include <string>
#include <cstdint>

class SHA256 {
public:
    SHA256();
    void add(const char* data, size_t length);
    std::string getHash();

private:
    void transform(const uint8_t* data);
    void pad();
    
    uint32_t m_state[8];
    uint64_t m_bitlen;
    uint8_t m_buffer[64];
    uint32_t m_buflen;
    
    static const uint32_t K[64];
    
    static uint32_t rotr(uint32_t x, uint32_t n);
    static uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t maj(uint32_t x, uint32_t y, uint32_t z);
    static uint32_t sigma0(uint32_t x);
    static uint32_t sigma1(uint32_t x);
    static uint32_t gamma0(uint32_t x);
    static uint32_t gamma1(uint32_t x);
};
