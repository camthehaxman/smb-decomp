#include <dolphin.h>

#include "__gx.h"

static void __GXSetGenMode(void);

void __GXSetDirtyState(void)
{
    if (gx->dirtyState & 1)
        __GXSetSUTexSize();
    if (gx->dirtyState & 2)
        __GXUpdateBPMask();
    if (gx->dirtyState & 4)
        __GXSetGenMode();
    if (gx->dirtyState & 8)
        __GXSetVCD();
    if (gx->dirtyState & 0x10)
        __GXSetVAT();
    gx->dirtyState = 0;
}

void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts)
{
    if (gx->dirtyState != 0)
        __GXSetDirtyState();
    if (*(u32 *)gx != 0)
        __GXSendFlushPrim();
    GX_WRITE_U8(vtxfmt | type);
    GX_WRITE_U16(nverts);
}

void __GXSendFlushPrim(void)
{
    u32 temp_r7 = gx->vNum * gx->unk4;
    int i;

    GX_WRITE_U8(GX_DRAW_TRIANGLE_STRIP);
    GX_WRITE_U16(gx->vNum);
    for (i = 0; i < temp_r7; i += 4)
        GX_WRITE_U32(0);
    gx->unk2 = 0;
}

void GXSetLineWidth(u8 width, GXTexOffset texOffsets)
{
    INSERT_FIELD(gx->lpSize, width,      8,  0);
    INSERT_FIELD(gx->lpSize, texOffsets, 3, 16);
    GX_WRITE_U8(0x61);
    GX_WRITE_U32(gx->lpSize);
    gx->unk2 = 1;
}

void GXSetPointSize(u8 pointSize, GXTexOffset texOffsets)
{
    INSERT_FIELD(gx->lpSize, pointSize,  8,  8);
    INSERT_FIELD(gx->lpSize, texOffsets, 3, 19);
    GX_WRITE_U8(0x61);
    GX_WRITE_U32(gx->lpSize);
    gx->unk2 = 1;
}

void GXEnableTexOffsets(GXTexCoordID coord, GXBool line_enable, GXBool point_enable)
{
    INSERT_FIELD(gx->suTs0[coord], line_enable,  1, 18);
    INSERT_FIELD(gx->suTs0[coord], point_enable, 1, 19);
    GX_WRITE_U8(0x61);
    GX_WRITE_U32(gx->suTs0[coord]);
    gx->unk2 = 1;
}

void GXSetCullMode(GXCullMode mode)
{
    switch (mode)
    {
    case GX_CULL_FRONT:
        mode = GX_CULL_BACK;
        break;
    case GX_CULL_BACK:
        mode = GX_CULL_FRONT;
        break;
    }
    INSERT_FIELD(gx->genMode, mode, 2, 14);
    gx->dirtyState |= 4;
}

void GXSetCoPlanar(GXBool enable)
{
    INSERT_FIELD(gx->genMode, enable, 1, 19);
    GX_WRITE_U8(0x61);
    GX_WRITE_U32(0xFE080000);
    GX_WRITE_U8(0x61);
    GX_WRITE_U32(gx->genMode);
}

static void __GXSetGenMode(void)
{
    GX_WRITE_U8(0x61);
    GX_WRITE_U32(gx->genMode);
    gx->unk2 = 1;
}
