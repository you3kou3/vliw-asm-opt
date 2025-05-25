/*
 * convolution_intrinsic.c
 *
 *  Created on: 2025/05/15
 *      Author: toru
 */

#include "param.h"
#include <c6x.h>


void convolution_intrinsic(const unsigned char *in, const char *k, unsigned int *out)
{
    int x, y;

    // Kernel
    int k0 = k[0], k1 = k[1], k2 = k[2];
    int k3 = k[3], k4 = k[4], k5 = k[5];
    int k6 = k[6], k7 = k[7], k8 = k[8];

    int k3_1 = (k2<<16) | (k1<<8) | (k0);
    int k6_4 = (k5<<16) | (k4<<8) | (k3);
    int k9_7 = (k8<<16) | (k7<<8) | (k6);

    for (y = 1; y < H - 1; y++) {
        for (x = 1; x < W - 1; x++) {
            // load 3 pixel from line memory
            unsigned char *in_ptr = (unsigned char *)in;
            unsigned int row3_1 = _mem4((void *)(in_ptr + (y-1)*W + (x-1))) & 0xFFFFFF;
            unsigned int row6_4 = _mem4((void *)(in_ptr + (y  )*W + (x-1))) & 0xFFFFFF;
            unsigned int row9_7 = _mem4((void *)(in_ptr + (y+1)*W + (x-1))) & 0xFFFFFF;

            // calc convolution
            int sum = 0;
            sum += _dotpsu4(k3_1, row3_1);
            sum += _dotpsu4(k6_4, row6_4);
            sum += _dotpsu4(k9_7, row9_7);

            // clip
            if (sum < 0)   sum = 0;
            if (sum > 255) sum = 255;

            out[y * W + x] = sum;
        }
    }
}

