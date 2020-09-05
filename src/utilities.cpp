//
// Created by oscil on 09/05/2020.
//

#include "utilities.h"

void interlace(size_t n, float* left, float* right, float* dst) {
    for(size_t i = 0; i < n; i++) {
        dst[2*i] = left[i];
        dst[2*i + 1] = right[i];
    }
}

void deinterlace(size_t n, float* left, float* right, float* src) {
    for(size_t i = 0; i < n; i++) {
        left[i] = src[2*i];
        right[i] = src[2*i + 1];
    }
}
