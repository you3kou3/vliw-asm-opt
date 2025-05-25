    .text
    .global _convolution_hand_asm1

_convolution_hand_asm1:
    .asg 28, W
    .asg 28, H
    .asg 26, Hm2
    .asg 26, Wm2

	ADD.D1 A4,  W, A11
	|| LDW.D2 *B4[0], B7
	|| MVKL.S1 Hm2, A1
	|| MVKL.S2 Wm2, B1
	|| MV.L1  A4, A10
	ADD.D1 A11, W, A12
	|| LDW.D2 *B4[4], B8
	|| MVKH.S1 Hm2, A1
	|| MVKH.S2 Wm2, B1
	LDW.D2 *B4[8], B9
	MV.D2 B6, B10
	|| MVKL.S1 0, A15
	ADD.D2 B10, W, B10
	|| MVKL.S1 255, A14
	ADD.D2 B10, 1, B10
	MV.S1 A1, A0

loop_y:
	MV.S2 B1, B0

loop_x:
	MV.D1 A10, A2
	LDNW.D1 *A2,  A7
	|| MV.S1 A11, A2
	LDNW.D1 *A2,  A8
	|| MV.L1  A12, A2
	LDNW.D1 *A2,  A9
	NOP
	NOP
	ADD.D1 A10, 1, A10
	|| DOTPU4.M1 B7, A7, A7
	ADD.S1 A11, 1, A11
	|| DOTPU4.M1 B8, A8, A8
	ADD.L1 A12, 1, A12
	|| DOTPU4.M1 B9, A9, A9
	NOP
	NOP
	[B0] BDEC.S2 loop_x, B0
	|| ADD.L1 A7, A8, A8
	ADD.L1 A8, A9, A13
	MAX2.S1 A15, A13, A13
	MIN2.S1 A14, A13, A13
	STW.D2 A13, *B10++

	ADD.L1 A10, 2, A10
	ADD.L1 A11, 2, A11
	ADD.L1 A12, 2, A12
	ADD.L2 B10, 2, B10
	[A0] BDEC.S1 loop_y, A0
	NOP
	NOP
	NOP
	NOP
	NOP


    ; –ß‚è
    B    B3
    NOP
