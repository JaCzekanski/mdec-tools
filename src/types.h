#pragma once
#include <array>

struct yuv_t {
    double y;
    double u;
    double v;
};

struct rgb_t {
    double r;
    double g;
    double b;

    yuv_t toYuv() const {
        yuv_t out = {};
        out.y = 0.299f * r + 0.587f * g + 0.114f * b;
        out.u = 0.492f * (b - out.y);
        out.v = 0.877f * (r - out.y);
        return out;
    }
};

using rgb_block_t = std::array<rgb_t, 8 * 8>;
using yuv_block_t = std::array<yuv_t, 8 * 8>;
using block_t = std::array<double, 8 * 8>;

yuv_block_t convertBlockToYuv(const rgb_block_t &in);
block_t extractR(const rgb_block_t& in);
block_t extractG(const rgb_block_t& in);
block_t extractB(const rgb_block_t& in);
block_t extractY(const yuv_block_t& in);
block_t extractU(const yuv_block_t& in);
block_t extractV(const yuv_block_t& in);

