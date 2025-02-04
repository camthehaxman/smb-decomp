/* 800D0D90 000CCCB0  7C 08 02 A6 */	mflr r0
/* 800D0D94 000CCCB4  54 85 07 3F */	clrlwi. r5, r4, 0x1c
/* 800D0D98 000CCCB8  90 01 00 04 */	stw r0, 4(r1)
/* 800D0D9C 000CCCBC  3C 60 80 2C */	lis r3, PADType@ha
/* 800D0DA0 000CCCC0  94 21 FF A0 */	stwu r1, -0x60(r1)
/* 800D0DA4 000CCCC4  BF 41 00 48 */	stmw r26, 0x48(r1)
/* 800D0DA8 000CCCC8  3B E3 7B F0 */	addi r31, r3, PADType@l
/* 800D0DAC 000CCCCC  41 82 00 18 */	beq lbl_800D0DC4
/* 800D0DB0 000CCCD0  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D0DB4 000CCCD4  38 80 00 00 */	li r4, 0
/* 800D0DB8 000CCCD8  54 00 10 3A */	slwi r0, r0, 2
/* 800D0DBC 000CCCDC  7C 7F 02 14 */	add r3, r31, r0
/* 800D0DC0 000CCCE0  90 83 00 10 */	stw r4, 0x10(r3)
lbl_800D0DC4:
/* 800D0DC4 000CCCE4  83 AD 97 98 */	lwz r29, ResettingChan
/* 800D0DC8 000CCCE8  3F C0 80 00 */	lis r30, 0x8000
/* 800D0DCC 000CCCEC  28 05 00 00 */	cmplwi r5, 0
/* 800D0DD0 000CCCF0  57 A0 10 3A */	slwi r0, r29, 2
/* 800D0DD4 000CCCF4  7C 7F 02 14 */	add r3, r31, r0
/* 800D0DD8 000CCCF8  83 83 00 10 */	lwz r28, 0x10(r3)
/* 800D0DDC 000CCCFC  7F C7 EC 30 */	srw r7, r30, r29
/* 800D0DE0 000CCD00  7C E6 38 F8 */	nor r6, r7, r7
/* 800D0DE4 000CCD04  7F 9F 01 2E */	stwx r28, r31, r0
/* 800D0DE8 000CCD08  80 AD A2 98 */	lwz r5, RecalibrateBits
/* 800D0DEC 000CCD0C  80 6D A2 B0 */	lwz r3, __PADFixBits
/* 800D0DF0 000CCD10  7C A4 30 38 */	and r4, r5, r6
/* 800D0DF4 000CCD14  7C 60 30 38 */	and r0, r3, r6
/* 800D0DF8 000CCD18  90 8D A2 98 */	stw r4, RecalibrateBits
/* 800D0DFC 000CCD1C  7C BB 38 38 */	and r27, r5, r7
/* 800D0E00 000CCD20  90 0D A2 B0 */	stw r0, __PADFixBits
/* 800D0E04 000CCD24  7C 7A 38 38 */	and r26, r3, r7
/* 800D0E08 000CCD28  40 82 00 14 */	bne lbl_800D0E1C
/* 800D0E0C 000CCD2C  57 83 00 C8 */	rlwinm r3, r28, 0, 3, 4
/* 800D0E10 000CCD30  3C 03 F8 00 */	addis r0, r3, 0xf800
/* 800D0E14 000CCD34  28 00 00 00 */	cmplwi r0, 0
/* 800D0E18 000CCD38  41 82 00 D8 */	beq lbl_800D0EF0
lbl_800D0E1C:
/* 800D0E1C 000CCD3C  4B FF 6D 5D */	bl __OSLockSramEx
/* 800D0E20 000CCD40  57 A0 08 3C */	slwi r0, r29, 1
/* 800D0E24 000CCD44  7C 63 02 14 */	add r3, r3, r0
/* 800D0E28 000CCD48  A4 03 00 1C */	lhzu r0, 0x1c(r3)
/* 800D0E2C 000CCD4C  28 00 00 00 */	cmplwi r0, 0
/* 800D0E30 000CCD50  41 82 00 18 */	beq lbl_800D0E48
/* 800D0E34 000CCD54  38 00 00 00 */	li r0, 0
/* 800D0E38 000CCD58  B0 03 00 00 */	sth r0, 0(r3)
/* 800D0E3C 000CCD5C  38 60 00 01 */	li r3, 1
/* 800D0E40 000CCD60  4B FF 70 C1 */	bl __OSUnlockSramEx
/* 800D0E44 000CCD64  48 00 00 0C */	b lbl_800D0E50
lbl_800D0E48:
/* 800D0E48 000CCD68  38 60 00 00 */	li r3, 0
/* 800D0E4C 000CCD6C  4B FF 70 B5 */	bl __OSUnlockSramEx
lbl_800D0E50:
/* 800D0E50 000CCD70  80 0D A2 90 */	lwz r0, ResettingBits
/* 800D0E54 000CCD74  7C 00 00 34 */	cntlzw r0, r0
/* 800D0E58 000CCD78  90 0D 97 98 */	stw r0, ResettingChan
/* 800D0E5C 000CCD7C  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D0E60 000CCD80  2C 00 00 00 */	cmpwi r0, 0
/* 800D0E64 000CCD84  41 80 06 48 */	blt lbl_800D14AC
/* 800D0E68 000CCD88  2C 00 00 04 */	cmpwi r0, 4
/* 800D0E6C 000CCD8C  40 80 06 40 */	bge lbl_800D14AC
/* 800D0E70 000CCD90  1C 00 00 0C */	mulli r0, r0, 0xc
/* 800D0E74 000CCD94  7C 7F 02 14 */	add r3, r31, r0
/* 800D0E78 000CCD98  38 80 00 00 */	li r4, 0
/* 800D0E7C 000CCD9C  38 A0 00 0C */	li r5, 0xc
/* 800D0E80 000CCDA0  38 63 00 20 */	addi r3, r3, 0x20
/* 800D0E84 000CCDA4  4B F3 24 9D */	bl memset
/* 800D0E88 000CCDA8  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D0E8C 000CCDAC  3C 80 80 0D */	lis r4, PADResetCallback@ha
/* 800D0E90 000CCDB0  39 04 0D 90 */	addi r8, r4, PADResetCallback@l
/* 800D0E94 000CCDB4  54 60 10 3A */	slwi r0, r3, 2
/* 800D0E98 000CCDB8  3B 60 00 00 */	li r27, 0
/* 800D0E9C 000CCDBC  7C DF 02 14 */	add r6, r31, r0
/* 800D0EA0 000CCDC0  97 66 00 10 */	stwu r27, 0x10(r6)
/* 800D0EA4 000CCDC4  38 8D A2 A4 */	addi r4, r13, cmdTypeAndStatus
/* 800D0EA8 000CCDC8  7F 7F 01 2E */	stwx r27, r31, r0
/* 800D0EAC 000CCDCC  38 A0 00 01 */	li r5, 1
/* 800D0EB0 000CCDD0  38 E0 00 03 */	li r7, 3
/* 800D0EB4 000CCDD4  39 40 00 00 */	li r10, 0
/* 800D0EB8 000CCDD8  39 20 00 00 */	li r9, 0
/* 800D0EBC 000CCDDC  4B FF 7B 05 */	bl SITransfer
/* 800D0EC0 000CCDE0  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D0EC4 000CCDE4  3C 80 80 00 */	lis r4, 0x8000
/* 800D0EC8 000CCDE8  80 AD A2 90 */	lwz r5, ResettingBits
/* 800D0ECC 000CCDEC  2C 03 00 00 */	cmpwi r3, 0
/* 800D0ED0 000CCDF0  7C 80 04 30 */	srw r0, r4, r0
/* 800D0ED4 000CCDF4  7C A0 00 78 */	andc r0, r5, r0
/* 800D0ED8 000CCDF8  90 0D A2 90 */	stw r0, ResettingBits
/* 800D0EDC 000CCDFC  40 82 05 D0 */	bne lbl_800D14AC
/* 800D0EE0 000CCE00  38 00 00 20 */	li r0, 0x20
/* 800D0EE4 000CCE04  93 6D A2 90 */	stw r27, ResettingBits
/* 800D0EE8 000CCE08  90 0D 97 98 */	stw r0, ResettingChan
/* 800D0EEC 000CCE0C  48 00 05 C0 */	b lbl_800D14AC
lbl_800D0EF0:
/* 800D0EF0 000CCE10  80 0D 97 A4 */	lwz r0, Spec
/* 800D0EF4 000CCE14  28 00 00 02 */	cmplwi r0, 2
/* 800D0EF8 000CCE18  40 80 00 D0 */	bge lbl_800D0FC8
/* 800D0EFC 000CCE1C  80 0D A2 8C */	lwz r0, EnabledBits
/* 800D0F00 000CCE20  38 7D 00 00 */	addi r3, r29, 0
/* 800D0F04 000CCE24  38 81 00 38 */	addi r4, r1, 0x38
/* 800D0F08 000CCE28  7C 00 3B 78 */	or r0, r0, r7
/* 800D0F0C 000CCE2C  90 0D A2 8C */	stw r0, EnabledBits
/* 800D0F10 000CCE30  4B FF 7A 01 */	bl SIGetResponse
/* 800D0F14 000CCE34  80 0D 97 A0 */	lwz r0, AnalogMode
/* 800D0F18 000CCE38  38 7D 00 00 */	addi r3, r29, 0
/* 800D0F1C 000CCE3C  64 04 00 40 */	oris r4, r0, 0x40
/* 800D0F20 000CCE40  4B FF 78 65 */	bl SISetCommand
/* 800D0F24 000CCE44  80 6D A2 8C */	lwz r3, EnabledBits
/* 800D0F28 000CCE48  4B FF 78 E1 */	bl SIEnablePolling
/* 800D0F2C 000CCE4C  80 0D A2 90 */	lwz r0, ResettingBits
/* 800D0F30 000CCE50  7C 00 00 34 */	cntlzw r0, r0
/* 800D0F34 000CCE54  90 0D 97 98 */	stw r0, ResettingChan
/* 800D0F38 000CCE58  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D0F3C 000CCE5C  2C 00 00 00 */	cmpwi r0, 0
/* 800D0F40 000CCE60  41 80 05 6C */	blt lbl_800D14AC
/* 800D0F44 000CCE64  2C 00 00 04 */	cmpwi r0, 4
/* 800D0F48 000CCE68  40 80 05 64 */	bge lbl_800D14AC
/* 800D0F4C 000CCE6C  1C 00 00 0C */	mulli r0, r0, 0xc
/* 800D0F50 000CCE70  7C 7F 02 14 */	add r3, r31, r0
/* 800D0F54 000CCE74  38 80 00 00 */	li r4, 0
/* 800D0F58 000CCE78  38 A0 00 0C */	li r5, 0xc
/* 800D0F5C 000CCE7C  38 63 00 20 */	addi r3, r3, 0x20
/* 800D0F60 000CCE80  4B F3 23 C1 */	bl memset
/* 800D0F64 000CCE84  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D0F68 000CCE88  3C 80 80 0D */	lis r4, PADResetCallback@ha
/* 800D0F6C 000CCE8C  39 04 0D 90 */	addi r8, r4, PADResetCallback@l
/* 800D0F70 000CCE90  54 60 10 3A */	slwi r0, r3, 2
/* 800D0F74 000CCE94  3B 60 00 00 */	li r27, 0
/* 800D0F78 000CCE98  7C DF 02 14 */	add r6, r31, r0
/* 800D0F7C 000CCE9C  97 66 00 10 */	stwu r27, 0x10(r6)
/* 800D0F80 000CCEA0  38 8D A2 A4 */	addi r4, r13, cmdTypeAndStatus
/* 800D0F84 000CCEA4  7F 7F 01 2E */	stwx r27, r31, r0
/* 800D0F88 000CCEA8  38 A0 00 01 */	li r5, 1
/* 800D0F8C 000CCEAC  38 E0 00 03 */	li r7, 3
/* 800D0F90 000CCEB0  39 40 00 00 */	li r10, 0
/* 800D0F94 000CCEB4  39 20 00 00 */	li r9, 0
/* 800D0F98 000CCEB8  4B FF 7A 29 */	bl SITransfer
/* 800D0F9C 000CCEBC  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D0FA0 000CCEC0  2C 03 00 00 */	cmpwi r3, 0
/* 800D0FA4 000CCEC4  80 6D A2 90 */	lwz r3, ResettingBits
/* 800D0FA8 000CCEC8  7F C0 04 30 */	srw r0, r30, r0
/* 800D0FAC 000CCECC  7C 60 00 78 */	andc r0, r3, r0
/* 800D0FB0 000CCED0  90 0D A2 90 */	stw r0, ResettingBits
/* 800D0FB4 000CCED4  40 82 04 F8 */	bne lbl_800D14AC
/* 800D0FB8 000CCED8  38 00 00 20 */	li r0, 0x20
/* 800D0FBC 000CCEDC  93 6D A2 90 */	stw r27, ResettingBits
/* 800D0FC0 000CCEE0  90 0D 97 98 */	stw r0, ResettingChan
/* 800D0FC4 000CCEE4  48 00 04 E8 */	b lbl_800D14AC
lbl_800D0FC8:
/* 800D0FC8 000CCEE8  57 80 00 01 */	rlwinm. r0, r28, 0, 0, 0
/* 800D0FCC 000CCEEC  41 82 00 0C */	beq lbl_800D0FD8
/* 800D0FD0 000CCEF0  57 80 01 4B */	rlwinm. r0, r28, 0, 5, 5
/* 800D0FD4 000CCEF4  41 82 01 50 */	beq lbl_800D1124
lbl_800D0FD8:
/* 800D0FD8 000CCEF8  4B FF 6B A1 */	bl __OSLockSramEx
/* 800D0FDC 000CCEFC  57 A0 08 3C */	slwi r0, r29, 1
/* 800D0FE0 000CCF00  7C 63 02 14 */	add r3, r3, r0
/* 800D0FE4 000CCF04  A4 03 00 1C */	lhzu r0, 0x1c(r3)
/* 800D0FE8 000CCF08  28 00 00 00 */	cmplwi r0, 0
/* 800D0FEC 000CCF0C  41 82 00 18 */	beq lbl_800D1004
/* 800D0FF0 000CCF10  38 00 00 00 */	li r0, 0
/* 800D0FF4 000CCF14  B0 03 00 00 */	sth r0, 0(r3)
/* 800D0FF8 000CCF18  38 60 00 01 */	li r3, 1
/* 800D0FFC 000CCF1C  4B FF 6F 05 */	bl __OSUnlockSramEx
/* 800D1000 000CCF20  48 00 00 0C */	b lbl_800D100C
lbl_800D1004:
/* 800D1004 000CCF24  38 60 00 00 */	li r3, 0
/* 800D1008 000CCF28  4B FF 6E F9 */	bl __OSUnlockSramEx
lbl_800D100C:
/* 800D100C 000CCF2C  57 80 01 CF */	rlwinm. r0, r28, 0, 7, 7
/* 800D1010 000CCF30  40 82 00 A4 */	bne lbl_800D10B4
/* 800D1014 000CCF34  80 0D A2 90 */	lwz r0, ResettingBits
/* 800D1018 000CCF38  7C 00 00 34 */	cntlzw r0, r0
/* 800D101C 000CCF3C  90 0D 97 98 */	stw r0, ResettingChan
/* 800D1020 000CCF40  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D1024 000CCF44  2C 00 00 00 */	cmpwi r0, 0
/* 800D1028 000CCF48  41 80 04 84 */	blt lbl_800D14AC
/* 800D102C 000CCF4C  2C 00 00 04 */	cmpwi r0, 4
/* 800D1030 000CCF50  40 80 04 7C */	bge lbl_800D14AC
/* 800D1034 000CCF54  1C 00 00 0C */	mulli r0, r0, 0xc
/* 800D1038 000CCF58  7C 7F 02 14 */	add r3, r31, r0
/* 800D103C 000CCF5C  38 80 00 00 */	li r4, 0
/* 800D1040 000CCF60  38 A0 00 0C */	li r5, 0xc
/* 800D1044 000CCF64  38 63 00 20 */	addi r3, r3, 0x20
/* 800D1048 000CCF68  4B F3 22 D9 */	bl memset
/* 800D104C 000CCF6C  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D1050 000CCF70  3C 80 80 0D */	lis r4, PADResetCallback@ha
/* 800D1054 000CCF74  39 04 0D 90 */	addi r8, r4, PADResetCallback@l
/* 800D1058 000CCF78  54 60 10 3A */	slwi r0, r3, 2
/* 800D105C 000CCF7C  3B 60 00 00 */	li r27, 0
/* 800D1060 000CCF80  7C DF 02 14 */	add r6, r31, r0
/* 800D1064 000CCF84  97 66 00 10 */	stwu r27, 0x10(r6)
/* 800D1068 000CCF88  38 8D A2 A4 */	addi r4, r13, cmdTypeAndStatus
/* 800D106C 000CCF8C  7F 7F 01 2E */	stwx r27, r31, r0
/* 800D1070 000CCF90  38 A0 00 01 */	li r5, 1
/* 800D1074 000CCF94  38 E0 00 03 */	li r7, 3
/* 800D1078 000CCF98  39 40 00 00 */	li r10, 0
/* 800D107C 000CCF9C  39 20 00 00 */	li r9, 0
/* 800D1080 000CCFA0  4B FF 79 41 */	bl SITransfer
/* 800D1084 000CCFA4  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D1088 000CCFA8  3C 80 80 00 */	lis r4, 0x8000
/* 800D108C 000CCFAC  80 AD A2 90 */	lwz r5, ResettingBits
/* 800D1090 000CCFB0  2C 03 00 00 */	cmpwi r3, 0
/* 800D1094 000CCFB4  7C 80 04 30 */	srw r0, r4, r0
/* 800D1098 000CCFB8  7C A0 00 78 */	andc r0, r5, r0
/* 800D109C 000CCFBC  90 0D A2 90 */	stw r0, ResettingBits
/* 800D10A0 000CCFC0  40 82 04 0C */	bne lbl_800D14AC
/* 800D10A4 000CCFC4  38 00 00 20 */	li r0, 0x20
/* 800D10A8 000CCFC8  93 6D A2 90 */	stw r27, ResettingBits
/* 800D10AC 000CCFCC  90 0D 97 98 */	stw r0, ResettingChan
/* 800D10B0 000CCFD0  48 00 03 FC */	b lbl_800D14AC
lbl_800D10B4:
/* 800D10B4 000CCFD4  28 1B 00 00 */	cmplwi r27, 0
/* 800D10B8 000CCFD8  41 82 00 38 */	beq lbl_800D10F0
/* 800D10BC 000CCFDC  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D10C0 000CCFE0  3C 80 80 0D */	lis r4, PADOriginCallback@ha
/* 800D10C4 000CCFE4  39 04 09 E4 */	addi r8, r4, PADOriginCallback@l
/* 800D10C8 000CCFE8  1C 03 00 0C */	mulli r0, r3, 0xc
/* 800D10CC 000CCFEC  7C DF 02 14 */	add r6, r31, r0
/* 800D10D0 000CCFF0  38 8D 97 B0 */	addi r4, r13, cmdCalibrate
/* 800D10D4 000CCFF4  38 A0 00 03 */	li r5, 3
/* 800D10D8 000CCFF8  38 E0 00 0A */	li r7, 0xa
/* 800D10DC 000CCFFC  39 40 00 00 */	li r10, 0
/* 800D10E0 000CD000  39 20 00 00 */	li r9, 0
/* 800D10E4 000CD004  38 C6 00 20 */	addi r6, r6, 0x20
/* 800D10E8 000CD008  4B FF 78 D9 */	bl SITransfer
/* 800D10EC 000CD00C  48 00 03 C0 */	b lbl_800D14AC
lbl_800D10F0:
/* 800D10F0 000CD010  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D10F4 000CD014  3C 80 80 0D */	lis r4, PADOriginCallback@ha
/* 800D10F8 000CD018  39 04 09 E4 */	addi r8, r4, PADOriginCallback@l
/* 800D10FC 000CD01C  1C 03 00 0C */	mulli r0, r3, 0xc
/* 800D1100 000CD020  7C DF 02 14 */	add r6, r31, r0
/* 800D1104 000CD024  38 8D 97 AC */	addi r4, r13, cmdReadOrigin
/* 800D1108 000CD028  38 A0 00 01 */	li r5, 1
/* 800D110C 000CD02C  38 E0 00 0A */	li r7, 0xa
/* 800D1110 000CD030  39 40 00 00 */	li r10, 0
/* 800D1114 000CD034  39 20 00 00 */	li r9, 0
/* 800D1118 000CD038  38 C6 00 20 */	addi r6, r6, 0x20
/* 800D111C 000CD03C  4B FF 78 A5 */	bl SITransfer
/* 800D1120 000CD040  48 00 03 8C */	b lbl_800D14AC
lbl_800D1124:
/* 800D1124 000CD044  4B FF 6A 55 */	bl __OSLockSramEx
/* 800D1128 000CD048  57 A0 08 3C */	slwi r0, r29, 1
/* 800D112C 000CD04C  7C 63 02 14 */	add r3, r3, r0
/* 800D1130 000CD050  A3 63 00 1C */	lhz r27, 0x1c(r3)
/* 800D1134 000CD054  38 60 00 00 */	li r3, 0
/* 800D1138 000CD058  4B FF 6D C9 */	bl __OSUnlockSramEx
/* 800D113C 000CD05C  2C 1A 00 00 */	cmpwi r26, 0
/* 800D1140 000CD060  57 7D 40 2E */	slwi r29, r27, 8
/* 800D1144 000CD064  41 82 00 54 */	beq lbl_800D1198
/* 800D1148 000CD068  57 A0 02 D7 */	rlwinm. r0, r29, 0, 0xb, 0xb
/* 800D114C 000CD06C  41 82 00 4C */	beq lbl_800D1198
/* 800D1150 000CD070  3C 80 00 D0 */	lis r4, 0x00CFFF00@ha
/* 800D1154 000CD074  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D1158 000CD078  38 04 FF 00 */	addi r0, r4, 0x00CFFF00@l
/* 800D115C 000CD07C  7F A0 00 38 */	and r0, r29, r0
/* 800D1160 000CD080  54 66 10 3A */	slwi r6, r3, 2
/* 800D1164 000CD084  7C 9F 32 14 */	add r4, r31, r6
/* 800D1168 000CD088  64 00 4E 10 */	oris r0, r0, 0x4e10
/* 800D116C 000CD08C  3C A0 80 0D */	lis r5, PADFixCallback@ha
/* 800D1170 000CD090  94 04 00 60 */	stwu r0, 0x60(r4)
/* 800D1174 000CD094  7C DF 32 14 */	add r6, r31, r6
/* 800D1178 000CD098  39 05 0B 3C */	addi r8, r5, PADFixCallback@l
/* 800D117C 000CD09C  38 A0 00 03 */	li r5, 3
/* 800D1180 000CD0A0  38 E0 00 03 */	li r7, 3
/* 800D1184 000CD0A4  39 40 00 00 */	li r10, 0
/* 800D1188 000CD0A8  39 20 00 00 */	li r9, 0
/* 800D118C 000CD0AC  38 C6 00 10 */	addi r6, r6, 0x10
/* 800D1190 000CD0B0  4B FF 78 31 */	bl SITransfer
/* 800D1194 000CD0B4  48 00 03 18 */	b lbl_800D14AC
lbl_800D1198:
/* 800D1198 000CD0B8  57 80 02 D7 */	rlwinm. r0, r28, 0, 0xb, 0xb
/* 800D119C 000CD0BC  41 82 01 24 */	beq lbl_800D12C0
/* 800D11A0 000CD0C0  3C 60 00 D0 */	lis r3, 0x00CFFF00@ha
/* 800D11A4 000CD0C4  38 63 FF 00 */	addi r3, r3, 0x00CFFF00@l
/* 800D11A8 000CD0C8  7F A0 18 38 */	and r0, r29, r3
/* 800D11AC 000CD0CC  7F 83 18 38 */	and r3, r28, r3
/* 800D11B0 000CD0D0  7C 00 18 40 */	cmplw r0, r3
/* 800D11B4 000CD0D4  41 82 00 84 */	beq lbl_800D1238
/* 800D11B8 000CD0D8  57 A0 02 D7 */	rlwinm. r0, r29, 0, 0xb, 0xb
/* 800D11BC 000CD0DC  40 82 00 40 */	bne lbl_800D11FC
/* 800D11C0 000CD0E0  64 7D 00 10 */	oris r29, r3, 0x10
/* 800D11C4 000CD0E4  83 6D 97 98 */	lwz r27, ResettingChan
/* 800D11C8 000CD0E8  4B FF 69 B1 */	bl __OSLockSramEx
/* 800D11CC 000CD0EC  57 60 08 3C */	slwi r0, r27, 1
/* 800D11D0 000CD0F0  7C 83 02 14 */	add r4, r3, r0
/* 800D11D4 000CD0F4  A4 04 00 1C */	lhzu r0, 0x1c(r4)
/* 800D11D8 000CD0F8  57 A3 C4 3E */	rlwinm r3, r29, 0x18, 0x10, 0x1f
/* 800D11DC 000CD0FC  7C 00 18 40 */	cmplw r0, r3
/* 800D11E0 000CD100  41 82 00 14 */	beq lbl_800D11F4
/* 800D11E4 000CD104  B0 64 00 00 */	sth r3, 0(r4)
/* 800D11E8 000CD108  38 60 00 01 */	li r3, 1
/* 800D11EC 000CD10C  4B FF 6D 15 */	bl __OSUnlockSramEx
/* 800D11F0 000CD110  48 00 00 0C */	b lbl_800D11FC
lbl_800D11F4:
/* 800D11F4 000CD114  38 60 00 00 */	li r3, 0
/* 800D11F8 000CD118  4B FF 6D 09 */	bl __OSUnlockSramEx
lbl_800D11FC:
/* 800D11FC 000CD11C  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D1200 000CD120  3C 80 80 0D */	lis r4, PADFixCallback@ha
/* 800D1204 000CD124  67 A0 4E 00 */	oris r0, r29, 0x4e00
/* 800D1208 000CD128  54 65 10 3A */	slwi r5, r3, 2
/* 800D120C 000CD12C  39 04 0B 3C */	addi r8, r4, PADFixCallback@l
/* 800D1210 000CD130  7C 9F 2A 14 */	add r4, r31, r5
/* 800D1214 000CD134  94 04 00 60 */	stwu r0, 0x60(r4)
/* 800D1218 000CD138  7C DF 2A 14 */	add r6, r31, r5
/* 800D121C 000CD13C  38 A0 00 03 */	li r5, 3
/* 800D1220 000CD140  38 E0 00 03 */	li r7, 3
/* 800D1224 000CD144  39 40 00 00 */	li r10, 0
/* 800D1228 000CD148  39 20 00 00 */	li r9, 0
/* 800D122C 000CD14C  38 C6 00 10 */	addi r6, r6, 0x10
/* 800D1230 000CD150  4B FF 77 91 */	bl SITransfer
/* 800D1234 000CD154  48 00 02 78 */	b lbl_800D14AC
lbl_800D1238:
/* 800D1238 000CD158  57 80 00 43 */	rlwinm. r0, r28, 0, 1, 1
/* 800D123C 000CD15C  41 82 00 48 */	beq lbl_800D1284
/* 800D1240 000CD160  57 80 03 19 */	rlwinm. r0, r28, 0, 0xc, 0xc
/* 800D1244 000CD164  40 82 00 40 */	bne lbl_800D1284
/* 800D1248 000CD168  57 80 03 5B */	rlwinm. r0, r28, 0, 0xd, 0xd
/* 800D124C 000CD16C  40 82 00 38 */	bne lbl_800D1284
/* 800D1250 000CD170  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D1254 000CD174  3C 80 80 0D */	lis r4, PADOriginCallback@ha
/* 800D1258 000CD178  39 04 09 E4 */	addi r8, r4, PADOriginCallback@l
/* 800D125C 000CD17C  1C 03 00 0C */	mulli r0, r3, 0xc
/* 800D1260 000CD180  7C DF 02 14 */	add r6, r31, r0
/* 800D1264 000CD184  38 8D 97 AC */	addi r4, r13, cmdReadOrigin
/* 800D1268 000CD188  38 A0 00 01 */	li r5, 1
/* 800D126C 000CD18C  38 E0 00 0A */	li r7, 0xa
/* 800D1270 000CD190  39 40 00 00 */	li r10, 0
/* 800D1274 000CD194  39 20 00 00 */	li r9, 0
/* 800D1278 000CD198  38 C6 00 20 */	addi r6, r6, 0x20
/* 800D127C 000CD19C  4B FF 77 45 */	bl SITransfer
/* 800D1280 000CD1A0  48 00 02 2C */	b lbl_800D14AC
lbl_800D1284:
/* 800D1284 000CD1A4  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D1288 000CD1A8  3C 80 80 0D */	lis r4, PADProbeCallback@ha
/* 800D128C 000CD1AC  39 04 06 74 */	addi r8, r4, PADProbeCallback@l
/* 800D1290 000CD1B0  1C 03 00 0C */	mulli r0, r3, 0xc
/* 800D1294 000CD1B4  54 64 10 3A */	slwi r4, r3, 2
/* 800D1298 000CD1B8  7C 9F 22 14 */	add r4, r31, r4
/* 800D129C 000CD1BC  7C DF 02 14 */	add r6, r31, r0
/* 800D12A0 000CD1C0  38 A0 00 03 */	li r5, 3
/* 800D12A4 000CD1C4  38 E0 00 08 */	li r7, 8
/* 800D12A8 000CD1C8  39 40 00 00 */	li r10, 0
/* 800D12AC 000CD1CC  39 20 00 00 */	li r9, 0
/* 800D12B0 000CD1D0  38 84 00 50 */	addi r4, r4, 0x50
/* 800D12B4 000CD1D4  38 C6 00 20 */	addi r6, r6, 0x20
/* 800D12B8 000CD1D8  4B FF 77 09 */	bl SITransfer
/* 800D12BC 000CD1DC  48 00 01 F0 */	b lbl_800D14AC
lbl_800D12C0:
/* 800D12C0 000CD1E0  57 80 00 43 */	rlwinm. r0, r28, 0, 1, 1
/* 800D12C4 000CD1E4  41 82 00 88 */	beq lbl_800D134C
/* 800D12C8 000CD1E8  3C 60 00 D0 */	lis r3, 0x00CFFF00@ha
/* 800D12CC 000CD1EC  83 6D 97 98 */	lwz r27, ResettingChan
/* 800D12D0 000CD1F0  38 03 FF 00 */	addi r0, r3, 0x00CFFF00@l
/* 800D12D4 000CD1F4  7F 9A 00 38 */	and r26, r28, r0
/* 800D12D8 000CD1F8  67 5A 00 10 */	oris r26, r26, 0x10
/* 800D12DC 000CD1FC  4B FF 68 9D */	bl __OSLockSramEx
/* 800D12E0 000CD200  57 60 08 3C */	slwi r0, r27, 1
/* 800D12E4 000CD204  7C 83 02 14 */	add r4, r3, r0
/* 800D12E8 000CD208  A4 04 00 1C */	lhzu r0, 0x1c(r4)
/* 800D12EC 000CD20C  57 43 C4 3E */	rlwinm r3, r26, 0x18, 0x10, 0x1f
/* 800D12F0 000CD210  7C 00 18 40 */	cmplw r0, r3
/* 800D12F4 000CD214  41 82 00 14 */	beq lbl_800D1308
/* 800D12F8 000CD218  B0 64 00 00 */	sth r3, 0(r4)
/* 800D12FC 000CD21C  38 60 00 01 */	li r3, 1
/* 800D1300 000CD220  4B FF 6C 01 */	bl __OSUnlockSramEx
/* 800D1304 000CD224  48 00 00 0C */	b lbl_800D1310
lbl_800D1308:
/* 800D1308 000CD228  38 60 00 00 */	li r3, 0
/* 800D130C 000CD22C  4B FF 6B F5 */	bl __OSUnlockSramEx
lbl_800D1310:
/* 800D1310 000CD230  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D1314 000CD234  3C 80 80 0D */	lis r4, PADFixCallback@ha
/* 800D1318 000CD238  67 40 4E 00 */	oris r0, r26, 0x4e00
/* 800D131C 000CD23C  54 65 10 3A */	slwi r5, r3, 2
/* 800D1320 000CD240  39 04 0B 3C */	addi r8, r4, PADFixCallback@l
/* 800D1324 000CD244  7C 9F 2A 14 */	add r4, r31, r5
/* 800D1328 000CD248  94 04 00 60 */	stwu r0, 0x60(r4)
/* 800D132C 000CD24C  7C DF 2A 14 */	add r6, r31, r5
/* 800D1330 000CD250  38 A0 00 03 */	li r5, 3
/* 800D1334 000CD254  38 E0 00 03 */	li r7, 3
/* 800D1338 000CD258  39 40 00 00 */	li r10, 0
/* 800D133C 000CD25C  39 20 00 00 */	li r9, 0
/* 800D1340 000CD260  38 C6 00 10 */	addi r6, r6, 0x10
/* 800D1344 000CD264  4B FF 76 7D */	bl SITransfer
/* 800D1348 000CD268  48 00 01 64 */	b lbl_800D14AC
lbl_800D134C:
/* 800D134C 000CD26C  83 6D 97 98 */	lwz r27, ResettingChan
/* 800D1350 000CD270  4B FF 68 29 */	bl __OSLockSramEx
/* 800D1354 000CD274  57 60 08 3C */	slwi r0, r27, 1
/* 800D1358 000CD278  7C 63 02 14 */	add r3, r3, r0
/* 800D135C 000CD27C  A4 03 00 1C */	lhzu r0, 0x1c(r3)
/* 800D1360 000CD280  28 00 00 00 */	cmplwi r0, 0
/* 800D1364 000CD284  41 82 00 18 */	beq lbl_800D137C
/* 800D1368 000CD288  38 00 00 00 */	li r0, 0
/* 800D136C 000CD28C  B0 03 00 00 */	sth r0, 0(r3)
/* 800D1370 000CD290  38 60 00 01 */	li r3, 1
/* 800D1374 000CD294  4B FF 6B 8D */	bl __OSUnlockSramEx
/* 800D1378 000CD298  48 00 00 0C */	b lbl_800D1384
lbl_800D137C:
/* 800D137C 000CD29C  38 60 00 00 */	li r3, 0
/* 800D1380 000CD2A0  4B FF 6B 81 */	bl __OSUnlockSramEx
lbl_800D1384:
/* 800D1384 000CD2A4  83 AD 97 98 */	lwz r29, ResettingChan
/* 800D1388 000CD2A8  3F 60 80 00 */	lis r27, 0x8000
/* 800D138C 000CD2AC  80 6D A2 8C */	lwz r3, EnabledBits
/* 800D1390 000CD2B0  38 81 00 1C */	addi r4, r1, 0x1c
/* 800D1394 000CD2B4  7F 65 EC 30 */	srw r5, r27, r29
/* 800D1398 000CD2B8  80 0D A2 94 */	lwz r0, ProbingBits
/* 800D139C 000CD2BC  7C 63 2B 78 */	or r3, r3, r5
/* 800D13A0 000CD2C0  90 6D A2 8C */	stw r3, EnabledBits
/* 800D13A4 000CD2C4  7F A3 EB 78 */	mr r3, r29
/* 800D13A8 000CD2C8  7C 00 2B 78 */	or r0, r0, r5
/* 800D13AC 000CD2CC  90 0D A2 94 */	stw r0, ProbingBits
/* 800D13B0 000CD2D0  4B FF 75 61 */	bl SIGetResponse
/* 800D13B4 000CD2D4  57 A0 10 3A */	slwi r0, r29, 2
/* 800D13B8 000CD2D8  7C 7F 02 14 */	add r3, r31, r0
/* 800D13BC 000CD2DC  80 83 00 10 */	lwz r4, 0x10(r3)
/* 800D13C0 000CD2E0  54 80 01 8D */	rlwinm. r0, r4, 0, 6, 6
/* 800D13C4 000CD2E4  40 82 00 1C */	bne lbl_800D13E0
/* 800D13C8 000CD2E8  A0 1B 30 E0 */	lhz r0, 0x30e0(r27)
/* 800D13CC 000CD2EC  57 A3 70 22 */	slwi r3, r29, 0xe
/* 800D13D0 000CD2F0  64 63 00 4D */	oris r3, r3, 0x4d
/* 800D13D4 000CD2F4  54 00 04 BE */	clrlwi r0, r0, 0x12
/* 800D13D8 000CD2F8  7C 64 03 78 */	or r4, r3, r0
/* 800D13DC 000CD2FC  48 00 00 24 */	b lbl_800D1400
lbl_800D13E0:
/* 800D13E0 000CD300  54 83 03 1A */	rlwinm r3, r4, 0, 0xc, 0xd
/* 800D13E4 000CD304  3C 03 FF FC */	addis r0, r3, 0xfffc
/* 800D13E8 000CD308  28 00 00 00 */	cmplwi r0, 0
/* 800D13EC 000CD30C  40 82 00 0C */	bne lbl_800D13F8
/* 800D13F0 000CD310  3C 80 00 50 */	lis r4, 0x50
/* 800D13F4 000CD314  48 00 00 0C */	b lbl_800D1400
lbl_800D13F8:
/* 800D13F8 000CD318  54 83 03 5E */	rlwinm r3, r4, 0, 0xd, 0xf
/* 800D13FC 000CD31C  3C 83 00 44 */	addis r4, r3, 0x44
lbl_800D1400:
/* 800D1400 000CD320  7F A3 EB 78 */	mr r3, r29
/* 800D1404 000CD324  4B FF 73 81 */	bl SISetCommand
/* 800D1408 000CD328  80 6D A2 8C */	lwz r3, EnabledBits
/* 800D140C 000CD32C  4B FF 73 FD */	bl SIEnablePolling
/* 800D1410 000CD330  80 0D A2 90 */	lwz r0, ResettingBits
/* 800D1414 000CD334  7C 00 00 34 */	cntlzw r0, r0
/* 800D1418 000CD338  90 0D 97 98 */	stw r0, ResettingChan
/* 800D141C 000CD33C  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D1420 000CD340  2C 00 00 00 */	cmpwi r0, 0
/* 800D1424 000CD344  41 80 00 88 */	blt lbl_800D14AC
/* 800D1428 000CD348  2C 00 00 04 */	cmpwi r0, 4
/* 800D142C 000CD34C  40 80 00 80 */	bge lbl_800D14AC
/* 800D1430 000CD350  1C 00 00 0C */	mulli r0, r0, 0xc
/* 800D1434 000CD354  7C 7F 02 14 */	add r3, r31, r0
/* 800D1438 000CD358  38 80 00 00 */	li r4, 0
/* 800D143C 000CD35C  38 A0 00 0C */	li r5, 0xc
/* 800D1440 000CD360  38 63 00 20 */	addi r3, r3, 0x20
/* 800D1444 000CD364  4B F3 1E DD */	bl memset
/* 800D1448 000CD368  80 6D 97 98 */	lwz r3, ResettingChan
/* 800D144C 000CD36C  3C 80 80 0D */	lis r4, PADResetCallback@ha
/* 800D1450 000CD370  39 04 0D 90 */	addi r8, r4, PADResetCallback@l
/* 800D1454 000CD374  54 60 10 3A */	slwi r0, r3, 2
/* 800D1458 000CD378  3B 60 00 00 */	li r27, 0
/* 800D145C 000CD37C  7C DF 02 14 */	add r6, r31, r0
/* 800D1460 000CD380  97 66 00 10 */	stwu r27, 0x10(r6)
/* 800D1464 000CD384  38 8D A2 A4 */	addi r4, r13, cmdTypeAndStatus
/* 800D1468 000CD388  7F 7F 01 2E */	stwx r27, r31, r0
/* 800D146C 000CD38C  38 A0 00 01 */	li r5, 1
/* 800D1470 000CD390  38 E0 00 03 */	li r7, 3
/* 800D1474 000CD394  39 40 00 00 */	li r10, 0
/* 800D1478 000CD398  39 20 00 00 */	li r9, 0
/* 800D147C 000CD39C  4B FF 75 45 */	bl SITransfer
/* 800D1480 000CD3A0  80 0D 97 98 */	lwz r0, ResettingChan
/* 800D1484 000CD3A4  3C 80 80 00 */	lis r4, 0x8000
/* 800D1488 000CD3A8  80 AD A2 90 */	lwz r5, ResettingBits
/* 800D148C 000CD3AC  2C 03 00 00 */	cmpwi r3, 0
/* 800D1490 000CD3B0  7C 80 04 30 */	srw r0, r4, r0
/* 800D1494 000CD3B4  7C A0 00 78 */	andc r0, r5, r0
/* 800D1498 000CD3B8  90 0D A2 90 */	stw r0, ResettingBits
/* 800D149C 000CD3BC  40 82 00 10 */	bne lbl_800D14AC
/* 800D14A0 000CD3C0  38 00 00 20 */	li r0, 0x20
/* 800D14A4 000CD3C4  93 6D A2 90 */	stw r27, ResettingBits
/* 800D14A8 000CD3C8  90 0D 97 98 */	stw r0, ResettingChan
lbl_800D14AC:
/* 800D14AC 000CD3CC  BB 41 00 48 */	lmw r26, 0x48(r1)
/* 800D14B0 000CD3D0  80 01 00 64 */	lwz r0, 0x64(r1)
/* 800D14B4 000CD3D4  38 21 00 60 */	addi r1, r1, 0x60
/* 800D14B8 000CD3D8  7C 08 03 A6 */	mtlr r0
/* 800D14BC 000CD3DC  4E 80 00 20 */	blr
