#!/usr/bin/env python3

import sys
import re

GXChannelID = {
    0:"GX_COLOR0",
    1:"GX_COLOR1",
    2:"GX_ALPHA0",
    3:"GX_ALPHA1",
    4:"GX_COLOR0A0",
    5:"GX_COLOR1A1",
    6:"GX_COLOR_ZERO",
    7:"GX_ALPHA_BUMP",
    8:"GX_ALPHA_BUMPN",
    0xFF:"GX_COLOR_NULL",
}

GXTevStageID = {
    0:"GX_TEVSTAGE0",
    1:"GX_TEVSTAGE1",
    2:"GX_TEVSTAGE2",
    3:"GX_TEVSTAGE3",
    4:"GX_TEVSTAGE4",
    5:"GX_TEVSTAGE5",
    6:"GX_TEVSTAGE6",
    7:"GX_TEVSTAGE7",
    8:"GX_TEVSTAGE8",
    9:"GX_TEVSTAGE9",
    10:"GX_TEVSTAGE10",
    11:"GX_TEVSTAGE11",
    12:"GX_TEVSTAGE12",
    13:"GX_TEVSTAGE13",
    14:"GX_TEVSTAGE14",
    15:"GX_TEVSTAGE15",
    16:"GX_MAX_TEVSTAGE",
}

GXTevColorArg = {
    0:"GX_CC_CPREV",
    1:"GX_CC_APREV",
    2:"GX_CC_C0",
    3:"GX_CC_A0",
    4:"GX_CC_C1",
    5:"GX_CC_A1",
    6:"GX_CC_C2",
    7:"GX_CC_A2",
    8:"GX_CC_TEXC",
    9:"GX_CC_TEXA",
    10:"GX_CC_RASC",
    11:"GX_CC_RASA",
    12:"GX_CC_ONE",
    13:"GX_CC_HALF",
    14:"GX_CC_KONST",
    15:"GX_CC_ZERO"
}

GXTevAlphaArg = {
    0:"GX_CA_APREV",
    1:"GX_CA_A0",
    2:"GX_CA_A1",
    3:"GX_CA_A2",
    4:"GX_CA_TEXA",
    5:"GX_CA_RASA",
    6:"GX_CA_KONST",
    7:"GX_CA_ZERO"
}

GXTevKAlphaSel = {
    0x00:"GX_TEV_KASEL_1",
    0x01:"GX_TEV_KASEL_7_8",
    0x02:"GX_TEV_KASEL_3_4",
    0x03:"GX_TEV_KASEL_5_8",
    0x04:"GX_TEV_KASEL_1_2",
    0x05:"GX_TEV_KASEL_3_8",
    0x06:"GX_TEV_KASEL_1_4",
    0x07:"GX_TEV_KASEL_1_8",
    0x10:"GX_TEV_KASEL_K0_R",
    0x11:"GX_TEV_KASEL_K1_R",
    0x12:"GX_TEV_KASEL_K2_R",
    0x13:"GX_TEV_KASEL_K3_R",
    0x14:"GX_TEV_KASEL_K0_G",
    0x15:"GX_TEV_KASEL_K1_G",
    0x16:"GX_TEV_KASEL_K2_G",
    0x17:"GX_TEV_KASEL_K3_G",
    0x18:"GX_TEV_KASEL_K0_B",
    0x19:"GX_TEV_KASEL_K1_B",
    0x1A:"GX_TEV_KASEL_K2_B",
    0x1B:"GX_TEV_KASEL_K3_B",
    0x1C:"GX_TEV_KASEL_K0_A",
    0x1D:"GX_TEV_KASEL_K1_A",
    0x1E:"GX_TEV_KASEL_K2_A",
    0x1F:"GX_TEV_KASEL_K3_A"
}

GXTevOp_rgb = {
    0:"GX_TEV_ADD",
    1:"GX_TEV_SUB",
    8:"GX_TEV_COMP_R8_GT",
    9:"GX_TEV_COMP_R8_EQ",
    10:"GX_TEV_COMP_GR16_GT",
    11:"GX_TEV_COMP_GR16_EQ",
    12:"GX_TEV_COMP_BGR24_GT",
    13:"GX_TEV_COMP_BGR24_EQ",
    14:"GX_TEV_COMP_RGB8_GT",
    15:"GX_TEV_COMP_RGB8_EQ",
}

GXTevOp_alpha = {
    0:"GX_TEV_ADD",
    1:"GX_TEV_SUB",
    8:"GX_TEV_COMP_R8_GT",
    9:"GX_TEV_COMP_R8_EQ",
    10:"GX_TEV_COMP_GR16_GT",
    11:"GX_TEV_COMP_GR16_EQ",
    12:"GX_TEV_COMP_BGR24_GT",
    13:"GX_TEV_COMP_BGR24_EQ",
    14:"GX_TEV_COMP_A8_GT",
    15:"GX_TEV_COMP_A8_EQ",
}

GXTevBias = {
    0:"GX_TB_ZERO",
    1:"GX_TB_ADDHALF",
    2:"GX_TB_SUBHALF",
    3:"GX_MAX_TEVBIAS"
}

GXTevScale = {
    0:"GX_CS_SCALE_1",
    1:"GX_CS_SCALE_2",
    2:"GX_CS_SCALE_4",
    3:"GX_CS_DIVIDE_2",
    4:"GX_MAX_TEVSCALE"
}

GXBool = {
	0:"GX_FALSE",
	1:"GX_TRUE",
}

GXTevRegID = {
    0:"GX_TEVPREV",
    1:"GX_TEVREG0",
    2:"GX_TEVREG1",
    3:"GX_TEVREG2",
    4:"GX_MAX_TEVREG",
}

GXTevSwapSel = {
    0:"GX_TEV_SWAP0",
    1:"GX_TEV_SWAP1",
    2:"GX_TEV_SWAP2",
    3:"GX_TEV_SWAP3",
    4:"GX_MAX_TEVSWAP"
}

GXTevKColorSel = {
    0x00:"GX_TEV_KCSEL_1",
    0x01:"GX_TEV_KCSEL_7_8",
    0x02:"GX_TEV_KCSEL_3_4",
    0x03:"GX_TEV_KCSEL_5_8",
    0x04:"GX_TEV_KCSEL_1_2",
    0x05:"GX_TEV_KCSEL_3_8",
    0x06:"GX_TEV_KCSEL_1_4",
    0x07:"GX_TEV_KCSEL_1_8",
    0x0C:"GX_TEV_KCSEL_K0",
    0x0D:"GX_TEV_KCSEL_K1",
    0x0E:"GX_TEV_KCSEL_K2",
    0x0F:"GX_TEV_KCSEL_K3",
    0x10:"GX_TEV_KCSEL_K0_R",
    0x11:"GX_TEV_KCSEL_K1_R",
    0x12:"GX_TEV_KCSEL_K2_R",
    0x13:"GX_TEV_KCSEL_K3_R",
    0x14:"GX_TEV_KCSEL_K0_G",
    0x15:"GX_TEV_KCSEL_K1_G",
    0x16:"GX_TEV_KCSEL_K2_G",
    0x17:"GX_TEV_KCSEL_K3_G",
    0x18:"GX_TEV_KCSEL_K0_B",
    0x19:"GX_TEV_KCSEL_K1_B",
    0x1A:"GX_TEV_KCSEL_K2_B",
    0x1B:"GX_TEV_KCSEL_K3_B",
    0x1C:"GX_TEV_KCSEL_K0_A",
    0x1D:"GX_TEV_KCSEL_K1_A",
    0x1E:"GX_TEV_KCSEL_K2_A",
    0x1F:"GX_TEV_KCSEL_K3_A"
}

GXIndTexMtxID = {
    0:"GX_ITM_OFF",
    1:"GX_ITM_0",
    2:"GX_ITM_1",
    3:"GX_ITM_2",
    5:"GX_ITM_S0",
    6:"GX_ITM_S1",
    7:"GX_ITM_S2",
    9:"GX_ITM_T0",
    10:"GX_ITM_T1",
    11:"GX_ITM_T2",
}

GXIndTexStageID = {
    0:"GX_INDTEXSTAGE0",
    1:"GX_INDTEXSTAGE1",
    2:"GX_INDTEXSTAGE2",
    3:"GX_INDTEXSTAGE3",
    4:"GX_MAX_INDTEXSTAGE",
}

GXIndTexFormat = {
    0:"GX_ITF_8",
    1:"GX_ITF_5",
    2:"GX_ITF_4",
    3:"GX_ITF_3",
    4:"GX_MAX_ITFORMAT",
}

GXIndTexBiasSel = {
    0:"GX_ITB_NONE",
    1:"GX_ITB_S",
    2:"GX_ITB_T",
    3:"GX_ITB_ST",
    4:"GX_ITB_U",
    5:"GX_ITB_SU",
    6:"GX_ITB_TU",
    7:"GX_ITB_STU",
    8:"GX_MAX_ITBIAS",
}

GXIndTexWrap = {
    0:"GX_ITW_OFF",
    1:"GX_ITW_256",
    2:"GX_ITW_128",
    3:"GX_ITW_64",
    4:"GX_ITW_32",
    5:"GX_ITW_16",
    6:"GX_ITW_0",
    7:"GX_MAX_ITWRAP",
}

GXIndTexAlphaSel = {
    0:"GX_ITBA_OFF",
    1:"GX_ITBA_S",
    2:"GX_ITBA_T",
    3:"GX_ITBA_U",
    4:"GX_MAX_ITBALPHA",
}

GXTexCoordID = {
    0:"GX_TEXCOORD0",
    1:"GX_TEXCOORD1",
    2:"GX_TEXCOORD2",
    3:"GX_TEXCOORD3",
    4:"GX_TEXCOORD4",
    5:"GX_TEXCOORD5",
    6:"GX_TEXCOORD6",
    7:"GX_TEXCOORD7",
    8:"GX_MAX_TEXCOORD",
    0xFF:"GX_TEXCOORD_NULL",
}

GXTexMapID = {
    0:"GX_TEXMAP0",
    1:"GX_TEXMAP1",
    2:"GX_TEXMAP2",
    3:"GX_TEXMAP3",
    4:"GX_TEXMAP4",
    5:"GX_TEXMAP5",
    6:"GX_TEXMAP6",
    7:"GX_TEXMAP7",
    8:"GX_MAX_TEXMAP",
    0xFF:"GX_TEXMAP_NULL",
    0x100:"GX_TEX_DISABLE",
}

GXPosNrmMtx = {
    0:"GX_PNMTX0",
    3:"GX_PNMTX1",
    6:"GX_PNMTX2",
    9:"GX_PNMTX3",
    12:"GX_PNMTX4",
    15:"GX_PNMTX5",
    18:"GX_PNMTX6",
    21:"GX_PNMTX7",
    24:"GX_PNMTX8",
    27:"GX_PNMTX9",
}

GXProjectionType = {
    0:"GX_PERSPECTIVE",
    1:"GX_ORTHOGRAPHIC",
}

GXTexMtx = {
    30:"GX_TEXMTX0",
    33:"GX_TEXMTX1",
    36:"GX_TEXMTX2",
    39:"GX_TEXMTX3",
    42:"GX_TEXMTX4",
    45:"GX_TEXMTX5",
    48:"GX_TEXMTX6",
    51:"GX_TEXMTX7",
    54:"GX_TEXMTX8",
    57:"GX_TEXMTX9",
    60:"GX_IDENTITY",
}

GXTexMtxType = {
    0:"GX_MTX3x4",
    1:"GX_MTX2x4",
}

GXPTTexMtx = {
    64:"GX_PTTEXMTX0",
    67:"GX_PTTEXMTX1",
    70:"GX_PTTEXMTX2",
    73:"GX_PTTEXMTX3",
    76:"GX_PTTEXMTX4",
    79:"GX_PTTEXMTX5",
    82:"GX_PTTEXMTX6",
    85:"GX_PTTEXMTX7",
    88:"GX_PTTEXMTX8",
    91:"GX_PTTEXMTX9",
    94:"GX_PTTEXMTX10",
    97:"GX_PTTEXMTX11",
    100:"GX_PTTEXMTX12",
    103:"GX_PTTEXMTX13",
    106:"GX_PTTEXMTX14",
    109:"GX_PTTEXMTX15",
    112:"GX_PTTEXMTX16",
    115:"GX_PTTEXMTX17",
    118:"GX_PTTEXMTX18",
    121:"GX_PTTEXMTX19",
    125:"GX_PTIDENTITY",
}

AnyMtx = GXPosNrmMtx | GXTexMtx | GXPTTexMtx

GXTexGenType = {
    0:"GX_TG_MTX3x4",
    1:"GX_TG_MTX2x4",
    2:"GX_TG_BUMP0",
    3:"GX_TG_BUMP1",
    4:"GX_TG_BUMP2",
    5:"GX_TG_BUMP3",
    6:"GX_TG_BUMP4",
    7:"GX_TG_BUMP5",
    8:"GX_TG_BUMP6",
    9:"GX_TG_BUMP7",
    10:"GX_TG_SRTG",
}

GXTexGenSrc = {
    0:"GX_TG_POS",
    1:"GX_TG_NRM",
    2:"GX_TG_BINRM",
    3:"GX_TG_TANGENT",
    4:"GX_TG_TEX0",
    5:"GX_TG_TEX1",
    6:"GX_TG_TEX2",
    7:"GX_TG_TEX3",
    8:"GX_TG_TEX4",
    9:"GX_TG_TEX5",
    10:"GX_TG_TEX6",
    11:"GX_TG_TEX7",
    12:"GX_TG_TEXCOORD0",
    13:"GX_TG_TEXCOORD1",
    14:"GX_TG_TEXCOORD2",
    15:"GX_TG_TEXCOORD3",
    16:"GX_TG_TEXCOORD4",
    17:"GX_TG_TEXCOORD5",
    18:"GX_TG_TEXCOORD6",
    19:"GX_TG_COLOR0",
    20:"GX_TG_COLOR1",
}

GXEnable = {
	0:"GX_DISABLE",
	1:"GX_ENABLE",
}

GXCompare = {
    0:"GX_NEVER",
    1:"GX_LESS",
    2:"GX_EQUAL",
    3:"GX_LEQUAL",
    4:"GX_GREATER",
    5:"GX_NEQUAL",
    6:"GX_GEQUAL",
    7:"GX_ALWAYS",
}

GXBlendMode = {
    0:"GX_BM_NONE",
    1:"GX_BM_BLEND",
    2:"GX_BM_LOGIC",
    3:"GX_BM_SUBTRACT",
    4:"GX_MAX_BLENDMODE",
}

GXBlendFactor_src = {
    0:"GX_BL_ZERO",
    1:"GX_BL_ONE",
    2:"GX_BL_SRCCLR",
    3:"GX_BL_INVSRCCLR",
    4:"GX_BL_SRCALPHA",
    5:"GX_BL_INVSRCALPHA",
    5:"GX_BL_DSTALPHA",
    6:"GX_BL_INVDSTALPHA",
}

GXBlendFactor_dst = {
    0:"GX_BL_ZERO",
    1:"GX_BL_ONE",
    2:"GX_BL_DSTCLR",
    3:"GX_BL_INVDSTCLR",
    4:"GX_BL_SRCALPHA",
    5:"GX_BL_INVSRCALPHA",
    5:"GX_BL_DSTALPHA",
    6:"GX_BL_INVDSTALPHA",
}

GXLogicOp = {
    0:"GX_LO_CLEAR",
    1:"GX_LO_AND",
    2:"GX_LO_REVAND",
    3:"GX_LO_COPY",
    4:"GX_LO_INVAND",
    5:"GX_LO_NOOP",
    6:"GX_LO_XOR",
    7:"GX_LO_OR",
    8:"GX_LO_NOR",
    9:"GX_LO_EQUIV",
    10:"GX_LO_INV",
    11:"GX_LO_REVOR",
    12:"GX_LO_INVCOPY",
    13:"GX_LO_INVOR",
    14:"GX_LO_NAND",
    15:"GX_LO_SET",
}

GXTexFilter = {
    0:"GX_NEAR",
    1:"GX_LINEAR",
    2:"GX_NEAR_MIP_NEAR",
    3:"GX_LIN_MIP_NEAR",
    4:"GX_NEAR_MIP_LIN",
    5:"GX_LIN_MIP_LIN",
}

GXAnisotropy = {
    0:"GX_ANISO_1",
    1:"GX_ANISO_2",
    2:"GX_ANISO_4",
    3:"GX_MAX_ANISOTROPY",
}

funcs = {
	'GXSetTevColorIn_cached':[GXTevStageID, GXTevColorArg, GXTevColorArg, GXTevColorArg, GXTevColorArg],
	'GXSetTevColorOp_cached':[GXTevStageID, GXTevOp_rgb, GXTevBias, GXTevScale, GXBool, GXTevRegID],
	'GXSetTevKAlphaSel_cached':[GXTevStageID, GXTevKAlphaSel],
	'GXSetTevAlphaIn_cached':[GXTevStageID, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg],
	'GXSetTevAlphaOp_cached':[GXTevStageID, GXTevOp_alpha, GXTevBias, GXTevScale, GXBool, GXTevRegID],
	'GXSetTevSwapMode_cached':[GXTevStageID, GXTevSwapSel, GXTevSwapSel],
	'GXSetTevKColorSel_cached':[GXTevStageID, GXTevKColorSel],
	'GXSetIndTexMtx':[GXIndTexMtxID, None, None],
	'GXSetTevIndirect':[GXTevStageID, GXIndTexStageID, GXIndTexFormat, GXIndTexBiasSel, GXIndTexMtxID, GXIndTexWrap, GXIndTexWrap, GXBool, GXBool, GXIndTexAlphaSel],
	'GXSetTevOrder_cached':[GXTevStageID, GXTexCoordID, GXTexMapID, GXChannelID],
	'GXLoadPosMtxImm':[None, GXPosNrmMtx],
	'GXLoadNrmMtxImm':[None, GXPosNrmMtx],
	'GXLoadTexMtxImm':[None, GXTexMtx, GXTexMtxType],
	'GXSetProjection':[None, GXProjectionType],
	'GXSetTexCoordGen2':[GXTexCoordID, GXTexGenType, GXTexGenSrc, AnyMtx, GXBool, AnyMtx],
	'avdisp_set_z_mode':[GXEnable, GXCompare, GXEnable],
	'gxutil_set_line_blend_params':[GXBlendMode, GXBlendFactor_src, GXBlendFactor_dst, GXLogicOp],
	'GXSetZMode_cached':[GXEnable, GXCompare, GXEnable],
	'GXInitTexObjLOD':[None, GXTexFilter, GXTexFilter, None, None, None, GXBool, GXBool, GXAnisotropy],
	'GXGetTexBufferSize':[None, None, None, GXBool, None],
	'GXLoadTexObj_cached':[None, GXTexMapID],
	'GXCopyTex':[None, GXBool],
}

def handle_func(name, argstr):
	if name in funcs:
		types = funcs[name]
		args = argstr.split(',')
		newArgs = []
		i = 0
		for arg in args:
			arg = arg.strip()
			# strip out U suffix
			m = re.match('(\\d+)U', arg)
			if m:
				arg = m.group(1)
			try:
				if types[i]:
					arg = types[i][int(arg, 0)]
			except ValueError:
				pass
			newArgs.append(arg)
			i += 1
		newstr = name + '(' + ', '.join(newArgs) + ');'
		if newstr != argstr:
			return newstr
	return None

modified = False

def search_func(line):
	expr = '([^( ]*)\\((.*)\\);'
	m = re.search(expr, line)
	if m:
		newArgs = []
		name = m.group(1)
		args = m.group(2)
		newstr = handle_func(name, args)
		if newstr:
			modified = True
			line = line[:m.start(0)] + newstr + line[m.end(0):]
	return line

out = ''
with open(sys.argv[1], "r") as f:
	for line in f:
		out += search_func(line)

modified = True
if modified:
	with open(sys.argv[1], "w") as f:
		f.write(out)
