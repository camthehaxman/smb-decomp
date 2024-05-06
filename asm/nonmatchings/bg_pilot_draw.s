/* 800646F4 00060614  7C 08 02 A6 */	mflr r0
/* 800646F8 00060618  3C 60 80 1F */	lis r3, polyDisp@ha
/* 800646FC 0006061C  90 01 00 04 */	stw r0, 4(r1)
/* 80064700 00060620  94 21 FE D8 */	stwu r1, -0x128(r1)
/* 80064704 00060624  DB E1 01 20 */	stfd f31, 0x120(r1)
/* 80064708 00060628  BF 61 01 0C */	stmw r27, 0x10c(r1)
/* 8006470C 0006062C  80 03 EC 90 */	lwz r0, polyDisp@l(r3)
/* 80064710 00060630  3C 60 80 1C */	lis r3, backgroundInfo@ha
/* 80064714 00060634  38 63 91 78 */	addi r3, r3, backgroundInfo@l
/* 80064718 00060638  54 00 07 FF */	clrlwi. r0, r0, 0x1f
/* 8006471C 0006063C  83 C3 00 9C */	lwz r30, 0x9c(r3)
/* 80064720 00060640  41 82 00 08 */	beq lbl_80064728
/* 80064724 00060644  48 00 00 14 */	b lbl_80064738
lbl_80064728:
/* 80064728 00060648  A8 0D 99 B2 */	lha r0, gameMode  //@sda21(r13)
/* 8006472C 0006064C  2C 00 00 02 */	cmpwi r0, 2
/* 80064730 00060650  41 82 00 08 */	beq lbl_80064738
/* 80064734 00060654  2C 00 00 04 */	cmpwi r0, 4
lbl_80064738:
/* 80064738 00060658  80 7E 00 14 */	lwz r3, 0x14(r30)
/* 8006473C 0006065C  28 03 00 00 */	cmplwi r3, 0
/* 80064740 00060660  41 82 00 10 */	beq lbl_80064750
/* 80064744 00060664  80 03 00 00 */	lwz r0, 0(r3)
/* 80064748 00060668  54 00 04 1C */	rlwinm r0, r0, 0, 0x10, 0xe
/* 8006474C 0006066C  90 03 00 00 */	stw r0, 0(r3)
lbl_80064750:
/* 80064750 00060670  4B FF 0E 09 */	bl bg_default_draw
/* 80064754 00060674  4B FA 32 59 */	bl mathutil_mtxA_from_mtxB
/* 80064758 00060678  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 8006475C 0006067C  38 80 00 00 */	li r4, 0
/* 80064760 00060680  48 07 FE 21 */	bl GXLoadPosMtxImm
/* 80064764 00060684  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 80064768 00060688  38 80 00 00 */	li r4, 0
/* 8006476C 0006068C  48 07 FE 51 */	bl GXLoadNrmMtxImm
/* 80064770 00060690  80 7E 00 04 */	lwz r3, 4(r30)
/* 80064774 00060694  28 03 00 00 */	cmplwi r3, 0
/* 80064778 00060698  41 82 00 08 */	beq lbl_80064780
/* 8006477C 0006069C  48 02 9D 21 */	bl avdisp_draw_model_culled_sort_none
lbl_80064780:
/* 80064780 000606A0  80 7E 00 08 */	lwz r3, 8(r30)
/* 80064784 000606A4  28 03 00 00 */	cmplwi r3, 0
/* 80064788 000606A8  41 82 00 08 */	beq lbl_80064790
/* 8006478C 000606AC  48 02 9D 11 */	bl avdisp_draw_model_culled_sort_none
lbl_80064790:
/* 80064790 000606B0  80 7E 00 0C */	lwz r3, 0xc(r30)
/* 80064794 000606B4  28 03 00 00 */	cmplwi r3, 0
/* 80064798 000606B8  41 82 00 08 */	beq lbl_800647A0
/* 8006479C 000606BC  48 02 9D 01 */	bl avdisp_draw_model_culled_sort_none
lbl_800647A0:
/* 800647A0 000606C0  80 7E 00 10 */	lwz r3, 0x10(r30)
/* 800647A4 000606C4  28 03 00 00 */	cmplwi r3, 0
/* 800647A8 000606C8  41 82 00 08 */	beq lbl_800647B0
/* 800647AC 000606CC  48 02 9C F1 */	bl avdisp_draw_model_culled_sort_none
lbl_800647B0:
/* 800647B0 000606D0  83 FE 00 14 */	lwz r31, 0x14(r30)
/* 800647B4 000606D4  28 1F 00 00 */	cmplwi r31, 0
/* 800647B8 000606D8  41 82 04 5C */	beq lbl_80064C14
/* 800647BC 000606DC  3C 60 80 1F */	lis r3, polyDisp@ha
/* 800647C0 000606E0  80 03 EC 90 */	lwz r0, polyDisp@l(r3)
/* 800647C4 000606E4  54 00 07 7B */	rlwinm. r0, r0, 0, 0x1d, 0x1d
/* 800647C8 000606E8  40 82 04 4C */	bne lbl_80064C14
/* 800647CC 000606EC  83 BE 00 E0 */	lwz r29, 0xe0(r30)
/* 800647D0 000606F0  28 1D 00 00 */	cmplwi r29, 0
/* 800647D4 000606F4  41 82 03 F8 */	beq lbl_80064BCC
/* 800647D8 000606F8  80 8D 9A 5C */	lwz r4, currentCamera  //@sda21(r13)
/* 800647DC 000606FC  3F 80 43 30 */	lis r28, 0x4330
/* 800647E0 00060700  C8 22 9D B8 */	lfd f1, 4503601774854144.0
/* 800647E4 00060704  38 61 00 50 */	addi r3, r1, 0x50
/* 800647E8 00060708  A8 04 00 30 */	lha r0, 0x30(r4)
/* 800647EC 0006070C  C0 A2 9D C0 */	lfs f5, 0.0054931640625f
/* 800647F0 00060710  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 800647F4 00060714  C0 44 00 34 */	lfs f2, 0x34(r4)
/* 800647F8 00060718  90 01 01 04 */	stw r0, 0x104(r1)
/* 800647FC 0006071C  C0 62 9D 98 */	lfs f3, 0.1f
/* 80064800 00060720  93 81 01 00 */	stw r28, 0x100(r1)
/* 80064804 00060724  C0 82 9D C4 */	lfs f4, 20000.0f
/* 80064808 00060728  C8 01 01 00 */	lfd f0, 0x100(r1)
/* 8006480C 0006072C  EC 00 08 28 */	fsubs f0, f0, f1
/* 80064810 00060730  EC 25 00 32 */	fmuls f1, f5, f0
/* 80064814 00060734  48 06 5E DD */	bl MTXPerspective
/* 80064818 00060738  4B FA 2D 4D */	bl mathutil_mtxA_from_identity
/* 8006481C 0006073C  80 6D 9A 5C */	lwz r3, currentCamera  //@sda21(r13)
/* 80064820 00060740  C0 21 00 50 */	lfs f1, 0x50(r1)
/* 80064824 00060744  C0 03 00 28 */	lfs f0, 0x28(r3)
/* 80064828 00060748  C0 43 00 34 */	lfs f2, 0x34(r3)
/* 8006482C 0006074C  EC 01 00 32 */	fmuls f0, f1, f0
/* 80064830 00060750  C0 23 00 38 */	lfs f1, 0x38(r3)
/* 80064834 00060754  C8 62 9D C8 */	lfd f3, 0.5
/* 80064838 00060758  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 8006483C 0006075C  EC 02 00 32 */	fmuls f0, f2, f0
/* 80064840 00060760  EC 01 00 32 */	fmuls f0, f1, f0
/* 80064844 00060764  FC 03 00 32 */	fmul f0, f3, f0
/* 80064848 00060768  FC 03 00 28 */	fsub f0, f3, f0
/* 8006484C 0006076C  FC 00 00 18 */	frsp f0, f0
/* 80064850 00060770  D0 03 00 08 */	stfs f0, 8(r3)
/* 80064854 00060774  80 6D 9A 5C */	lwz r3, currentCamera  //@sda21(r13)
/* 80064858 00060778  C0 21 00 64 */	lfs f1, 0x64(r1)
/* 8006485C 0006077C  C0 03 00 2C */	lfs f0, 0x2c(r3)
/* 80064860 00060780  C0 43 00 38 */	lfs f2, 0x38(r3)
/* 80064864 00060784  EC 01 00 32 */	fmuls f0, f1, f0
/* 80064868 00060788  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 8006486C 0006078C  EC 02 00 32 */	fmuls f0, f2, f0
/* 80064870 00060790  FC 03 00 32 */	fmul f0, f3, f0
/* 80064874 00060794  FC 03 00 28 */	fsub f0, f3, f0
/* 80064878 00060798  FC 00 00 18 */	frsp f0, f0
/* 8006487C 0006079C  D0 03 00 18 */	stfs f0, 0x18(r3)
/* 80064880 000607A0  80 6D 9A 5C */	lwz r3, currentCamera  //@sda21(r13)
/* 80064884 000607A4  C8 22 9D B8 */	lfd f1, 4503601774854144.0
/* 80064888 000607A8  A8 03 00 30 */	lha r0, 0x30(r3)
/* 8006488C 000607AC  6C 00 80 00 */	xoris r0, r0, 0x8000
/* 80064890 000607B0  90 01 00 FC */	stw r0, 0xfc(r1)
/* 80064894 000607B4  93 81 00 F8 */	stw r28, 0xf8(r1)
/* 80064898 000607B8  C8 01 00 F8 */	lfd f0, 0xf8(r1)
/* 8006489C 000607BC  FC 00 08 28 */	fsub f0, f0, f1
/* 800648A0 000607C0  FC 03 00 32 */	fmul f0, f3, f0
/* 800648A4 000607C4  FC 00 00 1E */	fctiwz f0, f0
/* 800648A8 000607C8  D8 01 00 F0 */	stfd f0, 0xf0(r1)
/* 800648AC 000607CC  80 61 00 F4 */	lwz r3, 0xf4(r1)
/* 800648B0 000607D0  4B FA 29 C5 */	bl mathutil_tan
/* 800648B4 000607D4  C0 02 9D D0 */	lfs f0, 0.5f
/* 800648B8 000607D8  38 61 00 90 */	addi r3, r1, 0x90
/* 800648BC 000607DC  80 AD 9A 5C */	lwz r5, currentCamera  //@sda21(r13)
/* 800648C0 000607E0  EC 20 08 24 */	fdivs f1, f0, f1
/* 800648C4 000607E4  80 8D 99 80 */	lwz r4, mathutilData  //@sda21(r13)
/* 800648C8 000607E8  C0 05 00 34 */	lfs f0, 0x34(r5)
/* 800648CC 000607EC  EC 01 00 24 */	fdivs f0, f1, f0
/* 800648D0 000607F0  D0 04 00 00 */	stfs f0, 0(r4)
/* 800648D4 000607F4  80 8D 99 80 */	lwz r4, mathutilData  //@sda21(r13)
/* 800648D8 000607F8  D0 24 00 14 */	stfs f1, 0x14(r4)
/* 800648DC 000607FC  4B FA 2F 9D */	bl mathutil_mtxA_to_mtx
/* 800648E0 00060800  38 61 00 90 */	addi r3, r1, 0x90
/* 800648E4 00060804  4B FA 2F CD */	bl mathutil_mtxA_from_mtx
/* 800648E8 00060808  38 61 00 C0 */	addi r3, r1, 0xc0
/* 800648EC 0006080C  4B FA 2F 8D */	bl mathutil_mtxA_to_mtx
/* 800648F0 00060810  4B FA 30 BD */	bl mathutil_mtxA_from_mtxB
/* 800648F4 00060814  38 7D 00 50 */	addi r3, r29, 0x50
/* 800648F8 00060818  4B FA 33 49 */	bl mathutil_mtxA_mult_right
/* 800648FC 0006081C  38 61 00 C0 */	addi r3, r1, 0xc0
/* 80064900 00060820  4B FA 33 55 */	bl mathutil_mtxA_mult_left
/* 80064904 00060824  38 7E 00 50 */	addi r3, r30, 0x50
/* 80064908 00060828  4B FA 2F 71 */	bl mathutil_mtxA_to_mtx
/* 8006490C 0006082C  38 61 00 C0 */	addi r3, r1, 0xc0
/* 80064910 00060830  4B FA 2F A1 */	bl mathutil_mtxA_from_mtx
/* 80064914 00060834  C0 42 9D 9C */	lfs f2, 1.0f
/* 80064918 00060838  C0 22 9D D4 */	lfs f1, neg1
/* 8006491C 0006083C  FC 60 10 90 */	fmr f3, f2
/* 80064920 00060840  4B FA 34 B9 */	bl mathutil_mtxA_scale_xyz
/* 80064924 00060844  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 80064928 00060848  38 63 00 30 */	addi r3, r3, 0x30
/* 8006492C 0006084C  4B FA 33 15 */	bl mathutil_mtxA_mult_right
/* 80064930 00060850  38 7D 00 50 */	addi r3, r29, 0x50
/* 80064934 00060854  4B FA 33 0D */	bl mathutil_mtxA_mult_right
/* 80064938 00060858  38 7E 00 80 */	addi r3, r30, 0x80
/* 8006493C 0006085C  4B FA 2F 3D */	bl mathutil_mtxA_to_mtx
/* 80064940 00060860  38 7D 00 50 */	addi r3, r29, 0x50
/* 80064944 00060864  4B FA 2F 6D */	bl mathutil_mtxA_from_mtx
/* 80064948 00060868  C0 22 9D 8C */	lfs f1, 0.0f
/* 8006494C 0006086C  38 61 00 14 */	addi r3, r1, 0x14
/* 80064950 00060870  FC 40 08 90 */	fmr f2, f1
/* 80064954 00060874  FC 60 08 90 */	fmr f3, f1
/* 80064958 00060878  4B FA 34 FD */	bl mathutil_mtxA_tf_point_xyz
/* 8006495C 0006087C  C0 22 9D 8C */	lfs f1, 0.0f
/* 80064960 00060880  38 61 00 20 */	addi r3, r1, 0x20
/* 80064964 00060884  C0 42 9D 9C */	lfs f2, 1.0f
/* 80064968 00060888  FC 60 08 90 */	fmr f3, f1
/* 8006496C 0006088C  4B FA 35 01 */	bl mathutil_mtxA_tf_vec_xyz
/* 80064970 00060890  38 60 68 00 */	li r3, 0x6800
/* 80064974 00060894  4B FA 2C E5 */	bl mathutil_mtxA_from_rotate_y
/* 80064978 00060898  3C 60 80 1F */	lis r3, g_bgLightInfo@ha
/* 8006497C 0006089C  38 63 06 14 */	addi r3, r3, g_bgLightInfo@l
/* 80064980 000608A0  A8 63 00 40 */	lha r3, 0x40(r3)
/* 80064984 000608A4  4B FA 36 05 */	bl mathutil_mtxA_rotate_x
/* 80064988 000608A8  C0 22 9D 8C */	lfs f1, 0.0f
/* 8006498C 000608AC  38 61 00 44 */	addi r3, r1, 0x44
/* 80064990 000608B0  C0 62 9D 9C */	lfs f3, 1.0f
/* 80064994 000608B4  FC 40 08 90 */	fmr f2, f1
/* 80064998 000608B8  4B FA 34 D5 */	bl mathutil_mtxA_tf_vec_xyz
/* 8006499C 000608BC  3B 81 00 20 */	addi r28, r1, 0x20
/* 800649A0 000608C0  C0 A1 00 20 */	lfs f5, 0x20(r1)
/* 800649A4 000608C4  C0 41 00 44 */	lfs f2, 0x44(r1)
/* 800649A8 000608C8  C0 81 00 24 */	lfs f4, 0x24(r1)
/* 800649AC 000608CC  C0 21 00 48 */	lfs f1, 0x48(r1)
/* 800649B0 000608D0  C0 61 00 28 */	lfs f3, 0x28(r1)
/* 800649B4 000608D4  C0 01 00 4C */	lfs f0, 0x4c(r1)
/* 800649B8 000608D8  EC A5 00 B2 */	fmuls f5, f5, f2
/* 800649BC 000608DC  EC A4 28 7A */	fmadds f5, f4, f1, f5
/* 800649C0 000608E0  EC A3 28 3A */	fmadds f5, f3, f0, f5
/* 800649C4 000608E4  C0 02 9D D8 */	lfs f0, neg2
/* 800649C8 000608E8  C0 61 00 20 */	lfs f3, 0x20(r1)
/* 800649CC 000608EC  C0 41 00 24 */	lfs f2, 0x24(r1)
/* 800649D0 000608F0  EC 20 01 72 */	fmuls f1, f0, f5
/* 800649D4 000608F4  C0 01 00 28 */	lfs f0, 0x28(r1)
/* 800649D8 000608F8  C0 C1 00 44 */	lfs f6, 0x44(r1)
/* 800649DC 000608FC  EC A1 00 F2 */	fmuls f5, f1, f3
/* 800649E0 00060900  C0 81 00 48 */	lfs f4, 0x48(r1)
/* 800649E4 00060904  EC 61 00 B2 */	fmuls f3, f1, f2
/* 800649E8 00060908  C0 41 00 4C */	lfs f2, 0x4c(r1)
/* 800649EC 0006090C  EC 01 00 32 */	fmuls f0, f1, f0
/* 800649F0 00060910  EC A6 28 2A */	fadds f5, f6, f5
/* 800649F4 00060914  EC 24 18 2A */	fadds f1, f4, f3
/* 800649F8 00060918  EC 02 00 2A */	fadds f0, f2, f0
/* 800649FC 0006091C  D0 A1 00 38 */	stfs f5, 0x38(r1)
/* 80064A00 00060920  D0 21 00 3C */	stfs f1, 0x3c(r1)
/* 80064A04 00060924  D0 01 00 40 */	stfs f0, 0x40(r1)
/* 80064A08 00060928  C0 01 00 38 */	lfs f0, 0x38(r1)
/* 80064A0C 0006092C  FC 00 00 50 */	fneg f0, f0
/* 80064A10 00060930  D0 01 00 38 */	stfs f0, 0x38(r1)
/* 80064A14 00060934  C0 01 00 3C */	lfs f0, 0x3c(r1)
/* 80064A18 00060938  FC 00 00 50 */	fneg f0, f0
/* 80064A1C 0006093C  D0 01 00 3C */	stfs f0, 0x3c(r1)
/* 80064A20 00060940  C0 01 00 40 */	lfs f0, 0x40(r1)
/* 80064A24 00060944  FC 00 00 50 */	fneg f0, f0
/* 80064A28 00060948  D0 01 00 40 */	stfs f0, 0x40(r1)
/* 80064A2C 0006094C  4B FA 2F 81 */	bl mathutil_mtxA_from_mtxB
/* 80064A30 00060950  38 61 00 2C */	addi r3, r1, 0x2c
/* 80064A34 00060954  4B FA 34 D1 */	bl mathutil_mtxA_rigid_inv_tf_tl
/* 80064A38 00060958  C0 21 00 2C */	lfs f1, 0x2c(r1)
/* 80064A3C 0006095C  3B 61 00 08 */	addi r27, r1, 8
/* 80064A40 00060960  C0 01 00 14 */	lfs f0, 0x14(r1)
/* 80064A44 00060964  C0 61 00 30 */	lfs f3, 0x30(r1)
/* 80064A48 00060968  EC 81 00 28 */	fsubs f4, f1, f0
/* 80064A4C 0006096C  C0 41 00 18 */	lfs f2, 0x18(r1)
/* 80064A50 00060970  C0 21 00 34 */	lfs f1, 0x34(r1)
/* 80064A54 00060974  C0 01 00 1C */	lfs f0, 0x1c(r1)
/* 80064A58 00060978  EC 43 10 28 */	fsubs f2, f3, f2
/* 80064A5C 0006097C  D0 81 00 08 */	stfs f4, 8(r1)
/* 80064A60 00060980  EC 01 00 28 */	fsubs f0, f1, f0
/* 80064A64 00060984  D0 41 00 0C */	stfs f2, 0xc(r1)
/* 80064A68 00060988  D0 01 00 10 */	stfs f0, 0x10(r1)
/* 80064A6C 0006098C  C0 A1 00 08 */	lfs f5, 8(r1)
/* 80064A70 00060990  C0 41 00 20 */	lfs f2, 0x20(r1)
/* 80064A74 00060994  C0 81 00 0C */	lfs f4, 0xc(r1)
/* 80064A78 00060998  C0 21 00 24 */	lfs f1, 0x24(r1)
/* 80064A7C 0006099C  C0 61 00 10 */	lfs f3, 0x10(r1)
/* 80064A80 000609A0  C0 01 00 28 */	lfs f0, 0x28(r1)
/* 80064A84 000609A4  EC C5 00 B2 */	fmuls f6, f5, f2
/* 80064A88 000609A8  EC C4 30 7A */	fmadds f6, f4, f1, f6
/* 80064A8C 000609AC  EC C3 30 3A */	fmadds f6, f3, f0, f6
/* 80064A90 000609B0  38 61 00 38 */	addi r3, r1, 0x38
/* 80064A94 000609B4  C0 A1 00 38 */	lfs f5, 0x38(r1)
/* 80064A98 000609B8  C0 81 00 3C */	lfs f4, 0x3c(r1)
/* 80064A9C 000609BC  C0 61 00 40 */	lfs f3, 0x40(r1)
/* 80064AA0 000609C0  EC 45 00 B2 */	fmuls f2, f5, f2
/* 80064AA4 000609C4  EC 44 10 7A */	fmadds f2, f4, f1, f2
/* 80064AA8 000609C8  EC 43 10 3A */	fmadds f2, f3, f0, f2
/* 80064AAC 000609CC  7F 64 DB 78 */	mr r4, r27
/* 80064AB0 000609D0  EF E6 00 B2 */	fmuls f31, f6, f2
/* 80064AB4 000609D4  FC 20 F8 90 */	fmr f1, f31
/* 80064AB8 000609D8  4B FA 36 BD */	bl mathutil_vec_set_len
/* 80064ABC 000609DC  C0 21 00 2C */	lfs f1, 0x2c(r1)
/* 80064AC0 000609E0  7F 85 E3 78 */	mr r5, r28
/* 80064AC4 000609E4  C0 01 00 08 */	lfs f0, 8(r1)
/* 80064AC8 000609E8  7F 66 DB 78 */	mr r6, r27
/* 80064ACC 000609EC  C0 81 00 0C */	lfs f4, 0xc(r1)
/* 80064AD0 000609F0  EC 01 00 2A */	fadds f0, f1, f0
/* 80064AD4 000609F4  C0 61 00 10 */	lfs f3, 0x10(r1)
/* 80064AD8 000609F8  C0 21 00 44 */	lfs f1, 0x44(r1)
/* 80064ADC 000609FC  38 61 00 C0 */	addi r3, r1, 0xc0
/* 80064AE0 00060A00  C0 41 00 48 */	lfs f2, 0x48(r1)
/* 80064AE4 00060A04  D0 01 00 2C */	stfs f0, 0x2c(r1)
/* 80064AE8 00060A08  38 81 00 2C */	addi r4, r1, 0x2c
/* 80064AEC 00060A0C  C0 01 00 4C */	lfs f0, 0x4c(r1)
/* 80064AF0 00060A10  C0 A1 00 30 */	lfs f5, 0x30(r1)
/* 80064AF4 00060A14  EC 85 20 2A */	fadds f4, f5, f4
/* 80064AF8 00060A18  D0 81 00 30 */	stfs f4, 0x30(r1)
/* 80064AFC 00060A1C  C0 81 00 34 */	lfs f4, 0x34(r1)
/* 80064B00 00060A20  EC 64 18 2A */	fadds f3, f4, f3
/* 80064B04 00060A24  D0 61 00 34 */	stfs f3, 0x34(r1)
/* 80064B08 00060A28  C0 81 00 2C */	lfs f4, 0x2c(r1)
/* 80064B0C 00060A2C  C0 61 00 30 */	lfs f3, 0x30(r1)
/* 80064B10 00060A30  EC 84 08 2A */	fadds f4, f4, f1
/* 80064B14 00060A34  C0 21 00 34 */	lfs f1, 0x34(r1)
/* 80064B18 00060A38  EC 43 10 2A */	fadds f2, f3, f2
/* 80064B1C 00060A3C  EC 01 00 2A */	fadds f0, f1, f0
/* 80064B20 00060A40  D0 81 00 08 */	stfs f4, 8(r1)
/* 80064B24 00060A44  D0 41 00 0C */	stfs f2, 0xc(r1)
/* 80064B28 00060A48  D0 01 00 10 */	stfs f0, 0x10(r1)
/* 80064B2C 00060A4C  48 06 59 9D */	bl MTXLookAt
/* 80064B30 00060A50  C0 22 9D D0 */	lfs f1, 0.5f
/* 80064B34 00060A54  C0 62 9D 8C */	lfs f3, 0.0f
/* 80064B38 00060A58  FC 40 08 90 */	fmr f2, f1
/* 80064B3C 00060A5C  4B FA 2A 99 */	bl mathutil_mtxA_from_translate_xyz
/* 80064B40 00060A60  C0 02 9D DC */	lfs f0, 4.0f
/* 80064B44 00060A64  C0 22 9D D0 */	lfs f1, 0.5f
/* 80064B48 00060A68  EC 00 07 F2 */	fmuls f0, f0, f31
/* 80064B4C 00060A6C  EC 21 00 24 */	fdivs f1, f1, f0
/* 80064B50 00060A70  4B FA 32 79 */	bl mathutil_mtxA_scale_s
/* 80064B54 00060A74  38 61 00 C0 */	addi r3, r1, 0xc0
/* 80064B58 00060A78  4B FA 30 E9 */	bl mathutil_mtxA_mult_right
/* 80064B5C 00060A7C  38 7E 00 B0 */	addi r3, r30, 0xb0
/* 80064B60 00060A80  4B FA 2D 19 */	bl mathutil_mtxA_to_mtx
/* 80064B64 00060A84  4B FA 2E 49 */	bl mathutil_mtxA_from_mtxB
/* 80064B68 00060A88  38 7D 00 50 */	addi r3, r29, 0x50
/* 80064B6C 00060A8C  4B FA 30 D5 */	bl mathutil_mtxA_mult_right
/* 80064B70 00060A90  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 80064B74 00060A94  38 80 00 00 */	li r4, 0
/* 80064B78 00060A98  48 07 FA 09 */	bl GXLoadPosMtxImm
/* 80064B7C 00060A9C  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 80064B80 00060AA0  38 80 00 00 */	li r4, 0
/* 80064B84 00060AA4  48 07 FA 39 */	bl GXLoadNrmMtxImm
/* 80064B88 00060AA8  3C 60 80 06 */	lis r3, func_80064C30@ha
/* 80064B8C 00060AAC  38 63 4C 30 */	addi r3, r3, func_80064C30@l
/* 80064B90 00060AB0  48 02 9A 49 */	bl u_avdisp_set_some_func_1
/* 80064B94 00060AB4  3B 63 00 00 */	addi r27, r3, 0
/* 80064B98 00060AB8  38 60 00 01 */	li r3, 1
/* 80064B9C 00060ABC  38 80 00 03 */	li r4, 3
/* 80064BA0 00060AC0  38 A0 00 00 */	li r5, 0
/* 80064BA4 00060AC4  48 02 9A 25 */	bl avdisp_set_z_mode
/* 80064BA8 00060AC8  80 7F 00 08 */	lwz r3, 8(r31)
/* 80064BAC 00060ACC  48 02 98 F1 */	bl avdisp_draw_model_culled_sort_none
/* 80064BB0 00060AD0  38 60 00 01 */	li r3, 1
/* 80064BB4 00060AD4  38 80 00 03 */	li r4, 3
/* 80064BB8 00060AD8  38 A0 00 01 */	li r5, 1
/* 80064BBC 00060ADC  48 02 9A 0D */	bl avdisp_set_z_mode
/* 80064BC0 00060AE0  7F 63 DB 78 */	mr r3, r27
/* 80064BC4 00060AE4  48 02 9A 15 */	bl u_avdisp_set_some_func_1
/* 80064BC8 00060AE8  48 00 00 4C */	b lbl_80064C14
lbl_80064BCC:
/* 80064BCC 00060AEC  38 7F 00 0C */	addi r3, r31, 0xc
/* 80064BD0 00060AF0  4B FA 2B 0D */	bl mathutil_mtxA_from_mtxB_translate
/* 80064BD4 00060AF4  A8 7F 00 1C */	lha r3, 0x1c(r31)
/* 80064BD8 00060AF8  4B FA 34 B1 */	bl mathutil_mtxA_rotate_z
/* 80064BDC 00060AFC  A8 7F 00 1A */	lha r3, 0x1a(r31)
/* 80064BE0 00060B00  4B FA 34 1D */	bl mathutil_mtxA_rotate_y
/* 80064BE4 00060B04  A8 7F 00 18 */	lha r3, 0x18(r31)
/* 80064BE8 00060B08  4B FA 33 A1 */	bl mathutil_mtxA_rotate_x
/* 80064BEC 00060B0C  38 7F 00 20 */	addi r3, r31, 0x20
/* 80064BF0 00060B10  4B FA 31 C9 */	bl mathutil_mtxA_scale
/* 80064BF4 00060B14  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 80064BF8 00060B18  38 80 00 00 */	li r4, 0
/* 80064BFC 00060B1C  48 07 F9 85 */	bl GXLoadPosMtxImm
/* 80064C00 00060B20  80 6D 99 80 */	lwz r3, mathutilData  //@sda21(r13)
/* 80064C04 00060B24  38 80 00 00 */	li r4, 0
/* 80064C08 00060B28  48 07 F9 B5 */	bl GXLoadNrmMtxImm
/* 80064C0C 00060B2C  80 7F 00 08 */	lwz r3, 8(r31)
/* 80064C10 00060B30  48 02 98 29 */	bl avdisp_draw_model_culled_sort_translucent
lbl_80064C14:
/* 80064C14 00060B34  BB 61 01 0C */	lmw r27, 0x10c(r1)
/* 80064C18 00060B38  80 01 01 2C */	lwz r0, 0x12c(r1)
/* 80064C1C 00060B3C  CB E1 01 20 */	lfd f31, 0x120(r1)
/* 80064C20 00060B40  38 21 01 28 */	addi r1, r1, 0x128
/* 80064C24 00060B44  7C 08 03 A6 */	mtlr r0
/* 80064C28 00060B48  4E 80 00 20 */	blr
