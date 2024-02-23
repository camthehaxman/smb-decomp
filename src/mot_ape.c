#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dolphin.h>
#include <ppcintrinsic.h>

#include "global.h"
#include "avdisp.h"
#include "ball.h"
#include "camera.h"
#include "gma.h"
#include "gxutil.h"
#include "light.h"
#include "load.h"
#include "mathutil.h"
#include "mode.h"
#include "mot_ape.h"
#include "nl2ngc.h"
#include "ord_tbl.h"
#include "polydisp.h"
#include "thread.h"
#include "window.h"

static struct SomeMotInfoStruct lbl_801C7A70 =
{
    0.0f,
    0,
    0,
    0,
    1,
    0,
    1.0f,
    0,
};

static u32 u_animMatrixIdxToJointIdx[] = { 1, 5, 7, 8, 12, 13, 16, 18, 19, 21, 22, 23, 24, 26, 27 };

// yaw angles for something?
static float lbl_801C7ACC[] = { 23.0f, 21.1f, 22.0f, 21.5f };
static u32 lbl_801C7ADC[] = { 16, 8, 32, 6, 9, 16, 24, 4, 6, 4, 14, 20, 24, 30, 20, 20 };
static s32 lbl_801C7B1C[] = { 0, 4, 6, 14, 6, 10 };

static void ape_dummy_1(struct Ape *);
static void ape_dummy_2(struct Ape *);
static void ape_dummy_3(struct Ape *);
static void ape_dummy_4(struct Ape *);
static void find_shirt_materials_in_gma(enum Character charaId, int lod, struct GMAModel *model1, struct GMAModel *model2);
static void recolor_ape_shirt(struct Ape *ape, struct GMAModel *unused);
static void mot_ape_8008CAAC(struct Ape *, float);

// none of these functions actually do anything
static void (*apeDummyFuncs[])(struct Ape *) =
{
    ape_dummy_1,
    ape_dummy_2,
    ape_dummy_3,
    ape_dummy_4,
    ape_dummy_4,
    ape_dummy_4,
    ape_dummy_4,
    ape_dummy_4,
    ape_dummy_4,
    ape_dummy_4,
};

// bss
static Mtx workMatrix;
static u32 uselessArray[0x20];
static struct Ape apeStructs[16];
static struct Ape *apeStructPtrs[16];

struct GMA *charaGMAs[8];  // GMAs for each character and LOD
struct TPL *charaTPLs[8];  // TPLs associated with the GMAs
u32 charaTplSizes[8];  // TPL sizes for each character and LOD
u32 charaTplAramAddrs[8];  // TPL addresses for each character and LOD
u32 charaGmaSizes[8];  // GMA sizes for each character and LOD
u32 charaGmaAramAddrs[8];  // GMA addresses for each character and LOD
u32 shirtTplSizes[4];
u32 shirtTplAramAddrs[4];
struct TPL *shirtTPLs[4];
GXTexObj *shirtTexObjs[4];
struct GMAShape *lbl_802B47F0[4];
struct GMATevLayer *apeShirtMaterials[8];  // pointers to shirt materials in the GMA for each character and LOD
Mtx lbl_802B4820;
Mtx lbl_802B4850[15];
u8 unused802B4B20[0x2D0];
GXTexObj *u_apeTextures[8];

u32 *motLabel;
s32 u_motAnimCount;
struct SkeletonFileData *motsklFileData;
struct MotInfo *motInfo;
u8 charaModelLoaded[8];
u8 lbl_802F2094[8];
int lbl_802F2090;
u8 lbl_802F208C;
Mtx **u_animTransformMatrices;
struct NlObj *apeFaceObj;
struct TPL *apeFaceTpl;
s32 lbl_802F207C;
float u_globalAnimSpeedScale;  // seems to control the speed of all animations, where 1 is normal speed
s8 lbl_802F2074;
volatile int transferInProgress;
s32 lbl_802F206C;
u32 nextApeIndex;

static void aram_transfer_callback(u32 a)
{
    transferInProgress = FALSE;
}

static void load_character_resources(void)
{
    OSHeapHandle oldHeap;
    ARQRequest arqReq;
    struct File file;
    char filename[0x100];
    char *charaNames[] =
    {
        "BOY H",  // why no underscore here?
        "BOY_L",
        "GAL_H",
        "GAL_L",
        "KID_H",
        "KID_L",
        "GOR_H",
        "GOR_L",
    };
    char *shirtTplNames[] =
    {
        "boy_ch.tpl.lz",
        NULL,
        "kid_ch.tpl.lz",
        "gor_ch.tpl.lz",
    };
    void *work;
    u32 aramTop = 0xFF0000;
    char *suffix;
    int baseLen;
    void *compressed;
    u32 i;
    u32 j;

    DVDChangeDir("ape");
    lbl_802F208C = 0;
    oldHeap = OSSetCurrentHeap(charaHeap);
    work = OSAlloc(0x280020);
    lbl_802F2090 = 1;
    if (work == NULL)
        OSPanic("mot_ape.c", 230, "cannot OSAlloc\n");
    if (debugFlags & (1 << 2))
        printf(" ===== Character Model Information =====\n");

    for (i = 0; i < 4; i++)
    {
        motInfo[i].unkB0 = (void *)motInfo;
        for (j = 0; j < 2; j++)
        {
            u32 index = i * 2 + j;
            int fileSize;

            // not loaded yet
            charaGMAs[index] = NULL;
            charaTPLs[index] = NULL;
            charaModelLoaded[index] = 0;
            lbl_802F2094[index] = 0;

            baseLen = strlen(apeGfxFileInfo[index].basename);
            strcpy(filename, apeGfxFileInfo[index].basename);
            suffix = filename + baseLen;

            // Load the TPL file into ARAM
            strcpy(suffix, ".tpl.lz");
            if (file_open(filename, &file) == 0)
            {
                printf("No Chara Texture <%s>\n", apeGfxFileInfo[index].basename);
                OSPanic("mot_ape.c", 250, "Stopped...\n");
            }
            fileSize = OSRoundUp32B(file_size(&file));
            compressed = OSAllocFromHeap(mainHeap, fileSize);
            if (compressed == NULL)
                OSPanic("mot_ape.c", 254, "cannot OSAlloc");
            file_read(&file, compressed, fileSize, 0);
            charaTplSizes[index] = __lwbrx(compressed, 4);
            aramTop -= charaTplSizes[index];
            if (aramTop < 0xB00000)
                OSPanic("mot_ape.c", 260, "Not enough ARAM for CharacterModel.\n");
            charaTplAramAddrs[index] = aramTop;
            lzs_decompress(compressed, work);
            OSFreeToHeap(mainHeap, compressed);
            DCStoreRange(work, charaTplSizes[index]);
            transferInProgress = TRUE;
            ARQPostRequest(
                &arqReq,
                0,
                ARQ_TYPE_MRAM_TO_ARAM,
                ARQ_PRIORITY_HIGH,
                (uintptr_t)work,
                aramTop,
                (((charaTplSizes[index] - 1) >> 2) + 1) << 2,
                aram_transfer_callback);
            while (transferInProgress != 0)
                ;
            file_close(&file);

            // Load the GMA file into ARAM
            strcpy(suffix, ".gma.lz");
            if (file_open(filename, &file) == 0)
            {
                printf("No Chara Model <%s>\n", apeGfxFileInfo[index].basename);
                OSPanic("mot_ape.c", 277, "Stopped...\n");
            }
            fileSize = OSRoundUp32B(file_size(&file));
            compressed = OSAllocFromHeap(mainHeap, fileSize);
            if (compressed == NULL)
                OSPanic("mot_ape.c", 281, "cannot OSAlloc");
            file_read(&file, compressed, fileSize, 0);
            charaGmaSizes[index] = __lwbrx(compressed, 4);
            aramTop -= charaGmaSizes[index];
            if (aramTop < 0xB00000)
                OSPanic("mot_ape.c", 287, "Not enough ARAM for CharacterModel.\n");
            charaGmaAramAddrs[index] = aramTop;
            lzs_decompress(compressed, work);
            DCStoreRange(work, charaGmaSizes[index]);
            OSFreeToHeap(mainHeap, compressed);
            transferInProgress = TRUE;
            ARQPostRequest(
                &arqReq,
                0,
                ARQ_TYPE_MRAM_TO_ARAM,
                ARQ_PRIORITY_HIGH,
                (uintptr_t)work,
                aramTop,
                (((charaGmaSizes[index] - 1) >> 2) + 1) << 2,
                aram_transfer_callback);
            while (transferInProgress != 0)
                ;
            file_close(&file);

            if (debugFlags & (1 << 2))
                printf("%s : Polygon[ %x ] Texture[ %x ]\n",
                    charaNames[index], charaGmaSizes[index], charaTplSizes[index]);
        }
    }

    for (i = 0; i < 4; i++)
    {
        int fileSize;
        void *compressed;

        if (shirtTplNames[i] != NULL)
        {
            file_open(shirtTplNames[i], &file);
            fileSize = OSRoundUp32B(file_size(&file));
            compressed = OSAllocFromHeap(mainHeap, fileSize);
            if (compressed == NULL)
                OSPanic("mot_ape.c", 311, "cannot OSAlloc");
            file_read(&file, compressed, fileSize, 0);
            shirtTplSizes[i] = __lwbrx(compressed, 4);
            aramTop -= shirtTplSizes[i];
            shirtTplAramAddrs[i] = aramTop;
            lzs_decompress(compressed, work);
            DCStoreRange(work, shirtTplSizes[i]);
            OSFreeToHeap(mainHeap, compressed);
            transferInProgress = 1;
            ARQPostRequest(
                &arqReq,
                0,
                ARQ_TYPE_MRAM_TO_ARAM,
                ARQ_PRIORITY_HIGH,
                (uintptr_t)work,
                aramTop,
                (((shirtTplSizes[i] - 1) >> 2) + 1) << 2,
                aram_transfer_callback);
            while (transferInProgress != 0)
                ;
            file_close(&file);
            if (debugFlags & (1 << 2))
                printf("change texture load. %s [ %x ]\n",
                    shirtTplNames[i], shirtTplSizes[i]);
        }
        else
        {
            shirtTplSizes[i] = 0;
            shirtTplAramAddrs[i] = 0;
        }
        shirtTPLs[i] = NULL;
    }
    DVDChangeDir("..");
    if (debugFlags & (1 << 2))
        printf("   Use ARAM Memory top : %x\n", aramTop);
    if (debugFlags & (1 << 2))
        printf(" =======================================\n");
    OSFree(work);
    OSSetCurrentHeap(oldHeap);
}

static void u_find_ape_body_part(const struct ApeGfxFileInfo *filesInfo, int lod, struct BodyPartNameInfo *c)
{
    int i;
    const struct ApeGfxFileInfo *thisFile = &filesInfo[(lod >> 1) & 1];

    for (i = 0; i < thisFile->partCounts[lod & 1]; i++)
    {
        struct BodyPartDesc *partDescs = thisFile->bodyPartInfo[lod & 1];

        if (strcmp(partDescs[i].name, c->names[lod]) == 0)
        {
            c->u_someLodRelatedIndexes[lod] = i;
            return;
        }
    }
    printf("obj: %s is nothing.\n", c->names[lod]);
    c->u_someLodRelatedIndexes[lod] = -1;
}

static struct ApeAnimationThing *u_create_joints_probably(struct Skeleton *skel)
{
    struct ApeAnimationThing *r30;

    if (gameSubmode == SMD_MINI_BILLIARDS_INIT || gameSubmode == SMD_MINI_BILLIARDS_MAIN)
    {
        lbl_802F2074 = 2;
        r30 = OSAllocFromHeap(backgroundHeap, sizeof(*r30));
    }
    else
    {
        lbl_802F2074 = 0;
        r30 = OSAllocFromHeap(subHeap, sizeof(*r30));
    }
    if (r30 == NULL)
        OSPanic("mot_ape.c", 396, "rob init Heap Over.\n");
    r30->u_animId = 1;
    r30->u_currKeyframe = 1;
    r30->unk36 = 0;
    r30->unk0 = 0;
    r30->u_nextAnimId = 0xFFFF;
    r30->u_someDeltaTime = 1.0f;
    r30->u_timeInKeyframe = 0.0f;
    r30->unk2C = 0x4000;
    r30->unk2E = 0x4000;
    r30->unk2A = 0;
    r30->unk28 = 0;
    u_create_joints_from_skeleton(r30->joints, skel, r30->unk36);
    func_80035FDC(r30);
    u_mot_joint_start_anim(r30);
    mot_joint_800355FC(r30);
    return r30;
}

static void u_iter_joints_80089BD4(struct AnimJoint *joint)
{
    int i;
    Vec spC;
    struct AnimJoint *var = joint;

    for (i = 0; i < 29; i++)
    {
        if (!(joint->flags & 1))
        {
            joint++;
            continue;
        }
        mathutil_mtxA_from_mtx(joint->transformMtx);
        mathutil_mtxA_to_quat(&joint->unk1B0);
        if (joint->parentIdx != 0xFFFFFFFF)
        {
            struct AnimJoint *parent = &var[joint->parentIdx];

            // find root joint
            while (parent->parentIdx != 0xFFFFFFFF)
            {
                if (parent->flags & 1)
                    break;
                parent = &var[parent->parentIdx];
            }
            mathutil_mtxA_from_mtx(parent->transformMtx);
        }
        else
            mathutil_mtxA_from_identity();
        spC.x = joint->transformMtx[0][3];
        spC.y = joint->transformMtx[1][3];
        spC.z = joint->transformMtx[2][3];
        mathutil_mtxA_rigid_inv_tf_point(&spC, &joint->unk1A4);
        joint++;
    }
}

static struct Struct80089CBC *func_80089CBC(void *unused, int b, int c)
{
    struct Struct80089CBC *r6 = (void *)((u8 *)motInfo + 0x28000);

    while (c != 0)
    {
        struct Struct80089CBC *r3 = &r6[c];

        if (r3->unk10 == b)
            return r3;
        c = r3->unk14;
    }
    return NULL;
}

static inline void func_80089CF4_inline(struct Ape *ape)
{
    int i;

    for (i = 0; i < ape->u_bodyPartCount; i++)
    {
        struct BodyPartNameInfo *r24 = &lbl_801C63B0[ape->charaId][i];

        if (ape->u_bodyParts[i].unkC == NULL)
            ape->u_bodyParts[i].unkC = func_80089CBC(ape->u_bodyParts, i, ape->unk1C->unk1C);
        ape->u_bodyParts[i].u_someLodRelatedIndexes[0] = r24->u_someLodRelatedIndexes[0];
        ape->u_bodyParts[i].u_someLodRelatedIndexes[1] = r24->u_someLodRelatedIndexes[1];
        ape->u_bodyParts[i].u_someLodRelatedIndexes[2] = r24->u_someLodRelatedIndexes[2];
    }
}

static void u_start_new_anim_maybe(struct Ape *ape, int animId)
{
    struct ApeAnimationThing *r6 = ape->unk0;
    float f31 = (float)r6->u_currKeyframe / (float)r6->u_keyframeCount;
    struct ApeAnimationThing *r28;
    struct ApeAnimationThing *r27;
    u32 i;
    u8 dummy[0x10];

    if (ape->unk1C->unkC & (1 << 2))
        ape->flags |= (1 << 9);
    else
        ape->flags &= ~(1 << 9);
    if (ape->unk20 != NULL)
    {
        ape->unk1C = ape->unk20;
        ape->unk20 = NULL;
        animId = ape->unk1C->animId;
    }
    if (ape->flags & (1 << 9))
    {
        // swap
        struct ApeAnimationThing *temp = ape->unk0;
        ape->unk0 = ape->unk4;
        ape->unk4 = temp;
    }
    else
    {
        u_iter_joints_80089BD4(ape->unk0->joints);
    }
    r27 = ape->unk0;
    r28 = ape->unk4;
    ape->animTimerCurr = 0.0f;
    ape->unkC2 = 0;
    ape->flags &= ~((1 << 12)|(1 << 13));
    if (lbl_802F206C != 0)
        ape->animTimerMax = ape->unk1C->unk14;
    else
        ape->animTimerMax = 0.0f;
    r27->u_animId = animId;
    r27->u_currKeyframe = 1;
    ape->unk24 = ape->unk28;
    ape->unk9C = ape->unkB0;
    ape->unk18 = ape->animTimerMax + 1.0f;
    r27->u_timeInKeyframe = 0.0f;
    func_80035FDC(r27);
    if (r27->u_animId == 0)
        r27->u_animId = 1;
    u_mot_joint_start_anim(r27);
    if (ape->animTimerMax > 9.9999999392252903e-09f && (ape->flags & (1 << 9)))
    {
        float f4 = ((float)r28->u_keyframeCount / r28->u_someDeltaTime);

        f4 /= ((float)r27->u_keyframeCount / r27->u_someDeltaTime);

        if (f4 < 1.0f)
        {
            r28->u_someDeltaTime *= f4;
            r27->u_someDeltaTime = ape->unk1C->u_maybeSpeed;
        }
        else
            r27->u_someDeltaTime = ape->unk1C->u_maybeSpeed / f4;
    }
    else
        r27->u_someDeltaTime = ape->unk1C->u_maybeSpeed;
    if (ape->unk1C->unkC & (1 << 3))
    {
        r27->u_currKeyframe = f31 * r27->u_keyframeCount;
        if (r27->u_currKeyframe >= r27->u_keyframeCount)
            r27->u_currKeyframe = 1;
    }
    for (i = 0; i < ape->u_bodyPartCount; i++)
    {
        struct BodyPart *part = &ape->u_bodyParts[i];

        part->unkC = 0;
        part->u_someLodRelatedIndexes[3] = 0;
    }
    func_80089CF4_inline(ape);
    for (i = 0; i < ape->u_bodyPartCount; i++)
    {
        struct BodyPart *part = &ape->u_bodyParts[i];

        if (part->unkC != NULL)
            part->unk10 = 0.0f;
        mot_ape_8008A55C(ape->charaId, part, r27->u_keyframeCount, 1);
        if (part->unkC == NULL)
        {
            if (part->unk4 > 0.0f)
                part->unk10 = -2.0f;
            else
                part->unk10 = 2.0f;
            part->unk8 = 0;
        }
    }
}

static void emptyfunc(void) {}

static int return_0(void)
{
    return 0;
}

static void ape_dummy_1(struct Ape *ape) {}

static void ape_dummy_2(struct Ape *ape) {}

static void ape_dummy_3(struct Ape *ape) {}

static void ape_dummy_4(struct Ape *ape) {}

static void u_iter_joints_8008A124(struct AnimJoint *joint, float b)
{
    int i;
    struct AnimJoint *a = joint;

    for (i = 0; i < 29; i++)
    {
        Vec sp20;
        Quaternion sp10;

        if (!(joint->flags & 1))
        {
            joint++;
            continue;
        }
        mathutil_mtxA_from_mtx(joint->transformMtx);
        mathutil_mtxA_to_quat(&sp10);
        mathutil_quat_slerp(&sp10, &joint->unk1B0, &sp10, b);
        mathutil_quat_normalize(&sp10);
        if (joint->parentIdx != 0xFFFFFFFF)
        {
            struct AnimJoint *parent = &a[joint->parentIdx];

            // find root joint
            while (parent->parentIdx != 0xFFFFFFFF)
            {
                if (parent->flags & 1)
                    break;
                parent = &a[parent->parentIdx];
            }
            mathutil_mtxA_from_mtx(parent->transformMtx);
            mathutil_mtxA_translate(&joint->unk1A4);
        }
        else
        {
            mathutil_mtxA_from_identity();
            sp20.x = joint->unk1A4.x + (joint->transformMtx[0][3] - joint->unk1A4.x) * b;
            sp20.y = joint->unk1A4.y + (joint->transformMtx[1][3] - joint->unk1A4.y) * b;
            sp20.z = joint->unk1A4.z + (joint->transformMtx[2][3] - joint->unk1A4.z) * b;
            mathutil_mtxA_set_translate(&sp20);
        }
        mathutil_mtxA_get_translate_alt(&sp20);
        mathutil_mtxA_from_quat(&sp10);
        mathutil_mtxA_normalize_basis();
        mathutil_mtxA_set_translate(&sp20);
        mathutil_mtxA_to_mtx(joint->transformMtx);
        joint++;
    }
}

static void u_iter_joints_8008A2C4(struct AnimJoint *joint)
{
    int i;
    struct AnimJoint *a = joint;

    for (i = 0; i < 29; i++)
    {
        Vec spC;

        if (!(joint->flags & 1))
        {
            joint++;
            continue;
        }
        mathutil_mtxA_from_mtx(joint->transformMtx);
        if (joint->parentIdx != 0xFFFFFFFF)
        {
            struct AnimJoint *parent = &a[joint->parentIdx];

            // find root joint
            while (parent->parentIdx != 0xFFFFFFFF)
            {
                if (parent->flags & 1)
                    break;
                parent = &a[parent->parentIdx];
            }
            mathutil_mtxA_from_mtx(parent->transformMtx);
        }
        else
        {
            mathutil_mtxA_from_identity();
        }
        spC.x = joint->transformMtx[0][3];
        spC.y = joint->transformMtx[1][3];
        spC.z = joint->transformMtx[2][3];
        mathutil_mtxA_rigid_inv_tf_point(&spC, &joint->unk1A4);
        joint++;
    }
}

static void u_iter_joints_8008A3A4(struct AnimJoint *r28, struct AnimJoint *r29, float c)
{
    int i;
    struct AnimJoint *a = r28;

    for (i = 0; i < 29; i++)
    {
        Vec sp30;
        Quaternion sp20;
        Vec sp14;

        if (!(r28->flags & 1))
        {
            r28++;
            r29++;
            continue;
        }
        mathutil_mtxA_from_mtx(r29->transformMtx);
        mathutil_mtxA_to_quat(&r29->unk1B0);
        mathutil_mtxA_from_mtx(r28->transformMtx);
        mathutil_mtxA_to_quat(&sp20);
        mathutil_quat_slerp(&sp20, &r29->unk1B0, &sp20, c);
        mathutil_quat_normalize(&sp20);
        if (r28->parentIdx != 0xFFFFFFFF)
        {
            struct AnimJoint *parent = &a[r28->parentIdx];

            // find root joint
            while (parent->parentIdx != 0xFFFFFFFF)
            {
                if (parent->flags & 1)
                    break;
                parent = &a[parent->parentIdx];
            }
            mathutil_mtxA_from_mtx(parent->transformMtx);
            mathutil_mtxA_translate(&r28->unk1A4);
        }
        else
        {
            mathutil_mtxA_from_identity();
            sp14.x = r29->transformMtx[0][3];
            sp14.y = r29->transformMtx[1][3];
            sp14.z = r29->transformMtx[2][3];
            sp30.x = r28->transformMtx[0][3];
            sp30.y = r28->transformMtx[1][3];
            sp30.z = r28->transformMtx[2][3];
            mathutil_scale_ray(&sp14, &sp30, &sp30, c);
            mathutil_mtxA_set_translate(&sp30);
        }
        mathutil_mtxA_get_translate_alt(&sp30);
        mathutil_mtxA_from_quat(&sp20);
        mathutil_mtxA_normalize_basis();
        mathutil_mtxA_set_translate(&sp30);
        mathutil_mtxA_to_mtx(r28->transformMtx);
        r28++;
        r29++;
    }
}

extern const double lbl_802F56D8;

// something related to face animation?
void mot_ape_8008A55C(u32 a, struct BodyPart *part, int c, int d)
{
    int dummy;
    int r8;

    part->unk4 += part->unk10;
    if (fabs(part->unk10) < lbl_802F56D8)
        r8 = dummy = part->unkC != NULL;
    else if (part->unk10 < 0.0f)
        r8 = dummy = part->unk4 < part->unk8;
    else
        r8 = dummy = part->unk4 >= part->unk8;
    if (part->u_someLodRelatedIndexes[3] < 0 && part->unkC != NULL)
        r8 = 1;
    if (r8)
        part->unk4 = part->unk8;
    if (--part->u_someLodRelatedIndexes[3] > 0)
        return;
    if (!r8)
        return;
    if (part->unkC != NULL)
    {
        struct Struct80089CBC *r7 = part->unkC;

        part->type = r7->unk0 & ~(1 << 31);
        part->unk8 = r7->unk8;
        if (r7->unk4 < d)
            part->u_someLodRelatedIndexes[3] = (c + r7->unk4) - d;
        else
            part->u_someLodRelatedIndexes[3] = r7->unk4 - d;
        if (part->u_someLodRelatedIndexes[3] == 0)
            part->unk10 = 0.0f;
        else
            part->unk10 = ((float)part->unk8 - part->unk4) / part->u_someLodRelatedIndexes[3];
        if (r7->unkC < 0x10000000)
        {
            if (r7->unkC > 0x0C000000)
                part->unkC = (void *)(r7->unkC + 0xF36E0000 + (uintptr_t)motInfo);
            else if (r7->unkC != 0)
            {
#ifdef NONMATCHING
                part->unkC = (void *)((u8 *)motInfo[a].unkB0 + r7->unkC);
#else
                struct MotInfo *info = &motInfo[a];
                uintptr_t ptr = r7->unkC;
                ptr += (uintptr_t)info->unkB0;
                part->unkC = (void *)ptr;
#endif
            }
            else
                part->unkC = NULL;
        }
        else
        {
            if (r7->unk0 > 0x100)
                part->unkC = 0;
            else if (r7->unkC >= (uintptr_t)motInfo + 0x80000)
                part->unkC = 0;
            else
                part->unkC = (void *)r7->unkC;
        }
    }
    else
    {
        part->unk4 = part->unk8;
        part->unk10 = 0.0f;
        part->u_someLodRelatedIndexes[3] = 0;
    }
}

static void func_8008A7F0_inline(struct Ape *ape, struct ApeAnimationThing *b)
{
    struct BodyPart *r28;
    int i;
    r28 = ape->u_bodyParts;

    for (i = 0; i < ape->u_bodyPartCount; i++)
        mot_ape_8008A55C(ape->charaId, r28++, b->u_keyframeCount, b->u_currKeyframe);
}

static void u_advance_ape_anim(struct Ape *ape, struct ApeAnimationThing *b)
{
    u8 dummy[16];

    // advance to next keyframe if this one is over?
    if (b->u_timeInKeyframe >= 1.0f)
    {
        if (!(ape->flags & (1 << 13)))
        {
            do
            {
                b->u_currKeyframe++;
                b->u_timeInKeyframe -= 1.0f;
            } while (b->u_timeInKeyframe >= 1.0f);
        }
        else
            b->u_timeInKeyframe = 0.0f;
        func_8008A7F0_inline(ape, b);
    }

    // advance to the next animation if this one is over
    if (b->u_currKeyframe >= b->u_keyframeCount)
    {
        if (!(ape->flags & (1 << 12)))
        {
            if (ape->unk1C->unkC & (1 << 4))
                b->u_currKeyframe = ape->unk1C->unk8;
            else
                b->u_currKeyframe = 1;
            ape->unkC2++;
            if (b->u_nextAnimId != 0xFFFF)
            {
                b->u_animId = b->u_nextAnimId;
                b->u_nextAnimId = 0xFFFF;
                u_start_new_anim_maybe(ape, b->u_animId);
            }
            else
            {
                func_80035FDC(b);
                u_mot_joint_start_anim(b);
                func_80089CF4_inline(ape);
                func_8008A7F0_inline(ape, b);
            }
        }
        else
        {
            ape->flags |= 0x2000;
            if (b->u_nextAnimId != 0xFFFF)
            {
                b->u_animId = b->u_nextAnimId;
                b->u_nextAnimId = 0xFFFF;
                u_start_new_anim_maybe(ape, b->u_animId);
            }
        }
    }
    mot_joint_800355FC(b);
}

// Frees either the high poly or low poly GMA and TPL for the character
// If lod is 0, the high poly resources are freed
// If lod is 1, the low poly resources are freed
static void u_free_character_graphics(int chara, int lod)
{
    int index = chara * 2;
    OSHeapHandle oldHeap;

    if (lod > 1)
        lod = 1;
    index += lod;

    if (charaModelLoaded[index] == 0)
    {
        printf("\nchara model is already free.\n");
        return;
    }
    charaModelLoaded[index]--;

    if (lbl_802F208C != 0)
        lbl_802F208C--;
    if (lbl_802F2094[index] != 0)
        oldHeap = OSSetCurrentHeap(charaHeap);
    else
        oldHeap = OSSetCurrentHeap(charaHeap);
    if (charaTPLs[index] != NULL || charaGMAs[index] != NULL)
    {
        VISetNextFrameBuffer(gfxBufferInfo->currFrameBuf);
        VIWaitForRetrace();
    }
    if (charaTPLs[index] != NULL)
    {
        free_tpl(charaTPLs[index]);
        charaTPLs[index] = NULL;
    }
    if (charaGMAs[index] != NULL)
    {
        free_gma(charaGMAs[index]);
        charaGMAs[index] = NULL;
    }
    if (lbl_802F2090 != 0)
    {
        if (shirtTPLs[chara] != NULL)
        {
            free_tpl(shirtTPLs[chara]);
            shirtTPLs[chara] = NULL;
        }
        if (shirtTexObjs[chara] != NULL)
        {
            OSFree(shirtTexObjs[chara]);
            shirtTexObjs[chara] = NULL;
        }
    }
    lbl_802F2090 = 1;
    OSSetCurrentHeap(oldHeap);
}

static struct GMAShape *next_shape(struct GMAShape *mesh);

static void *u_find_some_mesh_with_red(struct GMAModel *model)
{
    struct GMAShape *mesh;
    int i;

    mesh = (void *)((u8 *)model + model->shapeOffs);
    for (i = 0; i < model->opaqueShapeCount; i++)
    {
        if (mesh->materialColor.r == 0xFF)
            return mesh;
        mesh = next_shape(mesh);
    }
    return NULL;
}

// Loads the GMA and TPL for the character model
// If lod is 0, the high poly model is loaded
// If lod is 1, the low poly model is loaded
static void u_load_character_graphics(enum Character chara, int lod)
{
    OSHeapHandle oldHeap;
    int index = chara * 2;

    if (lod > 1)
        lod = 1;
    index += lod;

    if (charaModelLoaded[index] != 0)
    {
        // already loaded
        charaModelLoaded[index]++;
        return;
    }

    lbl_802F208C++;
    if (lbl_802F208C > 8)
    {
        int inSelInit = (gameMode == MD_SEL && gameSubmode == SMD_SEL_NGC_INIT);
        int inBilliards = (gameMode == MD_MINI && (gameSubmode == SMD_MINI_BILLIARDS_INIT || gameSubmode == SMD_MINI_BILLIARDS_MAIN));

        if (inSelInit | inBilliards)
        {
            lbl_802F2094[index] = 1;
            oldHeap = OSSetCurrentHeap(charaHeap);
        }
        else
            OSPanic("mot_ape.c", 1314, "APE Allocate limit over!!\n");
    }
    else
    {
        lbl_802F2094[index] = 0;
        oldHeap = OSSetCurrentHeap(charaHeap);
    }

    if (shirtTplAramAddrs[chara] != NULL)
    {
        shirtTPLs[chara] = load_tpl_from_aram(shirtTplAramAddrs[chara], shirtTplSizes[chara]);
        shirtTexObjs[chara] = create_tpl_tex_objs(shirtTPLs[chara]);
    }

    charaTPLs[index] = load_tpl_from_aram(charaTplAramAddrs[index], charaTplSizes[index]);
    charaGMAs[index] = load_gma_from_aram(charaGmaAramAddrs[index], charaGmaSizes[index], charaTPLs[index]);
    charaModelLoaded[index]++;
    OSSetCurrentHeap(oldHeap);

    if (chara == CHARACTER_MEEMEE)
    {
        struct GMAModel *model;
        int i;

        for (i = 0; i < 2; i++)
        {
            model = charaGMAs[index]->modelEntries[apeGfxFileInfo[index].mainBodyModelIDs[i]].model;
            lbl_802B47F0[lod * 2 + i] = u_find_some_mesh_with_red(model);
        }
        apeShirtMaterials[index + 0] = NULL;
        apeShirtMaterials[index + 1] = NULL;
    }
    else
    {
        struct GMAModel *lowPolyModel = charaGMAs[index]->modelEntries[apeGfxFileInfo[index].mainBodyModelIDs[0]].model;
        struct GMAModel *highPolyModel = charaGMAs[index]->modelEntries[apeGfxFileInfo[index].mainBodyModelIDs[1]].model;

        find_shirt_materials_in_gma(chara, lod, lowPolyModel, highPolyModel);
    }
}

struct Struct8008AE2C
{
    u8 filler0[8];
    u32 unk8;
    u32 unkC;
};

static struct GMAShape *next_shape(struct GMAShape *mesh)
{
    int i;
    u8 *ret = (u8 *)mesh + 0x60;

    for (i = 0; i < 2; i++)
    {
        if (mesh->dispListFlags & (1 << i))
            ret += mesh->dispListSizes[i];
    }
    if (mesh->dispListFlags & ((1 << 2)|(1 << 3)))
    {
        struct Struct8008AE2C *r6 = (void *)ret;

        ret += 0x20;
        ret += r6->unk8;
        ret += r6->unkC;
    }
    return (struct GMAShape *)ret;
}

void mot_ape_init(void)
{
    int i;

    nextApeIndex = 0;
    lbl_802F207C = 1;
    for (i = 0; i < 16; i++)
    {
        apeStructPtrs[i] = &apeStructs[i];
        apeStructs[i].unk70 = i;
    }
    for (i = 0; i < 32; i++)
        uselessArray[i] = 0;
    lbl_802F206C = 1;
    u_globalAnimSpeedScale = 1.0f;
    load_character_resources();
    u_animTransformMatrices = avdisp_allocate_matrices(30);
    mathutil_mtxA_push();
    mathutil_mtxA_from_identity();
    mathutil_mtxA_rotate_z(0x2FA4);
    mathutil_mtxA_to_mtx(lbl_802B4820);
    mathutil_mtxA_pop();
    u_something_with_skel_model_names();
    nlObjModelListLoad(&apeFaceObj, &apeFaceTpl, "ape/face_p.lz", "ape/face.lz");
}

void u_something_with_skel_model_names(void)
{
    int i;
    int j;
    int r10;

    for (i = 0; i < 8; i++)
    {
        motInfo[i].unkB0 = (void *)motInfo;
        r10 = 0;
        for (j = 0; j < 16; j++)
        {
            if (j == 10)
                r10 = 0x400;
            // hmm...
            motInfo[i].unk30[j] = (void *)((uintptr_t)motInfo + 0x2000 + (i * 0x80 + r10) * 0x20);
#ifdef NONMATCHING
            // Avoids negative array access to emulate the original behavior
            if (j >= 10)
                motInfo[i].unk70[j] = lbl_801C7ADC[j] - lbl_801C7B1C[j - 10];
            else
                motInfo[i].unk70[j] = lbl_801C7ADC[j] - lbl_801C7ADC[j + 6];
#else
            motInfo[i].unk70[j] = lbl_801C7ADC[j] - lbl_801C7B1C[j - 10];
#endif
            r10 += lbl_801C7ADC[j];
        }
        if (i >= 4)
        {
            strcpy(motInfo[i].skelName, motInfo[0].skelName);
            strcpy(motInfo[i].modelName, motInfo[0].modelName);
        }
    }
}

void ape_destroy(struct Ape *ape)
{
    thread_kill(ape->threadId);
    if (lbl_802F2074 == 2)
    {
        OSFreeToHeap(backgroundHeap, ape->unk0);
        OSFreeToHeap(backgroundHeap, ape->unk4);
    }
    else if (lbl_802F2074 == 1)
    {
        OSFreeToHeap(mainHeap, ape->unk0);
        OSFreeToHeap(mainHeap, ape->unk4);
    }
    else
    {
        OSFreeToHeap(subHeap, ape->unk0);
        OSFreeToHeap(subHeap, ape->unk4);
    }
    OSFreeToHeap(subHeap, ape->u_bodyParts);
    apeStructPtrs[--nextApeIndex] = ape;
    u_free_character_graphics(ape->charaId, (ape->lod >= 2));
}

static u8 lbl_802F12D8[8] = {0, 0, 2, 4, 6, 0, 0, 0};
static u8 lbl_802F12E0[8] = {1, 1, 3, 5, 7, 0, 0, 0};

static void u_make_ape_inline(struct Ape *ape)
{
    int partIdx;
    int lod;
    struct BodyPartNameInfo *nameInfo;
    int index = ape->charaId * 2;

    for (partIdx = 0; partIdx < g_bodyPartCountsPerCharacter[ape->charaId]; partIdx++)
    {
        nameInfo = &lbl_801C63B0[ape->charaId][partIdx];
        for (lod = 0; lod < 4; lod++)
        {
            u_find_ape_body_part(&apeGfxFileInfo[index], lod, nameInfo);
            ape->u_bodyParts[partIdx].u_someLodRelatedIndexes[lod] = nameInfo->u_someLodRelatedIndexes[lod];
        }
    }
}

#pragma force_active on
const Vec unused_80171980 = {1, 0, 0};
#pragma force_active reset

static void func_8008B3B8_inline_3(u8 skelIdx, struct ApeAnimationThing *r24_)
{
    void *r3;

    r3 = &r24_->unk4114;
    u_init_something_joints_from_something(r3, r24_, 1, lbl_802F12D8[skelIdx]);
    r3 = &r24_->unk84;
    u_init_something_joints_from_something(r3, r24_, 2, lbl_802F12E0[skelIdx]);
}

static u8 u_get_skeleton_index_from_name(char *skelName)
{
    u8 i;
    u8 skelIdx;
    for (skelIdx = 0, i = 0; i < motsklFileData->skeletonsCount; i++)
    {
        if (strcmp(skelName, motsklFileData->skeletons[i].name) == 0)
        {
            skelIdx = i;
            break;
        }
    }
    return skelIdx;
}

static void u_get_skeleton_from_name(char *skelName, struct Skeleton **out)
{
    int i;
    *out = &motsklFileData->skeletons[0];
    for (i = 0; i < motsklFileData->skeletonsCount; i++)
    {
        if (strcmp(skelName, motsklFileData->skeletons[i].name) == 0)
        {
            *out = &motsklFileData->skeletons[i];
            break;
        }
    }
}

static struct Ape *u_make_ape_sub(char *skelName, char *modelName /*unused*/)
{
    struct Ape *ape;
    struct ApeAnimationThing *r24;
    struct ApeAnimationThing *r31;
    struct Skeleton *skel;
    int i;
    u8 skelIdx;
    int r20;

    ape = apeStructPtrs[nextApeIndex];
    r20 = ape->unk70;
    memset(ape, 0, sizeof(*ape));
    ape->unk70 = r20;

    u_get_skeleton_from_name(skelName, &skel);

    r24 = u_create_joints_probably(skel);
    r31 = u_create_joints_probably(skel);

    ape->u_bodyPartCount = 5;
    ape->u_bodyParts = OSAllocFromHeap(subHeap, ape->u_bodyPartCount * sizeof(*ape->u_bodyParts));
    if (ape->u_bodyParts == NULL)
        OSPanic("mot_ape.c", 1494, "cannot OSAlloc\n");

    for (i = 0; i < ape->u_bodyPartCount; i++)
    {
        struct BodyPart *part = &ape->u_bodyParts[i];

        part->type = 0x7FFFFFFF;
        part->unk4 = -1.0f;
        part->unk8 = 0;
        part->unkC = 0;
        part->unk10 = 0.0f;
        part->u_someLodRelatedIndexes[3] = 0;
    }

    ape->unk0 = r24;
    ape->unk4 = r31;
    ape->unk1C = &lbl_801C7A70;
    ape->unk20 = 0;
    ape->animTimerCurr = 0.0f;
    ape->charaId = 0;
    ape->animTimerMax = 0.0f;
    ape->flags = 0;
    ape->unk18 = 0;
    ape->unk24 = 1;
    ape->unk28 = 0;
    ape->unk9C = 0;
    ape->unkB0 = 0;
    ape->skel = skel;
    ape->unk54 = 0;

    // These really should be assigned using compound literals, but that causes the stack usage to not match.
    { static const Vec v = {0}; Vec v_; ape->pos = v_ = v; } //0x3C
    { static const Vec v = {0}; Vec v_; ape->unk3C = v_ = v; } //0x48
    { static const Vec v = {0}; Vec v_; ape->unk48 = v_ = v; } //0x54
    { static const Quaternion q = {1, 0, 0, 0}; Quaternion q_; *(Quaternion *)&ape->unkA0 = q_ = q; } //0x60
    ape->modelScale = 1.0f;
    { static const Quaternion q = {0, 0, 0, 1}; Quaternion q_; ape->unk60 = q_ = q; } //0x70
    ape->colorId = 0;
    ape->lod = lbl_802F207C;

    {u8 stackpad[0x10];}

    u_make_ape_inline(ape);
    skelIdx = u_get_skeleton_index_from_name(skelName);
    func_8008B3B8_inline_3(skelIdx, ape->unk0);
    skelIdx++;skelIdx--;
    func_8008B3B8_inline_3(skelIdx, ape->unk4);

    ape->unkB8 = return_0;
    ape->unkBC = emptyfunc;
    ape->threadId = thread_create(mot_ape_thread, ape, THREAD_GROUP_7);
    nextApeIndex++;
    return ape;
}

float force_lbl_802F56D4() { return 60.0f; }
const double lbl_802F56D8 = 0.0000000099999999392252903;

struct Ape *u_make_ape(enum Character charaId)
{
    struct Ape *ape;
    struct AnimJoint *r5;

    ape = u_make_ape_sub(motInfo[charaId].skelName, motInfo[charaId].modelName);
    ape->charaId = charaId & 3;
    ape->unk20 = &((struct MotInfo *)&motInfo[charaId])->unk30[2]->unk180;
    ape->unk28 = 1;

    u_make_ape_inline(ape);

    ape->unkB0 = 0;
    u_load_character_graphics(charaId, lbl_802F207C >> 1);
    u_start_new_anim_maybe(ape, ((struct MotInfo *)&motInfo[charaId])->unk30[2]->unk180.animId);
    mot_joint_800355FC(ape->unk0);

    r5 = &ape->unk0->joints[0];
    r5->unk1A4.x = r5->transformMtx[0][3];
    r5->unk1A4.y = r5->transformMtx[1][3];
    r5->unk1A4.z = r5->transformMtx[2][3];

    ape->unk4->joints[0].transformMtx[0][3] = r5->unk1A4.x;
    ape->unk4->joints[0].transformMtx[1][3] = r5->unk1A4.y;
    ape->unk4->joints[0].transformMtx[2][3] = r5->unk1A4.z;

    return ape;
}

#pragma force_active on
void func_8008B9DC(struct Ape *ape, int b)
{
    if (ape->unk18 <= 0)
    {
        if (ape->unk1C->unkC & (1 << 1))
            ape->unk0->u_nextAnimId = b;
        else
            u_start_new_anim_maybe(ape, b);
    }
}
#pragma force_active reset

void mot_ape_set_some_var_1(int a)
{
    lbl_802F206C = a;
}

void u_start_indexed_anim(struct Ape *ape, int b, int index)
{
    struct SomeMotInfoStruct *r6 = ((struct SomeMotInfoStruct *)(((struct MotInfo *)&motInfo[ape->charaId])->unk30[b]));

    ape->unk20 = &r6[index];
    ape->unk28 = b;
    ape->unkB0 = index;
    ape->unk18 = 0;
    u_start_new_anim_maybe(ape, r6[index].animId);
    mot_joint_800355FC(ape->unk0);
    ape->animTimerMax = 0.0f;
}

#pragma force_active on
void mot_ape_8008BAA8(int *a, int *b)
{
    int i;

    if (*b < 4)
        return;
    *a = 10;
    *b -= 4;
    for (i = 0; i < 6; i++)
    {
        if (*b < lbl_801C7B1C[i])
            break;
        *b -= lbl_801C7B1C[i];
        (*a)++;
    }
    *b += lbl_801C7ADC[*a] - lbl_801C7ADC[6 + *a];
}
#pragma force_active reset

void new_ape_stat_motion(struct Ape *ape, int b, int c, int d, float speed)
{
    struct MotInfo *r30;
    struct ApeAnimationThing *r7;
    struct SomeMotInfoStruct *r8;
    int r9;
    u32 r10;
    int i;
    struct SomeMotInfoStruct *r5;

    r10 = 0xFFFFFFFF;
    r9 = 0;
    r7 = ape->unk0;
    r30 = &motInfo[ape->charaId];
    r8 = (void *)((struct MotInfo *)r30)->unk30[b];
    switch (b)
    {
    case 7:
        r10 = c;
        mot_ape_8008BAA8(&b, &c);
        r8 = (void *)((struct MotInfo *)r30)->unk30[b];
        r9 = c;
        break;
    case 2:
        if (c < 9)
        {
            r9 = c * 3;
            r9++; r9--;  // needed to match
            for (i = 0; i < 2; i++, r9++)
            {
                if (r8[r9].unk0 > speed * 216.0f)
                    break;
            }
        }
        else
            r9 = c;
        break;
    case 1:
        if (d != 0)
            r9 = d - 1;
        else
        {
            for (i = 0; r8[i].unk4 == 0; i++, r9++)
            {
                if (r8[i].unk0 > speed / 60.0f)
                    break;
            }
            r5 = &r8[r9];
            if (r8[r9].unk4 == 1 && r7->u_animId != r8[r9].animId)
            {
                if (r7->u_animId != r8[r9 + 1].animId || r7->u_currKeyframe < r7->u_keyframeCount - r5->unk14)
                    r9++;
            }
        }
        break;
    case 4:
        for (i = 2; i > 0; i--)
        {
            if (r8[i].unk0 < speed * 216.0f)
                break;
        }
        r9 = i;
        break;
    case 6:
        if (d != 0)
            r9 = d - 1;
        else
        {
            for (i = 0; r8[i].unk4 == 0; i++, r9++)
            {
                if (r8[i].unk0 > speed * 216.0f)
                    break;
            }
        }
        break;
    case 0:
        for (i = 0; r8[i].unk4 == 0; i++, r9++)
        {
            if (r8[i].unk0 > speed * 216.0f)
                break;
        }
        break;
    case 3:
        break;
    default:
        r9 = c;
        break;
    }
    r8 += r9;
    if (ape->unk1C != r8)
    {
        int r4;

        ape->unk20 = r8;
        ape->unk28 = b;
        if (r10 != 0xFFFFFFFF)
            ape->unkB0 = r10;
        else
            ape->unkB0 = r9;
        func_8008B9DC(ape, r8->animId);
    }
}

void mot_ape_set_default_lod(int a)
{
    lbl_802F207C = a;
}

// Changes which level of detail model to use (0-3) with 0 being
// the most detailed and 3 being the least detailed
void set_ape_model_lod(struct Ape *ape, int lod)
{
    int prevLod;

    if (lod >= 4 || lod < 0)
        return;
    prevLod = ape->lod;
    if (prevLod >= 2 && lod <= 1)
    {
        lbl_802F2090 = 0;
        // Free the low poly GMA and load the high poly one
        u_free_character_graphics(ape->charaId, 1);
        u_load_character_graphics(ape->charaId, 0);
    }
    else if (lod >= 2 && prevLod <= 1)
    {
        lbl_802F2090 = 0;
        // Free the high poly GMA and load the low poly one
        u_free_character_graphics(ape->charaId, 0);
        u_load_character_graphics(ape->charaId, 1);
    }
    ape->lod = lod;
}

void mot_ape_8008BFB4(struct Ape *ape, int b, int *c, float *d)
{
    *c = ape->u_bodyParts[b].type;
    *d = ape->u_bodyParts[b].unk4;
}

#pragma force_active on
void func_8008BFD8(void) {}
#pragma force_active reset

void mot_ape_8008BFDC(struct Ape *ape, u16 b, u16 c)
{
    struct AnimJoint *r31;
    struct AnimJoint *r30 = ape->unk0->joints;
    Mtx sp10;

    if ((debugFlags & 0xA) || (ape->flags & (1 << 3)))
        return;
    r31 = &r30[5];  // head?
    mathutil_mtxA_from_mtx(r30[1].transformMtx);
    mathutil_mtxA_invert();
    mathutil_mtxA_mult_right(r31->transformMtx);
    mathutil_mtxA_to_mtx(sp10);
    mathutil_mtxA_from_mtx(r30[1].transformMtx);
    mathutil_mtxA_rotate_x(b);
    mathutil_mtxA_rotate_y(c);
    mathutil_mtxA_mult_right(sp10);
    mathutil_mtxA_to_mtx(r31->transformMtx);
}

// Makes the ape's head turn and look at something (such as the goal)
void ape_face_dir(struct Ape *ape, Vec *lookPoint)
{
    struct AnimJoint *joint = ape->unk0->joints;
    Vec sp2C;
    int r3;
    int r27_;
    struct AnimJoint *r30;  // head joint?
    float f1;
    Vec sp20;
    Quaternion sp10;

    if ((debugFlags & 0xA) || (ape->flags & (1 << 3)))
        return;
    if (!(ape->unk1C->unkC & 1) && (ape->flags & (1 << 6)))
        return;

    joint++;
    mathutil_mtxA_from_quat(&ape->unk60);
    mathutil_mtxA_to_mtx(workMatrix);
    mathutil_mtxA_mult_right(joint->transformMtx);
    if (ape->charaId == CHARACTER_BABY && (ape->flags & (1 << 22)))
        mathutil_mtxA_rotate_z(-5461);
    else
        mathutil_mtxA_rotate_z(-16384);
    sp2C.x = lookPoint->x - ape->pos.x;
    sp2C.y = lookPoint->y - ape->pos.y;
    sp2C.z = lookPoint->z - ape->pos.z;
    mathutil_mtxA_rigid_inv_tf_vec(&sp2C, &sp2C);
    mathutil_vec_normalize_len(&sp2C);
    r27_ = (ape->unk1C->unkC & 1) == 0;
    r30 = joint + 4;
    r3 = sp2C.x < -0.8f;
    if (!r27_)
        ape->flags &= ~(1 << 6);
    if (r3 || r27_)
    {
        mathutil_mtxA_push();
        mathutil_mtxA_from_mtx(workMatrix);
        mathutil_mtxA_mult_right(r30->transformMtx);
        mathutil_mtxA_tf_vec_xyz(&sp2C, 1.0f, 0.0f, 0.0f);
        mathutil_mtxA_pop();
        mathutil_mtxA_rigid_inv_tf_vec(&sp2C, &sp2C);
    }
    else if (sp2C.x < 0.09f)
        sp2C = ape->unkA0;
    if (sp2C.y > 0.4f)
        sp2C.y = 0.4f;
    else if (sp2C.y < -0.3f)
        sp2C.y = -0.3f;
    f1 = mathutil_vec_dot_normalized(&sp2C, &ape->unkA0);
    if (f1 > 0.996f || (ape->flags & (1 << 6)))
    {
        ape->unkA0 = sp2C;
        if (r27_)
            ape->flags |= (1 << 6);
    }
    else
    {
        float f31;

        f31 = 5.0f;
        if (r27_)
            f31 = 8.0f;
        f31 *= mathutil_sqrt(1.0f - f1 * f1) * 0.8f + 0.4f;
        if (f31 > 6.0f)
            f31 = 6.0f;
        mathutil_vec_normalize_len(&sp2C);
        mathutil_vec_cross_prod(&ape->unkA0, &sp2C, &sp2C);
        mathutil_vec_normalize_len(&sp2C);
        ape->unkAC = 0.0f;
        mathutil_quat_from_axis_angle(&sp10, &sp2C, DEGREES_TO_S16(f31));
        mathutil_quat_mult((Quaternion *)&ape->unkA0, &sp10, (Quaternion *)&ape->unkA0);
    }
    mathutil_vec_normalize_len(&ape->unkA0);
    mathutil_mtxA_tf_vec(&ape->unkA0, &sp2C);
    mathutil_mtxA_from_mtx(workMatrix);
    mathutil_mtxA_mult_right(r30->transformMtx);
    mathutil_mtxA_rigid_inv_tf_vec(&sp2C, &sp2C);
    sp20.x = 1.0f;
    sp20.y = 0.0f;
    sp20.z = 0.0f;
    mathutil_quat_from_dirs(&sp10, &sp20, &sp2C);
    mathutil_mtxA_from_quat(&sp10);
    mathutil_mtxA_mult_left(r30->transformMtx);
    mathutil_mtxA_to_mtx(r30->transformMtx);
}

void mot_ape_set_quat_from_vec(struct Ape *ape, Vec *b)
{
    Vec sp10 = *b;

    sp10.y = 0.0f;
    mathutil_vec_normalize_len(&sp10);
    sp10.x = -sp10.x;
    sp10.z = -sp10.z;
    mathutil_mtxA_from_identity();
    mathutil_mtxA_rotate_y_sin_cos(-sp10.z, sp10.x);
    ape->unkA0.x = 1.0f;
    ape->unkA0.y = 0.0f;
    ape->unkA0.z = 0.0f;
    mathutil_mtxA_to_quat(&ape->unk60);
}

void u_set_global_skelanim_speed_scale(float a)
{
    u_globalAnimSpeedScale = a;
}

// The main function that updates skeletal animation
// Does not animate the ears, though. That is done somewhere else
void ape_skel_anim_main(struct Ape *ape)
{
    struct ApeAnimationThing *r31 = ape->unk0;
    struct ApeAnimationThing *r29;

    if ((debugFlags & 0xA) || (ape->flags & (1 << 3)))
        return;

    ape->unk18--;
    ape->flags &= ~(1 << 16);
    u_something_with_eyes_blinking(ape);
    r31->u_timeInKeyframe += r31->u_someDeltaTime * u_globalAnimSpeedScale;
    u_advance_ape_anim(ape, r31);
    if (ape->animTimerMax > 9.9999999392252903e-09f)
    {
        r29 = ape->unk4;
        r31 = ape->unk0;
        if (ape->flags & (1 << 9))
        {
            r29->u_timeInKeyframe += r29->u_someDeltaTime;
            if (r29->u_timeInKeyframe >= 1.0f)
            {
                r29->u_currKeyframe++;
                r29->u_timeInKeyframe -= 1.0f;
            }
            if (r29->u_currKeyframe >= r29->u_keyframeCount)
            {
                if (ape->unk1C->unkC & (1 << 4))
                    r29->u_currKeyframe = ape->unk1C->unk8;
                else
                    r29->u_currKeyframe = 1;
                u_mot_joint_start_anim(r29);
            }
            mot_joint_800355FC(r29);
            u_iter_joints_8008A2C4(r31->joints);
            u_iter_joints_8008A2C4(r29->joints);
            u_iter_joints_8008A3A4(r31->joints, r29->joints, ape->animTimerCurr / ape->animTimerMax);
        }
        else
            u_iter_joints_8008A124(r31->joints, ape->animTimerCurr / ape->animTimerMax);
        ape->animTimerCurr += 1.0f;
        if (ape->animTimerCurr > ape->animTimerMax)
        {
            ape->animTimerMax = 0.0f;
            r31->u_someDeltaTime = ape->unk1C->u_maybeSpeed;
        }
    }
    u_animate_ape_hands(r31);
}

static void u_draw_ape_transformed(struct Ape *ape, struct AnimJoint *joints)
{
    int i;
    // LODs 0 and 1 are in one GMA, while LODS 2 and 3 are in the other
    u32 gmaIndex = (ape->lod >> 1) + (ape->charaId * 2);
    struct ApeGfxFileInfo *gfxInfo = &apeGfxFileInfo[gmaIndex];
    struct BodyPartDesc *partDesc = gfxInfo->bodyPartInfo[ape->lod & 1];
    struct GMAModel *model;
    struct BodyPart *partsArr[10];
    struct BodyPart *part;
    struct BodyPart **partsIter;
    u8 dummy[8];

    partsIter = partsArr;
    for (i = 0; i < gfxInfo->partCounts[ape->lod & 1]; partsIter++, i++)
        *partsIter = NULL;

    // fill partsArr with parts for current LOD?
    part = ape->u_bodyParts;
    for (i = 0; i < ape->u_bodyPartCount; part++, i++)
    {
        if (part->u_someLodRelatedIndexes[ape->lod] != -1)
           partsArr[part->u_someLodRelatedIndexes[ape->lod]] = part;
    }

    // Draw all body parts except for main body
    partsIter = partsArr;
    for (i = 0; i < gfxInfo->partCounts[ape->lod & 1]; partDesc++, partsIter++, i++)
    {
        struct AnimJoint *joint = &joints[partDesc->jointIdx];
        struct GMAModel *model = charaGMAs[gmaIndex]->modelEntries[partDesc->modelId].model;

        if (model != NULL)
        {
            mathutil_mtxA_push();
            mathutil_mtxA_mult_right(joint->transformMtx);
            mathutil_mtxA_translate(&partDesc->offset);  // positions ears
            gxutil_load_pos_nrm_matrix(mathutilData->mtxA, 0);

            if (partDesc->draw != NULL)
                partDesc->draw(ape, partDesc, *partsIter);
            else
                avdisp_draw_model_unculled_sort_none(model);  // fallback code (not called)?

            mathutil_mtxA_pop();
        }
    }

    // Compute anim transform matrices
    for (i = 0; (u32)i < 15; i++)
    {
        struct AnimJoint *joint = &joints[u_animMatrixIdxToJointIdx[i]];

        if (i == 9 || i == 10 || i == 13 || i == 14)
        {
            mathutil_mtx_mult(joint->transformMtx, lbl_802B4820, lbl_802B4850[i]);
            if (i == 10 || i == 14)
            {
                mathutil_mtxA_push();
                mathutil_mtxA_from_mtx(lbl_802B4850[i]);
                mathutil_mtxA_rotate_z(DEGREES_TO_S16(lbl_801C7ACC[ape->charaId]));
                mathutil_mtxA_to_mtx(lbl_802B4850[i]);
                mathutil_mtxA_pop();
            }
            u_animTransformMatrices[i] = &lbl_802B4850[i];
        }
        else
            u_animTransformMatrices[i] = &joint->transformMtx;
    }

    // Draw the main body
    model = charaGMAs[gmaIndex]->modelEntries[gfxInfo->mainBodyModelIDs[ape->lod & 1]].model;
    recolor_ape_shirt(ape, model);
    avdisp_draw_model_unculled_sort_none(model);
}

struct ApeDrawNode
{
    struct OrdTblNode node;
    u32 lightGroup;  // unused
    struct Ape *ape;
    float opacity;
};  // 0x14

static void ape_draw_callback(struct ApeDrawNode *);

void mot_ape_thread(struct Ape *ape, int status)
{
    BOOL dontDraw = (status == THREAD_STATUS_KILLED || (ape->flags & APE_FLAG_INVISIBLE));
    u8 dummy[16];

    if (dontDraw)
        return;
    if (ape->unkC1 & (1 << currentCamera->unk204))
        return;
    if ((polyDisp.flags & (1 << 2)) && get_height_world_mirror_plane(&ape->pos) < 0.0f)
        return;

    if (ape->flags & APE_FLAG_TRANSLUCENT)
    {
        struct ApeDrawNode *node;
        struct OrdTblNode *entry;

        if (ballInfo[ape->ballId].u_opacity[currentCamera->unk204] < 1.1920928955078125e-07f)
            return;
        mathutil_mtxA_from_mtxB();
        entry = ord_tbl_get_entry_for_pos(&ape->pos);
        node = ord_tbl_alloc_node(sizeof(*node));
        node->node.drawFunc = (OrdTblDrawFunc)ape_draw_callback;
        node->lightGroup = peek_light_group();
        node->ape = ape;
        node->opacity = ballInfo[ape->ballId].u_opacity[currentCamera->unk204];
        ord_tbl_insert_node(entry, &node->node);
    }
    else
        mot_ape_8008CAAC(ape, 1.0f);
}

static void ape_draw_callback(struct ApeDrawNode *node)
{
    struct Ape *ape = node->ape;

    mot_ape_8008CAAC(ape, node->opacity);
}

static void mot_ape_8008CAAC(struct Ape *ape, float opacity)
{
    int charaId = ape->charaId;
    struct AnimJoint *joints = ape->unk0->joints;
    u8 dummy[8];
    Vec sp10;

    polydisp_set_some_color_based_on_curr_mode(opacity);

    mathutil_mtxA_push();
    mathutil_mtxA_from_quat(&ape->unk60);
    mathutil_mtxA_to_mtx(workMatrix);
    mathutil_mtxA_pop();

    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_translate(&ape->pos);
    mathutil_mtxA_scale_xyz(ape->modelScale, ape->modelScale, ape->modelScale);
    nlSetScaleFactor(ape->modelScale);
    mathutil_mtxA_translate(&ape->unk3C);
    mathutil_mtxA_mult_right(workMatrix);
    sp10.x = ape->unk0->joints[0].transformMtx[0][3];
    sp10.y = ape->unk0->joints[0].transformMtx[1][3];
    sp10.z = ape->unk0->joints[0].transformMtx[2][3];
    if (test_scaled_sphere_in_frustum(&sp10, ape->modelScale * 0.5f, ape->modelScale) != 0)
    {
        apeDummyFuncs[charaId](ape);
        u_draw_ape_transformed(ape, joints);
    }
    fade_color_base_default();
}

u16 lbl_801C7D80[] = { 9, 1, 3, 2 };
u16 lbl_801C7D88[] = { 7, 5, 7, 6 };
u16 lbl_801C7D90[] = { 1, 1, 3, 2 };
u16 lbl_801C7D98[] = { 7, 1, 3, 2 };
u16 lbl_801C7DA0[] = { 10, 1, 3, 2 };
u16 lbl_801C7DA8[] = { 4, 9, 11, 10 };

// indexes of the texture for each shirt color in the character GMAs?
static u16 *shirtTexIndexes[] =
{
    lbl_801C7D80,
    lbl_801C7D88,
    NULL,
    NULL,
    lbl_801C7DA0,
    lbl_801C7DA8,
    lbl_801C7D90,
    lbl_801C7D98,
};

// Finds the material with the specified texture index
static struct GMATevLayer *find_material(struct GMAModel *model, u32 texIndex)
{
    struct GMATevLayer *materials = model->tevLayers;
    int i;

    for (i = 0; i < model->tevLayerCount; i++)
    {
        if (texIndex == materials[i].tplIndex)
        {
            materials[i].texObj = &model->texObjs[i];
            materials[i].flags &= ~(1 << 16);
            return &materials[i];
        }
    }
    return NULL;
}

FORCE_BSS_ORDER(unused802B4B20)

static void find_shirt_materials_in_gma(enum Character charaId, int lod, struct GMAModel *lowPolyModel, struct GMAModel *highPolyModel)
{
    int i;
    struct GMAModel *models[2];
    u8 dummy[4];
    u32 var = charaId * 2;

    models[0] = lowPolyModel;
    models[1] = highPolyModel;
    for (i = 0; i < 2; i++)
    {
        struct GMATevLayer *mtrl = find_material(models[i], shirtTexIndexes[var + lod][0]);

        u_apeTextures[var + i] = mtrl->texObj;
        apeShirtMaterials[var + i] = mtrl;
    }
}

// Re-colors the shirt by assigning a different texture object to the shirt material
static void recolor_ape_shirt(struct Ape *ape, struct GMAModel *unused)
{
    u32 index = (ape->charaId * 2) + (ape->lod & 1);

    if (apeShirtMaterials[index] != NULL)
    {
        struct GMATevLayer *mtrl = apeShirtMaterials[index];
        u16 *indexes = shirtTexIndexes[(ape->charaId * 2) + (ape->lod >> 1)];

        if (ape->colorId != 0)  // non-default shirt color
            mtrl->texObj = &shirtTexObjs[ape->charaId][indexes[ape->colorId]];
        else
            mtrl->texObj = u_apeTextures[index];
        mtrl->flags |= (1 << 16);
    }
    else  // for MeeMee?
    {
        GXColor colors[] =
        {
            {0xFF, 0x7F, 0xBF, 0xFF},  // pink
            {0x4C, 0x99, 0xFF, 0xFF},  // blue
            {0xE5, 0xA5, 0x4C, 0xFF},  // orange
            {0x4C, 0xB2, 0x4C, 0xFF},  // green
        };

        lbl_802B47F0[ape->lod]->materialColor = colors[ape->colorId];
        lbl_802B47F0[ape->lod]->ambientColor = colors[ape->colorId];
    }
}
