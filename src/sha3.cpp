#include "sha3.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>

const uint64_t SHA3::ROUND_CONSTANTS[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
    0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

const int SHA3::ROTATION_OFFSETS[24] = {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14,
    27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
};

SHA3::SHA3(Bits bits) : m_bufferSize(0) {
    m_hashSize = bits / 8;
    m_blockSize = 200 - 2 * m_hashSize;
    reset();
}

void SHA3::reset() {
    std::memset(m_state, 0, sizeof(m_state));
    std::memset(m_buffer, 0, sizeof(m_buffer));
    m_bufferSize = 0;
}

uint64_t SHA3::rotateLeft(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

void SHA3::keccakF() {
    for (int round = 0; round < 24; ++round) {
        // Theta
        uint64_t c[5];
        for (int x = 0; x < 5; ++x) {
            c[x] = m_state[x] ^ m_state[x + 5] ^ m_state[x + 10] ^ m_state[x + 15] ^ m_state[x + 20];
        }
        
        uint64_t d[5];
        for (int x = 0; x < 5; ++x) {
            d[x] = c[(x + 4) % 5] ^ rotateLeft(c[(x + 1) % 5], 1);
        }
        
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                m_state[x + 5 * y] ^= d[x];
            }
        }
        
        // Rho and Pi
        uint64_t temp[25];
        std::memcpy(temp, m_state, sizeof(temp));
        
        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                int index = x + 5 * y;
                int newX = y;
                int newY = (2 * x + 3 * y) % 5;
                int newIndex = newX + 5 * newY;
                
                if (index == 0) {
                    m_state[0] = temp[0];
                } else {
                    int rotation = ROTATION_OFFSETS[index - 1];
                    m_state[newIndex] = rotateLeft(temp[index], rotation);
                }
            }
        }
        
        // Chi
        std::memcpy(temp, m_state, sizeof(temp));
        for (int y = 0; y < 5; ++y) {
            for (int x = 0; x < 5; ++x) {
                m_state[x + 5 * y] = temp[x + 5 * y] ^ 
                    ((~temp[(x + 1) % 5 + 5 * y]) & temp[(x + 2) % 5 + 5 * y]);
            }
        }
        
        // Iota
        m_state[0] ^= ROUND_CONSTANTS[round];
    }
}

void SHA3::processBlock() {
    for (size_t i = 0; i < m_blockSize / 8; ++i) {
        uint64_t value = 0;
        for (int j = 0; j < 8; ++j) {
            value |= static_cast<uint64_t>(m_buffer[i * 8 + j]) << (8 * j);
        }
        m_state[i] ^= value;
    }
    keccakF();
}

void SHA3::add(const char* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        m_buffer[m_bufferSize++] = static_cast<uint8_t>(data[i]);
        
        if (m_bufferSize == m_blockSize) {
            processBlock();
            m_bufferSize = 0;
        }
    }
}

std::string SHA3::getHash() {
    // Padding
    m_buffer[m_bufferSize++] = 0x06;
    
    while (m_bufferSize < m_blockSize) {
        m_buffer[m_bufferSize++] = 0x00;
    }
    
    m_buffer[m_blockSize - 1] |= 0x80;
    
    processBlock();
    
    // Extract hash
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for (size_t i = 0; i < m_hashSize; ++i) {
        uint8_t byte = static_cast<uint8_t>((m_state[i / 8] >> (8 * (i % 8))) & 0xFF);
        ss << std::setw(2) << static_cast<int>(byte);
    }
    
    return ss.str();
}
