

#include "param.h"

void convolution(const unsigned char *in, const char *k, unsigned int *out) {
    int x, y;
    int kx, ky;
    for (y = 1; y < H - 1; y++) {
        for (x = 1; x < W - 1; x++) {
            int sum = 0;
            for (ky = -1; ky <= 1; ky++) {
                for (kx = -1; kx <= 1; kx++) {
                    int pixel = in[(y + ky) * W + (x + kx)];
                    int coeff = k[(ky + 1) * 3 + (kx + 1)];
                    sum += pixel * coeff;
                }
            }
            // 出力値のクリップ
            if (sum < 0) sum = 0;
            if (sum > 255) sum = 255;
            out[y * W + x] = sum;
        }
    }
}


