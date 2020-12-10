#pragma once
#include "types.h"
#include "quant_tables.h"

block_t quantize(const block_t &in, const quant_table_t &quantTable);