#include "types.h"

yuv_block_t convertBlockToYuv(const rgb_block_t &in) {
    yuv_block_t out;
    for (int i = 0; i < in.size(); i++) {
        out[i] = in[i].toYuv();
    }
    return out;
}

block_t extractR(const rgb_block_t& in) {
    block_t out;
    for (int i = 0; i<in.size(); i++) out[i] = in[i].r;
    return out;
}

block_t extractG(const rgb_block_t& in) {
    block_t out;
    for (int i = 0; i<in.size(); i++) out[i] = in[i].g;
    return out;
}

block_t extractB(const rgb_block_t& in) {
    block_t out;
    for (int i = 0; i<in.size(); i++) out[i] = in[i].g;
    return out;
}

block_t extractY(const yuv_block_t& in) {
    block_t out;
    for (int i = 0; i<in.size(); i++) out[i] = in[i].y;
    return out;
}

block_t extractU(const yuv_block_t& in) {
    block_t out;
    for (int i = 0; i<in.size(); i++) out[i] = in[i].u;
    return out;
}

block_t extractV(const yuv_block_t& in) {
    block_t out;
    for (int i = 0; i<in.size(); i++) out[i] = in[i].v;
    return out;
}