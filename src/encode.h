#pragma once
#include "image.h"
#include "bitstream.h"

bitstream_t encodeImageToColorBitstream(const image_t &img);
bitstream_t encodeImageToMonoBitstream(const image_t& img);