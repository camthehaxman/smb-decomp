#ifndef _SRC_GMA_H_
#define _SRC_GMA_H_

#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXStruct.h>
#include <dolphin/mtx.h>
#include <dolphin/types.h>

// A TEV layer describes how to set up one or more consecutive TEV stages. Each layer is fed an
// "input" color/alpha argument - the first layer gets RASC/RASA (unless both lighting and vertex
// colors are disabled, then it gets TEVREG0), and subsequent layers get TEVPREV.
struct GMATevLayer
{
    /*0x00*/ u32 flags;  // see below
    /*0x04*/ u16 tplIndex;  // index of image to use in the associated TPL
    /*0x06*/ s8 lodBias;
    /*0x07*/ u8 maxAniso;
    /*0x08*/ GXTexObj *texObj;  // pointer to a texture object (set at runtime)
    /*0x0C*/ u8 pad32[0x20-0x0C];  // pad to a multiple of 32 bytes
}; // size = 0x20

// Values for GMATevLayer.flags
enum
{
    // A TEV layer is one of five types. If none of the "TYPE" flags are set it's the first type:
    // standard diffuse lighting (light * texture dotted with normals etc.)

    // "Specular" light layer pointing in view direction?
    // Used in Water and Master at least
    GMA_TEV_LAYER_FLAG_TYPE_VIEW_SPECULAR = 1 << 0,

    GMA_TEV_LAYER_FLAG_TYPE3 = 1 << 1, // Unused/ignored?

    // Bits 2-3 for S wrap mode
    // Bits 4-5 for T wrap mode
    GMA_TEV_LAYER_FLAG_DO_EDGE_LOD = 1 << 6,
    // Bits 7-10 for max LOD
    GMA_TEV_LAYER_FLAG_MAGFILT_NEAR = 1 << 11,

    // Alpha blend layer. Passes color through unchanged, multiplies alpha by lookup from alpha
    // texture
    GMA_TEV_LAYER_FLAG_TYPE_ALPHA_BLEND = 1 << 13,

    // "Specular" light layer pointing in fixed direction?
    // Used for specular highlight on party ball and monkey ball
    GMA_TEV_LAYER_FLAG_TYPE_WORLD_SPECULAR = 1 << 15,

    GMA_TEV_LAYER_FLAG_UNK16 = 1 << 16,
};

// model header
struct GMAModel  // Deluxe: GCM_Header
{
    /*0x00*/ u32 magic; // "GCMF"
    /*0x04*/ u32 flags;  // see below
    /*0x08*/ Vec boundSphereCenter;
    /*0x14*/ float boundSphereRadius;
    /*0x18*/ u16 tevLayerCount;
    /*0x1A*/ u16 opaqueShapeCount;  // number of opaque shapes
    /*0x1C*/ u16 translucentShapeCount;  // number of translucent shapes
    /*0x1E*/ u8 mtxCount;
    /*0x20*/ u32 shapeOffs;  // offset from this struct to to the first GMAShape
                             // for effective models, this is instead an offset to GMAEffVtxInfo
    /*0x24*/ GXTexObj *texObjs;
    /*0x28*/ u8 mtxIndexes[8];  // selects which transform matrices are assigned to GX_PNMTX1
                                // through GX_PNMTX9. A value of 0xFF means no matrix is assigned.
                                // This is only applicable to models with the GCMF_STITCHING flag
    /*0x30*/ u8 pad32[0x40-0x30];  // pad to a multiple of 32 bytes
    /*0x40*/ struct GMATevLayer tevLayers[0]; // Shapes reference at most three from here
};  // size = 0x40

// GMAModel flags. STITCHING, SKIN, and EFFECTIVE are mutually exclusive but 16BIT may be
// interspersed in there. No flags or only 16BIT can also be set (most models are this way).
enum
{
    // Uses VAT with compressed 16-bit vert pos/norm/texcoord instead of floats
    GCMF_16BIT = 0x01,
    // Skinned meshes with one bone per vertex (what GX hardware supports). Uses tristrips
    GCMF_STITCHING = 0x04,  // each shape has matrix indices?
    // Linear blend skin meshes (>=1 bone per vertex) to be computed on CPU? Uses indexed meshes
    GCMF_SKIN = 0x08,
    // Meshes where each vertex is explicitly positioned by CPU each frame?  Uses indexed meshes
    GCMF_EFFECTIVE = 0x10,
};

// Vertex info for effective models
// If model flags GCMF_SKIN or GCMF_EFFECTIVE are set, then model+shapeOffs points to this structure
// instead of the shapes. The shapes immediately follow this structure, and each GMAShape structure
// is exactly 0x60 bytes, since they do not contain display list data.
struct GMAEffVtxInfo  // Deluxe: GCM_VtxInfo
{
    /*0x00*/ u8 filler0[8];  // unused/unknown
    /*0x08*/ u32 vtxDataOffs;  // offset from this struct to vertex data
    /*0x0C*/ u32 indexDataOffs;  // offset from this struct to index data
                                 // Index data consists of an array of 32-bit values. The first value
                                 // specifies the number of vertices, and each value following it is an
                                 // offset into the vertex data to where the vertex is stored.
                                 // Each vertex is a 52-byte structure shown below. Which attributes are
                                 // used depends on the vtxAttrs member of the GMAShape.
    /*0x10*/ u8 pad32[0x20-0x10];  // pad to a multiple of 32 bytes
};  // size = 0x20

struct GMAEffVtx
{
    /*0x00*/ float x, y, z;     // always used
    /*0x0C*/ float nx, ny, nz;  // always used
    /*0x18*/ float u0, v0;      // used if (1 << GX_VA_TEX0) is set
    /*0x20*/ float u1, v1;      // used if (1 << GX_VA_TEX1) is set
    /*0x28*/ float u2, v2;      // used if (1 << GX_VA_TEX2) is set
    /*0x30*/ u8 r, g, b, a;     // used if (1 << GX_VA_CLR0) is set
};  // size = 0x34

struct GMAShape  // Deluxe: GCM_Material
{
    /*0x00*/ u32 flags;  // see below
    /*0x04*/ GXColor materialColor;
    /*0x08*/ GXColor ambientColor;
    /*0x0C*/ GXColor specularColor;
    /*0x10*/ u8 reserved;  // not used
    /*0x11*/ u8 alpha;
    /*0x12*/ u8 tevLayerCount;
    /*0x13*/ u8 dispListFlags;  // bit mask that determines which display lists are present
    /*0x14*/ u8 unk14;
    /*0x16*/ u16 tevLayerIdxs[3];   // Up to 3 indices into model's tev layer list. -1 means end of list
    /*0x1C*/ u32 vtxAttrs; // One bit for each GXAttr vertex attribute
    /*0x20*/ u8 mtxIndices[8];  // selects which transform matrices are assigned to GX_PNMTX1
                                // through GX_PNMTX9. A value of 0xFF means no matrix is assigned.
                                // This is only applicable to models with the GCMF_STITCHING flag
    /*0x28*/ u32 dispListSizes[2];  // size of each display list
                                    // for GCMF_SKIN or GCMF_EFFECTIVE models, this is instead the length of each
                                    // index list (number of u32 words)
    /*0x30*/ Point3d origin; // Reference point for depth sorting
    u8 filler3C[4];
    /*0x40*/ u32 blendFactors; // 0xF bitmask for src blend factor, 0xF0 for dst blend factor
    /*0x44*/ u8 pad32[0x60-0x44];  // pad to a multiple of 32 bytes
    /*0x60*/ u8 dispLists[0]; // Display list data immediately follows this structure.
                              // This is not present in models with the GCMF_SKIN or GCMF_EFFECTIVE flags
}; // size = 0x60

// Values for GMAShape.flags
enum
{
    GMA_SHAPE_FLAG_UNLIT = 1 << 0,
    GMA_SHAPE_FLAG_DOUBLE_SIDED = 1 << 1, // Draw front and back sides of tris/quads
    GMA_SHAPE_FLAG_NO_FOG = 1 << 2,
    GMA_SHAPE_FLAG_CUSTOM_MAT_AMB_COLOR = 1 << 3, // Use material/ambient colors in shape in mat/amb color registers
    GMA_SHAPE_FLAG_CUSTOM_BLEND_SRC = 1 << 5,
    GMA_SHAPE_FLAG_CUSTOM_BLEND_DST = 1 << 6,
    GMA_SHAPE_FLAG_SIMPLE_MATERIAL = 1 << 7, // Only 1 tev stage that spits out color/alpha input
                                             // directly
    GMA_SHAPE_FLAG_VERT_COLORS = 1 << 8,
};

// Values for GMAShape.dispListFlags
enum
{
    GMA_SHAPE_HAS_DLIST0 = 1 << 0, // Display list 0 present, cull front faces by default
    GMA_SHAPE_HAS_DLIST1 = 1 << 1, // Display list 1 present, cull back faces by default
    // Extra display lists (always both present or neither?)
    GMA_SHAPE_HAS_DLIST2 = 1 << 2, // Display list 2 present, cull front faces
    GMA_SHAPE_HAS_DLIST3 = 1 << 3, // Display list 3 present, cull back faces
};

// Two extra display lists which may be included in a shape. This structure, if present, follows
// directly after the first two disp lists if present. Not applicable to effective models.
struct GMAExtraDispLists
{
    /*0x00*/ u8 mtxIndices[8];  // selects which transform matrices are assigned to GX_PNMTX1
                                // through GX_PNMTX9. A value of 0xFF means no matrix is assigned.
    /*0x08*/ u32 dispListSizes[2];
    /*0x10*/ u8 pad32[0x20-0x10];  // pad to a multiple of 32 bytes
    /*0x20*/ u8 dispLists[0];  // Display list data immediately follows this structure.
};

struct GMAModelEntry
{
    struct GMAModel *model;
    char *name;
};

// Represents an archive of models
struct GMA  // Deluxe: GMA_Manager
{
    /*0x00*/ u32 numModels;
    /*0x04*/ u8 *modelsBase;
    /*0x08*/ struct GMAModelEntry *modelEntries;
    /*0x0C*/ char *namesBase;
    /*0x10*/ u8 padding[0x20 - 0x10];  // pad to 32 bytes
    /*0x20*/ u8 fileData[0]; // raw file data
};
// size = 0x20

#endif
