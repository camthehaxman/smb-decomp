/* 80082C6C 0007EB8C  7C 08 02 A6 */	mflr r0
/* 80082C70 0007EB90  90 01 00 04 */	stw r0, 4(r1)
/* 80082C74 0007EB94  94 21 FF 10 */	stwu r1, -0xf0(r1)
/* 80082C78 0007EB98  DB E1 00 E8 */	stfd f31, 0xe8(r1)
/* 80082C7C 0007EB9C  DB C1 00 E0 */	stfd f30, 0xe0(r1)
/* 80082C80 0007EBA0  DB A1 00 D8 */	stfd f29, 0xd8(r1)
/* 80082C84 0007EBA4  DB 81 00 D0 */	stfd f28, 0xd0(r1)
/* 80082C88 0007EBA8  DB 61 00 C8 */	stfd f27, 0xc8(r1)
/* 80082C8C 0007EBAC  DB 41 00 C0 */	stfd f26, 0xc0(r1)
/* 80082C90 0007EBB0  DB 21 00 B8 */	stfd f25, 0xb8(r1)
/* 80082C94 0007EBB4  DB 01 00 B0 */	stfd f24, 0xb0(r1)
/* 80082C98 0007EBB8  DA E1 00 A8 */	stfd f23, 0xa8(r1)
/* 80082C9C 0007EBBC  DA C1 00 A0 */	stfd f22, 0xa0(r1)
/* 80082CA0 0007EBC0  DA A1 00 98 */	stfd f21, 0x98(r1)
/* 80082CA4 0007EBC4  DA 81 00 90 */	stfd f20, 0x90(r1)
/* 80082CA8 0007EBC8  DA 61 00 88 */	stfd f19, 0x88(r1)
/* 80082CAC 0007EBCC  DA 41 00 80 */	stfd f18, 0x80(r1)
/* 80082CB0 0007EBD0  DA 21 00 78 */	stfd f17, 0x78(r1)
/* 80082CB4 0007EBD4  DA 01 00 70 */	stfd f16, 0x70(r1)
/* 80082CB8 0007EBD8  D9 E1 00 68 */	stfd f15, 0x68(r1)
/* 80082CBC 0007EBDC  BE 41 00 30 */	stmw r18, 0x30(r1)
/* 80082CC0 0007EBE0  3B A3 00 00 */	addi r29, r3, 0
/* 80082CC4 0007EBE4  3C 60 80 1F */	lis r3, modeCtrl@ha
/* 80082CC8 0007EBE8  38 63 EC 20 */	addi r3, r3, modeCtrl@l
/* 80082CCC 0007EBEC  1C 1D 00 09 */	mulli r0, r29, 9
/* 80082CD0 0007EBF0  3B C4 00 00 */	addi r30, r4, 0
/* 80082CD4 0007EBF4  3B E5 00 00 */	addi r31, r5, 0
/* 80082CD8 0007EBF8  80 63 00 28 */	lwz r3, 0x28(r3)
/* 80082CDC 0007EBFC  7C 03 02 14 */	add r0, r3, r0
/* 80082CE0 0007EC00  3C 60 80 1C */	lis r3, pageCounts@ha
/* 80082CE4 0007EC04  3B 03 23 18 */	addi r24, r3, pageCounts@l
/* 80082CE8 0007EC08  54 00 10 3A */	slwi r0, r0, 2
/* 80082CEC 0007EC0C  7C 78 02 14 */	add r3, r24, r0
/* 80082CF0 0007EC10  82 E3 0D D0 */	lwz r23, 0xdd0(r3)
/* 80082CF4 0007EC14  28 17 00 00 */	cmplwi r23, 0
/* 80082CF8 0007EC18  41 82 03 E8 */	beq lbl_800830E0
/* 80082CFC 0007EC1C  CA E2 AA 50 */	lfd f23, 4503601774854144.0
/* 80082D00 0007EC20  3A A0 00 00 */	li r21, 0
/* 80082D04 0007EC24  CB 02 AA 20 */	lfd f24, 140.0
/* 80082D08 0007EC28  3F 20 01 00 */	lis r25, 0x100
/* 80082D0C 0007EC2C  C3 22 AA 28 */	lfs f25, 76.0f
/* 80082D10 0007EC30  CB 42 AA 10 */	lfd f26, 0.5
/* 80082D14 0007EC34  3F 40 43 30 */	lis r26, 0x4330
/* 80082D18 0007EC38  C3 82 AA 48 */	lfs f28, 140.0f
/* 80082D1C 0007EC3C  C3 A2 A9 A4 */	lfs f29, 1.0f
/* 80082D20 0007EC40  CB C2 AA 38 */	lfd f30, 0.125
/* 80082D24 0007EC44  CB E2 AA 40 */	lfd f31, 0.001
/* 80082D28 0007EC48  CA C2 AA 00 */	lfd f22, 0.002
/* 80082D2C 0007EC4C  CB 62 AA 30 */	lfd f27, 0.00050000000000000001
/* 80082D30 0007EC50  48 00 03 9C */	b lbl_800830CC
lbl_80082D34:
/* 80082D34 0007EC54  C0 1E 00 4C */	lfs f0, 0x4c(r30)
/* 80082D38 0007EC58  7C 60 07 74 */	extsb r0, r3
/* 80082D3C 0007EC5C  1C 00 00 0C */	mulli r0, r0, 0xc
/* 80082D40 0007EC60  FC 20 B0 28 */	fsub f1, f0, f22
/* 80082D44 0007EC64  7E 78 02 14 */	add r19, r24, r0
/* 80082D48 0007EC68  FC 20 08 18 */	frsp f1, f1
/* 80082D4C 0007EC6C  3A 73 0C 2C */	addi r19, r19, 0xc2c
/* 80082D50 0007EC70  4B FE ED CD */	bl func_80071B1C
/* 80082D54 0007EC74  88 13 00 00 */	lbz r0, 0(r19)
/* 80082D58 0007EC78  38 7F 00 00 */	addi r3, r31, 0
/* 80082D5C 0007EC7C  7C 00 07 74 */	extsb r0, r0
/* 80082D60 0007EC80  1C 00 00 0C */	mulli r0, r0, 0xc
/* 80082D64 0007EC84  7F 98 02 14 */	add r28, r24, r0
/* 80082D68 0007EC88  84 1C 03 2C */	lwzu r0, 0x32c(r28)
/* 80082D6C 0007EC8C  90 1F 00 00 */	stw r0, 0(r31)
/* 80082D70 0007EC90  A8 13 00 02 */	lha r0, 2(r19)
/* 80082D74 0007EC94  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 80082D78 0007EC98  90 01 00 2C */	stw r0, 0x2c(r1)
/* 80082D7C 0007EC9C  93 41 00 28 */	stw r26, 0x28(r1)
/* 80082D80 0007ECA0  C8 01 00 28 */	lfd f0, 0x28(r1)
/* 80082D84 0007ECA4  EC 00 B8 28 */	fsubs f0, f0, f23
/* 80082D88 0007ECA8  D0 1F 00 04 */	stfs f0, 4(r31)
/* 80082D8C 0007ECAC  A8 13 00 04 */	lha r0, 4(r19)
/* 80082D90 0007ECB0  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 80082D94 0007ECB4  90 01 00 24 */	stw r0, 0x24(r1)
/* 80082D98 0007ECB8  93 41 00 20 */	stw r26, 0x20(r1)
/* 80082D9C 0007ECBC  C8 01 00 20 */	lfd f0, 0x20(r1)
/* 80082DA0 0007ECC0  EC 00 B8 28 */	fsubs f0, f0, f23
/* 80082DA4 0007ECC4  D0 1F 00 08 */	stfs f0, 8(r31)
/* 80082DA8 0007ECC8  4B FF 0A 81 */	bl nlSprPut
/* 80082DAC 0007ECCC  3A 80 00 00 */	li r20, 0
/* 80082DB0 0007ECD0  56 9B 10 3A */	slwi r27, r20, 2
/* 80082DB4 0007ECD4  48 00 00 EC */	b lbl_80082EA0
lbl_80082DB8:
/* 80082DB8 0007ECD8  2C 14 00 00 */	cmpwi r20, 0
/* 80082DBC 0007ECDC  40 82 00 0C */	bne lbl_80082DC8
/* 80082DC0 0007ECE0  C8 02 AA 08 */	lfd f0, 0.54166599999999998
/* 80082DC4 0007ECE4  48 00 00 08 */	b lbl_80082DCC
lbl_80082DC8:
/* 80082DC8 0007ECE8  C8 02 AA 10 */	lfd f0, 0.5
lbl_80082DCC:
/* 80082DCC 0007ECEC  2C 14 00 00 */	cmpwi r20, 0
/* 80082DD0 0007ECF0  FD E0 00 18 */	frsp f15, f0
/* 80082DD4 0007ECF4  40 82 00 0C */	bne lbl_80082DE0
/* 80082DD8 0007ECF8  C8 02 AA 08 */	lfd f0, 0.54166599999999998
/* 80082DDC 0007ECFC  48 00 00 08 */	b lbl_80082DE4
lbl_80082DE0:
/* 80082DE0 0007ED00  C8 02 AA 18 */	lfd f0, 0.57999999999999996
lbl_80082DE4:
/* 80082DE4 0007ED04  2C 14 00 00 */	cmpwi r20, 0
/* 80082DE8 0007ED08  FE 20 00 18 */	frsp f17, f0
/* 80082DEC 0007ED0C  40 82 00 0C */	bne lbl_80082DF8
/* 80082DF0 0007ED10  38 19 FF FF */	addi r0, r25, -1
/* 80082DF4 0007ED14  48 00 00 08 */	b lbl_80082DFC
lbl_80082DF8:
/* 80082DF8 0007ED18  38 00 00 00 */	li r0, 0
lbl_80082DFC:
/* 80082DFC 0007ED1C  88 7E 00 01 */	lbz r3, 1(r30)
/* 80082E00 0007ED20  7C 12 03 78 */	mr r18, r0
/* 80082E04 0007ED24  4B FF 02 81 */	bl u_get_jpn_text_width
/* 80082E08 0007ED28  6C 60 80 00 */	xoris r0, r3, 0x8000
/* 80082E0C 0007ED2C  90 01 00 24 */	stw r0, 0x24(r1)
/* 80082E10 0007ED30  93 41 00 20 */	stw r26, 0x20(r1)
/* 80082E14 0007ED34  C8 01 00 20 */	lfd f0, 0x20(r1)
/* 80082E18 0007ED38  EE 00 B8 28 */	fsubs f16, f0, f23
/* 80082E1C 0007ED3C  FC 18 80 24 */	fdiv f0, f24, f16
/* 80082E20 0007ED40  FC 00 78 40 */	fcmpo cr0, f0, f15
/* 80082E24 0007ED44  40 80 00 08 */	bge lbl_80082E2C
/* 80082E28 0007ED48  48 00 00 08 */	b lbl_80082E30
lbl_80082E2C:
/* 80082E2C 0007ED4C  FC 00 78 90 */	fmr f0, f15
lbl_80082E30:
/* 80082E30 0007ED50  FD E0 00 18 */	frsp f15, f0
/* 80082E34 0007ED54  7E 43 93 78 */	mr r3, r18
/* 80082E38 0007ED58  4B FE EC AD */	bl set_text_mul_color
/* 80082E3C 0007ED5C  FC 20 78 90 */	fmr f1, f15
/* 80082E40 0007ED60  FC 40 88 90 */	fmr f2, f17
/* 80082E44 0007ED64  4B FE EC E9 */	bl set_text_scale
/* 80082E48 0007ED68  38 14 00 04 */	addi r0, r20, 4
/* 80082E4C 0007ED6C  FC 1A 04 32 */	fmul f0, f26, f16
/* 80082E50 0007ED70  7C 1C 00 AE */	lbzx r0, r28, r0
/* 80082E54 0007ED74  C0 7F 00 04 */	lfs f3, 4(r31)
/* 80082E58 0007ED78  7C 00 07 74 */	extsb r0, r0
/* 80082E5C 0007ED7C  C0 5F 00 08 */	lfs f2, 8(r31)
/* 80082E60 0007ED80  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 80082E64 0007ED84  FC 20 03 F2 */	fmul f1, f0, f15
/* 80082E68 0007ED88  90 01 00 24 */	stw r0, 0x24(r1)
/* 80082E6C 0007ED8C  EC 79 18 2A */	fadds f3, f25, f3
/* 80082E70 0007ED90  93 41 00 20 */	stw r26, 0x20(r1)
/* 80082E74 0007ED94  C8 01 00 20 */	lfd f0, 0x20(r1)
/* 80082E78 0007ED98  FC 23 08 28 */	fsub f1, f3, f1
/* 80082E7C 0007ED9C  EC 00 B8 28 */	fsubs f0, f0, f23
/* 80082E80 0007EDA0  FC 20 08 18 */	frsp f1, f1
/* 80082E84 0007EDA4  EC 42 00 2A */	fadds f2, f2, f0
/* 80082E88 0007EDA8  4B FE EC D9 */	bl set_text_pos
/* 80082E8C 0007EDAC  80 73 00 08 */	lwz r3, 8(r19)
/* 80082E90 0007EDB0  7C 63 D8 2E */	lwzx r3, r3, r27
/* 80082E94 0007EDB4  4B FE EF C5 */	bl sprite_puts
/* 80082E98 0007EDB8  3B 7B 00 04 */	addi r27, r27, 4
/* 80082E9C 0007EDBC  3A 94 00 01 */	addi r20, r20, 1
lbl_80082EA0:
/* 80082EA0 0007EDC0  80 73 00 08 */	lwz r3, 8(r19)
/* 80082EA4 0007EDC4  7C 83 D8 2E */	lwzx r4, r3, r27
/* 80082EA8 0007EDC8  28 04 00 00 */	cmplwi r4, 0
/* 80082EAC 0007EDCC  40 82 FF 0C */	bne lbl_80082DB8
/* 80082EB0 0007EDD0  88 16 00 00 */	lbz r0, 0(r22)
/* 80082EB4 0007EDD4  2C 1D 00 00 */	cmpwi r29, 0
/* 80082EB8 0007EDD8  C2 3F 00 0C */	lfs f17, 0xc(r31)
/* 80082EBC 0007EDDC  7C 00 07 74 */	extsb r0, r0
/* 80082EC0 0007EDE0  C2 1F 00 10 */	lfs f16, 0x10(r31)
/* 80082EC4 0007EDE4  1C 00 00 14 */	mulli r0, r0, 0x14
/* 80082EC8 0007EDE8  FC 11 D8 2A */	fadd f0, f17, f27
/* 80082ECC 0007EDEC  C1 FF 00 14 */	lfs f15, 0x14(r31)
/* 80082ED0 0007EDF0  82 9F 00 34 */	lwz r20, 0x34(r31)
/* 80082ED4 0007EDF4  7E 78 02 14 */	add r19, r24, r0
/* 80082ED8 0007EDF8  FC 00 00 18 */	frsp f0, f0
/* 80082EDC 0007EDFC  C0 93 0E 20 */	lfs f4, 0xe20(r19)
/* 80082EE0 0007EE00  C0 73 0E 18 */	lfs f3, 0xe18(r19)
/* 80082EE4 0007EE04  C0 53 0E 24 */	lfs f2, 0xe24(r19)
/* 80082EE8 0007EE08  C0 33 0E 1C */	lfs f1, 0xe1c(r19)
/* 80082EEC 0007EE0C  82 5F 00 38 */	lwz r18, 0x38(r31)
/* 80082EF0 0007EE10  EE A4 18 28 */	fsubs f21, f4, f3
/* 80082EF4 0007EE14  EE 82 08 28 */	fsubs f20, f2, f1
/* 80082EF8 0007EE18  3A 73 0E 18 */	addi r19, r19, 0xe18
/* 80082EFC 0007EE1C  D0 1F 00 0C */	stfs f0, 0xc(r31)
/* 80082F00 0007EE20  41 82 00 0C */	beq lbl_80082F0C
/* 80082F04 0007EE24  C0 0D 9E 64 */	lfs f0, lbl_802F2044
/* 80082F08 0007EE28  48 00 00 08 */	b lbl_80082F10
lbl_80082F0C:
/* 80082F0C 0007EE2C  C0 0D 9E 5C */	lfs f0, lbl_802F203C
lbl_80082F10:
/* 80082F10 0007EE30  D0 1F 00 2C */	stfs f0, 0x2c(r31)
/* 80082F14 0007EE34  3B 60 0C 2B */	li r27, 0xc2b
/* 80082F18 0007EE38  FC 7A 05 72 */	fmul f3, f26, f21
/* 80082F1C 0007EE3C  80 13 00 10 */	lwz r0, 0x10(r19)
/* 80082F20 0007EE40  FE 7A 05 32 */	fmul f19, f26, f20
/* 80082F24 0007EE44  90 1F 00 38 */	stw r0, 0x38(r31)
/* 80082F28 0007EE48  EC 15 05 72 */	fmuls f0, f21, f21
/* 80082F2C 0007EE4C  93 7F 00 00 */	stw r27, 0(r31)
/* 80082F30 0007EE50  EE 54 05 32 */	fmuls f18, f20, f20
/* 80082F34 0007EE54  C0 53 00 00 */	lfs f2, 0(r19)
/* 80082F38 0007EE58  EC 20 90 2A */	fadds f1, f0, f18
/* 80082F3C 0007EE5C  FC 03 10 2A */	fadd f0, f3, f2
/* 80082F40 0007EE60  FC 00 00 18 */	frsp f0, f0
/* 80082F44 0007EE64  D0 1F 00 04 */	stfs f0, 4(r31)
/* 80082F48 0007EE68  C0 13 00 04 */	lfs f0, 4(r19)
/* 80082F4C 0007EE6C  FC 13 00 2A */	fadd f0, f19, f0
/* 80082F50 0007EE70  FC 00 00 18 */	frsp f0, f0
/* 80082F54 0007EE74  D0 1F 00 08 */	stfs f0, 8(r31)
/* 80082F58 0007EE78  D3 BF 00 10 */	stfs f29, 0x10(r31)
/* 80082F5C 0007EE7C  4B F8 41 9D */	bl mathutil_sqrt
/* 80082F60 0007EE80  FC 1E 00 72 */	fmul f0, f30, f1
/* 80082F64 0007EE84  FC 20 A8 90 */	fmr f1, f21
/* 80082F68 0007EE88  FC 40 A0 90 */	fmr f2, f20
/* 80082F6C 0007EE8C  FC 00 00 18 */	frsp f0, f0
/* 80082F70 0007EE90  D0 1F 00 14 */	stfs f0, 0x14(r31)
/* 80082F74 0007EE94  4B F8 43 31 */	bl mathutil_atan2
/* 80082F78 0007EE98  7C 60 07 34 */	extsh r0, r3
/* 80082F7C 0007EE9C  90 1F 00 28 */	stw r0, 0x28(r31)
/* 80082F80 0007EEA0  7F E3 FB 78 */	mr r3, r31
/* 80082F84 0007EEA4  80 1E 00 74 */	lwz r0, 0x74(r30)
/* 80082F88 0007EEA8  54 00 00 36 */	rlwinm r0, r0, 0, 0, 0x1b
/* 80082F8C 0007EEAC  60 00 00 0A */	ori r0, r0, 0xa
/* 80082F90 0007EEB0  90 1F 00 34 */	stw r0, 0x34(r31)
/* 80082F94 0007EEB4  4B FF 08 95 */	bl nlSprPut
/* 80082F98 0007EEB8  3B 80 0C 2A */	li r28, 0xc2a
/* 80082F9C 0007EEBC  93 9F 00 00 */	stw r28, 0(r31)
/* 80082FA0 0007EEC0  7F E3 FB 78 */	mr r3, r31
/* 80082FA4 0007EEC4  C0 13 00 08 */	lfs f0, 8(r19)
/* 80082FA8 0007EEC8  D0 1F 00 04 */	stfs f0, 4(r31)
/* 80082FAC 0007EECC  C0 13 00 0C */	lfs f0, 0xc(r19)
/* 80082FB0 0007EED0  D0 1F 00 08 */	stfs f0, 8(r31)
/* 80082FB4 0007EED4  C0 1F 00 0C */	lfs f0, 0xc(r31)
/* 80082FB8 0007EED8  FC 00 F8 28 */	fsub f0, f0, f31
/* 80082FBC 0007EEDC  FC 00 00 18 */	frsp f0, f0
/* 80082FC0 0007EEE0  D0 1F 00 0C */	stfs f0, 0xc(r31)
/* 80082FC4 0007EEE4  D3 BF 00 10 */	stfs f29, 0x10(r31)
/* 80082FC8 0007EEE8  D3 BF 00 14 */	stfs f29, 0x14(r31)
/* 80082FCC 0007EEEC  80 9F 00 28 */	lwz r4, 0x28(r31)
/* 80082FD0 0007EEF0  3C 84 00 01 */	addis r4, r4, 1
/* 80082FD4 0007EEF4  38 04 80 00 */	addi r0, r4, -32768
/* 80082FD8 0007EEF8  90 1F 00 28 */	stw r0, 0x28(r31)
/* 80082FDC 0007EEFC  4B FF 08 4D */	bl nlSprPut
/* 80082FE0 0007EF00  88 16 00 00 */	lbz r0, 0(r22)
/* 80082FE4 0007EF04  2C 00 00 16 */	cmpwi r0, 0x16
/* 80082FE8 0007EF08  40 82 00 C0 */	bne lbl_800830A8
/* 80082FEC 0007EF0C  C0 33 00 08 */	lfs f1, 8(r19)
/* 80082FF0 0007EF10  C0 13 00 00 */	lfs f0, 0(r19)
/* 80082FF4 0007EF14  EC 01 00 28 */	fsubs f0, f1, f0
/* 80082FF8 0007EF18  93 7F 00 00 */	stw r27, 0(r31)
/* 80082FFC 0007EF1C  C0 33 00 00 */	lfs f1, 0(r19)
/* 80083000 0007EF20  EE BC 00 2A */	fadds f21, f28, f0
/* 80083004 0007EF24  FC 5A 05 72 */	fmul f2, f26, f21
/* 80083008 0007EF28  EC 15 05 72 */	fmuls f0, f21, f21
/* 8008300C 0007EF2C  FC 42 08 2A */	fadd f2, f2, f1
/* 80083010 0007EF30  EC 20 90 2A */	fadds f1, f0, f18
/* 80083014 0007EF34  FC 00 10 18 */	frsp f0, f2
/* 80083018 0007EF38  D0 1F 00 04 */	stfs f0, 4(r31)
/* 8008301C 0007EF3C  C0 13 00 04 */	lfs f0, 4(r19)
/* 80083020 0007EF40  FC 13 00 2A */	fadd f0, f19, f0
/* 80083024 0007EF44  FC 00 00 18 */	frsp f0, f0
/* 80083028 0007EF48  D0 1F 00 08 */	stfs f0, 8(r31)
/* 8008302C 0007EF4C  D2 3F 00 0C */	stfs f17, 0xc(r31)
/* 80083030 0007EF50  D3 BF 00 10 */	stfs f29, 0x10(r31)
/* 80083034 0007EF54  4B F8 40 C5 */	bl mathutil_sqrt
/* 80083038 0007EF58  FC 1E 00 72 */	fmul f0, f30, f1
/* 8008303C 0007EF5C  FC 20 A8 90 */	fmr f1, f21
/* 80083040 0007EF60  FC 40 A0 90 */	fmr f2, f20
/* 80083044 0007EF64  FC 00 00 18 */	frsp f0, f0
/* 80083048 0007EF68  D0 1F 00 14 */	stfs f0, 0x14(r31)
/* 8008304C 0007EF6C  4B F8 42 59 */	bl mathutil_atan2
/* 80083050 0007EF70  7C 60 07 34 */	extsh r0, r3
/* 80083054 0007EF74  90 1F 00 28 */	stw r0, 0x28(r31)
/* 80083058 0007EF78  7F E3 FB 78 */	mr r3, r31
/* 8008305C 0007EF7C  4B FF 07 CD */	bl nlSprPut
/* 80083060 0007EF80  93 9F 00 00 */	stw r28, 0(r31)
/* 80083064 0007EF84  7F E3 FB 78 */	mr r3, r31
/* 80083068 0007EF88  C0 13 00 08 */	lfs f0, 8(r19)
/* 8008306C 0007EF8C  EC 1C 00 2A */	fadds f0, f28, f0
/* 80083070 0007EF90  D0 1F 00 04 */	stfs f0, 4(r31)
/* 80083074 0007EF94  C0 13 00 0C */	lfs f0, 0xc(r19)
/* 80083078 0007EF98  D0 1F 00 08 */	stfs f0, 8(r31)
/* 8008307C 0007EF9C  C0 1F 00 0C */	lfs f0, 0xc(r31)
/* 80083080 0007EFA0  FC 00 F8 28 */	fsub f0, f0, f31
/* 80083084 0007EFA4  FC 00 00 18 */	frsp f0, f0
/* 80083088 0007EFA8  D0 1F 00 0C */	stfs f0, 0xc(r31)
/* 8008308C 0007EFAC  D3 BF 00 10 */	stfs f29, 0x10(r31)
/* 80083090 0007EFB0  D3 BF 00 14 */	stfs f29, 0x14(r31)
/* 80083094 0007EFB4  80 9F 00 28 */	lwz r4, 0x28(r31)
/* 80083098 0007EFB8  3C 84 00 01 */	addis r4, r4, 1
/* 8008309C 0007EFBC  38 04 80 00 */	addi r0, r4, -32768
/* 800830A0 0007EFC0  90 1F 00 28 */	stw r0, 0x28(r31)
/* 800830A4 0007EFC4  4B FF 07 85 */	bl nlSprPut
lbl_800830A8:
/* 800830A8 0007EFC8  D2 3F 00 0C */	stfs f17, 0xc(r31)
/* 800830AC 0007EFCC  38 00 00 00 */	li r0, 0
/* 800830B0 0007EFD0  3A B5 00 01 */	addi r21, r21, 1
/* 800830B4 0007EFD4  D2 1F 00 10 */	stfs f16, 0x10(r31)
/* 800830B8 0007EFD8  D1 FF 00 14 */	stfs f15, 0x14(r31)
/* 800830BC 0007EFDC  90 1F 00 28 */	stw r0, 0x28(r31)
/* 800830C0 0007EFE0  D3 BF 00 2C */	stfs f29, 0x2c(r31)
/* 800830C4 0007EFE4  92 9F 00 34 */	stw r20, 0x34(r31)
/* 800830C8 0007EFE8  92 5F 00 38 */	stw r18, 0x38(r31)
lbl_800830CC:
/* 800830CC 0007EFEC  7E D7 AA 14 */	add r22, r23, r21
/* 800830D0 0007EFF0  88 76 00 00 */	lbz r3, 0(r22)
/* 800830D4 0007EFF4  7C 60 07 74 */	extsb r0, r3
/* 800830D8 0007EFF8  2C 00 FF FF */	cmpwi r0, -1
/* 800830DC 0007EFFC  40 82 FC 58 */	bne lbl_80082D34
lbl_800830E0:
/* 800830E0 0007F000  BA 41 00 30 */	lmw r18, 0x30(r1)
/* 800830E4 0007F004  80 01 00 F4 */	lwz r0, 0xf4(r1)
/* 800830E8 0007F008  CB E1 00 E8 */	lfd f31, 0xe8(r1)
/* 800830EC 0007F00C  CB C1 00 E0 */	lfd f30, 0xe0(r1)
/* 800830F0 0007F010  7C 08 03 A6 */	mtlr r0
/* 800830F4 0007F014  CB A1 00 D8 */	lfd f29, 0xd8(r1)
/* 800830F8 0007F018  CB 81 00 D0 */	lfd f28, 0xd0(r1)
/* 800830FC 0007F01C  CB 61 00 C8 */	lfd f27, 0xc8(r1)
/* 80083100 0007F020  CB 41 00 C0 */	lfd f26, 0xc0(r1)
/* 80083104 0007F024  CB 21 00 B8 */	lfd f25, 0xb8(r1)
/* 80083108 0007F028  CB 01 00 B0 */	lfd f24, 0xb0(r1)
/* 8008310C 0007F02C  CA E1 00 A8 */	lfd f23, 0xa8(r1)
/* 80083110 0007F030  CA C1 00 A0 */	lfd f22, 0xa0(r1)
/* 80083114 0007F034  CA A1 00 98 */	lfd f21, 0x98(r1)
/* 80083118 0007F038  CA 81 00 90 */	lfd f20, 0x90(r1)
/* 8008311C 0007F03C  CA 61 00 88 */	lfd f19, 0x88(r1)
/* 80083120 0007F040  CA 41 00 80 */	lfd f18, 0x80(r1)
/* 80083124 0007F044  CA 21 00 78 */	lfd f17, 0x78(r1)
/* 80083128 0007F048  CA 01 00 70 */	lfd f16, 0x70(r1)
/* 8008312C 0007F04C  C9 E1 00 68 */	lfd f15, 0x68(r1)
/* 80083130 0007F050  38 21 00 F0 */	addi r1, r1, 0xf0
/* 80083134 0007F054  4E 80 00 20 */	blr
