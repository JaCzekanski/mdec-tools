#include <cstdio>
#include <cstring>
#include <vector>
#include <stb_image.h>
#include "types.h"
#include "dct.h"
#include "rle.h"
#include "quantize.h"
#include "quant_tables.h"

struct image_t {
    uint8_t *data = nullptr;
    int width = 0;
    int height = 0;
    int channels = STBI_rgb;
};

rgb_block_t getBlock(const image_t &img, int blockx, int blocky) {
    rgb_block_t rgbBlock;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
#define BLOCK ((rgb_t(*)[8])rgbBlock.data())

            uint8_t *ptr = img.data + ((blocky * 8 + y) * img.width + (blockx * 8 + x)) * img.channels;
            BLOCK[y][x] = rgb_t{
                    .r = (double) *(ptr + 0) / 255.0,
                    .g = (double) *(ptr + 1) / 255.0,
                    .b = (double) *(ptr + 2) / 255.0,
            };

#undef BLOCK
        }
    }
    return rgbBlock;
}

void dumpBlock(const block_t &in) {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            printf("% .1f ", in[y * 8 + x]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: mdec-tools [--color] input.png output.mdec\n");
        return 1;
    }

    bool colorMode = false;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--color") == 0) colorMode = true;
    }

    const char *srcPath = argv[argc - 2];
    const char *dstPath = argv[argc - 1];

    image_t img;
    img.data = stbi_load(srcPath, &img.width, &img.height, nullptr, img.channels);
    if (img.data == nullptr) {
        printf("Unable to load %s (%s)\n", srcPath, stbi_failure_reason());
        return 1;
    }

    int mustBeMultipleOf = 8;
    if (colorMode) mustBeMultipleOf = 16;

    if (img.width % mustBeMultipleOf != 0 || img.height % mustBeMultipleOf != 0) {
        printf("Dimensions of the input file must be a multiple of %d (%dx%d given).\n", mustBeMultipleOf, img.width,
               img.height);
        return 1;
    }

    FILE *out = fopen(dstPath, "wb");
    if (out == nullptr) {
        printf("Cannot open %s for writing.\n", dstPath);
        return 1;
    }

    // Block - 8x8 image chunk
    // Macroblock - 16x16 image chunk (4:2:0 - 4 Y blocks, 1 Cb block, 1 Cr block)

    DoubleDCT dct;

    if (colorMode) {
        printf("Not implemented\n");
    } else {
        // monochromatic mode (Y blocks only)
        for (int blockx = 0; blockx < img.width / 8; blockx++) {
            for (int blocky = 0; blocky < img.height / 8; blocky++) {
                rgb_block_t rgbBlock = getBlock(img, blockx, blocky);
                yuv_block_t yuvBlock = convertBlockToYuv(rgbBlock);
                block_t yBlock = extractY(yuvBlock);

                // 0 - 1 -> -128 - 127
                float MAX = 255.0;
                for (auto &y : yBlock) y = y * MAX - ceil(MAX / 2.0);

                block_t dctBlock = dct.forwardDCT(yBlock);
                block_t quantBlock = quantize(dctBlock, lumaQuantTable);
                std::vector<uint16_t> rle = noRleBitstream(quantBlock);
                fwrite(rle.data(), 2, rle.size(), out);
            }
        }
    }

    stbi_image_free(img.data);
    fclose(out);
    return 0;
}
