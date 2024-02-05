#ifndef _SRC_NLSPRITE_H_
#define _SRC_NLSPRITE_H_

// Naomi sprite definition from the Naomi SDK.
typedef struct tagNLsprarg
{
    /*0x00*/ s32 sprno;  // ID of bitmap image to use as texture (see bitmap.h)

    // Position
    /*0x04*/ float x;  // position of sprite (0-640) from left edge of screen
    /*0x08*/ float y;  // position of sprite (0-480) from top edge of screen
    /*0x0C*/ float z;

    // Scale. The size of the sprite is this scale multiplied by the dimensions of the sprite's texture.
    /*0x10*/ float zm_x;
    /*0x14*/ float zm_y;

    // Texture coordinates
    /*0x18*/ float u0;  // x texture coordinate of left edge
    /*0x1C*/ float v0;  // y texture coordinate of top edge
    /*0x20*/ float u1;  // x texture coordinate of right edge
    /*0x24*/ float v1;  // y texture coordinate of bottom edge

    /*0x28*/ s32 ang;  // counterclockwise rotation in units of 1/65536 turn
    /*0x2C*/ float trnsl;  // opacity (0 is invisible, 1 is fully opaque)
    s32 listType;
    /*0x34*/ u32 attr;  // bit mask of flags (see below)

    // Color of sprite. The final color is computed by texColor * base_color + offset_color
    /*0x38*/ u32 base_color;    // RGBA color. Note: The alpha component of this color is ignored.
                                // The above "opacity" field is used instead.
    /*0x3C*/ u32 offset_color;  // RGBA color

    /*0x40*/ s32 reserved[4];
} NLsprarg;

// listType (doesn't have any effect on GameCube)

#define	NLSPR_LISTTYPE_OPAQUE  (0)
#define	NLSPR_LISTTYPE_TRANS   (2)
#define	NLSPR_LISTTYPE_PUNCH   (4)
#define	NLSPR_LISTTYPE_AUTO   (-1)

// attr

#define NLSPR_DISP_xL (1)
#define NLSPR_DISP_xC (2)
#define NLSPR_DISP_xR (3)

#define NLSPR_DISP_yT (1)
#define NLSPR_DISP_yC (2)
#define NLSPR_DISP_yB (3)

// Specifies which corner of the sprite the point specified by x and y corresponds to
#define	NLSPR_DISP_LT ((NLSPR_DISP_xL<<0)|(NLSPR_DISP_yT<<2)) /* Disp base Left Top */
#define	NLSPR_DISP_LC ((NLSPR_DISP_xL<<0)|(NLSPR_DISP_yC<<2)) /* Disp base Left Center */
#define	NLSPR_DISP_LB ((NLSPR_DISP_xL<<0)|(NLSPR_DISP_yB<<2)) /* Disp base Left Bottom */
#define	NLSPR_DISP_CT ((NLSPR_DISP_xC<<0)|(NLSPR_DISP_yT<<2)) /* Disp base Center Top */
#define	NLSPR_DISP_CC ((NLSPR_DISP_xC<<0)|(NLSPR_DISP_yC<<2)) /* Disp base Center Center */
#define	NLSPR_DISP_CB ((NLSPR_DISP_xC<<0)|(NLSPR_DISP_yB<<2)) /* Disp base Center Bottom */
#define	NLSPR_DISP_RT ((NLSPR_DISP_xR<<0)|(NLSPR_DISP_yT<<2)) /* Disp base Right Top */
#define	NLSPR_DISP_RC ((NLSPR_DISP_xR<<0)|(NLSPR_DISP_yC<<2)) /* Disp base Right Center */
#define	NLSPR_DISP_RB ((NLSPR_DISP_xR<<0)|(NLSPR_DISP_yB<<2)) /* Disp base Right Bottom */

/* FLIP */
#define	NLSPR_FLIP_H (1 << 19) /* h flip */  // This is (1 << 4) in the SDK
#define	NLSPR_FLIP_V (1 << 20) /* v flip */  // This is (1 << 5) in the SDK

// These flags were not in the Naomi SDK, but appear to be something custom
#define NLSPR_UNKFLAG_12   (1 << 12)  // Doesn't appear to do anything
#define NLSPR_UNKFLAG_17   (1 << 17)  // Doesn't appear to do anything
#define NLSPR_PERSP_SCALE  (1 << 18)  // Scales the sprite based on its depth
#define NLSPR_DEPTH_UPDATE (1 << 21)  // Determines whether the sprite will write to the depth buffer

#endif
