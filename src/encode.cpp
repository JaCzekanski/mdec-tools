#include "encode.h"
#include "dct.h"
#include "rle.h"
#include "subsample.h"
#include "quant_tables.h"
#include "quantize.h"

DoubleDCT dct;

bitstream_t encode( block_t &block, const quant_table_t &quantTable) {
    for (auto &y : block) y -= 128.0;
    block_t dctBlock = dct.forwardDCT(block);
    block_t quantBlock = quantize(dctBlock, quantTable);

    return rleBitstream(quantBlock);
};

// color mode (4:2:0 YCbCr)
bitstream_t encodeImageToColorBitstream(const image_t &img) {
    bitstream_t bitstream;
    for (int macroblockx = 0; macroblockx < img.width / 16; macroblockx++) {
        for (int macroblocky = 0; macroblocky < img.height / 16; macroblocky++) {
            rgb_macroblock_t rgbMacroblock = img.getMacroblock(macroblockx, macroblocky);
            ycbcr_macroblock_t ycbcrMacroblock = convertToYCbCr(rgbMacroblock);

            block_t crBlock = subsample(extractCr(ycbcrMacroblock));
            block_t cbBlock = subsample(extractCb(ycbcrMacroblock));

            bitstream += encode(crBlock, chromaQuantTable);
            bitstream += encode(cbBlock, chromaQuantTable);

            for (int y = 0; y < 2; y++) {
                for (int x = 0; x < 2; x++) {
                    auto rgbBlock = img.getBlock(macroblockx * 2 + x, macroblocky * 2 + y);
                    block_t yBlock = extractY(convertToYCbCr(rgbBlock));

                    bitstream += encode(yBlock, lumaQuantTable);
                }
            }
        }
    }

    return bitstream;
}

// monochromatic mode (Y blocks only)
bitstream_t encodeImageToMonoBitstream(const image_t &img) {
    bitstream_t bitstream;
    for (int blockx = 0; blockx < img.width / 8; blockx++) {
        for (int blocky = 0; blocky < img.height / 8; blocky++) {
            rgb_block_t rgbBlock = img.getBlock(blockx, blocky);
            ycbcr_block_t ycbcrBlock = convertToYCbCr(rgbBlock);

            block_t yBlock = extractY(ycbcrBlock);
            bitstream += encode(yBlock, lumaQuantTable);
        }
    }
    return bitstream;
}
