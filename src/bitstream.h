#pragma once
#include <vector>
#include <cstdint>

struct bitstream_t : std::vector<uint16_t> {
    bitstream_t &operator+=(const bitstream_t &b) {
        align(4);
        insert(end(), b.begin(), b.end());
        return *this;
    }

    void prealign(size_t bytes = 4) {
        while ((size() * sizeof(uint16_t)) % bytes != 0) {
            insert(begin(), 0xfe00);
        }
    }

    void align(size_t bytes = 4) {
        while ((size() * sizeof(uint16_t)) % bytes != 0) {
            push_back(0xfe00);
        }
    }
};