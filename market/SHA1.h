// market/SHA1.h
#pragma once
#include <cstdint>
#include <string>

class SHA1 {
public:
    // 计算 SHA1 并返回 20 字节的原始二进制字符串（而非 Hex 字符串）
    static std::string hash_raw(const std::string& input) {
        uint32_t digest[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
        std::string buffer = input;
        
        // 预处理：补位
        uint64_t bit_len = buffer.length() * 8;
        buffer += (char)0x80;
        while (buffer.length() % 64 != 56) {
            buffer += (char)0x00;
        }
        
        // 追加长度
        for (int i = 7; i >= 0; --i) {
            buffer += (char)((bit_len >> (i * 8)) & 0xFF);
        }

        // 处理数据块
        for (size_t offset = 0; offset < buffer.length(); offset += 64) {
            uint32_t w[80];
            for (int i = 0; i < 16; ++i) {
                w[i] = ((uint8_t)buffer[offset + i*4] << 24) |
                       ((uint8_t)buffer[offset + i*4 + 1] << 16) |
                       ((uint8_t)buffer[offset + i*4 + 2] << 8) |
                       ((uint8_t)buffer[offset + i*4 + 3]);
            }
            for (int i = 16; i < 80; ++i) {
                w[i] = rol(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
            }

            uint32_t a = digest[0], b = digest[1], c = digest[2], d = digest[3], e = digest[4];

            for (int i = 0; i < 80; ++i) {
                uint32_t f, k;
                if (i < 20)      { f = (b & c) | (~b & d); k = 0x5A827999; }
                else if (i < 40) { f = b ^ c ^ d;          k = 0x6ED9EBA1; }
                else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
                else             { f = b ^ c ^ d;          k = 0xCA62C1D6; }

                uint32_t temp = rol(a, 5) + f + e + k + w[i];
                e = d;
                d = c;
                c = rol(b, 30);
                b = a;
                a = temp;
            }

            digest[0] += a;
            digest[1] += b;
            digest[2] += c;
            digest[3] += d;
            digest[4] += e;
        }

        // 拼接成 20 字节的原始流
        std::string raw_bytes;
        for (int i = 0; i < 5; ++i) {
            raw_bytes += (char)((digest[i] >> 24) & 0xFF);
            raw_bytes += (char)((digest[i] >> 16) & 0xFF);
            raw_bytes += (char)((digest[i] >> 8) & 0xFF);
            raw_bytes += (char)(digest[i] & 0xFF);
        }
        return raw_bytes;
    }

private:
    static uint32_t rol(uint32_t value, uint32_t bits) {
        return (value << bits) | (value >> (32 - bits));
    }
};
