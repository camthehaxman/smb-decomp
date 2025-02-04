#include <assert.h>
#include <dolphin.h>
#include <ppcintrinsic.h>
#include <string.h>
#include <dolphin/gx/GXEnum.h>

#include "bitmap.h"
#include "global.h"
#include "gxcache.h"
#include "gxutil.h"
#include "light.h"
#include "load.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "ord_tbl.h"

float s_fogEndZ;
float s_fogStartZ;
GXColor s_fogColor;
GXFogType s_fogType;
s32 s_fogEnabled;
u32 s_lightMask;

struct Color3f s_ambientColor;
FORCE_BSS_ORDER(s_ambientColor)

static struct
{
    BOOL isVtxTypeA; // Type of vertices in disp list
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 meshType;
    u8 unkA;
    u8 fillerB[1];
    GXTexObj *texObj;
    GXTexMapID texMapId;
    GXColor materialColor;
    GXColor ambientColor;
    float alpha;
    u8 unk20;
    u8 filler21[3];
    u32 unk24;
    u8 unk28;
    u8 filler29[0x34 - 0x29];
} s_nlMaterialCache;
FORCE_BSS_ORDER(s_nlMaterialCache)

static u8 s_lzssHeader[32] __attribute__((aligned(32)));

struct
{
    struct Color3f materialColor;
    float unused1;
    float unused2;
    float unused3;
    float scale;
    float prevScale;
} s_renderParams = {{1.0f, 1.0f, 1.0f}, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

static BOOL nlObjModelListInit(struct NlObj *obj);
static void nlObjPutCheckModel(struct NlModel *model);
static void nlObjPut_InitRenderState(void);
static void nlObjPut_SetMaterial(struct NlMesh *pmesh);
static void nlObjPutTrnsl_InitRenderState(void);
void nlObjPutTrnsl_SetMaterial(struct NlMesh *pmesh);

#pragma force_active on
void nlLineSetThickness(float width)
{
    gxutil_set_line_width(width * 6.0f);
}

void nl2ngc_set_line_blend_params(GXBlendFactor srcFactor, GXBlendFactor dstFactor)
{
    gxutil_set_line_blend_params(GX_BM_BLEND, srcFactor, dstFactor, GX_LO_CLEAR);
}

void nlSingleLine(Point3d *start, Point3d *end, u32 color)
{
    GXColor gxColor;

    gxColor.r = (color >> 16) & 0xFF;
    gxColor.g = (color >> 8) & 0xFF;
    gxColor.b = (color >> 0) & 0xFF;
    gxColor.a = (color >> 24) & 0xFF;
    gxutil_draw_line(start, end, &gxColor);
}
#pragma force_active reset

void nl2ngc_draw_line_deferred(Point3d *start, Point3d *end, u32 color)
{
    GXColor gxColor;

    gxColor.r = (color >> 16) & 0xFF;
    gxColor.g = (color >> 8) & 0xFF;
    gxColor.b = (color >> 0) & 0xFF;
    gxColor.a = (color >> 24) & 0xFF;
    gxutil_draw_line_deferred(start, end, &gxColor);
}

void nlSetScaleFactor(float x)
{
    s_renderParams.scale = x;
}

void nlObjPutSetFadeColorBase(float r, float g, float b)
{
    s_renderParams.materialColor.r = r;
    s_renderParams.materialColor.g = g;
    s_renderParams.materialColor.b = b;
}

BOOL nlObjModelListLoad(struct NlObj **nlObj, struct TPL **tpl, char *nlobjPath,
                        char *tplPath)
{
    int len;
    struct NlModel **pmodel;
    u32 size;
    struct File file;

    // Free object if it's already loaded
    if (*nlObj != NULL)
    {
        OSFree(*nlObj);
        *nlObj = NULL;
    }
    if (!file_open(nlobjPath, &file))
        return FALSE;
    len = strlen(nlobjPath);
    if (len >= 3 && strcmp(nlobjPath + (len - 3), ".lz") == 0)
    {
        u32 uncompSize;
        void *compressed;
        void *uncompressed;

        // Read LZSS header
        if (file_read(&file, s_lzssHeader, 32, 0) < 0)
            return FALSE;
        size = OSRoundUp32B(__lwbrx(s_lzssHeader, 0));
        uncompSize = OSRoundUp32B(__lwbrx(s_lzssHeader, 4));

        uncompressed = OSAlloc(uncompSize);
        if (uncompressed == NULL)
            return FALSE;

        // Read whole file
        compressed = OSAllocFromHeap(mainHeap, size);
        if (compressed == NULL)
        {
            OSFree(uncompressed);
            return FALSE;
        }
        if (file_read(&file, compressed, size, 0) < 0)
            return FALSE;
        if (file_close(&file) != 1)
            return FALSE;

        // Decompress data
        lzs_decompress(compressed, uncompressed);
        OSFreeToHeap(mainHeap, compressed);
        *nlObj = uncompressed;
        if (*nlObj == NULL)
            return FALSE;
    }
    else
    {
        size = OSRoundUp32B(file_size(&file));
        *nlObj = OSAlloc(size);
        if (*nlObj == NULL)
            OSPanic("nl2ngc.c", 476, "cannot OSAlloc");
        file_read(&file, *nlObj, size, 0);
        file_close(&file);
    }

    nlObjModelListInit(*nlObj);
    if (*tpl != NULL)
        bitmap_free_tpl(*tpl);
    *tpl = bitmap_load_tpl(tplPath);
    if (*tpl == NULL)
        return FALSE;

    pmodel = (*nlObj)->models;
    while (*pmodel != NULL)
    {
        nlObjPutLoadTexture(*pmodel, *tpl);
        pmodel++;
    }
    return TRUE;
}

BOOL nlObjModelListFree(struct NlObj **nlObj, struct TPL **tpl)
{
    u8 unused[8];

    if (*nlObj != NULL)
    {
        OSFree(*nlObj);
        *nlObj = NULL;
    }
    if (*tpl != NULL)
    {
        bitmap_free_tpl(*tpl);
        *tpl = NULL;
    }
    return TRUE;
}

// This function converts file all file offsets in the struct into memory pointers
// Featuring some insane pointer arithmetic.
static BOOL nlObjModelListInit(struct NlObj *obj)
{
    struct NlModel *volatile *pmodel = obj->models;
    struct NlObj_UnkChild *volatile *unkptr;
    struct NlObj_UnkChild_Child *unkchild;

    // Adjust pointers in models?
    while (*pmodel != NULL)
    {
        u32 *ptr;

        // adjust pointer to model?
        *pmodel = (void *)((u32)*pmodel + (u32)obj);

        // What the hell is this for?
        ptr = (u32 *)*pmodel;
        ptr[-2] += (u32)obj;

        ptr = (u32 *)*pmodel;
        ptr[-1] += (u32)obj;

        nlObjPutCheckModel(*pmodel);
        pmodel++;
    }

    obj->unk0 = (void *)((u32)obj->unk0 + (u32)obj);
    unkptr = obj->unk0;
    while (*unkptr != NULL)
    {
        u32 *ptr;
        *unkptr = (void *)((u32)*unkptr + (u32)obj);

        ptr = (u32 *)*unkptr;
        ptr[0] += (u32)obj;

        ptr = (u32 *)*unkptr;
        ptr[1] += (u32)obj;

        unkchild = (*unkptr)->childStructs;
        while (unkchild->unk0 != 0)
        {
            unkchild->unk0 += (u32)obj;
            unkchild++;
        }
        unkptr++;
    }
    return TRUE;
}

static void nlObjPutCheckModel(struct NlModel *model)
{
    if (model->u_valid != -1)
    {
        struct NlMesh *mesh = (struct NlMesh *)model->meshStart;
        BOOL translucent = FALSE;
        BOOL opaque = FALSE;
        BOOL r8 = TRUE;
        BOOL r9 = TRUE;

        while (mesh->flags != 0)
        {
            if (((mesh->flags >> 24) & 7) != 0)
            {
                translucent = TRUE;
                if (r8 && (mesh->type != -1 || mesh->type != -3))
                    r8 = FALSE;
            }
            else
            {
                opaque = TRUE;
                if (r9 && (mesh->type != -1 || mesh->type != -3))
                    r9 = FALSE;
            }
            mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
        }
        if (translucent)
            model->flags |= (NL_MODEL_FLAG_TRANSLUCENT);
        if (opaque)
            model->flags |= (NL_MODEL_FLAG_OPAQUE);
        if (r8)
            model->flags |= (1 << 10);
        if (r9)
            model->flags |= (1 << 11);
    }
}

void nlObjPutLoadTexture(struct NlModel *model, struct TPL *tpl)
{
    u8 unused[8];

    if (model->u_valid != -1)
    {
        struct NlMesh *mesh = (struct NlMesh *)model->meshStart;

        while (mesh->flags != 0)
        {
            if (mesh->tplTexIdx >= 0)
            {
                u32 flags = mesh->texFlags;
                GXTexObj *texObj = &tpl->texObjs[mesh->tplTexIdx];
                GXTexWrapMode wrapS, wrapT;
                GXTexFilter minFilt, magFilt;

                if (flags & (NL_TEX_FLAG_S_CLAMP))
                    wrapS = GXGetTexObjWrapS(texObj);
                else if (flags & (NL_TEX_FLAG_S_MIRROR))
                    wrapS = GX_MIRROR;
                else
                    wrapS = GX_REPEAT;

                if (flags & (NL_TEX_FLAG_T_CLAMP))
                    wrapT = GXGetTexObjWrapT(texObj);
                else if (flags & (NL_TEX_FLAG_T_MIRROR))
                    wrapT = GX_MIRROR;
                else
                    wrapT = GX_REPEAT;

                GXInitTexObj(texObj,                                 // obj
                             GXGetTexObjData(texObj),                // image_ptr
                             GXGetTexObjWidth(texObj),               // width
                             GXGetTexObjHeight(texObj),              // height
                             GXGetTexObjFmt(texObj),                 // format
                             wrapS,                                  // wrap_s
                             wrapT,                                  // wrap_t
                             tpl->texHeaders[mesh->tplTexIdx].unkC); // mipmap

                switch (((flags >> 13) & 0x3))
                {
                case 0:
                    if (GXGetTexObjMipMap(texObj))
                        minFilt = GX_LIN_MIP_NEAR;
                    else
                        minFilt = GX_NEAR;
                    magFilt = GX_NEAR;
                    break;
                default:
                    if (GXGetTexObjMipMap(texObj))
                        minFilt = GX_LIN_MIP_LIN;
                    else
                        minFilt = GX_LINEAR;
                    magFilt = GX_LINEAR;
                    break;
                }

                GXInitTexObjLOD(texObj,      // obj
                                minFilt,     // min_filt
                                magFilt,     // mag_filt
                                0.0f,        // min_lod
                                0.0f,        // max_lod
                                0.0f,        // lod_bias
                                GX_FALSE,    // bias_clamp
                                GX_FALSE,    // do_edge_lod
                                GX_ANISO_1); // max_aniso
                mesh->texObj = texObj;
            }
            mesh = (struct NlMesh *)(mesh->dispListStart + mesh->dispListSize);
        }
    }
}

struct DrawModelDeferredNode
{
    struct OrdTblNode node;
    struct NlModel *model;
    Mtx viewFromModel;
    struct Color3f materialColor;
    u32 lightGroup;
    struct Color3f ambientColor;
    u32 fogEnabled;
};

static void nlObjPut_TrnslList_OP(struct DrawModelDeferredNode *);

// Draw opaque meshes in model immediately, depth sort translucent meshes
void nlObjPut(struct NlModel *model)
{
    u32 *modelFlags;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (test_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            // Always reset a non-one scale before next draw call
            if (test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius,
                                              s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
            s_renderParams.scale = 1.0f;
        }
        modelFlags = &model->flags;
        if (model->flags & (NL_MODEL_FLAG_OPAQUE))
            nlObjPut_OpaqueList(model);
        if (*modelFlags & (NL_MODEL_FLAG_TRANSLUCENT))
        {
            struct DrawModelDeferredNode *drawNode;
            struct OrdTblNode *list = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
            drawNode = ord_tbl_alloc_node(sizeof(*drawNode));

            drawNode->node.drawFunc = (OrdTblDrawFunc)nlObjPut_TrnslList_OP;
            drawNode->model = model;
            drawNode->materialColor.r = s_renderParams.materialColor.r;
            drawNode->materialColor.g = s_renderParams.materialColor.g;
            drawNode->materialColor.b = s_renderParams.materialColor.b;
            drawNode->lightGroup = peek_light_group();
            drawNode->ambientColor.r = s_ambientColor.r;
            drawNode->ambientColor.g = s_ambientColor.g;
            drawNode->ambientColor.b = s_ambientColor.b;
            drawNode->fogEnabled = s_fogEnabled;
            mathutil_mtxA_to_mtx(drawNode->viewFromModel);
            ord_tbl_insert_node(list, &drawNode->node);
        }
    }
}

Mtx textureMatrix = {
    {1, 0, 0, 0},
    {0, -1, 0, 1},
    {0, 0, 1, 0},
};

// Draw opaque and translucent meshes in model immediately without depth sorting
void nlObjPutImm(struct NlModel *model)
{
    struct NlMesh *mesh;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (test_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius,
                                              s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
        }
        if (model->flags & (NL_MODEL_FLAG_VTX_TYPE_A))
        {
            gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));
            s_nlMaterialCache.isVtxTypeA = TRUE;
        }
        else
        {
            gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));
            s_nlMaterialCache.isVtxTypeA = FALSE;
        }

        nlObjPut_InitRenderState();
        GXLoadTexMtxImm(textureMatrix, GX_TEXMTX0, GX_MTX2x4);
        GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
        GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            struct NlDispList *dlstart;
            struct NlMesh *next;

            nlObjPut_SetMaterial(mesh);
            dlstart = (void *)(mesh->dispListStart);
            next = (void *)(mesh->dispListStart + mesh->dispListSize);
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                // Other non-negative model types cover the lit + const material color case
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                nlObjPut_PutStrip_VTC(dlstart, next);
                break;
            default:
                nlObjPut_PutStrip(dlstart, next);
                break;
            }
            mesh = next;
        }
        unk_empty();
    }
}

// Same as normal model deferred node but with alpha parameter
struct DrawAlphaModelDeferredNode
{
    struct OrdTblNode node;
    struct NlModel *model;
    Mtx viewFromModel;
    struct Color3f materialColor;
    float alpha;
    u32 lightGroup;
    struct Color3f ambientColor;
    u32 fogEnabled;
};

void nlObjPutTrnsl_TrnslList_OP(struct DrawAlphaModelDeferredNode *);

// Draw model with global alpha blend. Both opaque and translucent meshes in model are depth-sorted
void nlObjPutTrnsl(struct NlModel *model, float alpha)
{
    struct DrawAlphaModelDeferredNode *node;
    struct OrdTblNode *entry;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (test_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius,
                                              s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
            s_renderParams.scale = 1.0f;
        }

        entry = ord_tbl_get_entry_for_pos(&model->boundSphereCenter);
        node = ord_tbl_alloc_node(sizeof(*node));

        node->node.drawFunc = (OrdTblDrawFunc)nlObjPutTrnsl_TrnslList_OP;
        node->model = model;
        node->alpha = alpha;
        node->materialColor.r = s_renderParams.materialColor.r;
        node->materialColor.g = s_renderParams.materialColor.g;
        node->materialColor.b = s_renderParams.materialColor.b;
        node->lightGroup = peek_light_group();
        node->ambientColor.r = s_ambientColor.r;
        node->ambientColor.g = s_ambientColor.g;
        node->ambientColor.b = s_ambientColor.b;
        node->fogEnabled = s_fogEnabled;
        mathutil_mtxA_to_mtx(node->viewFromModel);
        ord_tbl_insert_node(entry, &node->node);
    }
}

// Draw model with global alpha blend. Neither opaque nor translucent meshes in model are depth-sorted
void nl2ngc_draw_model_alpha_sort_none(struct NlModel *model, float alpha)
{
    struct NlMesh *mesh;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (test_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius,
                                              s_renderParams.scale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
        }
        if (model->flags & (NL_MODEL_FLAG_VTX_TYPE_A))
        {
            gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));
            s_nlMaterialCache.isVtxTypeA = TRUE;
        }
        else
        {
            gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));
            s_nlMaterialCache.isVtxTypeA = FALSE;
        }

        s_nlMaterialCache.alpha = alpha;
        nlObjPutTrnsl_InitRenderState();
        GXLoadTexMtxImm(textureMatrix, GX_TEXMTX0, GX_MTX2x4);
        GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
        GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            struct NlDispList *dlstart;
            struct NlMesh *next;

            nlObjPutTrnsl_SetMaterial(mesh);
            dlstart = (void *)(mesh->dispListStart);
            next = (void *)(mesh->dispListStart + mesh->dispListSize);
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                // Other non-negative model types cover the lit + const material color case
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                nlObjPutTrnsl_PutStrip_VTC(dlstart, next);
                break;
            default:
                nlObjPut_PutStrip(dlstart, next);
                break;
            }
            mesh = next;
        }
        unk_empty();
    }
}

void nl2ngc_draw_model_sort_translucent_alt(struct NlModel *model)
{
    nlObjPut(model);
}

void nl2ngc_draw_model_sort_none_alt(struct NlModel *model)
{
    nlObjPutImm(model);
}

// unused stuff?

u32 lbl_801B79E4[] = {9, 1, 10, 1, 13, 1, 255, 1};
u32 lbl_801B7A04[] = {9, 1, 4, 0, 10, 0, 4, 0, 13, 1, 4, 0, 255, 1, 4, 0};
u32 lbl_801B7A44[] = {9, 1, 11, 1, 13, 1, 255, 1};
u32 lbl_801B7A64[] = {9, 1, 4, 0, 11, 1, 5, 0, 13, 1, 4, 0, 255, 1, 4, 0};

void *lbl_801B7AA4[] = {
    lbl_801B79E4,
    lbl_801B7A04,
    lbl_801B7A44,
    lbl_801B7A64,
};

GXBlendFactor lbl_801B7AB4[] = {0, 1, 2, 3, 4, 5, 6, 7};
GXBlendFactor lbl_801B7AD4[] = {0, 1, 2, 3, 4, 5, 6, 7};
GXCompare s_nlToGXCompare[] = {GX_NEVER,  GX_GEQUAL, GX_EQUAL,  GX_GEQUAL,
                                GX_LEQUAL, GX_NEQUAL, GX_LEQUAL, GX_ALWAYS};
GXCullMode s_nlToGXCullModes[] = {GX_CULL_ALL, GX_CULL_NONE, GX_CULL_BACK, GX_CULL_FRONT};

static void nlObjPut_InitRenderState(void)
{
    GXColor ambColor;

    s_nlMaterialCache.unk4 = 0;
    s_nlMaterialCache.unk5 = 1;
    s_nlMaterialCache.unk6 = 0;
    GXSetBlendMode_cached(GX_BM_NONE, lbl_801B7AB4[1], lbl_801B7AD4[0], GX_LO_CLEAR);
    s_nlMaterialCache.unk20 = gxCache->compareEnable;
    s_nlMaterialCache.unk24 = gxCache->compareFunc;
    s_nlMaterialCache.unk28 = gxCache->updateEnable;
    s_nlMaterialCache.unk7 = 4;
    s_nlMaterialCache.unk8 = 0;

    GXSetZMode_cached(GX_ENABLE, s_nlToGXCompare[s_nlMaterialCache.unk7], (!s_nlMaterialCache.unk8));

    if (s_fogEnabled != 0)
        GXSetFog_cached(s_fogType, s_fogStartZ, s_fogEndZ, 0.1f, 20000.0f, s_fogColor);
    else
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, s_fogColor);

    s_nlMaterialCache.unkA = 2;
    GXSetCullMode_cached(s_nlToGXCullModes[2]);
    s_nlMaterialCache.texObj = 0;
    s_nlMaterialCache.texMapId = 0;

    s_nlMaterialCache.materialColor.r = s_renderParams.materialColor.r * 255.0f;
    s_nlMaterialCache.materialColor.g = s_renderParams.materialColor.g * 255.0f;
    s_nlMaterialCache.materialColor.b = s_renderParams.materialColor.b * 255.0f;
    s_nlMaterialCache.materialColor.a = 255;
    GXSetChanMatColor(GX_COLOR0A0, s_nlMaterialCache.materialColor);

    ambColor.r = s_nlMaterialCache.ambientColor.r = 0;
    ambColor.g = s_nlMaterialCache.ambientColor.g = 0;
    ambColor.b = s_nlMaterialCache.ambientColor.b = 0;
    ambColor.a = s_nlMaterialCache.ambientColor.a = s_nlMaterialCache.materialColor.a;
    GXSetChanAmbColor(GX_COLOR0A0, ambColor);

    s_nlMaterialCache.meshType = 0;
    GXSetChanCtrl(GX_COLOR0A0,    // chan
                  GX_ENABLE,      // enable
                  GX_SRC_REG,     // amb_src
                  GX_SRC_REG,     // mat_src
                  s_lightMask, // light_mask
                  GX_DF_CLAMP,    // diff_fn
                  GX_AF_SPOT);    // attn_fn
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode_cached(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens(1);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
}

static void nlObjPut_SetMaterial(struct NlMesh *pmesh)
{
    struct NlMesh mesh = *pmesh;
    GXColor color;
    u32 r28;
    u32 r25;
    u32 r26;
    u32 r27;

    switch ((mesh.flags >> 24) & 7)
    {
    case 0:
        if (s_nlMaterialCache.unk4 != 0)
        {
            GXSetBlendMode_cached(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
            s_nlMaterialCache.unk4 = 0;
            s_nlMaterialCache.unk5 = 1;
            s_nlMaterialCache.unk6 = 0;
        }
        break;
    default:
        r25 = mesh.texFlags >> 29;
        r27 = (mesh.texFlags >> 26) & 7;
        if (s_nlMaterialCache.unk4 != 2 || s_nlMaterialCache.unk5 != r25 || s_nlMaterialCache.unk6 != r27)
        {
            GXSetBlendMode_cached(GX_BM_BLEND, lbl_801B7AB4[r25], lbl_801B7AD4[r27], GX_LO_CLEAR);
            s_nlMaterialCache.unk4 = 2;
            s_nlMaterialCache.unk5 = r25;
            s_nlMaterialCache.unk6 = r27;
        }
        break;
    }

    r28 = mesh.unk4 >> 29;
    r26 = mesh.unk4 & 0x4000000;
    if (s_nlMaterialCache.unk7 != r28 || s_nlMaterialCache.unk8 != r26)
    {
        GXSetZMode_cached(GX_ENABLE, s_nlToGXCompare[r28], (!r26));
        s_nlMaterialCache.unk7 = r28;
        s_nlMaterialCache.unk8 = r26;
    }

    if (s_fogEnabled != 0)
        GXSetFog_cached(s_fogType, s_fogStartZ, s_fogEndZ, 0.1f, 20000.0f, s_fogColor);
    else
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, s_fogColor);

    if (mesh.tplTexIdx < 0)
    {
        GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GXSetTevOp_cached(GX_TEVSTAGE0, GX_PASSCLR);
    }
    else
    {
        int texMapId = s_nlMaterialCache.texMapId;

        if (s_nlMaterialCache.texObj != mesh.texObj)
        {
            s_nlMaterialCache.texObj = mesh.texObj;
            if (--texMapId < 0)
                texMapId = 7;
            s_nlMaterialCache.texMapId = texMapId;
            GXLoadTexObj_cached(mesh.texObj, texMapId);
        }
        GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD0, texMapId, GX_COLOR0A0);
        switch ((mesh.texFlags >> 6) & 3)
        {
        case 0:
            GXSetTevOp_cached(GX_TEVSTAGE0, GX_REPLACE);
            break;
        case 1:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        case 2:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        case 3:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        }
    }

    color.r = mesh.sub2C.materialColorR * s_renderParams.materialColor.r * 255.0f;
    color.g = mesh.sub2C.materialColorG * s_renderParams.materialColor.g * 255.0f;
    color.b = mesh.sub2C.materialColorB * s_renderParams.materialColor.b * 255.0f;
    color.a = mesh.sub2C.materialColorA * 255.0f;
    if (s_nlMaterialCache.materialColor.r != color.r || s_nlMaterialCache.materialColor.g != color.g ||
        s_nlMaterialCache.materialColor.b != color.b || s_nlMaterialCache.materialColor.a != color.a)
    {
        GXSetChanMatColor(GX_COLOR0A0, color);
        s_nlMaterialCache.materialColor = color;
    }

    color.r = mesh.ambientColorScale * s_ambientColor.r * 255.0f;
    color.g = mesh.ambientColorScale * s_ambientColor.g * 255.0f;
    color.b = mesh.ambientColorScale * s_ambientColor.b * 255.0f;
    color.a = s_nlMaterialCache.materialColor.a;
    if (s_nlMaterialCache.ambientColor.r != color.r || s_nlMaterialCache.ambientColor.g != color.g ||
        s_nlMaterialCache.ambientColor.b != color.b || s_nlMaterialCache.ambientColor.a != color.a)
    {
        GXSetChanAmbColor(GX_COLOR0A0, color);
        s_nlMaterialCache.ambientColor = color;
    }

    if (s_nlMaterialCache.meshType != mesh.type)
    {
        s_nlMaterialCache.meshType = mesh.type;
        switch (mesh.type)
        {
        case NL_MODEL_TYPE_UNLIT_CONST_MAT_COLOR:
            GXSetChanCtrl(GX_COLOR0A0,   // chan
                          GX_DISABLE,    // enable
                          GX_SRC_REG,    // amb_src
                          GX_SRC_REG,    // mat_src
                          GX_LIGHT_NULL, // light_mask
                          GX_DF_CLAMP,   // diff_fn
                          GX_AF_SPOT);   // attn_fn
            break;
        case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
            GXSetChanCtrl(GX_COLOR0A0,   // chan
                          GX_DISABLE,    // enable
                          GX_SRC_VTX,    // amb_src
                          GX_SRC_VTX,    // mat_src
                          GX_LIGHT_NULL, // light_mask
                          GX_DF_CLAMP,   // diff_fn
                          GX_AF_SPOT);   // attn_fn
            break;
        case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR: // Ignored
        default:
            GXSetChanCtrl(GX_COLOR0A0,    // chan
                          GX_ENABLE,      // enable
                          GX_SRC_REG,     // amb_src
                          GX_SRC_REG,     // mat_src
                          s_lightMask, // light_mask
                          GX_DF_CLAMP,    // diff_fn
                          GX_AF_SPOT);    // attn_fn
            break;
        }
    }
}

void nlObjPut_PutStrip(struct NlDispList *dl, void *end)
{
    gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));

    if (s_nlMaterialCache.isVtxTypeA)
        s_nlMaterialCache.isVtxTypeA = FALSE;

    while (dl < (struct NlDispList *)end)
    {
        int faceCount;
        int i;
        u8 *vtxData = dl->vtxData;
        struct NlVtxTypeB *vtx;
        u8 r4 = dl->flags & 3;

        faceCount = dl->faceCount;
        if (s_nlMaterialCache.unkA != r4)
        {
            s_nlMaterialCache.unkA = r4;
            GXSetCullMode_cached(s_nlToGXCullModes[r4]);
        }

        if (dl->flags & (NL_DLIST_FLAG_TRIANGLESTRIP))
        {
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, faceCount);
            while (faceCount > 0)
            {
                if (*(u32 *)vtxData & 1)
                {
                    vtx = (void *)vtxData;
                    GXPosition3f32(vtx->x, vtx->y, vtx->z);
                    GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
                    GXTexCoord2f32(vtx->s, vtx->t);
                    vtxData += 32;
                }
                else
                {
                    vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                    GXPosition3f32(vtx->x, vtx->y, vtx->z);
                    GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
                    GXTexCoord2f32(vtx->s, vtx->t);
                    vtxData += 8;
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & (NL_DLIST_FLAG_TRIANGLES))
        {
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, faceCount * 3);
            while (faceCount > 0)
            {
                for (i = 0; i < 3; i++)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        vtx = (void *)vtxData;
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & (NL_DLIST_FLAG_QUADS))
        {
            GXBegin(GX_QUADS, GX_VTXFMT0, faceCount * 4);
            while (faceCount > 0)
            {
                for (i = 4; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        vtx = (void *)vtxData;
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        GXNormal3f32(vtx->nx, vtx->ny, vtx->nz);
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        dl = (void *)vtxData;
    }
}

// Without global alpha param
void nlObjPut_PutStrip_VTC(struct NlDispList *dl, void *end)
{
    gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));

    if (s_nlMaterialCache.isVtxTypeA != 1)
        s_nlMaterialCache.isVtxTypeA = TRUE;

    while (dl < (struct NlDispList *)end)
    {
        int faceCount;
        int i;
        u8 *vtxData = dl->vtxData;
        struct NlVtxTypeA *vtx;
        u8 r4 = dl->flags & 3;

        faceCount = dl->faceCount;
        if (s_nlMaterialCache.unkA != r4)
        {
            s_nlMaterialCache.unkA = r4;
            GXSetCullMode_cached(s_nlToGXCullModes[r4]);
        }

        if (dl->flags & (1 << 4))
        {
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, faceCount);
            while (faceCount > 0)
            {
                if (*(u32 *)vtxData & 1)
                {
                    u32 color;
                    vtx = (void *)vtxData;
                    GXPosition3f32(vtx->x, vtx->y, vtx->z);
                    color = vtx->color;
                    GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
                               ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
                               ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
                               ((color >> 24) & 0xFF));
                    GXTexCoord2f32(vtx->s, vtx->t);
                    vtxData += 32;
                }
                else
                {
                    u32 color;
                    vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                    GXPosition3f32(vtx->x, vtx->y, vtx->z);
                    color = vtx->color;
                    GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
                               ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
                               ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
                               ((color >> 24) & 0xFF));
                    GXTexCoord2f32(vtx->s, vtx->t);
                    vtxData += 8;
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & (1 << 3))
        {
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, faceCount * 3);
            while (faceCount > 0)
            {
                for (i = 0; i < 3; i++)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        u32 color;
                        vtx = (void *)vtxData;
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        color = vtx->color;
                        GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
                                   ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
                                   ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
                                   ((color >> 24) & 0xFF));
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 32;
                    }
                    else
                    {
                        u32 color;
                        vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        color = vtx->color;
                        GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
                                   ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
                                   ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
                                   ((color >> 24) & 0xFF));
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & (1 << 2))
        {
            GXBegin(GX_QUADS, GX_VTXFMT0, faceCount * 4);
            while (faceCount > 0)
            {
                for (i = 4; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        u32 color;
                        vtx = (void *)vtxData;
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        color = vtx->color;
                        GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
                                   ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
                                   ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
                                   ((color >> 24) & 0xFF));
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 32;
                    }
                    else
                    {
                        u32 color;
                        vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                        GXPosition3f32(vtx->x, vtx->y, vtx->z);
                        color = vtx->color;
                        GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
                                   ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
                                   ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
                                   ((color >> 24) & 0xFF));
                        GXTexCoord2f32(vtx->s, vtx->t);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        dl = (void *)vtxData;
    }
}

static void nlObjPutTrnsl_InitRenderState(void)
{
    GXColor sp18;

    s_nlMaterialCache.unk4 = 0;
    s_nlMaterialCache.unk5 = 4;
    s_nlMaterialCache.unk6 = 5;

    GXSetBlendMode_cached(GX_BM_BLEND, lbl_801B7AB4[4], lbl_801B7AD4[5], GX_LO_CLEAR);

    s_nlMaterialCache.unk20 = gxCache->compareEnable;
    s_nlMaterialCache.unk20 = gxCache->compareFunc; //! mistake?
    s_nlMaterialCache.unk28 = gxCache->updateEnable;
    s_nlMaterialCache.unk7 = 4;
    s_nlMaterialCache.unk8 = 0;

    GXSetZMode_cached(GX_ENABLE, s_nlToGXCompare[s_nlMaterialCache.unk7], (!s_nlMaterialCache.unk8));

    if (s_fogEnabled != 0)
        GXSetFog_cached(s_fogType, s_fogStartZ, s_fogEndZ, 0.1f, 20000.0f, s_fogColor);
    else
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, s_fogColor);

    s_nlMaterialCache.unkA = 2;
    GXSetCullMode_cached(s_nlToGXCullModes[2]);
    s_nlMaterialCache.texObj = 0;
    s_nlMaterialCache.texMapId = 0;

    s_nlMaterialCache.materialColor.r = s_renderParams.materialColor.r * 255.0f;
    s_nlMaterialCache.materialColor.g = s_renderParams.materialColor.g * 255.0f;
    s_nlMaterialCache.materialColor.b = s_renderParams.materialColor.b * 255.0f;
    s_nlMaterialCache.materialColor.a = s_nlMaterialCache.alpha * 255.0f;
    GXSetChanMatColor(GX_COLOR0A0, s_nlMaterialCache.materialColor);

    sp18.r = s_nlMaterialCache.ambientColor.r = 0;
    sp18.g = s_nlMaterialCache.ambientColor.g = 0;
    sp18.b = s_nlMaterialCache.ambientColor.b = 0;
    sp18.a = s_nlMaterialCache.ambientColor.a = s_nlMaterialCache.materialColor.a;
    GXSetChanAmbColor(GX_COLOR0A0, sp18);

    s_nlMaterialCache.meshType = 0;
    GXSetChanCtrl(GX_COLOR0A0,    // chan
                  GX_ENABLE,      // enable
                  GX_SRC_REG,     // amb_src
                  GX_SRC_REG,     // mat_src
                  s_lightMask, // light_mask
                  GX_DF_CLAMP,    // diff_fn
                  GX_AF_SPOT);    // attn_fn
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode_cached(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTevStages_cached(1);
    GXSetNumTexGens(1);
    GXSetNumIndStages(0);
    GXSetNumChans(1);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
}

void nlObjPutTrnsl_SetMaterial(struct NlMesh *pmesh)
{
    struct NlMesh mesh = *pmesh;
    GXColor color;
    u32 r28;
    u32 r25;
    u32 r26;
    u32 r27;

    switch ((mesh.flags >> 24) & 7)
    {
    case 0:
        if (s_nlMaterialCache.unk4 != 0)
        {
            GXSetBlendMode_cached(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
            s_nlMaterialCache.unk4 = 0;
            s_nlMaterialCache.unk5 = 4;
            s_nlMaterialCache.unk6 = 5;
        }
        break;
    default:
        r25 = mesh.texFlags >> 29;
        r27 = (mesh.texFlags >> 26) & 7;
        if (s_nlMaterialCache.unk4 != 2 || s_nlMaterialCache.unk5 != r25 || s_nlMaterialCache.unk6 != r27)
        {
            GXSetBlendMode_cached(GX_BM_BLEND, lbl_801B7AB4[r25], lbl_801B7AD4[r27], GX_LO_CLEAR);
            s_nlMaterialCache.unk4 = 2;
            s_nlMaterialCache.unk5 = r25;
            s_nlMaterialCache.unk6 = r27;
        }
        break;
    }

    r28 = mesh.unk4 >> 29;
    r26 = mesh.unk4 & 0x4000000;
    if (s_nlMaterialCache.unk7 != r28 || s_nlMaterialCache.unk8 != r26)
    {
        GXSetZMode_cached(GX_ENABLE, s_nlToGXCompare[r28], (!r26));
        s_nlMaterialCache.unk7 = r28;
        s_nlMaterialCache.unk8 = r26;
    }

    if (s_fogEnabled != 0)
        GXSetFog_cached(s_fogType, s_fogStartZ, s_fogEndZ, 0.1f, 20000.0f, s_fogColor);
    else
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, s_fogColor);

    if (mesh.tplTexIdx < 0)
    {
        GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GXSetTevOp_cached(GX_TEVSTAGE0, GX_PASSCLR);
    }
    else
    {
        int r25 = s_nlMaterialCache.texMapId;

        if (s_nlMaterialCache.texObj != mesh.texObj)
        {
            s_nlMaterialCache.texObj = mesh.texObj;
            if (--r25 < 0)
                r25 = 7;
            s_nlMaterialCache.texMapId = r25;
            GXLoadTexObj_cached(mesh.texObj, r25);
        }
        GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD0, r25, GX_COLOR0A0);
        switch ((mesh.texFlags >> 6) & 3)
        {
        case 0:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        case 1:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        case 2:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_RASC, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        case 3:
            GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
            GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
            GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                                   GX_TEVPREV);
            break;
        }
    }

    color.r = mesh.sub2C.materialColorR * s_renderParams.materialColor.r * 255.0f;
    color.g = mesh.sub2C.materialColorG * s_renderParams.materialColor.g * 255.0f;
    color.b = mesh.sub2C.materialColorB * s_renderParams.materialColor.b * 255.0f;
    color.a = mesh.sub2C.materialColorA * s_nlMaterialCache.alpha * 255.0f;
    if (s_nlMaterialCache.materialColor.r != color.r || s_nlMaterialCache.materialColor.g != color.g ||
        s_nlMaterialCache.materialColor.b != color.b || s_nlMaterialCache.materialColor.a != color.a)
    {
        GXSetChanMatColor(GX_COLOR0A0, color);
        s_nlMaterialCache.materialColor = color;
    }

    color.r = mesh.ambientColorScale * s_ambientColor.r * 255.0f;
    color.g = mesh.ambientColorScale * s_ambientColor.g * 255.0f;
    color.b = mesh.ambientColorScale * s_ambientColor.b * 255.0f;
    color.a = s_nlMaterialCache.materialColor.a;
    if (s_nlMaterialCache.ambientColor.r != color.r || s_nlMaterialCache.ambientColor.g != color.g ||
        s_nlMaterialCache.ambientColor.b != color.b || s_nlMaterialCache.ambientColor.a != color.a)
    {
        GXSetChanAmbColor(GX_COLOR0A0, color);
        s_nlMaterialCache.ambientColor = color;
    }

    if (s_nlMaterialCache.meshType != mesh.type)
    {
        s_nlMaterialCache.meshType = mesh.type;
        switch (mesh.type)
        {
        case NL_MODEL_TYPE_UNLIT_CONST_MAT_COLOR:
            GXSetChanCtrl(GX_COLOR0A0,   // chan
                          GX_DISABLE,    // enable
                          GX_SRC_REG,    // amb_src
                          GX_SRC_REG,    // mat_src
                          GX_LIGHT_NULL, // light_mask
                          GX_DF_CLAMP,   // diff_fn
                          GX_AF_SPOT);   // attn_fn
            break;
        case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
            GXSetChanCtrl(GX_COLOR0A0,   // chan
                          GX_DISABLE,    // enable
                          GX_SRC_VTX,    // amb_src
                          GX_SRC_VTX,    // mat_src
                          GX_LIGHT_NULL, // light_mask
                          GX_DF_CLAMP,   // diff_fn
                          GX_AF_SPOT);   // attn_fn
            break;
        case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR: // Ignored
        default:
            GXSetChanCtrl(GX_COLOR0A0,    // chan
                          GX_ENABLE,      // enable
                          GX_SRC_REG,     // amb_src
                          GX_SRC_REG,     // mat_src
                          s_lightMask, // light_mask
                          GX_DF_CLAMP,    // diff_fn
                          GX_AF_SPOT);    // attn_fn
            break;
        }
    }
}

static inline void write_vtx_type_a_with_alpha(struct NlVtxTypeA *vtx)
{
    u32 color;
    GXPosition3f32(vtx->x, vtx->y, vtx->z);
    color = vtx->color;
    GXColor4u8(((color >> 16) & 0xFF) * s_renderParams.materialColor.r,
               ((color >> 8) & 0xFF) * s_renderParams.materialColor.g,
               ((color >> 0) & 0xFF) * s_renderParams.materialColor.b,
               ((color >> 24) & 0xFF) * s_nlMaterialCache.alpha);
    GXTexCoord2f32(vtx->s, vtx->t);
}

// With global alpha param
void nlObjPutTrnsl_PutStrip_VTC(struct NlDispList *dl, void *end)
{
    gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));

    if (s_nlMaterialCache.isVtxTypeA != 1)
        s_nlMaterialCache.isVtxTypeA = TRUE;

    while (dl < (struct NlDispList *)end)
    {
        int faceCount;
        int i;
        u8 *vtxData = dl->vtxData;
        struct NlVtxTypeA *vtx;
        u8 r4 = dl->flags & 3;

        faceCount = dl->faceCount;
        if (s_nlMaterialCache.unkA != r4)
        {
            s_nlMaterialCache.unkA = r4;
            GXSetCullMode_cached(s_nlToGXCullModes[r4]);
        }

        if (dl->flags & (1 << 4))
        {
            GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, faceCount);
            while (faceCount > 0)
            {
                if (*(u32 *)vtxData & 1)
                {
                    vtx = (void *)vtxData;
                    write_vtx_type_a_with_alpha(vtx);
                    vtxData += 32;
                }
                else
                {
                    vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                    write_vtx_type_a_with_alpha(vtx);
                    vtxData += 8;
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & (1 << 3))
        {
            GXBegin(GX_TRIANGLES, GX_VTXFMT0, faceCount * 3);
            while (faceCount > 0)
            {
                for (i = 0; i < 3; i++)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        vtx = (void *)vtxData;
                        write_vtx_type_a_with_alpha(vtx);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                        write_vtx_type_a_with_alpha(vtx);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        else if (dl->flags & (1 << 2))
        {
            GXBegin(GX_QUADS, GX_VTXFMT0, faceCount * 4);
            while (faceCount > 0)
            {
                for (i = 4; i > 0; i--)
                {
                    if (*(u32 *)vtxData & 1)
                    {
                        vtx = (void *)vtxData;
                        write_vtx_type_a_with_alpha(vtx);
                        vtxData += 32;
                    }
                    else
                    {
                        vtx = (void *)(vtxData + *(u32 *)(vtxData + 4) + 8);
                        write_vtx_type_a_with_alpha(vtx);
                        vtxData += 8;
                    }
                }
                faceCount--;
            }
            GXEnd();
        }
        dl = (void *)vtxData;
    }
}

void nl2ngc_draw_model_sort_translucent_alt2(struct NlModel *model)
{
    nlObjPut(model);
}

void nl2ngc_draw_model_sort_none_alt2(struct NlModel *model)
{
    nlObjPutImm(model);
}

void nl2ngc_draw_model_alpha_sort_all_alt(struct NlModel *model, float alpha)
{
    nlObjPutTrnsl(model, alpha);
}

void nlLightMask(u32 lightMask)
{
    s_lightMask = lightMask;
}

void nlLightAmbRGB(float r, float g, float b)
{
    s_ambientColor.r = r;
    s_ambientColor.g = g;
    s_ambientColor.b = b;
}

void nlSetFog(int enabled)
{
    s_fogEnabled = enabled;
}

void nlSetFogType(u32 a, float b, float c)
{
    s_fogType = a;
    s_fogStartZ = b;
    s_fogEndZ = c;
}

void nlSetFogColor(int r, int g, int b)
{
    s_fogColor.r = r;
    s_fogColor.g = g;
    s_fogColor.b = b;
}

// Draw all opaque meshes in model immediately
void nlObjPut_OpaqueList(struct NlModel *model)
{
    struct NlMesh *mesh;

    if (model->flags & (NL_MODEL_FLAG_VTX_TYPE_A))
    {
        gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));
        s_nlMaterialCache.isVtxTypeA = TRUE;
    }
    else
    {
        gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));
        s_nlMaterialCache.isVtxTypeA = FALSE;
    }

    nlObjPut_InitRenderState();
    GXLoadTexMtxImm(textureMatrix, GX_TEXMTX0, GX_MTX2x4);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

    mesh = (struct NlMesh *)model->meshStart;
    while (mesh->flags != 0)
    {
        struct NlDispList *dlstart;
        struct NlMesh *next;

        nlObjPut_SetMaterial(mesh);
        next = (void *)(mesh->dispListStart + mesh->dispListSize);
        if (((mesh->flags >> 24) & 7) != 0)
            mesh = next;
        else
        {
            dlstart = (void *)(mesh->dispListStart);
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                nlObjPut_PutStrip_VTC(dlstart, next);
                break;
            default:
                nlObjPut_PutStrip(dlstart, next);
                break;
            }
            mesh = next;
        }
    }
    unk_empty();
}

static void nlObjPut_TrnslList_OP(struct DrawModelDeferredNode *a)
{
    float f31, f30, f29;

    mathutil_mtxA_from_mtx(a->viewFromModel);

    f31 = s_renderParams.materialColor.r;
    f30 = s_renderParams.materialColor.g;
    f29 = s_renderParams.materialColor.b;

    s_renderParams.materialColor.r = a->materialColor.r;
    s_renderParams.materialColor.g = a->materialColor.g;
    s_renderParams.materialColor.b = a->materialColor.b;
    if (!(a->model->flags & (1 << 10)))
    {
        load_light_group_cached(a->lightGroup);
        nlLightAmbRGB(a->ambientColor.r, a->ambientColor.g, a->ambientColor.b);
    }
    s_fogEnabled = a->fogEnabled;
    nlObjPut_TrnslList(a->model);

    s_renderParams.materialColor.r = f31;
    s_renderParams.materialColor.g = f30;
    s_renderParams.materialColor.b = f29;
}

void nlObjPut_TrnslList(struct NlModel *model)
{
    struct NlMesh *mesh;

    if (model->flags & (NL_MODEL_FLAG_VTX_TYPE_A))
    {
        gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));
        s_nlMaterialCache.isVtxTypeA = TRUE;
    }
    else
    {
        gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));
        s_nlMaterialCache.isVtxTypeA = FALSE;
    }

    nlObjPut_InitRenderState();
    GXLoadTexMtxImm(textureMatrix, GX_TEXMTX0, GX_MTX2x4);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

    mesh = (struct NlMesh *)model->meshStart;
    while (mesh->flags != 0)
    {
        struct NlDispList *dlstart;
        struct NlMesh *next;

        nlObjPut_SetMaterial(mesh);
        next = (void *)(mesh->dispListStart + mesh->dispListSize);
        if (((mesh->flags >> 24) & 7) == 0)
            mesh = next;
        else
        {
            dlstart = (void *)(mesh->dispListStart);
            switch (mesh->type)
            {
            case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                break;
            case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                nlObjPut_PutStrip_VTC(dlstart, next);
                break;
            default:
                nlObjPut_PutStrip(dlstart, next);
                break;
            }
            mesh = next;
        }
    }
    unk_empty();
}

void nlObjPutTrnsl_TrnslList_OP(struct DrawAlphaModelDeferredNode *a)
{
    float f31, f30, f29;

    mathutil_mtxA_from_mtx(a->viewFromModel);

    f31 = s_renderParams.materialColor.r;
    f30 = s_renderParams.materialColor.g;
    f29 = s_renderParams.materialColor.b;

    s_renderParams.materialColor.r = a->materialColor.r;
    s_renderParams.materialColor.g = a->materialColor.g;
    s_renderParams.materialColor.b = a->materialColor.b;
    s_nlMaterialCache.alpha = a->alpha;
    if (!(a->model->flags & (1 << 10)))
    {
        load_light_group_cached(a->lightGroup);
        nlLightAmbRGB(a->ambientColor.r, a->ambientColor.g, a->ambientColor.b);
    }
    s_fogEnabled = a->fogEnabled;
    nlObjPutTrnsl_TrnslList(a->model);

    s_renderParams.materialColor.r = f31;
    s_renderParams.materialColor.g = f30;
    s_renderParams.materialColor.b = f29;
}

// Draw all meshes in model immediately with global alpha parameter
void nlObjPutTrnsl_TrnslList(struct NlModel *model)
{
    struct NlMesh *mesh;

    if (model->u_valid == -1)
        return;

    if (model->flags & (NL_MODEL_FLAG_VTX_TYPE_A))
    {
        gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));
        s_nlMaterialCache.isVtxTypeA = TRUE;
    }
    else
    {
        gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));
        s_nlMaterialCache.isVtxTypeA = FALSE;
    }

    nlObjPutTrnsl_InitRenderState();
    GXLoadTexMtxImm(textureMatrix, GX_TEXMTX0, GX_MTX2x4);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

    mesh = (struct NlMesh *)model->meshStart;
    while (mesh->flags != 0)
    {
        struct NlDispList *dlstart;
        struct NlMesh *next;

        nlObjPutTrnsl_SetMaterial(mesh);
        next = (void *)(mesh->dispListStart + mesh->dispListSize);
        dlstart = (void *)(mesh->dispListStart);
        switch (mesh->type)
        {
        case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
            break;
        case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
            nlObjPutTrnsl_PutStrip_VTC(dlstart, next);
            break;
        default:
            nlObjPut_PutStrip(dlstart, next);
            break;
        }
        mesh = next;
    }
    unk_empty();
}

void u_nl2ngc_draw_model_with_mesh_func(struct NlModel *model, int (*func)())
{
    struct NlMesh *mesh;

    if (model->u_valid != -1)
    {
        s_renderParams.prevScale = s_renderParams.scale;
        if (s_renderParams.scale == 1.0f)
        {
            if (test_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius) == 0)
                return;
        }
        else
        {
            if (test_scaled_sphere_in_frustum(&model->boundSphereCenter, model->boundSphereRadius,
                                              s_renderParams.prevScale) == 0)
            {
                s_renderParams.scale = 1.0f;
                return;
            }
        }
        if (model->flags & (NL_MODEL_FLAG_VTX_TYPE_A))
        {
            gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_CLR0) | (1 << GX_VA_TEX0));
            s_nlMaterialCache.isVtxTypeA = TRUE;
        }
        else
        {
            gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_NRM) | (1 << GX_VA_TEX0));
            s_nlMaterialCache.isVtxTypeA = FALSE;
        }
        s_nlMaterialCache.alpha = 1.0f;

        mesh = (struct NlMesh *)model->meshStart;
        while (mesh->flags != 0)
        {
            struct NlDispList *dlstart;
            struct NlMesh *next = (void *)(mesh->dispListStart + mesh->dispListSize);

            if (func(mesh, mesh->dispListSize) == 0)
                mesh = next;
            else
            {
                dlstart = (void *)(mesh->dispListStart);
                switch (mesh->type)
                {
                case NL_MODEL_TYPE_LIT_CONST_MAT_COLOR:
                    break;
                case NL_MODEL_TYPE_UNLIT_VERT_MAT_COLOR:
                    nlObjPut_PutStrip_VTC(dlstart, next);
                    break;
                default:
                    nlObjPut_PutStrip(dlstart, next);
                    break;
                }
                mesh = next;
            }
        }
    }
}

void unk_empty(void)
{
}
