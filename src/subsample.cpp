#include "subsample.h"

block_t subsample(const macroblock_t &in) {
#define IN ((double(*)[16])in.data())
#define OUT ((double(*)[8])out.data())
    std::array<double, 8 * 8> out = {};
    for (int y = 0; y < 16; y += 2) {
        for (int x = 0; x < 16; x += 2) {
            double sample = 0.0;
            sample += IN[y + 0][x + 0];
            sample += IN[y + 0][x + 1];
            sample += IN[y + 1][x + 0];
            sample += IN[y + 1][x + 1];
            OUT[y / 2][x / 2] = sample / 4;
        }
    }
    return out;
#undef OUT
#undef IN
}