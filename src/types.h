#pragma once

#include <array>

struct ycbcr_t {
    double y;
    double cb;
    double cr;
};

struct rgb_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    ycbcr_t toYCbCr() const {
        ycbcr_t out = {};
        out.y  = 0   + (0.299    * r) + (0.587    * g) + (0.114    * b);
        out.cb = 128 - (0.168736 * r) - (0.331264 * g) + (0.5      * b);
        out.cr = 128 + (0.5      * r) - (0.418688 * g) - (0.081312 * b);
        return out;
    }
};

// Block - 8x8 image chunk
// Macroblock - 16x16 image chunk (4:2:0 - 4 Y blocks, 1 Cb block, 1 Cr block)

using rgb_macroblock_t = std::array<rgb_t, 16 * 16>;
using ycbcr_macroblock_t = std::array<ycbcr_t, 16 * 16>;
using rgb_block_t = std::array<rgb_t, 8 * 8>;
using ycbcr_block_t = std::array<ycbcr_t, 8 * 8>;
using block_t = std::array<double, 8 * 8>;
using macroblock_t = std::array<double, 16 * 16>;

template<size_t size>
std::array<ycbcr_t, size> convertToYCbCr(const std::array<rgb_t, size> &in) {
    std::array<ycbcr_t, size> out;
    for (int i = 0; i < in.size(); i++) {
        out[i] = in[i].toYCbCr();
    }
    return out;
}

template<size_t size>
std::array<double, size> extractY(const std::array<ycbcr_t, size> &in) {
    std::array<double, size> out;
    for (int i = 0; i < in.size(); i++) out[i] = in[i].y;
    return out;
}

template<size_t size>
std::array<double, size> extractCb(const std::array<ycbcr_t, size> &in) {
    std::array<double, size> out;
    for (int i = 0; i < in.size(); i++) out[i] = in[i].cb;
    return out;
}

template<size_t size>
std::array<double, size> extractCr(const std::array<ycbcr_t, size> &in) {
    std::array<double, size> out;
    for (int i = 0; i < in.size(); i++) out[i] = in[i].cr;
    return out;
}

