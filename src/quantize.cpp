#include "quantize.h"
#include "zigzag.h"
#include <cmath>

block_t quantize(const block_t &in, const quant_table_t &quantTable) {
    block_t out;
    out[0] = round(in[0] / ((double) quantTable[0]));

    for (int i = 1; i < 64; i++) {
        int pos = zagzig[i];
        out[i] = in[pos] * 8.0 / ((double) quantTable[pos]);
    }
    return out;
}