#pragma once
#include <stb_image.h>
#include "types.h"

struct image_t {
    uint8_t *data = nullptr;
    int width = 0;
    int height = 0;
    int channels = STBI_rgb;

    // macroblockx/y are in /16 increments
    rgb_macroblock_t getMacroblock(int macroblockx, int macroblocky) const{
        rgb_macroblock_t rgbBlock;
        for (int y = 0; y < 16; y++) {
            for (int x = 0; x < 16; x++) {
#define BLOCK ((rgb_t(*)[16])rgbBlock.data())

                uint8_t *ptr = data + ((macroblocky * 16 + y) * width + (macroblockx * 16 + x)) * channels;
                BLOCK[y][x] = rgb_t{
                        .r = *(ptr + 0),
                        .g = *(ptr + 1),
                        .b = *(ptr + 2),
                };

#undef BLOCK
            }
        }
        return rgbBlock;
    }

    // blockx/y are in /8 increments
    rgb_block_t getBlock(int blockx, int blocky) const {
        rgb_block_t rgbBlock;
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
#define BLOCK ((rgb_t(*)[8])rgbBlock.data())

                uint8_t *ptr = data + ((blocky * 8 + y) * width + (blockx * 8 + x)) * channels;
                BLOCK[y][x] = rgb_t{
                        .r = *(ptr + 0),
                        .g = *(ptr + 1),
                        .b = *(ptr + 2),
                };

#undef BLOCK
            }
        }
        return rgbBlock;
    }
};
