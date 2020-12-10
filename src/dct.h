#pragma once
#include <cmath>
#include "types.h"

class DoubleDCT {
    double c[8][8];
    double cT[8][8];

public:
    DoubleDCT() {
        for (int j = 0; j < 8; j++) {
            c[0][j] = 1.0 / sqrt(8);
            cT[j][0] = c[0][j];
        }

        for (int i = 1; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                c[i][j] = sqrt(2.0 / 8.0) * cos(((2.0 * j + 1.0) * i * M_PI) / (2.0 * 8.0));
                cT[j][i] = c[i][j];
            }
        }
    }

    block_t forwardDCT(const block_t& in) {
        block_t out = {};
        block_t temp;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                temp[i + j * 8] = 0.0;
                for (int k = 0; k < 8; k++) {
                    temp[i + j * 8] += in[i + k * 8] * cT[k][j];
                }
            }
        }

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                double temp1 = 0.0;

                for (int k = 0; k < 8; k++) {
                    temp1 += (c[i][k] * temp[k + j * 8]);
                }

                out[i + j * 8] = temp1;
            }
        }

        return out;
    }

    // TODO: Use int version + feed psx tables
};