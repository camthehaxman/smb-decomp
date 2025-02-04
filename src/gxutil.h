#ifndef _SRC_GXUTIL_H_
#define _SRC_GXUTIL_H_

#include <dolphin/mtx.h>
#include <dolphin/gx/GXStruct.h>
#include <dolphin/gx/GXEnum.h>

struct PointWithColor
{
    Point3d pos;
    GXColor color;
};

void gxutil_begin_display_list(void *, u32);
u32 gxutil_end_display_list(void);
void gxutil_clear_vtx_attrs(void);
void gxutil_set_vtx_attrs(u32);
void gxutil_dummy(void);
void gxutil_load_pos_nrm_matrix(Mtx, int);
void u_init_bg_fog_params(void);
void fog_main(void);
void u_gxutil_set_fog_enabled(s8);
void u_gxutil_fog_something_2(void);
void fog_gx_set(void);
void gxutil_set_line_width(int);
void gxutil_set_line_blend_params(GXBlendMode blendMode, GXBlendFactor srcFactor, GXBlendFactor dstFactor, GXLogicOp blendLogicOp);
void gxutil_draw_line(Vec *pos1, Vec *pos2, GXColor *c);
void gxutil_draw_line_strip(u16 numPoints, Point3d *points, GXColor *color);
void gxutil_draw_line_deferred(Point3d *start, Point3d *end, GXColor *c);
void gxutil_draw_line_multicolor_deferred(struct PointWithColor *start, struct PointWithColor *end);
void prepare_for_drawing_lines(void);

#endif
