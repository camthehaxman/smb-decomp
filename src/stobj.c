#include <stdio.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "avdisp.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "mathutil.h"
#include "mode.h"
#include "name_entry.h"
#include "nl2ngc.h"
#include "obj_collision.h"
#include "pool.h"
#include "polydisp.h"
#include "sound.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "vibration.h"
#include "window.h"
#include "world.h"

#include "../data/bg_jun.gma.h"
#include "../data/bg_stm.gma.h"
#include "../data/common.gma.h"

struct Stobj g_stobjInfo[MAX_STOBJS];

static s16 s_nextUid;
struct GMAModel *jamabarModel;

static void func_8006B518(struct Stobj *);
static void find_jamabar_and_bumper_models(void);
static void spawn_bumpers(struct StageAnimGroup *, int);
static void spawn_jamabars(struct StageAnimGroup *, int);
static void stobj_bumper_init(struct Stobj *);
static void stobj_bumper_main(struct Stobj *);
static void stobj_bumper_draw(struct Stobj *);
static void stobj_bumper_coli(struct Stobj *, struct PhysicsBall *);
static void stobj_bumper_destroy(struct Stobj *);
static void stobj_bumper_debug(struct Stobj *);
static void stobj_bumper_bgspecial_init(struct Stobj *);
static void stobj_bumper_bgspecial_main(struct Stobj *);
static void stobj_bumper_bgspecial_draw(struct Stobj *);
static void stobj_bumper_bgspecial_coli(struct Stobj *, struct PhysicsBall *);
static void stobj_bumper_bgspecial_destroy(struct Stobj *);
static void stobj_bumper_bgspecial_debug(struct Stobj *);
static void stobj_jamabar_init(struct Stobj *);
static void stobj_jamabar_main(struct Stobj *);
static void stobj_jamabar_draw(struct Stobj *);
static void stobj_jamabar_coli(struct Stobj *, struct PhysicsBall *);
static void stobj_jamabar_destroy(struct Stobj *);
static void stobj_jamabar_debug(struct Stobj *);
static void stobj_dummy_init(struct Stobj *);
static void stobj_dummy_main(struct Stobj *);
static void stobj_dummy_draw(struct Stobj *);
static void stobj_dummy_coli(struct Stobj *, struct PhysicsBall *);
static void stobj_dummy_destroy(struct Stobj *);
static void stobj_dummy_debug(struct Stobj *);

void ev_stobj_init(void)
{
    int i;
    struct Stobj *stobj;

    s_nextUid = 0;
    memset(g_stobjInfo, 0, sizeof(g_stobjInfo));
    stobj = g_stobjInfo;
    for (i = 0; i < ARRAY_COUNT(g_stobjInfo); i++, stobj++)
    {
        stobj->index = i;
        stobj->uid = -1;
    }
    pool_reset(&g_poolInfo.stobjPool);
    find_jamabar_and_bumper_models();
    spawn_bumpers(decodedStageLzPtr->animGroups, decodedStageLzPtr->animGroupCount);
    spawn_jamabars(decodedStageLzPtr->animGroups, decodedStageLzPtr->animGroupCount);
    u_spawn_goal_stobjs(decodedStageLzPtr->animGroups, decodedStageLzPtr->animGroupCount);
}

#pragma force_active on
char *stobjTypeNames[] =
{
    "SOT_BUMPER",
    "SOT_JAMABAR",
    "SOT_GOALTAPE",
    "SOT_GOALBAG",
    "SOT_GOALBAG_EXMASTER",
    "SOT_MF_PNL_BUMPER",
    "SOT_MF_PNL_ELECTRAP",
    "SOT_MF_BULLET_TEST",
    "SOT_MF_BOX",
    "SOT_BUMPER_BGSPECIAL",
    "SOT_NAMEENT_BTN",
};
#pragma force_active reset

static void (*stobjInitFuncs[])(struct Stobj *) =
{
    stobj_bumper_init,
    stobj_jamabar_init,
    stobj_goaltape_init,
    stobj_goalbag_init,
    stobj_goalbag_exmaster_init,
    stobj_dummy_init,
    stobj_dummy_init,
    stobj_dummy_init,
    stobj_dummy_init,
    stobj_bumper_bgspecial_init,
    stobj_nameent_btn_init,
    NULL,
};

static void (*stobjMainFuncs[])(struct Stobj *) =
{
    stobj_bumper_main,
    stobj_jamabar_main,
    stobj_goaltape_main,
    stobj_goalbag_main,
    stobj_goalbag_exmaster_main,
    stobj_dummy_main,
    stobj_dummy_main,
    stobj_dummy_main,
    stobj_dummy_main,
    stobj_bumper_bgspecial_main,
    stobj_nameent_btn_main,
    NULL,
};

static void (*stobjDrawFuncs[])(struct Stobj *) =
{
    stobj_bumper_draw,
    stobj_jamabar_draw,
    stobj_goaltape_draw,
    stobj_goalbag_draw,
    stobj_goalbag_exmaster_draw,
    stobj_dummy_draw,
    stobj_dummy_draw,
    stobj_dummy_draw,
    stobj_dummy_draw,
    stobj_bumper_bgspecial_draw,
    stobj_nameent_btn_draw,
    NULL,
};

static void (*stobjCollisionFuncs[])(struct Stobj *, struct PhysicsBall *) =
{
    stobj_bumper_coli,
    stobj_jamabar_coli,
    stobj_goaltape_coli,
    stobj_goalbag_coli,
    stobj_goalbag_exmaster_coli,
    stobj_dummy_coli,
    stobj_dummy_coli,
    stobj_dummy_coli,
    stobj_dummy_coli,
    stobj_bumper_bgspecial_coli,
    stobj_nameent_btn_coli,
    NULL,
};

static void (*stobjDestroyFuncs[])(struct Stobj *) =
{
    stobj_bumper_destroy,
    stobj_jamabar_destroy,
    stobj_goaltape_destroy,
    stobj_goalbag_destroy,
    stobj_goalbag_exmaster_destroy,
    stobj_dummy_destroy,
    stobj_dummy_destroy,
    stobj_dummy_destroy,
    stobj_dummy_destroy,
    stobj_bumper_bgspecial_destroy,
    stobj_nameent_btn_destroy,
    NULL,
};

static void (*stobjDebugFuncs[])(struct Stobj *) =
{
    stobj_bumper_debug,
    stobj_jamabar_debug,
    stobj_goaltape_debug,
    stobj_goalbag_debug,
    stobj_goalbag_exmaster_debug,
    stobj_dummy_debug,
    stobj_dummy_debug,
    stobj_dummy_debug,
    stobj_dummy_debug,
    stobj_bumper_bgspecial_debug,
    stobj_nameent_btn_debug,
    NULL,
};

#pragma force_active on

char string_STATUS___s_n_2[] = "STATUS: %s\n";
char lbl_801BE25C[] =
{
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x16,
    0x16, 0x16, 0x16, 0x0A,
    0x00
};
char string_TYPE___s_n_2[] = "TYPE: %s\n";
char string_PART_ID___d_n[] = "PART ID: %d\n";
char string_POS__X__7_3f_n_2[] = "POS: X,%7.3f\n";
char string______Y__7_3f_n_2[] = "     Y,%7.3f\n";
char string______Z__7_3f_n_2[] = "     Z,%7.3f\n";
char string_SPD__X__7_3f_n_2[] = "SPD: X,%7.3f\n";
char string_ROT__X_0x_04X_n_2[] = "ROT: X,0x%04X\n";
char string______Y_0x_04X_n_2[] = "     Y,0x%04X\n";
char string______Z_0x_04X_n_2[] = "     Z,0x%04X\n";
char string_ROTSPD__X_0x_04X_n_2[] = "ROTSPD: X,0x%04X\n";
char string_________Y_0x_04X_n_2[] = "        Y,0x%04X\n";
char string_________Z_0x_04X_n_2[] = "        Z,0x%04X\n";
char string_COLI_RAD___7_3f_n_2[] = "COLI RAD: %7.3f\n";
char string_Flag__0x_08X_n_2[] = "Flag: 0x%08X\n";
char lbl_802F0B40[2] = "\n";
#pragma force_active reset

void ev_stobj_main(void)
{
    int i;
    struct Stobj *stobj;
    s8 *status;

    if (debugFlags & 0xA)
        return;

    status = g_poolInfo.stobjPool.statusList;
    stobj = g_stobjInfo;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, status++, stobj++)
    {
        if (*status != STAT_NULL)
        {
            if (*status == STAT_DEST)
            {
                stobjDestroyFuncs[stobj->type](stobj);
                *status = STAT_NULL;
            }
            else
            {
                stobj->unk7C = stobj->localPos;
                stobj->unk88 = stobj->rotX;
                stobj->unk8A = stobj->rotY;
                stobj->unk8C = stobj->rotZ;
                stobjMainFuncs[stobj->type](stobj);
                stobj->prevPos = stobj->pos;
                func_8006B518(stobj);

                // Set a look point for the object
                if (stobj->flags & STOBJ_FLAG_LOOKPOINT)
                {
                    Point3d pos;

                    if (stobj->animGroupId != 0)
                    {
                        mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
                        mathutil_mtxA_translate(&stobj->localPos);
                    }
                    else
                        mathutil_mtxA_from_translate(&stobj->localPos);
                    if (stobj->flags & STOBJ_FLAG_ROTATION_UNK)
                    {
                        mathutil_mtxA_rotate_y(stobj->rotY);
                        mathutil_mtxA_rotate_x(stobj->rotX);
                        mathutil_mtxA_rotate_z(stobj->rotZ);
                    }
                    else
                    {
                        mathutil_mtxA_rotate_z(stobj->rotZ);
                        mathutil_mtxA_rotate_y(stobj->rotY);
                        mathutil_mtxA_rotate_x(stobj->rotX);
                    }
                    mathutil_mtxA_tf_point(&stobj->lookPoint, &pos);
                    set_ball_look_point(5, &pos, stobj->lookPointPrio);
                }
            }
        }
    }
}

void ev_stobj_dest(void)
{
    int i;
    struct Stobj *stobj;
    s8 *status;

    status = g_poolInfo.stobjPool.statusList;
    stobj = g_stobjInfo;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, status++, stobj++)
    {
        if (*status != STAT_NULL)
        {
            stobjDestroyFuncs[stobj->type](stobj);
            *status = STAT_NULL;
        }
    }
}

void stobj_draw(void)
{
    s32 i;
    struct Stobj *stobj;
    s8 *status;
    EnvMapFunc func;
    int animGrpId;
    Mtx mtx;

    func = backgroundInfo.stageEnvMapFunc;
    if (func != NULL)
        u_avdisp_set_some_func_1(func);
    mathutil_mtx_copy(mathutilData->mtxB, mtx);

    status = g_poolInfo.stobjPool.statusList;
    animGrpId = 0;
    stobj = g_stobjInfo;
    for (i = g_poolInfo.stobjPool.count; i > 0; i--, status++, stobj++)
    {
        if (*status != STAT_NULL)
        {
            // Set up a matrix to transform from anim group space to world space
            if (animGrpId != stobj->animGroupId)
            {
                mathutil_mtxA_from_mtx(mtx);
                mathutil_mtxA_mult_right(animGroups[stobj->animGroupId].transform);
                mathutil_mtxA_to_mtx(mathutilData->mtxB);
                animGrpId = stobj->animGroupId;
            }
            // Draw the object
            stobjDrawFuncs[stobj->type](stobj);
        }
    }
    mathutil_mtx_copy(mtx, mathutilData->mtxB);
    if (func != NULL)
        u_avdisp_set_some_func_1(NULL);
}

s16 spawn_stobj(struct Stobj *stobj)
{
    int index;
    struct Stobj *newStobj;

    index = pool_alloc(&g_poolInfo.stobjPool, 1);
    if (index < 0)
        return -1;

    newStobj = &g_stobjInfo[index];
    memcpy(newStobj, stobj, sizeof(*newStobj));
    newStobj->index = index;
    stobjInitFuncs[newStobj->type](newStobj);
    newStobj->unk7C = newStobj->localPos;
    func_8006B518(newStobj);
    newStobj->prevPos = newStobj->pos;
    newStobj->coliFunc = stobjCollisionFuncs[newStobj->type];
    newStobj->uid = s_nextUid;
    s_nextUid++;
    if (s_nextUid < 0)
        s_nextUid = 0;
    return newStobj->uid;
}

struct StobjFuncs
{
    void (*init)(struct Stobj *);
    void (*main)(struct Stobj *);
    void (*draw)(struct Stobj *);
    void (*coli)(struct Stobj *, struct PhysicsBall *);
    void (*destroy)(struct Stobj *);
    void (*unk14)(struct Stobj *);
};

struct StobjFuncs stobjDummyFuncs =
{
    stobj_dummy_init,
    stobj_dummy_main,
    stobj_dummy_draw,
    stobj_dummy_coli,
    stobj_dummy_destroy,
    stobj_dummy_debug,
};

#pragma force_active on
void func_8006B3E8(s32 arg0, struct StobjFuncs *arg1)
{
    struct StobjFuncs sp10;

    if (arg1 == NULL)
        sp10 = stobjDummyFuncs;
    else
    {
        sp10 = *arg1;
        if (sp10.init == NULL)
            sp10.init = stobjDummyFuncs.init;
        if (sp10.main == NULL)
            sp10.main = stobjDummyFuncs.main;
        if (sp10.draw == NULL)
            sp10.draw = stobjDummyFuncs.draw;
        if (sp10.coli == NULL)
            sp10.coli = stobjDummyFuncs.coli;
        if (sp10.destroy == NULL)
            sp10.destroy = stobjDummyFuncs.destroy;
        if (sp10.unk14 == NULL)
            sp10.unk14 = stobjDummyFuncs.unk14;
    }

    stobjInitFuncs[arg0] = sp10.init;
    stobjMainFuncs[arg0] = sp10.main;
    stobjDrawFuncs[arg0] = sp10.draw;
    stobjCollisionFuncs[arg0] = sp10.coli;
    stobjDestroyFuncs[arg0] = sp10.destroy;
    stobjDebugFuncs[arg0] = sp10.unk14;
}
#pragma force_active reset

static void func_8006B518(struct Stobj *stobj)
{
    mathutil_mtxA_from_translate(&stobj->localPos);
    if (stobj->flags & STOBJ_FLAG_ROTATION_UNK)
    {
        mathutil_mtxA_rotate_y(stobj->rotY);
        mathutil_mtxA_rotate_x(stobj->rotX);
        mathutil_mtxA_rotate_z(stobj->rotZ);
    }
    else
    {
        mathutil_mtxA_rotate_z(stobj->rotZ);
        mathutil_mtxA_rotate_y(stobj->rotY);
        mathutil_mtxA_rotate_x(stobj->rotX);
    }
    mathutil_mtxA_tf_point(&stobj->u_model_origin, &stobj->pos);
}

struct Struct8028C0B0 lbl_8028C0B0;

struct GMA **lbl_801BE37C[] = { &decodedStageGmaPtr, &decodedBgGma, NULL };

static inline void func_8006B594_inline(void)
{
    struct GMAModel **r7;
    struct GMAModel **r6;
    struct GMAModel **r8;
    int r4;
    int r3;
    int i;
    int j;

    r6 = r8 = lbl_8028C0B0.unk14;
    for (i = 0, r7 = r6; i < 4; i++, r7++)
    {
        if (*r7 == NULL)
        {
            r8 = r7;
            for (j = i; j < 3; j++, r8++)
                r8[0] = r8[1];
            *r8 = NULL;
        }
    }

    r4 = 0;

    for (r3 = 4; r3 > 0 && *r6 != NULL; r3--, r6++)
        r4++;
    lbl_8028C0B0.unk10 = r4;
}

void find_jamabar_and_bumper_models(void)
{
    int i;
    int numModels;
    struct GMA ***gmaIter;
    struct GMAModelEntry *entry;
    int found;
    struct GMA *gma;

    memset(&lbl_8028C0B0, 0, sizeof(lbl_8028C0B0));
    switch (backgroundInfo.bgId)
    {
    case 21:
    case 10:
        lbl_8028C0B0.unk0[0] = 0.095f;
        lbl_8028C0B0.unk0[1] = 0.0565f;
        lbl_8028C0B0.unk0[2] = 0.00565f;
        lbl_8028C0B0.unk0[3] = 0.00056f;
        break;
    case 20:
        lbl_8028C0B0.unk0[0] = 0.103f;
        lbl_8028C0B0.unk0[1] = 0.085f;
        lbl_8028C0B0.unk0[2] = 0.0085f;
        lbl_8028C0B0.unk0[3] = 0.00085f;
        break;
    case 19:
        lbl_8028C0B0.unk0[0] = 0.028f;
        lbl_8028C0B0.unk0[1] = 0.0075f;
        lbl_8028C0B0.unk0[2] = 0.00075f;
        lbl_8028C0B0.unk0[3] = 0.000075f;
        break;
    default:
        lbl_8028C0B0.unk0[0] = 0.075f;
        lbl_8028C0B0.unk0[1] = 0.0075f;
        lbl_8028C0B0.unk0[2] = 0.00075f;
        lbl_8028C0B0.unk0[3] = 0.000075f;
        break;
    }

    found = FALSE;
    for (gmaIter = lbl_801BE37C; *gmaIter != NULL; gmaIter++)
    {
        gma = **gmaIter;
        if (gma != NULL)
        {
            for (i = 0; i < 4; i++)
            {
                if (lbl_8028C0B0.unk14[i] == 0)
                {
                    char name[32];

                    sprintf(name, "BUMPER_L%d", i + 1);
                    entry = gma->modelEntries;
                    numModels = gma->numModels;
                    while (numModels > 0)
                    {
                        if (strcmp(entry->name + 4, name) == 0)
                        {
                            lbl_8028C0B0.unk14[i] = entry->model;
                            found = TRUE;
                        }
                        numModels--;
                        entry++;
                    }
                }
            }
        }
    }

    if (found)
    {
        func_8006B594_inline();
    }
    else
    {
        lbl_8028C0B0.unk14[0] = commonGma->modelEntries[mb_bumper].model;
        lbl_8028C0B0.unk14[1] = commonGma->modelEntries[mb_bumper_low].model;
        lbl_8028C0B0.unk10 = 2;
    }

    memset(&jamabarModel, 0, sizeof(jamabarModel));  //! why not just set it to NULL?

    found = FALSE;
    for (gmaIter = lbl_801BE37C; *gmaIter != NULL; gmaIter++)
    {
        gma = **gmaIter;
        if (gma != NULL)
        {
            entry = gma->modelEntries;
            numModels = gma->numModels;
            while (numModels > 0)
            {
                if (strcmp(entry->name + 4, "JAMABAR") == 0)
                {
                    jamabarModel = entry->model;
                    found = TRUE;
                    break;
                }
                numModels--;
                entry++;
            }
            if (found)
                break;
        }
    }
    if (!found)
        jamabarModel = commonGma->modelEntries[mb_jamabar].model;
}

void spawn_bumpers(struct StageAnimGroup *arg0, int arg1)
{
    struct Stobj stobj;
    int i, j;

    memset(&stobj, 0, sizeof(stobj));
    switch (backgroundInfo.bgId)
    {
    case BG_TYPE_JUN:
    case BG_TYPE_STM:
        stobj.type = SOT_BUMPER_BGSPECIAL;
        break;
    default:
        stobj.type = SOT_BUMPER;
        break;
    }

    for (i = 0; i < arg1; i++, arg0++)
    {
        struct StageBumper *bumper = arg0->bumpers;

        for (j = 0; j < arg0->bumperCount; j++, bumper++)
        {
            stobj.localPos = bumper->pos;
            stobj.rotX = bumper->rotX;
            stobj.rotY = bumper->rotY;
            stobj.rotZ = bumper->rotZ;
            stobj.unk3C = bumper->unk14;
            stobj.animGroupId = i;
            spawn_stobj(&stobj);
        }
    }
}

void spawn_jamabars(struct StageAnimGroup *arg0, int arg1)
{
    struct Stobj stobj;
    int i, j;

    memset(&stobj, 0, sizeof(stobj));
    stobj.type = SOT_JAMABAR;

    for (i = 0; i < arg1; i++, arg0++)
    {
        struct StageJamabar *jamabar = arg0->jamabars;

        for (j = 0; j < arg0->jamabarCount; j++, jamabar++)
        {
            stobj.unkA8 = jamabar->pos;
            stobj.rotX = jamabar->rotX;
            stobj.rotY = jamabar->rotY;
            stobj.rotZ = jamabar->rotZ;
            stobj.unk3C = jamabar->unk14;
            stobj.animGroupId = i;
            spawn_stobj(&stobj);
        }
    }
}

static void stobj_bumper_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->flags |= STOBJ_FLAG_TANGIBLE|STOBJ_FLAG_LOOKPOINT;
    stobj->model = lbl_8028C0B0.unk14[0];
    stobj->boundSphereRadius = 0.75f * stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->unk48 = 1.0f;
    stobj->unk4C = 1.0f;
    stobj->unk50 = 1.0f;
    stobj->unk76 = 0;
    stobj->lookPointPrio = 0.75f;
    stobj->lookPoint = stobj->u_model_origin;
}

static void stobj_bumper_main(struct Stobj *stobj)
{
    switch (stobj->state)
    {
    case 0:  // idle
        stobj->unk78 += (0x100 - stobj->unk78) >> 6;
        if (stobj->unk48 > 1.0)
        {
            stobj->unk48 -= 0.06666666666666667;
            if (stobj->unk48 < 1.0)
                stobj->unk48 = 1.0f;
            stobj->unk50 = stobj->unk48;
        }
        break;
    case 1:  // collided
        stobj->state = 2;
        stobj->counter = 7;
        // fall through
    case 2:
        stobj->counter--;
        if (stobj->counter < 0)
            stobj->state = 0;
        stobj->unk78 += 0x100;
        stobj->unk48 += 0.5 * (2.0 - stobj->unk48);
        stobj->unk50 = stobj->unk48;
        break;
    }
    stobj->unk76 += stobj->unk78;
}

static void stobj_bumper_draw(struct Stobj *stobj)
{
    Vec sp18;
    Vec spC;
    struct GMAModel *model;
    float radius;

    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_translate(&stobj->localPos);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    mathutil_mtxA_rotate_y(stobj->unk76);
    spC.x = stobj->unk48 * stobj->unk3C.x;
    spC.y = stobj->unk3C.y;
    spC.z = stobj->unk50 * stobj->unk3C.z;
    mathutil_mtxA_scale_xyz(spC.x, spC.y, spC.z);
    mathutil_mtxA_get_translate_alt(&sp18);
    if (sp18.z > 0.0)
        return;

    model = stobj->model;
    radius = model->boundSphereRadius;
    if (test_scaled_sphere_in_frustum(&model->boundSphereCenter, radius, spC.x) != 0)
    {
        float temp_f1;
        int temp_r0;
        struct GMAModel **phi_r5;
        float *phi_r6;

        GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
        GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

        temp_f1 = -((radius * currentCamera->sub28.unk3C * currentCamera->sub28.vp.height) / sp18.z);
        phi_r6 = lbl_8028C0B0.unk0;
        phi_r5 = lbl_8028C0B0.unk14;
        for (temp_r0 = lbl_8028C0B0.unk10 - 1; temp_r0 > 0; temp_r0--, phi_r5++, phi_r6++)
        {
            if (temp_f1 > *phi_r6)
                break;
        }
        model = *phi_r5;
        if (temp_f1 > 0.03f)
            avdisp_draw_model_unculled_sort_translucent(model);
        else
        {
            EnvMapFunc temp_r31 = u_avdisp_set_some_func_1(NULL);
            avdisp_draw_model_unculled_sort_translucent(model);
            u_avdisp_set_some_func_1(temp_r31);
        }
    }
    if (stobj->unk48 > 1.0f)
    {
        float temp_f31_2;
        float phi_f30;

        mathutil_mtxA_from_mtxB();
        mathutil_mtxA_translate_xyz(stobj->localPos.x, 0.05f + stobj->localPos.y, stobj->localPos.z);
        mathutil_mtxA_rotate_z(stobj->rotZ);
        mathutil_mtxA_rotate_y(stobj->rotY);
        mathutil_mtxA_rotate_x(stobj->rotX - 0x4000);
        temp_f31_2 = 2.0f * spC.x;
        phi_f30 = 2.0f * (spC.x - 1.0f);
        if (phi_f30 > 1.0f)
            phi_f30 = 1.0f;
        mathutil_mtxA_scale_s(temp_f31_2);
        nlSetScaleFactor(temp_f31_2);
        nlObjPutSetFadeColorBase(phi_f30, phi_f30, phi_f30);
        nlObjPutImm(g_commonNlObj->models[0x2B]);
        fade_color_base_default();
    }
}

static void stobj_bumper_coli(struct Stobj *stobj, struct PhysicsBall *physBall)
{
    Vec bumperPos;
    Vec dist;
    float temp_f1;
    float dotRefl;
    struct Ball *ball;
    BOOL rumble;

    ball = currentBall;
    stobj->state = 1;
    bumperPos = stobj->pos;
    func_8006AAEC(&physBall->prevPos, &physBall->pos, &stobj->prevPos, &bumperPos, physBall->radius, stobj->model->boundSphereRadius);

    // Get normalized direction from bumper to ball
    dist = physBall->pos;
    dist.x -= stobj->pos.x;
    dist.y -= stobj->pos.y;
    dist.z -= stobj->pos.z;
    mathutil_vec_normalize_len(&dist);

    // Reflect the ball's velocity off the bumper
    dotRefl = -1.5 * (dist.x * physBall->vel.x + dist.y * physBall->vel.y + dist.z * physBall->vel.z);
    if (dotRefl > 0.0)
    {
        physBall->vel.x += dotRefl * dist.x;
        physBall->vel.y += dotRefl * dist.y;
        physBall->vel.z += dotRefl * dist.z;
    }

    // add some additional force?
    physBall->vel.x += 0.05 * dist.x;
    physBall->vel.y += 0.05 * dist.y;
    physBall->vel.z += 0.05 * dist.z;

    // Adjust ball to be outside of the bumper
    temp_f1 = stobj->boundSphereRadius + physBall->radius + 0.01;
    dist.x *= temp_f1;
    dist.y *= temp_f1;
    dist.z *= temp_f1;
    physBall->pos.x = stobj->pos.x + dist.x;
    physBall->pos.y = stobj->pos.y + dist.y;
    physBall->pos.z = stobj->pos.z + dist.z;

    u_play_sound_0(0x5011);
    ball->flags |= BALL_FLAG_05;

    {
        s16 sp14[] =
        {
            0x005A, 0x005B,
            0x005C, 0x005D,
            0x005E, 0x005F,
            0x0060, 0x0061,
        };
        u8 dummy[4];

        lbl_802F1DFC = ball->ape->charaId;
        u_play_sound_0(sp14[globalAnimTimer & 7]);
        rumble = TRUE;
        if (modeCtrl.gameType == GAMETYPE_MINI_RACE)
        {
            if (ball->unk144 != NULL && (ball->unk144->unk14 & 0x20))
                rumble = FALSE;
        }
        if (rumble)
            vibration_control(playerControllerIDs[ball->playerId], VIBRATION_STATE_1, 30);
    }
}

static void stobj_bumper_destroy(struct Stobj *stobj) {}

static void stobj_bumper_debug(struct Stobj *stobj) {}

static void stobj_bumper_bgspecial_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->flags |= STOBJ_FLAG_TANGIBLE|STOBJ_FLAG_LOOKPOINT;
    stobj->model = lbl_8028C0B0.unk14[0];
    stobj->boundSphereRadius = 0.75f * stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->unk48 = 1.0f;
    stobj->unk4C = 1.0f;
    stobj->unk50 = 1.0f;
    stobj->unk76 = 0;
    stobj->lookPointPrio = 0.75f;
    stobj->lookPoint = stobj->u_model_origin;
}

static void stobj_bumper_bgspecial_main(struct Stobj *stobj)
{
    stobj_bumper_main(stobj);
}

static s16 stmFireModelIDs[] =
{
    STM_FIRE00,
    STM_FIRE01,
    STM_FIRE02,
    STM_FIRE03,
    STM_FIRE04,
    STM_FIRE05,
    STM_FIRE06,
    STM_FIRE07,
    STM_FIRE08,
    STM_FIRE09,
    STM_FIRE10,
    STM_FIRE11,
    STM_FIRE12,
    STM_FIRE13,
    STM_FIRE14,
    STM_FIRE15,
    STM_FIRE16,
    STM_FIRE17,
    STM_FIRE18,
    STM_FIRE19,
    STM_FIRE20,
    STM_FIRE21,
    STM_FIRE22,
    STM_FIRE23,
    STM_FIRE24,
    STM_FIRE25,
    STM_FIRE26,
    STM_FIRE27,
    STM_FIRE28,
    STM_FIRE29,
    STM_FIRE30,
    STM_FIRE31,
};

static void stobj_bumper_bgspecial_draw(struct Stobj *stobj)
{
    Vec spC;
    float birdY;
    float temp_f31_2;
    float var_f1;
    struct GMAModel *birdModel;
    struct GMAModel *flameModel;
    int modelId;

    // Draw normal bumper model
    stobj_bumper_draw(stobj);

    // Draw special objects
    switch (backgroundInfo.bgId)
    {
    case BG_TYPE_JUN:
        // Draw bird
        if (stobj->unk48 > 1.0f)
        {
            var_f1 = 1.1f * (stobj->unk48 - 1.0f);
            birdModel = decodedBgGma->modelEntries[JUN_PIYO].model;
            if (var_f1 > 1.0f)
                var_f1 = 1.0f;
            birdY = stobj->model->boundSphereRadius + stobj->model->boundSphereRadius * mathutil_sin(16384.0f * var_f1);
            mathutil_mtxA_from_mtxB_translate(&stobj->localPos);
            mathutil_mtxA_rotate_z(stobj->rotZ);
            mathutil_mtxA_rotate_y(stobj->rotY);
            mathutil_mtxA_rotate_x(stobj->rotX);
            mathutil_mtxA_rotate_y(stobj->index << 11);
            mathutil_mtxA_translate_xyz(0.0f, birdY, 0.0f);
            GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
            GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
            avdisp_draw_model_culled_sort_translucent(birdModel);
        }
        break;
    case BG_TYPE_STM:
        // Draw animated flame
        modelId = stmFireModelIDs[globalAnimTimer & 0x1F];
        flameModel = decodedBgGma->modelEntries[modelId].model;
        mathutil_mtxA_from_mtxB_translate(&stobj->localPos);
        mathutil_mtxA_get_translate_alt(&spC);
        temp_f31_2 = spC.z + (8.0f * currentCamera->sub28.unk3C * currentCamera->sub28.vp.height);
        if (temp_f31_2 > 0.0f)
        {
            temp_f31_2 *= 0.5f;
            mathutil_mtxA_rigid_inv_tf_tl(&spC);
            mathutil_mtxA_rotate_y(mathutil_atan2(spC.x, spC.z));
            mathutil_mtxA_scale_xyz(0.025f * stobj->unk48, 0.035f, 1.0f);
            if (temp_f31_2 < 1.0f)
                avdisp_set_alpha(temp_f31_2);
            avdisp_set_bound_sphere_scale(0.035f);
            avdisp_draw_model_culled_sort_all(flameModel);
        }
        break;
    }
}

static void stobj_bumper_bgspecial_coli(struct Stobj *stobj, struct PhysicsBall *arg1)
{
    stobj_bumper_coli(stobj, arg1);
}

static void stobj_bumper_bgspecial_destroy(struct Stobj *stobj) {}

static void stobj_bumper_bgspecial_debug(struct Stobj *stobj) {}

static void stobj_jamabar_init(struct Stobj *stobj)
{
    stobj->state = 0;
    stobj->flags |= STOBJ_FLAG_TANGIBLE|STOBJ_FLAG_LOOKPOINT;
    stobj->model = jamabarModel;
    stobj->boundSphereRadius = stobj->model->boundSphereRadius * stobj->unk3C.x;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->localPos = stobj->unkA8;
    stobj->lookPoint.x = 0.0f;
    stobj->lookPoint.y = 0.5f;
    stobj->lookPoint.z = 1.75f;
    stobj->lookPointPrio = 1.0f;
}

static void stobj_jamabar_main(struct Stobj *stobj)
{
    Vec spC;

    mathutil_mtxA_from_translate(&stobj->unkA8);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    mathutil_mtxA_push();
    mathutil_mtxA_mult_left(animGroups[stobj->animGroupId].transform);
    mathutil_mtxA_rigid_inv_tf_vec(&g_gravityDir, &spC);
    mathutil_mtxA_pop();
    spC.z *= 0.016;
    stobj->offsetVel.z += spC.z;
    stobj->offsetVel.z *= 0.97;
    stobj->offsetPos.z += stobj->offsetVel.z;
    if (stobj->offsetPos.z < -2.5)
    {
        stobj->offsetPos.z = -2.5f;
        if (stobj->offsetVel.z < 0.0)
            stobj->offsetVel.z = -stobj->offsetVel.z;
    }
    else if (stobj->offsetPos.z > 0.0)
    {
        stobj->offsetPos.z = 0.0f;
        if (stobj->offsetVel.z > 0.0)
            stobj->offsetVel.z = -stobj->offsetVel.z;
    }
    mathutil_mtxA_tf_point(&stobj->offsetPos, &stobj->localPos);
    stobj->unk64.x = stobj->localPos.x - stobj->unk7C.x;
    stobj->unk64.y = stobj->localPos.y - stobj->unk7C.y;
    stobj->unk64.z = stobj->localPos.z - stobj->unk7C.z;
}

static void stobj_jamabar_draw(struct Stobj *stobj)
{
    Vec spC;

    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_translate(&stobj->localPos);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    spC = stobj->unk3C;
    mathutil_mtxA_scale_xyz(spC.x, spC.y, spC.z);
    avdisp_set_bound_sphere_scale(spC.z);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    avdisp_draw_model_culled_sort_translucent(stobj->model);
}

static void stobj_jamabar_coli(struct Stobj *stobj, struct PhysicsBall *arg1)
{
    collide_ball_with_jamabar(arg1, stobj);
}

static void stobj_jamabar_destroy(struct Stobj *stobj) {}

static void stobj_jamabar_debug(struct Stobj *stobj)
{
    window_printf(2, lbl_801BE25C);
    window_printf(2, "OFS: X,%7.3f\n", stobj->offsetPos.x);
    window_printf(2, string______Y__7_3f_n_2, stobj->offsetPos.y);
    window_printf(2, string______Z__7_3f_n_2, stobj->offsetPos.z);
    u_set_window_text(2, lbl_802F0B40);
    window_printf(2, "OFS SPD: X,%7.3f\n", stobj->offsetVel.x);
    window_printf(2, "         Y,%7.3f\n", stobj->offsetVel.y);
    window_printf(2, "         Z,%7.3f\n", stobj->offsetVel.z);
    u_set_window_text(2, lbl_802F0B40);
}

static void stobj_dummy_init(struct Stobj *stobj) {}

static void stobj_dummy_main(struct Stobj *stobj) {}

static void stobj_dummy_draw(struct Stobj *stobj) {}

static void stobj_dummy_coli(struct Stobj *stobj, struct PhysicsBall *arg1) {}

static void stobj_dummy_destroy(struct Stobj *stobj) {}

static void stobj_dummy_debug(struct Stobj *stobj) {}
