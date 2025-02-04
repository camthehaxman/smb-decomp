#include <dolphin.h>

#include "global.h"
#include "avdisp.h"
#include "background.h"
#include "gxcache.h"
#include "gxutil.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "ord_tbl.h"

struct FogParams
{
    s8 bgId;
    u8 unk1;
    u8 filler2;
    float unk4;  // start
    float unk8;  // end
    GXColor unkC;
};

void gxutil_begin_display_list(void *buffer, u32 size)
{
    GXBeginDisplayList(buffer, size);
}

u32 gxutil_end_display_list(void)
{
    return GXEndDisplayList();
}

u32 vtxAttrs;

void gxutil_clear_vtx_attrs(void)
{
    vtxAttrs = 0;
}

#pragma dont_inline on
void gxutil_set_vtx_attrs(u32 attrs)
{
    u32 i;

    if (attrs != vtxAttrs)
    {
        GXClearVtxDesc();
        vtxAttrs = attrs;
        for (i = 0; i <= 16; i++, attrs >>= 1)
        {
            if (attrs & 1)
                GXSetVtxDesc(i, GX_DIRECT);
        }
    }
}
#pragma dont_inline reset

void gxutil_dummy(void) {}

#ifdef C_ONLY
void gxutil_load_pos_nrm_matrix(register Mtx mtx, register int index)
{
    GXLoadPosMtxImm(mtx, index * 3);
    GXLoadNrmMtxImm(mtx, index * 3);
}
#else
asm void gxutil_load_pos_nrm_matrix(register Mtx mtx, register int index)
{
    nofralloc
    mr r5, r4
    add r4, r4, r4  // r4 = index * 2
    add r4, r4, r5  // r4 = index * 3
    li r7, 0x10
    add r5, r4, r4  // r5 = index * 6
    lis r6, GXFIFO_ADDR@h
    add r4, r5, r4  // r4 = index * 9
    ori r6, r6, GXFIFO_ADDR@l
    add r5, r5, r5   // r5 = index * 12
    addis r5, r5, 0xb
    stb r7, 0(r6)
    stw r5, 0(r6)

    // Copy matrix into FIFO
    psq_l f0, 0(mtx), 0, qr0
    psq_l f1, 8(mtx), 0, qr0
    psq_l f2, 16(mtx), 0, qr0
    psq_l f3, 24(mtx), 0, qr0
    psq_l f4, 32(mtx), 0, qr0
    psq_l f5, 40(mtx), 0, qr0
    psq_st f0, 0(r6), 0, qr0
    psq_st f1, 0(r6), 0, qr0
    psq_st f2, 0(r6), 0, qr0
    psq_st f3, 0(r6), 0, qr0
    psq_st f4, 0(r6), 0, qr0
    psq_st f5, 0(r6), 0, qr0
    addis r4, r4, 8
    addi r4, r4, 0x400
    stb r7, 0(r6)
    stw r4, 0(r6)
    psq_st f0, 0(r6), 0, qr0
    psq_st f1, 0(r6), 1, qr0
    psq_st f2, 0(r6), 0, qr0
    psq_st f3, 0(r6), 1, qr0
    psq_st f4, 0(r6), 0, qr0
    psq_st f5, 0(r6), 1, qr0
    blr
}
#endif

#pragma peephole on  // above function isn't pure asm?

const struct FogParams bgFogParamsTable[] =
{
    { BG_TYPE_ICE2, 5, 0, 150.0f, 1880.0f, { 236, 250, 255, 0 } },
    { -1,           0, 0,   0.0f,    0.0f, { 0 } },
};

struct FogInfo fogInfo;

void u_init_bg_fog_params(void)
{
    const struct FogParams *params = bgFogParamsTable;

    while (params->bgId != -1)
    {
        if (params->bgId == backgroundInfo.bgId)
        {
            fogInfo.enabled = 1;
            fogInfo.unk0 = params->unk1;
            fogInfo.unk4 = params->unk4;
            fogInfo.unk8 = params->unk8;
            fogInfo.r = params->unkC.r;
            fogInfo.g = params->unkC.g;
            fogInfo.b = params->unkC.b;
            return;
        }
        params++;
    }

    fogInfo.enabled = 0;
    fogInfo.unk0 = 5;
    fogInfo.unk4 = 0.0f;
    fogInfo.unk8 = 100.0f;
    fogInfo.r = 0;
    fogInfo.g = 0;
    fogInfo.b = 0;
}

void fog_main(void)
{
    avdisp_enable_fog(fogInfo.enabled);
    nlSetFog(fogInfo.enabled);
    if (fogInfo.enabled != 0)
    {
        avdisp_set_fog_params(fogInfo.unk0, fogInfo.unk4, fogInfo.unk8);
        avdisp_set_fog_color(fogInfo.r, fogInfo.g, fogInfo.b);
        nlSetFogType(fogInfo.unk0, fogInfo.unk4, fogInfo.unk8);
        nlSetFogColor(fogInfo.r, fogInfo.g, fogInfo.b);
    }
}

void u_gxutil_set_fog_enabled(s8 enable)
{
    avdisp_enable_fog(enable);
    nlSetFog(enable);
}

void u_gxutil_fog_something_2(void)
{
    avdisp_enable_fog(fogInfo.enabled);
    nlSetFog(fogInfo.enabled);
}

void fog_gx_set(void)
{
    GXColor sp10;

    sp10.r = fogInfo.r;
    sp10.g = fogInfo.g;
    sp10.b = fogInfo.b;
    if (fogInfo.enabled)
        GXSetFog_cached(fogInfo.unk0, fogInfo.unk4, fogInfo.unk8, 0.1f, 20000.0f, sp10);
    else
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, sp10);
}

struct LineInfo
{
    u8 lineWidth;
    GXBlendMode blendMode;
    GXBlendFactor blendSrcFactor;
    GXBlendFactor blendDstFactor;
    GXLogicOp blendLogicOp;
    GXTexOffset texOffset;
    u8 filler18[4];
};

struct LineInfo lineInfo = { 6, 0, 1, 0, 0, 0 };

void gxutil_set_line_width(int width)
{
    lineInfo.lineWidth = width;
}

void gxutil_set_line_blend_params(GXBlendMode mode, GXBlendFactor srcFactor, GXBlendFactor dstFactor, GXLogicOp logicOp)
{
    lineInfo.blendMode = mode;
    lineInfo.blendSrcFactor = srcFactor;
    lineInfo.blendDstFactor = dstFactor;
    lineInfo.blendLogicOp = logicOp;
}

void gxutil_draw_line(Vec *start, Vec *end, GXColor *c)
{
    GXColor color;

    prepare_for_drawing_lines();
    color = *c;
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXBegin(GX_LINES, GX_VTXFMT0, 2);
        GXPosition3f32(start->x, start->y, start->z);
        GXColor4u8(color.r, color.g, color.b, color.a);
        GXPosition3f32(end->x, end->y, end->z);
        GXColor4u8(color.r, color.g, color.b, color.a);
    GXEnd();
}

#pragma force_active on
void gxutil_draw_line_strip(u16 numPoints, Point3d *points, GXColor *color)
{
    GXColor c;
    int i;
    Point3d *p;

    if (numPoints < 2)
        return;
    prepare_for_drawing_lines();
    c = *color;
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXBegin(GX_LINESTRIP, GX_VTXFMT0, numPoints);

    p = points;
    for (i = numPoints; i > 0; i--, p++)
    {
        GXPosition3f32(p->x, p->y, p->z);
        GXColor4u8(c.r, c.g, c.b, c.a);
    }
    GXEnd();
}
#pragma force_active reset

struct DrawLineDeferredNode
{
    struct OrdTblNode node;
    Mtx mtx;
    GXPrimitive primType;
    u16 vtxCount;
    Point3d *points;
    GXColor color;
    struct LineInfo lineInfo;
};

static void draw_line_deferred_callback(struct DrawLineDeferredNode *);

void gxutil_draw_line_deferred(Point3d *start, Point3d *end, GXColor *c)
{
    struct DrawLineDeferredNode *node;
    Point3d *points;
    struct OrdTblNode *entry = ord_tbl_get_entry_for_pos(start);

    node = ord_tbl_alloc_node(sizeof(*node));
    points = ord_tbl_alloc_node(2 * sizeof(Point3d));
    node->node.drawFunc = (OrdTblDrawFunc)draw_line_deferred_callback;
    node->primType = GX_LINES;
    node->vtxCount = 2;
    node->lineInfo = lineInfo;
    node->color = *c;
    points[0] = *start;
    points[1] = *end;
    node->points = points;
    mathutil_mtxA_to_mtx(node->mtx);
    ord_tbl_insert_node(entry, &node->node);
}

static void draw_line_deferred_callback(struct DrawLineDeferredNode *node)
{
    struct LineInfo lineInfoBackup = lineInfo;
    GXColor c;
    int i;
    Point3d *p;

    lineInfo = node->lineInfo;
    prepare_for_drawing_lines();
    GXSetZMode_cached(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    c = node->color;
    GXLoadPosMtxImm(node->mtx, GX_PNMTX0);
    GXBegin(node->primType, GX_VTXFMT0, node->vtxCount);
    p = node->points;
    for (i = node->vtxCount; i > 0; i--, p++)
    {
        GXPosition3f32(p->x, p->y, p->z);
        GXColor4u8(c.r, c.g, c.b, c.a);
    }
    GXEnd();
    lineInfo = lineInfoBackup;
}

#pragma force_active on
void gxutil_draw_line_multicolor(struct PointWithColor *start, struct PointWithColor *end)
{
    prepare_for_drawing_lines();
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXBegin(GX_LINES, GX_VTXFMT0, 2);
        GXPosition3f32(start->pos.x, start->pos.y, start->pos.z);
        GXColor4u8(start->color.r, start->color.g, start->color.b, start->color.a);
        GXPosition3f32(end->pos.x, end->pos.y, end->pos.z);
        GXColor4u8(end->color.r, end->color.g, end->color.b, start->color.a);
    GXEnd();
}
#pragma force_active reset

struct DrawLineMulticolorDeferredNode
{
    struct OrdTblNode node;
    Mtx mtx;
    GXPrimitive primType;
    u16 vtxCount;
    struct PointWithColor *points;
    struct LineInfo lineInfo;
};

static void draw_line_multicolor_deferred_callback(struct DrawLineMulticolorDeferredNode *);

void gxutil_draw_line_multicolor_deferred(struct PointWithColor *start, struct PointWithColor *end)
{
    struct DrawLineMulticolorDeferredNode *node;
    struct PointWithColor *points;
    struct OrdTblNode *entry = ord_tbl_get_entry_for_pos(&start->pos);

    node = ord_tbl_alloc_node(sizeof(*node));
    points = ord_tbl_alloc_node(2 * sizeof(struct PointWithColor));
    node->node.drawFunc = (OrdTblDrawFunc)draw_line_multicolor_deferred_callback;
    node->primType = GX_LINES;
    node->vtxCount = 2;
    node->lineInfo = lineInfo;
    points[0] = *start;
    points[1] = *end;
    node->points = points;
    mathutil_mtxA_to_mtx(node->mtx);
    ord_tbl_insert_node(entry, &node->node);
}

void prepare_for_drawing_lines(void)
{
    if (lineInfo.lineWidth != gxCache->lineWidth || lineInfo.texOffset != gxCache->texOffsets)
    {
        GXSetLineWidth(lineInfo.lineWidth, lineInfo.texOffset);
        gxCache->lineWidth = lineInfo.lineWidth;
        gxCache->texOffsets = lineInfo.texOffset;
    }
    gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0));
    GXSetBlendMode_cached(lineInfo.blendMode, lineInfo.blendSrcFactor, lineInfo.blendDstFactor, lineInfo.blendLogicOp);
    GXSetChanCtrl(
        GX_COLOR0A0,  // chan
        GX_DISABLE,  // enable
        GX_SRC_VTX,  // amb_src
        GX_SRC_VTX,  // mat_src
        GX_LIGHT_NULL,  // light_mask
        GX_DF_CLAMP,  // diff_fn
        GX_AF_SPOT);  // attn_fn
    GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevOp_cached(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens(0);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
}

static void draw_line_multicolor_deferred_callback(struct DrawLineMulticolorDeferredNode *node)
{
    struct LineInfo lineInfoBackup = lineInfo;
    int i;
    struct PointWithColor *p;

    lineInfo = node->lineInfo;
    prepare_for_drawing_lines();
    GXSetZMode_cached(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
    GXLoadPosMtxImm(node->mtx, GX_PNMTX0);
    GXBegin(node->primType, GX_VTXFMT0, node->vtxCount);
    p = node->points;
    for (i = node->vtxCount; i > 0; i--, p++)
    {
        GXPosition3f32(p->pos.x, p->pos.y, p->pos.z);
        GXColor4u8(p->color.r, p->color.g, p->color.b, p->color.a);
    }
    GXEnd();
    lineInfo = lineInfoBackup;
}
