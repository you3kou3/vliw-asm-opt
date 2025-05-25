/*
 * convolution.h
 *
 *  Created on: 2025/05/15
 *      Author: toru
 */

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

extern void convolution(const unsigned char *in, const char *k, unsigned int *out);
extern void convolution_intrinsic(const unsigned char *in, const char *k, unsigned int *out);
extern void _convolution_hand_asm1(const unsigned char *in, const char *k, unsigned int *out);
extern void _convolution_hand_asm2(const unsigned char *in, const char *k, unsigned int *out);
extern void _convolution_hand_asm3(const unsigned char *in, const char *k, unsigned int *out);

#endif /* CONVOLUTION_H_ */
