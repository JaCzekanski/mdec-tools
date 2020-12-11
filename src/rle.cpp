#include "rle.h"
#include <cmath>

// First byte in block
union DCT {
    struct {
        uint16_t dc: 10;      // Direct Current
        uint16_t qFactor: 6;  // Quantization factor
    };
    uint16_t _;

    DCT(uint16_t val) : _(val) {};

    DCT() : _(0) {};
};

// compressed data
union RLE {
    struct {
        uint16_t ac: 10;     // relative value
        uint16_t zeroes: 6;  // bytes to skip
    };
    uint16_t _;

    RLE(uint16_t val) : _(val) {};

    RLE() : _(0) {};
};

uint16_t toInt(double in) {
    if (in > 0x3ff) return 0x3ff;
    else if (in < -0x400) return -0x400;
    return (uint16_t) (int16_t) round(in);
}

bitstream_t noRleBitstream(const block_t &block) {
    int qFactor = 1;
    bitstream_t out;
    DCT dct;
    dct.dc = toInt(block[0]);
    dct.qFactor = qFactor;
    out.push_back(dct._);

    for (int n = 1; n < 64; n++) {
        RLE rle;
        rle.ac = toInt(block[n] / (double) qFactor);
        rle.zeroes = 0; // No RLE compression
        out.push_back(rle._);
    }

    return out;
}

bitstream_t rleBitstream(const block_t &block) {
    int qFactor = 1;
    bitstream_t out;
    DCT dct;
    dct.dc = toInt(block[0]);
    dct.qFactor = qFactor;
    out.push_back(dct._);

    RLE rle;

    int zeroesToSkip = 0;
    auto writeZeroes = [&]() {
        rle.zeroes = zeroesToSkip;
        out.push_back(rle._);
        zeroesToSkip = 0;
    };

    int n;
    for (n = 1; n < 64; n++) {
        rle.ac = toInt(block[n] / (double) qFactor);
        rle.zeroes = 0; // No RLE compression

        if (rle.ac == 0) {
            zeroesToSkip++;
        } else {
            writeZeroes();
        }
    }
    if (zeroesToSkip != 0) {
        // TODO: Not sure which padding method should be used
//        out.push_back(0xfe00);
        printf("Writing %d zeroes\n", zeroesToSkip);
        zeroesToSkip -= 1;
        writeZeroes();
    }

    return out;
}