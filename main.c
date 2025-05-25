/*
 * main.c
 *
 *  Created on: 2025/05/15
 *      Author: toru
 */

#include "param.h"
#include "convolution.h"


unsigned char image[(H+1) * W];
unsigned int output1[H * W] = {0};
unsigned int output2[H * W] = {0};
unsigned int output3[H * W] = {0};
unsigned int output4[H * W] = {0};
unsigned int output5[H * W] = {0};

const char kernel[9] = {
     0, -1,  0,
    -1,  4, -1,
     0, -1,  0
};


const char kernel_for_asm[12] = {
     0, -1,  0, 0,
    -1,  4, -1, 0,
     0, -1,  0, 0
};

void init_image()
{
    int x, y;
    int center_x = W / 2;
    int center_y = H / 2;
    for (y = 0; y < H; y++) {
        for (x = 0; x < W; x++) {
            int dx = x - center_x;
            int dy = y - center_y;
            int dist2 = dx * dx + dy * dy;
            int intensity = 255 - (dist2 * 255) / (center_x * center_x + center_y * center_y);
            if (intensity < 0) intensity = 0;
            image[y * W + x] = (unsigned char)intensity;
        }
    }
}


int main(void) {
    init_image();

    convolution(image, kernel, output1);

    convolution_intrinsic(image, kernel_for_asm, output2);

    _convolution_hand_asm1(image, kernel_for_asm, output3);

    _convolution_hand_asm2(image, kernel_for_asm, output4);

    _convolution_hand_asm2(image, kernel_for_asm, output5);

    return 0;
}

