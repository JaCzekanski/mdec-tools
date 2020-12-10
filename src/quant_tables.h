#pragma once
#include <cstdint>

using quant_table_t = uint8_t[64];

extern quant_table_t lumaQuantTable;
extern quant_table_t chromaQuantTable;