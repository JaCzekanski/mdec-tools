#include <cstdio>
#include <cstring>
#include <stb_image.h>
#include "image.h"
#include "encode.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("usage: mdec-tools [--color] input.png [output.mdec]\n");
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

    bitstream_t bitstream;

    if (colorMode) {
        bitstream = encodeImageToColorBitstream(img);
    } else {
        bitstream = encodeImageToMonoBitstream(img);
    }

    // TODO: Add aligment switch
    bitstream.align(0x20 * 4);
    fwrite(bitstream.data(), 2, bitstream.size(), out);
    fclose(out);

    stbi_image_free(img.data);

    printf("Converted %dx%d image to MDEC stream (%lu bytes).\n", img.width, img.height, bitstream.size() * 2);
    return 0;
}
