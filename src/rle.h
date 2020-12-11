#pragma once
#include "bitstream.h"
#include "types.h"

bitstream_t noRleBitstream(const block_t &block);
bitstream_t rleBitstream(const block_t &block);