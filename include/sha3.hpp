#pragma once

#include <string>
#include <cstdint>

class SHA3 {
public:
    enum Bits {
        Bits224 = 224,
        Bits256 = 256,
        Bits384 = 384,
        Bits512 = 512
    };

    explicit SHA3(Bits bits);
    void add(const char* data, size_t length);
    std::string getHash();

private:
    void reset();
    void processBlock();
    void keccakF();
    
    uint64_t m_state[25];
    uint8_t m_buffer[200];
    size_t m_bufferSize;
    size_t m_blockSize;
    size_t m_hashSize;
    
    static const uint64_t ROUND_CONSTANTS[24];
    static const int ROTATION_OFFSETS[24];
    
    static uint64_t rotateLeft(uint64_t x, int n);
};
