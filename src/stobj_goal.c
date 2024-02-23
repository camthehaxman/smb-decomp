#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "avdisp.h"
#include "ball.h"
#include "effect.h"
#include "gma.h"
#include "info.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "polydisp.h"
#include "pool.h"
#include "recplay.h"
#include "sound.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "window.h"
#include "world.h"

#include "../data/common.gma.h"
#include "../data/common.nlobj.h"

#define MAX_GOALS 8

struct GoalTapeSegment
{
    Vec unk0;
    Vec unkC;
    float unk18;
    Vec unk1C;
    u32 unk28;
    float unk2C;
};  // size = 0x30

struct GoalTape
{
    u32 u_flags;
    float unk4;
    float unk8;
    float unkC;
    s32 u_breakTime;  // time tape was broken?
    struct Stobj *stobj;
    struct GoalTapeSegment segments[8];
};  // size = 0x198

static struct GoalTape goalTapes[MAX_GOALS];

struct GoalBag  // The "party ball", known as a "goal bag" internally
{
    u32 u_flags;
    float open;  // value from 0 to 1, where 0 is completely closed, and 1 is completely open
    float openSpeed;  // rate of change for open
    struct Stobj *stobj;
    struct StageGoal goal;
    s32 u_openTime;  // time ball was opened?
};  // size = 0x28

static struct GoalBag goalBags[MAX_GOALS];

static s16 smallLCDModelIDs[] =
{
    NLMODEL_common_S_LCD_0,
    NLMODEL_common_S_LCD_1,
    NLMODEL_common_S_LCD_2,
    NLMODEL_common_S_LCD_3,
    NLMODEL_common_S_LCD_4,
    NLMODEL_common_S_LCD_5,
    NLMODEL_common_S_LCD_6,
    NLMODEL_common_S_LCD_7,
    NLMODEL_common_S_LCD_8,
    NLMODEL_common_S_LCD_9,
};

static s16 largeLCDModelIDs[] =
{
    NLMODEL_common_L_LCD_0,
    NLMODEL_common_L_LCD_1,
    NLMODEL_common_L_LCD_2,
    NLMODEL_common_L_LCD_3,
    NLMODEL_common_L_LCD_4,
    NLMODEL_common_L_LCD_5,
    NLMODEL_common_L_LCD_6,
    NLMODEL_common_L_LCD_7,
    NLMODEL_common_L_LCD_8,
    NLMODEL_common_L_LCD_9,
};

static void deform_goaltape_model(struct GoalTapeSegment *arg0, int arg1, struct NlModel *arg2, struct NlModel *arg3);
static void open_goalbag(int goalId, struct PhysicsBall *arg1);
static void close_goalbag(int arg0);
static float func_8006FCD0(Point3d *, float);
static void func_8006FD44(struct GoalTape *tape);

void u_spawn_goal_stobjs(struct StageAnimGroup *arg0, int arg1)
{
    struct Stobj stobj;
    struct StageAnimGroup *stageAg;
    int i, j;
    int totalGoals = 0;
    struct StageGoal *goal;
    struct GoalTape *tape;
    struct GoalBag *bag;
    Point3d sp10;

    // Spawn goal tapes

    memset(goalTapes, 0, sizeof(goalTapes));
    memset(&stobj, 0, sizeof(stobj));

    stobj.type = SOT_GOALTAPE;

    tape = goalTapes;
    stageAg = arg0;
    totalGoals = 0;
    for (i = 0; i < arg1; i++, stageAg++)
    {
        goal = stageAg->goals;
        for (j = 0; j < stageAg->goalCount; j++, goal++, tape++)
        {
            if (totalGoals >= MAX_GOALS)
            {
                window_set_cursor_pos(16, 16);
                window_printf_2("Warning!!! Goal Tape Max(%d) Over!!!\n", MAX_GOALS);
                break;
            }
            stobj.localPos = goal->pos;
            stobj.rotX = goal->rotX;
            stobj.rotY = goal->rotY;
            stobj.rotZ = goal->rotZ;
            stobj.animGroupId = i;
            tape->u_flags = 0;
            stobj.extraData = tape;
            spawn_stobj(&stobj);
            totalGoals++;
        }
    }

    // Spawn goal bags

    memset(goalBags, 0, sizeof(goalBags));
    memset(&stobj, 0, sizeof(stobj));

    if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
        stobj.type = SOT_GOALBAG_EXMASTER;
    else
        stobj.type = SOT_GOALBAG;

    sp10.x = 0.0f;
    sp10.y = 2.8f;
    sp10.z = 0.0f;

    bag = goalBags;
    stageAg = arg0;
    totalGoals = 0;
    for (i = 0; i < arg1; i++, stageAg++)
    {
        goal = stageAg->goals;
        for (j = 0; j < stageAg->goalCount; j++, goal++, bag++)
        {
            if (totalGoals >= MAX_GOALS)
                break;
            mathutil_mtxA_from_translate(&goal->pos);
            mathutil_mtxA_rotate_z(goal->rotZ);
            mathutil_mtxA_rotate_y(goal->rotY);
            mathutil_mtxA_rotate_x(goal->rotX);
            mathutil_mtxA_tf_point(&sp10, &stobj.unkA8);
            stobj.rotX = goal->rotX;
            stobj.rotY = goal->rotY;
            stobj.rotZ = goal->rotZ;
            stobj.animGroupId = i;
            bag->u_flags = 0;
            bag->goal = *goal;
            stobj.extraData = bag;
            spawn_stobj(&stobj);
            totalGoals++;
        }

    }
}

static struct NlModel *smallLCDModels[10];
static struct NlModel *largeLCDModels[10];

void stobj_goaltape_init(struct Stobj *stobj)
{
    float temp_f10;
    float temp_f11;
    int i;
    int j;
    struct GoalTapeSegment *seg;
    struct GoalTape *tape;
    Point3d sp28;
    struct RaycastHit spC;
    s16 *idxPtr;
    struct NlModel **mdlPtr;

    stobj->state = 0;
    stobj->flags |= STOBJ_FLAG_TANGIBLE;
    stobj->model = (struct GMAModel *)g_commonNlObj->models[NLMODEL_common_GOAL_TAPE];
    stobj->boundSphereRadius = 1.3125f;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    tape = stobj->extraData;
    tape->stobj = stobj;
    tape->u_breakTime = -1;
    mathutil_mtxA_from_translate(&stobj->localPos);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    mathutil_mtxA_tf_point(&stobj->u_model_origin, &sp28);
    mathutil_mtxA_push();
    raycast_stage_down(&sp28, &spC, NULL);
    mathutil_mtxA_pop();
    if (spC.flags & 1)
    {
        mathutil_mtxA_rigid_inv_tf_point(&spC.pos, &sp28);
        tape->unk4 = 0.002 + sp28.y;
    }
    else
        tape->unk4 = 0.002f;
    temp_f11 = stobj->u_model_origin.y;
    seg = tape->segments;
    for (i = 8; i > 0; i--, seg++)
    {
        temp_f10 = (i - 1) / 7.0f;
        seg->unk0.x = (1.75 * temp_f10) - 0.875;
        seg->unk0.y = temp_f11;
        seg->unk0.z = 0.0f;
        seg->unk18 = temp_f10;
        seg->unkC.x = 0.0f;
        seg->unkC.y = 0.0f;
        seg->unkC.z = 1.0f;
        seg->unk1C.x = 0.0f;
        seg->unk1C.y = 0.0f;
        seg->unk1C.z = 0.0f;
        seg->unk28 = 6;
        seg->unk2C = 0.225f;
    }
    tape->segments[0].unk28 &= 0xFFFFFFFD;
    tape->segments[0].unk28 |= 1;
    tape->segments[7].unk28 &= 0xFFFFFFFB;
    tape->segments[7].unk28 |= 1;
    tape->unk8 = temp_f11;
    tape->unkC = temp_f11;

    idxPtr = smallLCDModelIDs;
    mdlPtr = smallLCDModels;
    for (i = 0; i < 10; i++, mdlPtr++, idxPtr++)
         *mdlPtr = NLOBJ_MODEL(g_commonNlObj, *idxPtr);
    idxPtr = largeLCDModelIDs;
    mdlPtr = largeLCDModels;
    for(i = 0; i < 10; i++, mdlPtr++, idxPtr++)
         *mdlPtr = NLOBJ_MODEL(g_commonNlObj, *idxPtr);
}

void stobj_goaltape_main(struct Stobj *stobj)
{
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    float temp_f0;
    float temp_f1;
    float temp_f1_2;
    int i;
    struct GoalTape *tape;
    struct GoalTapeSegment *seg;
    struct GoalTapeSegment *temp_r27_2;
    struct AnimGroupInfo *animGrp;
    u32 var_r30;

    tape = stobj->extraData;
    switch (stobj->state)
    {
    case 1:
    case 2:
        switch (gameMode)
        {
        case MD_GAME:
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MAIN_COMPETITION:
                stobj->state = 3;
                stobj->counter = 90;
                break;
            default:
                stobj->state = 0;
                break;
            }
            break;
        default:
            stobj->state = 0;
            break;
        }
        break;
    case 3:
        stobj->counter--;
        if (stobj->counter < 0)
        {
            if (!(infoWork.flags & 0x20))
                stobj->state = 4;
            else
                stobj->state = 0;
        }
        break;
    case 4:
    case 5:
        stobj->state = 0;
        tape->unk8 = -0.25f;
        tape->unkC = stobj->u_model_origin.y;
        func_8006FD44(stobj->extraData);
        break;
    }

    if (stobj->animGroupId == 0)
        var_r30 = 0;
    else
    {
        animGrp = &animGroups[stobj->animGroupId];
        var_r30 = 1;
        mathutil_mtxA_from_mtx(animGrp->prevTransform);
        mathutil_mtxA_tf_point(&stobj->localPos, &spC);
        mathutil_mtxA_from_mtx(animGrp->transform);
        mathutil_mtxA_tf_point(&stobj->localPos, &sp30);
        spC.x -= sp30.x;
        spC.y -= sp30.y;
        spC.z -= sp30.z;
        mathutil_mtxA_rotate_z(stobj->rotZ);
        mathutil_mtxA_rotate_y(stobj->rotY);
        mathutil_mtxA_rotate_x(stobj->rotX);
        mathutil_mtxA_rigid_inv_tf_vec(&spC, &spC);
    }
    mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
    mathutil_mtxA_translate(&stobj->localPos);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    sp18.x = 0.004 * g_gravityDir.x;
    sp18.y = 0.004 * g_gravityDir.y;
    sp18.z = 0.004 * g_gravityDir.z;
    mathutil_mtxA_rigid_inv_tf_vec(&sp18, &sp18);

    seg = tape->segments;
    for (i = 8; i > 0; i--, seg++)
    {
        if (!(seg->unk28 & 1))
        {
            sp24.x = 0.0f;
            sp24.y = 0.0f;
            sp24.z = 0.0f;
            if (seg->unk28 & 2)
            {
                temp_r27_2 = seg - 1;
                sp24.x += temp_r27_2->unkC.x;
                sp24.y += temp_r27_2->unkC.y;
                sp24.z += temp_r27_2->unkC.z;
                sp30.x = temp_r27_2->unk0.x - seg->unk0.x;
                sp30.y = temp_r27_2->unk0.y - seg->unk0.y;
                sp30.z = temp_r27_2->unk0.z - seg->unk0.z;
                temp_f1 = mathutil_vec_normalize_len(&sp30);
                temp_f0 = temp_r27_2->unk2C;
                if (temp_f1 > temp_f0)
                {
                    temp_f0 = 1.05 * (temp_f1 - temp_f0);
                    seg->unk1C.x += sp30.x * temp_f0;
                    seg->unk1C.y += sp30.y * temp_f0;
                    seg->unk1C.z += sp30.z * temp_f0;
                }
                else
                {
                    temp_f0 = -temp_f0;
                    seg->unk0.x = temp_r27_2->unk0.x + (sp30.x * temp_f0);
                    seg->unk0.y = temp_r27_2->unk0.y + (sp30.y * temp_f0);
                    seg->unk0.z = temp_r27_2->unk0.z + (sp30.z * temp_f0);
                    temp_f0 = -mathutil_vec_dot_prod(&seg->unk1C, &sp30);
                    seg->unk1C.x += sp30.x * temp_f0;
                    seg->unk1C.y += sp30.y * temp_f0;
                    seg->unk1C.z += sp30.z * temp_f0;
                }
            }
            if (seg->unk28 & 4)
            {
                temp_r27_2 = seg + 1;
                sp24.x += temp_r27_2->unkC.x;
                sp24.y += temp_r27_2->unkC.y;
                sp24.z += temp_r27_2->unkC.z;
                sp30.x = temp_r27_2->unk0.x - seg->unk0.x;
                sp30.y = temp_r27_2->unk0.y - seg->unk0.y;
                sp30.z = temp_r27_2->unk0.z - seg->unk0.z;
                temp_f1_2 = mathutil_vec_normalize_len(&sp30);
                temp_f0 = seg->unk2C;
                if (temp_f1_2 > temp_f0)
                {
                    temp_f0 = 1.05 * (temp_f1_2 - temp_f0);
                    seg->unk1C.x += sp30.x * temp_f0;
                    seg->unk1C.y += sp30.y * temp_f0;
                    seg->unk1C.z += sp30.z * temp_f0;
                }
                else
                {
                    temp_f0 = -temp_f0;
                    seg->unk0.x = temp_r27_2->unk0.x + (sp30.x * temp_f0);
                    seg->unk0.y = temp_r27_2->unk0.y + (sp30.y * temp_f0);
                    seg->unk0.z = temp_r27_2->unk0.z + (sp30.z * temp_f0);
                    temp_f0 = -mathutil_vec_dot_prod(&seg->unk1C, &sp30);
                    seg->unk1C.x += sp30.x * temp_f0;
                    seg->unk1C.y += sp30.y * temp_f0;
                    seg->unk1C.z += sp30.z * temp_f0;
                }
            }
            seg->unk1C.x += sp18.x;
            seg->unk1C.y += sp18.y;
            seg->unk1C.z += sp18.z;
            mathutil_mtxA_from_rotate_y(49152.0 * (seg->unk1C.x + seg->unk1C.z));
            mathutil_mtxA_rotate_x((s16)(98304.0 * seg->unk1C.y));
            mathutil_mtxA_tf_vec(&seg->unkC, &seg->unkC);
            seg->unkC.x += 0.075 * (sp24.x - seg->unkC.x);
            seg->unkC.y += 0.075 * (sp24.y - seg->unkC.y);
            seg->unkC.z += 0.075 * (sp24.z - seg->unkC.z);
            mathutil_vec_normalize_len(&seg->unkC);
        }
    }

    if (tape->unk8 > tape->unkC)
    {
        tape->unk8 -= 0.016666668f;
        if (tape->unk8 < tape->unkC)
            tape->unk8 = tape->unkC;
    }
    else if (tape->unk8 < tape->unkC)
    {
        tape->unk8 += 0.016666668f;
        if (tape->unk8 > tape->unkC)
            tape->unk8 = tape->unkC;
    }

    tape->segments[0].unk0.y = tape->unk8;
    tape->segments[7].unk0.y = tape->unk8;

    seg = tape->segments;
    for (i = 8; i > 0; i--, seg++)
    {
        if (!(seg->unk28 & 1))
        {
            if (var_r30 && ((seg->unk28 & 6) != 6))
            {
                seg->unk1C.x += (1.0 - 0.85) * (spC.x - seg->unk1C.x);
                seg->unk1C.y += (1.0 - 0.85) * (spC.y - seg->unk1C.y);
                seg->unk1C.z += (1.0 - 0.85) * (spC.z - seg->unk1C.z);
            }
            else
            {
                seg->unk1C.x *= 0.85;
                seg->unk1C.y *= 0.85;
                seg->unk1C.z *= 0.85;
            }
            seg->unk0.x += seg->unk1C.x;
            seg->unk0.y += seg->unk1C.y;
            seg->unk0.z += seg->unk1C.z;
            if (seg->unk0.y < tape->unk4)
            {
                seg->unk0.y = tape->unk4;
                seg->unk1C.y = 0.0f;
                seg->unkC.x *= 0.9;
                seg->unkC.y += 0.1 * (1.0 - seg->unkC.y);
                seg->unkC.z *= 0.9;
                mathutil_vec_normalize_len(&seg->unkC);
            }
        }
    }
}

void stobj_goaltape_draw(struct Stobj *stobj)
{
    int i;
    struct GoalTapeSegment *var_r31;
    struct GoalTapeSegment *temp_r28;
    int var_r27;
    int temp_r0;
    int temp_r5;
    Vec spC;
    int time;
    int digit;

    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_translate(&stobj->localPos);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    set_render_ambient(0.8f, 0.8f, 0.8f);

    var_r31 = ((struct GoalTape *)stobj->extraData)->segments;
    for (i = 8; i > 0; i--, var_r31++)
    {
        temp_r28 = var_r31;
        var_r27 = 1;
        while ((i > 0) && ((var_r31->unk28 & 4) != 0))
        {
            i--;
            var_r31++;
            var_r27++;
        }
        if (var_r27 > 1)
        {
            mathutil_mtxA_push();
            deform_goaltape_model(temp_r28, var_r27, (void *)stobj->model, lbl_802F1B4C);
            mathutil_mtxA_pop();
            nl2ngc_draw_model_sort_none_alt2(lbl_802F1B4C);
        }
    }
    apply_curr_light_group_ambient();
    nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);
    temp_r5 = g_recplayInfo.u_playerId;
    if (g_poolInfo.playerPool.statusList[temp_r5] == STAT_NORMAL && (ballInfo[temp_r5].flags & 0x01000000))
        time = (100.0 * recplay_get_info_timer(g_recplayInfo.u_replayIndexes[temp_r5], g_recplayInfo.u_timeOffset)) / 60.0;
    else
    {
        temp_r0 = infoWork.timerCurr * 0x64;
        time = temp_r0 / 60;
    }
    if (time < 0)
        time = 0;

    spC.x = -0.45f;
    spC.y = 0.0f;
    spC.z = 0.0f;

    // Draw LCD display on goal

    mathutil_mtxA_push();

    digit = time % 10;
    time /= 10;
    nl2ngc_draw_model_sort_translucent_alt2(smallLCDModels[digit]);

    mathutil_mtxA_translate(&spC);

    digit = time % 10;
    time /= 10;
    nl2ngc_draw_model_sort_translucent_alt2(smallLCDModels[digit]);

    mathutil_mtxA_pop();

    spC.x = -0.6666f;

    digit = time % 10;
    time /= 10;
    nl2ngc_draw_model_sort_translucent_alt2(largeLCDModels[digit]);

    mathutil_mtxA_translate(&spC);

    digit = time % 10;
    time /= 10;
    nl2ngc_draw_model_sort_translucent_alt2(largeLCDModels[digit]);

    mathutil_mtxA_translate(&spC);

    digit = time % 10;
    time /= 10;
    nl2ngc_draw_model_sort_translucent_alt2(largeLCDModels[digit]);

    fade_color_base_default();
}

void stobj_goaltape_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    Vec sp58;
    Vec sp4C;
    Vec sp40;
    Vec sp34;
    Vec sp28;
    Vec sp1C;
    Vec sp10;
    float radius;
    float radiusSq;
    int i;
    struct GoalTapeSegment *seg;
    u32 var_r28 = 0;

    sp1C.x = 0.0f;
    sp1C.y = 0.0f;
    sp1C.z = 0.0f;
    mathutil_mtxA_from_identity();
    mathutil_mtxA_translate(&stobj->localPos);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    mathutil_mtxA_rigid_inv_tf_point(&ball->pos, &sp58);
    radius = ball->radius;
    radiusSq = radius * radius;

    seg = ((struct GoalTape *)stobj->extraData)->segments;
    for (i = 8; i > 0; i--, seg++)
    {
        if (seg->unk28 & 1)
            continue;
        sp4C.x = seg->unk0.x - sp58.x;
        if (fabs(sp4C.x) > radius)
            continue;
        sp4C.z = seg->unk0.z - sp58.z;
        if (fabs(sp4C.z) > radius)
            continue;
        sp4C.y = seg->unk0.y - sp58.y;
        if (fabs(sp4C.y) > radius)
            continue;
        if (mathutil_vec_sq_len(&sp4C) > radiusSq)
            continue;
        mathutil_vec_set_len(&sp4C, &sp4C, radius);
        var_r28 = 1;
        sp34.x = sp58.x + sp4C.x;
        sp34.y = sp58.y + sp4C.y;
        sp34.z = sp58.z + sp4C.z;
        sp28.x = sp34.x - seg->unk0.x;
        sp28.y = sp34.y - seg->unk0.y;
        sp28.z = sp34.z - seg->unk0.z;
        seg->unk0.x = sp34.x;
        seg->unk0.y = sp34.y;
        seg->unk0.z = sp34.z;
        seg->unk1C.x += sp28.x;
        seg->unk1C.y += sp28.y;
        seg->unk1C.z += sp28.z;
        mathutil_vec_set_len(&sp4C, &sp10, -0.05 * mathutil_vec_len(&sp28));
        sp1C.x += sp10.x;
        sp1C.y += sp10.y;
        sp1C.z += sp10.z;
        sp40.x = seg->unkC.x;
        sp40.y = seg->unkC.y;
        sp40.z = seg->unkC.z;
        mathutil_vec_normalize_len(&sp4C);
        if (mathutil_vec_dot_prod(&sp4C, &sp40) < 0.0)
        {
            sp4C.x = -sp4C.x;
            sp4C.y = -sp4C.y;
            sp4C.z = -sp4C.z;
        }
        sp40.x += 0.125 * (sp4C.x - sp40.x);
        sp40.y += 0.125 * (sp4C.y - sp40.y);
        sp40.z += 0.125 * (sp4C.z - sp40.z);
        mathutil_vec_normalize_len(&sp40);
        seg->unkC.x = sp40.x;
        seg->unkC.y = sp40.y;
        seg->unkC.z = sp40.z;
    }
    if (var_r28 != 0)
    {
        mathutil_mtxA_tf_vec(&sp1C, &sp1C);
        ball->vel.x += sp1C.x;
        ball->vel.y += sp1C.y;
        ball->vel.z += sp1C.z;
    }
}

void stobj_goaltape_destroy(struct Stobj *stobj) {}

void stobj_goaltape_debug(struct Stobj *stobj) {}

static void deform_goaltape_model(struct GoalTapeSegment *arg0, int faceCount, struct NlModel *model1, struct NlModel *model2)
{
    Point3d sp24;
    Point3d sp18;
    s16 rotX;
    struct NlVtxTypeB *vtx;
    s16 rotY;
    struct NlMesh *mesh2;
    struct NlMesh *mesh1;
    u32 var_r4;

    memcpy(model2, model1, sizeof(*model2));

    mesh2 = (struct NlMesh *)model2->meshStart;
    mesh1 = (struct NlMesh *)model1->meshStart;
    memcpy(mesh2, mesh1, sizeof(*mesh2));

    mesh2->dispListSize = (faceCount * 64) + 8;
    var_r4 = ((struct NlDispList *)mesh1->dispListStart)->flags;
    var_r4 &= ~0x14;
    var_r4 |= 0x10;
    ((struct NlDispList *)mesh2->dispListStart)->flags = var_r4;
    ((struct NlDispList *)mesh2->dispListStart)->faceCount = faceCount * 2;

    vtx = (void *)(mesh2 = (void *)((struct NlDispList *)mesh2->dispListStart)->vtxData);
    while (faceCount > 0)
    {
        sp24.x = arg0->unkC.x;
        sp24.y = arg0->unkC.y;
        sp24.z = arg0->unkC.z;
        rotY = mathutil_atan2(sp24.x, sp24.z) - 0x8000;
        rotX = mathutil_atan2(sp24.y, mathutil_sqrt(mathutil_sum_of_sq_2(sp24.x, sp24.z)));
        mathutil_mtxA_from_rotate_y(rotY);
        mathutil_mtxA_rotate_x(rotX);
        sp18.x = 0.0f;
        sp18.y = 0.125f;
        sp18.z = 0.0f;
        mathutil_mtxA_tf_vec(&sp18, &sp18);

        vtx[0].x = arg0->unk0.x + sp18.x;
        *(u32 *)&vtx[0].x |= 1;
        vtx[0].y = arg0->unk0.y + sp18.y;
        vtx[0].z = arg0->unk0.z + sp18.z;
        vtx[0].nx = sp24.x;
        vtx[0].ny = sp24.y;
        vtx[0].nz = sp24.z;
        vtx[0].s = arg0->unk18;
        vtx[0].t = 1.0f;
        *(u32 *)&vtx[0].t |= 1;

        vtx[1].x = arg0->unk0.x - sp18.x;
        *(u32 *)&vtx[1].x |= 1;
        vtx[1].y = arg0->unk0.y - sp18.y;
        vtx[1].z = arg0->unk0.z - sp18.z;
        vtx[1].nx = sp24.x;
        vtx[1].ny = sp24.y;
        vtx[1].nz = sp24.z;
        vtx[1].s = arg0->unk18;
        vtx[1].t = 0.0f;
        *(u32 *)&vtx[1].t |= 1;

        vtx += 2;
        arg0++;
        faceCount--;
    }
    *(u32 *)vtx = 0;
}

void stobj_goalbag_init(struct Stobj *stobj)
{
    struct GoalBag *bag;

    stobj->state = 1;
    stobj->flags = STOBJ_FLAG_TANGIBLE|STOBJ_FLAG_ROTATION_UNK;
    stobj->model = commonGma->modelEntries[NEW_SCENT_BAG_WHOLE].model;
    stobj->boundSphereRadius = stobj->model->boundSphereRadius;
    stobj->u_model_origin = stobj->model->boundSphereCenter;
    stobj->localPos = stobj->unkA8;
    stobj->offsetPos.x = 0.0f;
    stobj->offsetPos.y = -1.0f;
    stobj->offsetPos.z = 0.1f;
    mathutil_vec_normalize_len(&stobj->offsetPos);
    stobj->offsetVel.x = 0.0f;
    stobj->offsetVel.y = 0.0f;
    stobj->offsetVel.z = 0.0f;
    bag = stobj->extraData;
    bag->stobj = stobj;
    bag->u_openTime = -1;
    bag->open = 0.0f;
    bag->openSpeed = 0.0f;
}

const Vec lbl_80117A58 = { 0.0f, 3.55f, 0.0f };
const Vec lbl_80117A64 = { 1.0f, 0.0f, 0.0f };

void stobj_goalbag_main(struct Stobj *stobj)
{
    Point3d currPos;
    Point3d sp3C;
    Point3d sp30;
    Point3d sp24;
    Point3d sp18;
    Point3d spC;
    float temp_f12;
    float temp_f2_4;
    float temp_f2_6;
    float temp_f31;
    struct AnimGroupInfo *temp_r29;
    struct GoalBag *bag = stobj->extraData;

    switch (stobj->state)
    {
    case 1:
        break;
    case 2:  // opening ball?
    case 3:
        stobj->state = 4;
        switch (gameMode)
        {
        case MD_GAME:
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MAIN_COMPETITION:
                stobj->counter = 120;
                break;
            default:
                stobj->counter = -1;
                break;
            }
            break;
        default:
            stobj->counter = -1;
            break;
        }
        bag->openSpeed = 0.05 + 0.1 * RAND_FLOAT();
        // fall through
    case 4:
        if (stobj->counter > 0)
        {
            stobj->counter--;
            if (stobj->counter == 0 && !(infoWork.flags & 0x20))
                stobj->state = 5;
        }
        bag->openSpeed += 0.005;
        bag->openSpeed *= 0.99;
        bag->open += bag->openSpeed;
        if (bag->open < 0.0)
        {
            bag->open = 0.0f;
            if (bag->openSpeed < 0.0)
                bag->openSpeed = 0.5 * -bag->openSpeed;
        }
        else if (bag->open > 1.0)
        {
            bag->open = 1.0f;
            if (bag->openSpeed > 0.0)
                bag->openSpeed = 0.5 * -bag->openSpeed;
        }
        break;
    case 5:
    case 6:
        stobj->state = 7;
        stobj->counter = 60;
        bag->openSpeed = 0.05 + 0.1 * RAND_FLOAT();
        // fall through
    case 7:
        stobj->counter--;
        bag->openSpeed -= 0.005;
        bag->openSpeed *= 0.99;
        bag->open += bag->openSpeed;
        if (bag->open < 0.0)
        {
            bag->open = 0.0f;
            bag->u_flags = 0;
            bag->u_openTime = -1;
            if (stobj->counter < 0)
            {
                stobj->state = 1;
                bag->openSpeed = 0.0f;
            }
            else
            {
                if (bag->openSpeed < 0.0)
                    bag->openSpeed = 0.5 * -bag->openSpeed;
            }
        }
        else if (bag->open > 1.0)
        {
            bag->open = 1.0f;
            if (bag->openSpeed > 0.0)
                bag->openSpeed = 0.5 * -bag->openSpeed;
        }
        break;
    }

    sp3C.x = 0.008f * g_gravityDir.x;
    sp3C.y = 0.008f * g_gravityDir.y;
    sp3C.z = 0.008f * g_gravityDir.z;
    if (stobj->animGroupId > 0)
    {
        mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
        mathutil_mtxA_rigid_inv_tf_vec(&sp3C, &sp3C);
    }
    stobj->offsetVel.x += sp3C.x;
    stobj->offsetVel.y += sp3C.y;
    stobj->offsetVel.z += sp3C.z;
    if (stobj->animGroupId > 0)
    {
        temp_r29 = &animGroups[stobj->animGroupId];
        mathutil_mtxA_from_mtx(temp_r29->prevTransform);
        mathutil_mtxA_tf_point(&stobj->pos, &sp3C);
        mathutil_mtxA_from_mtx(temp_r29->transform);
        mathutil_mtxA_tf_point(&stobj->pos, &currPos);
        sp3C.x -= currPos.x;
        sp3C.y -= currPos.y;
        sp3C.z -= currPos.z;
        mathutil_mtxA_rigid_inv_tf_vec(&sp3C, &sp3C);
        stobj->offsetVel.x += (1.0 - 0.98) * (sp3C.x - stobj->offsetVel.x);
        stobj->offsetVel.y += (1.0 - 0.98) * (sp3C.y - stobj->offsetVel.y);
        stobj->offsetVel.z += (1.0 - 0.98) * (sp3C.z - stobj->offsetVel.z);
    }
    else
    {
        stobj->offsetVel.x *= 0.98;
        stobj->offsetVel.y *= 0.98;
        stobj->offsetVel.z *= 0.98;
    }
    sp30 = lbl_80117A58;
    sp24 = lbl_80117A64;
    mathutil_mtxA_from_translate(&bag->goal.pos);
    mathutil_mtxA_rotate_z(bag->goal.rotZ);
    mathutil_mtxA_rotate_y(bag->goal.rotY);
    mathutil_mtxA_rotate_x(bag->goal.rotX);
    mathutil_mtxA_tf_point(&sp30, &sp30);
    mathutil_mtxA_tf_vec(&sp24, &sp24);
    temp_f31 = stobj->boundSphereRadius;
    sp18.x = stobj->localPos.x + (temp_f31 * stobj->offsetPos.x);
    sp18.y = stobj->localPos.y + (temp_f31 * stobj->offsetPos.y);
    sp18.z = stobj->localPos.z + (temp_f31 * stobj->offsetPos.z);
    sp3C.x = sp18.x - sp30.x;
    sp3C.y = sp18.y - sp30.y;
    sp3C.z = sp18.z - sp30.z;

    temp_f12 = sp3C.x * sp24.x + sp3C.y * sp24.y + sp3C.z * sp24.z;
    if (mathutil_vec_sq_len(&sp3C) - temp_f12 * temp_f12 < temp_f31 * temp_f31)
    {
        sp3C.x = sp30.x + sp24.x * temp_f12;
        sp3C.y = sp30.y + sp24.y * temp_f12;
        sp3C.z = sp30.z + sp24.z * temp_f12;
        spC.x = sp18.x - sp3C.x;
        spC.y = sp18.y - sp3C.y;
        spC.z = sp18.z - sp3C.z;
        mathutil_vec_normalize_len(&spC);
        stobj->offsetPos.x = sp3C.x + spC.x * temp_f31 - stobj->localPos.x;
        stobj->offsetPos.y = sp3C.y + spC.y * temp_f31 - stobj->localPos.y;
        stobj->offsetPos.z = sp3C.z + spC.z * temp_f31 - stobj->localPos.z;
        mathutil_vec_normalize_len(&stobj->offsetPos);
        temp_f2_4 = spC.x * stobj->offsetVel.x + spC.y * stobj->offsetVel.y + spC.z * stobj->offsetVel.z;
        if (temp_f2_4 < 0.0)
        {
            temp_f2_4 *= -1.5;
            stobj->offsetVel.x += temp_f2_4 * spC.x;
            stobj->offsetVel.y += temp_f2_4 * spC.y;
            stobj->offsetVel.z += temp_f2_4 * spC.z;
        }
    }
    temp_f2_6 = -mathutil_vec_dot_prod(&stobj->offsetPos, &stobj->offsetVel);
    stobj->offsetVel.x += temp_f2_6 * stobj->offsetPos.x;
    stobj->offsetVel.y += temp_f2_6 * stobj->offsetPos.y;
    stobj->offsetVel.z += temp_f2_6 * stobj->offsetPos.z;
    stobj->offsetPos.x += stobj->offsetVel.x;
    stobj->offsetPos.y += stobj->offsetVel.y;
    stobj->offsetPos.z += stobj->offsetVel.z;
    mathutil_vec_normalize_len(&stobj->offsetPos);
    mathutil_mtxA_from_rotate_y(-stobj->rotY);
    mathutil_mtxA_rotate_x(0);
    mathutil_mtxA_tf_vec(&stobj->offsetPos, &currPos);
    stobj->rotX = mathutil_atan2(currPos.z, currPos.y) - 0x8000;
    stobj->rotZ = mathutil_atan2(currPos.x, mathutil_sqrt(mathutil_sum_of_sq_2(currPos.z, currPos.y)));
    currPos.x = 2.0 * stobj->pos.x - stobj->localPos.x;
    currPos.y = 2.0 * stobj->pos.y - stobj->localPos.y;
    currPos.z = 2.0 * stobj->pos.z - stobj->localPos.z;
    // transform into world space
    if (stobj->animGroupId > 0)
    {
        mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
        mathutil_mtxA_tf_point(&currPos, &currPos);
    }
    set_ball_look_point(5, &currPos, 1.0f);
}

void stobj_goalbag_draw(struct Stobj *stobj)
{
    Point3d spC;
    float alpha;
    struct GMAModel *model;
    struct GoalBag *bag = stobj->extraData;
    int rotZ;

    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_translate(&stobj->localPos);
    mathutil_mtxA_rotate_y(stobj->rotY);
    mathutil_mtxA_rotate_x(stobj->rotX);
    mathutil_mtxA_rotate_z(stobj->rotZ);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);

    if (bag->open == 0.0)
    {
        // Draw closed ball
        model = stobj->model;
        alpha = func_8006FCD0(&model->boundSphereCenter, model->boundSphereRadius);
        if (alpha > 0.0f)
        {
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_culled_sort_all(model);
            }
            else
            {
                GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
                GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
                avdisp_draw_model_culled_sort_translucent(model);
            }
        }
    }
    else
    {
        // Draw two halves
        rotZ = 9102.0f * bag->open;
        spC.x = 0.0f;
        spC.y = -0.5 * bag->open;
        spC.z = 0.0f;
        mathutil_mtxA_translate(&spC);
        mathutil_mtxA_push();
        mathutil_mtxA_rotate_z(-(s16)(int)rotZ);
        model = commonGma->modelEntries[NEW_SCENT_BAG_A].model;
        alpha = func_8006FCD0(&model->boundSphereCenter, model->boundSphereRadius);
        if (alpha > 0.0f)
        {
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_culled_sort_all(model);
            }
            else
            {
                GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
                GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
                avdisp_draw_model_culled_sort_translucent(model);
            }
        }
        mathutil_mtxA_pop();
        mathutil_mtxA_rotate_z((s16)rotZ);
        model = commonGma->modelEntries[NEW_SCENT_BAG_B].model;
        alpha = func_8006FCD0(&model->boundSphereCenter, model->boundSphereRadius);
        if (alpha > 0.0f)
        {
            if (alpha < 1.0f)
            {
                avdisp_set_alpha(alpha);
                avdisp_draw_model_culled_sort_all(model);
            }
            else
            {
                GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
                GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
                avdisp_draw_model_culled_sort_translucent(model);
            }
        }
    }
}

void stobj_goalbag_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    Vec dirToBall;
    Vec bagVel;
    Vec ballRelVel;
    Vec ballVel;
    Vec ballDisplacement;
    float dotProd;
    float coliImpulse = 0.0f;
    float coliRadius;
    struct GoalBag *bag;

    // calculate normalized direction from bag to ball
    dirToBall.x = ball->pos.x - stobj->pos.x;
    dirToBall.y = ball->pos.y - stobj->pos.y;
    dirToBall.z = ball->pos.z - stobj->pos.z;
    mathutil_vec_normalize_len(&dirToBall);

    // bag velocity
    bagVel.x = stobj->pos.z - stobj->prevPos.z;
    bagVel.y = stobj->pos.y - stobj->prevPos.y;
    bagVel.z = stobj->pos.z - stobj->prevPos.z;

    // ball velocity
    ballVel.x = ball->vel.x;
    ballVel.y = ball->vel.y;
    ballVel.z = ball->vel.z;

    // ball velocity relative to bag
    ballRelVel.x = ballVel.x - bagVel.x;
    ballRelVel.y = ballVel.y - bagVel.y;
    ballRelVel.z = ballVel.z - bagVel.z;

    // calculate collision impulse
    dotProd = -1.5 * mathutil_vec_dot_prod(&dirToBall, &ball->vel);
    if (dotProd > 0.0)
    {
        coliImpulse = dotProd;
        // reflect ball velocity
        ballRelVel.x += dotProd * dirToBall.x;
        ballRelVel.y += dotProd * dirToBall.y;
        ballRelVel.z += dotProd * dirToBall.z;
        ball->vel.x = ballRelVel.x + bagVel.x;
        ball->vel.y = ballRelVel.y + bagVel.y;
        ball->vel.z = ballRelVel.z + bagVel.z;
    }
    coliRadius = stobj->boundSphereRadius + ball->radius;
    dirToBall.x *= coliRadius;
    dirToBall.y *= coliRadius;
    dirToBall.z *= coliRadius;

    // move ball outside of bag
    ballDisplacement.x = stobj->pos.x + dirToBall.x - ball->pos.x;
    ballDisplacement.y = stobj->pos.y + dirToBall.y - ball->pos.y;
    ballDisplacement.z = stobj->pos.z + dirToBall.z - ball->pos.z;
    ball->pos.x += ballDisplacement.x;
    ball->pos.y += ballDisplacement.y;
    ball->pos.z += ballDisplacement.z;

    dotProd = -mathutil_vec_dot_prod(&dirToBall, &stobj->offsetPos);
    dirToBall.x += dotProd * stobj->offsetPos.x;
    dirToBall.y += dotProd * stobj->offsetPos.y;
    dirToBall.z += dotProd * stobj->offsetPos.z;
    mathutil_vec_normalize_len(&dirToBall);
    ballRelVel.x = bagVel.x + stobj->offsetVel.x - ballVel.x;
    ballRelVel.y = bagVel.y + stobj->offsetVel.y - ballVel.y;
    ballRelVel.z = bagVel.z + stobj->offsetVel.z - ballVel.z;
    dotProd = -2.0 * mathutil_vec_dot_prod(&dirToBall, &ballRelVel);
    if (dotProd < 0.0)
    {
        ballRelVel.x += dotProd * dirToBall.x;
        ballRelVel.y += dotProd * dirToBall.y;
        ballRelVel.z += dotProd * dirToBall.z;
        stobj->offsetVel.x = ballVel.x + (ballRelVel.x - bagVel.x);
        stobj->offsetVel.y = ballVel.y + (ballRelVel.y - bagVel.y);
        stobj->offsetVel.z = ballVel.z + (ballRelVel.z - bagVel.z);
    }
    dotProd = -mathutil_vec_dot_prod(&stobj->offsetPos, &stobj->offsetVel);
    stobj->offsetVel.x += dotProd * stobj->offsetPos.x;
    stobj->offsetVel.y += dotProd * stobj->offsetPos.y;
    stobj->offsetVel.z += dotProd * stobj->offsetPos.z;
    bag = stobj->extraData;

    if (bag->u_flags != 0)  // bag is open
    {
        // opening speed is affected by collision somehow
        dotProd = -2.0 * mathutil_vec_dot_prod(&stobj->offsetPos, &ballVel);
        if (dotProd < 0.0)
            bag->openSpeed += dotProd;
    }

    if (coliImpulse > 0.1f)
    {
        struct Effect effect;
        Vec sp28 = { 0.0f, 0.0f, -1.0f };
        Vec sp1C;
        Vec bagPos;
        int var_r30;
        int i;
        int temp_r28;
        float temp_f6;
        float var_f24;

        mathutil_mtxA_push();
        coliRadius = stobj->boundSphereRadius;
        bagPos = stobj->pos;
        if (stobj->animGroupId > 0)
        {
            mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
            mathutil_mtxA_tf_point(&bagPos, &bagPos);
            mathutil_mtxA_tf_vec(&bagVel, &bagVel);
            mathutil_mtxA_tf_vec(&ballVel, &ballVel);
        }
        ballRelVel.x = 0.25f * (bagVel.x + ballVel.x);
        ballRelVel.y = 0.25f * (bagVel.y + ballVel.y);
        ballRelVel.z = 0.25f * (bagVel.z + ballVel.z);
        memset(&effect, 0, sizeof(effect));
        var_f24 = 0.1f;
        effect.playerId = currentBall->playerId;
        for (i = 2; i > 0; i--)
        {
            var_r30 = 16.0f + (16.0f * coliImpulse);
            effect.type = ET_COLISTAR_PARTICLE;
            if (var_r30 > 32)
                var_r30 = 32;
            temp_r28 = 65536.0f / var_r30;
            mathutil_mtxA_from_rotate_y(rand() & 0x7FFF);
            mathutil_mtxA_rotate_x(rand() & 0x7FFF);
            mathutil_mtxA_rotate_z(rand() & 0x7FFF);
            while (var_r30 > 0)
            {
                mathutil_mtxA_rotate_y(temp_r28);
                mathutil_mtxA_tf_vec(&sp28, &sp1C);
                effect.pos.x = bagPos.x + (coliRadius * sp1C.x);
                effect.pos.y = bagPos.y + (coliRadius * sp1C.y);
                effect.pos.z = bagPos.z + (coliRadius * sp1C.z);
                temp_f6 = var_f24 + 0.02f * RAND_FLOAT();
                effect.vel.x = ballRelVel.x + (temp_f6 * sp1C.x);
                effect.vel.y = ballRelVel.y + (temp_f6 * sp1C.y);
                effect.vel.z = ballRelVel.z + (temp_f6 * sp1C.z);
                spawn_effect(&effect);
                var_r30--;
            }
            var_r30 = 12.0f + (12.0f * coliImpulse);
            effect.type = ET_COLI_PARTICLE;
            if (var_r30 > 24)
                var_r30 = 24;
            temp_r28 = 65536.0f / var_r30;
            while (var_r30 > 0)
            {
                mathutil_mtxA_rotate_y(temp_r28);
                mathutil_mtxA_tf_vec(&sp28, &sp1C);
                effect.pos.x = bagPos.x + (coliRadius * sp1C.x);
                effect.pos.y = bagPos.y + (coliRadius * sp1C.y);
                effect.pos.z = bagPos.z + (coliRadius * sp1C.z);
                temp_f6 = var_f24 + 0.02f * RAND_FLOAT();
                effect.vel.x = ballRelVel.x + (temp_f6 * sp1C.x);
                effect.vel.y = ballRelVel.y + (temp_f6 * sp1C.y);
                effect.vel.z = ballRelVel.z + (temp_f6 * sp1C.z);
                spawn_effect(&effect);
                var_r30--;
            }
            var_f24 *= 0.5f;
            ballRelVel.x *= 2.0f;
            ballRelVel.y *= 2.0f;
            ballRelVel.z *= 2.0f;
        }
        mathutil_mtxA_pop();
    }
}

void stobj_goalbag_destroy(struct Stobj *stobj) {}

void stobj_goalbag_debug(struct Stobj *stobj) {}

void stobj_goalbag_exmaster_init(struct Stobj *stobj)
{
    stobj_goalbag_init(stobj);
}

void stobj_goalbag_exmaster_main(struct Stobj *stobj)
{
    stobj_goalbag_main(stobj);
}

void stobj_goalbag_exmaster_draw(struct Stobj *stobj)
{
    stobj_goalbag_draw(stobj);
}

void stobj_goalbag_exmaster_coli(struct Stobj *stobj, struct PhysicsBall *ball)
{
    stobj_goalbag_coli(stobj, ball);
}

void stobj_goalbag_exmaster_destroy(struct Stobj *stobj) {}

void stobj_goalbag_exmaster_debug(struct Stobj *stobj) {}

void break_goaltape(int goalId, struct PhysicsBall *arg1)
{
    Vec sp1C;
    Vec sp10;
    float temp_f1;
    float var_f31;
    int i;
    struct GoalTape *tape;
    struct GoalTapeSegment *seg;
    int var_r28;
    struct Stobj *stobj;

    open_goalbag(goalId, arg1);
    if (goalId < ARRAY_COUNT(goalTapes))
    {
        tape = &goalTapes[goalId];
        if (tape->u_flags == 0)
        {
            stobj = tape->stobj;
            mathutil_mtxA_from_identity();
            mathutil_mtxA_translate(&stobj->localPos);
            mathutil_mtxA_rotate_z(stobj->rotZ);
            mathutil_mtxA_rotate_y(stobj->rotY);
            mathutil_mtxA_rotate_x(stobj->rotX);
            mathutil_mtxA_rigid_inv_tf_point(&arg1->pos, &sp1C);
            mathutil_mtxA_rigid_inv_tf_vec(&arg1->vel, &sp10);
            if (tape->u_breakTime < 0)
                tape->u_breakTime = infoWork.timerCurr;
            var_f31 = 17.5f;
            var_r28 = -1;
            seg = tape->segments;
            for (i = 0; i < 8; i++, seg++)
            {
                if (seg->unk28 & 4)
                {
                    temp_f1 = mathutil_vec_distance(&seg->unk0, &sp1C);
                    if (var_f31 > temp_f1)
                    {
                        var_f31 = temp_f1;
                        var_r28 = i;
                    }
                }
            }
            if (var_r28 >= 0)
            {
                seg = &tape->segments[var_r28];
                seg[0].unk28 &= 0xFFFFFFFB;
                seg[1].unk28 &= 0xFFFFFFFD;
                seg[0].unk1C.x += sp10.x;
                seg[0].unk1C.y += sp10.y;
                seg[0].unk1C.z += sp10.z;
                seg[1].unk1C.x += sp10.x;
                seg[1].unk1C.y += sp10.y;
                seg[1].unk1C.z += sp10.z;
                stobj->state = 1;
                tape->u_flags = 1;
            }
        }
    }
}

void relink_goaltape(int time)
{
    int i;
    int j;
    struct GoalTape *tape;
    struct GoalTapeSegment *seg;
    u8 dummy[8];

    close_goalbag(time);
    tape = goalTapes;
    for (i = ARRAY_COUNT(goalTapes); i > 0; i--, tape++)
    {
        if (tape->u_breakTime <= time && tape->u_flags != 0)
        {
            tape->u_flags = 0;
            seg = tape->segments;
            for (j = 8; j > 0; j--, seg++)
            {
                seg->unk0.x = 1.75 * ((j - 1) / 7.0f) - 0.875;
                seg->unk0.y = tape->stobj->u_model_origin.y;
                seg->unk0.z = 0.0f;
                seg->unkC.x *= 0.25;
                seg->unkC.y *= 0.25;
                seg->unkC.z = 0.75 + 0.25 * seg->unkC.z;
                mathutil_vec_normalize_len(&seg->unkC);
                seg->unk28 |= 6;
            }
            tape->segments[0].unk28 &= 0xFFFFFFFD;
            tape->segments[7].unk28 &= 0xFFFFFFFB;
        }
    }
}

static void open_goalbag(int goalId, struct PhysicsBall *physBall)
{
    struct Effect effect;
    Point3d confettiPos;
    Point3d bagPos;
    Point3d bagPrevPos;
    float spread;
    int confettiCount;
    struct GoalBag *bag;
    struct Stobj *stobj;
    u16 cameraMask;

    if (goalId < 8)
    {
        bag = &goalBags[goalId];
        stobj = bag->stobj;
        if (bag->u_flags == 0)
        {
            bag->u_flags = 1;
            stobj->state = 2;
            stobj->boundSphereRadius = 0.5 * stobj->model->boundSphereRadius;
            stobj->u_model_origin = stobj->model->boundSphereCenter;
            stobj->u_model_origin.x *= 0.5;
            stobj->u_model_origin.y *= 0.5;
            stobj->u_model_origin.z *= 0.5;
            stobj->offsetVel.x += 0.5 * physBall->vel.x;
            stobj->offsetVel.y += 0.5 * physBall->vel.y;
            stobj->offsetVel.z += 0.5 * physBall->vel.z;
            u_play_sound_0(0x16);
            u_play_sound_0(0x127);
            if (bag->u_openTime < 0)
                bag->u_openTime = infoWork.timerCurr;
            cameraMask = 1 << currentBall->playerId;

            memset(&effect, 0, sizeof(effect));

            // set the velocity of confetti to the bag velocity plus ball velocity
            effect.type = ET_PAPERFRAG;
            mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
            mathutil_mtxA_tf_vec(&physBall->vel, &effect.vel);
            mathutil_mtxA_tf_point(&stobj->pos, &bagPos);
            mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].prevTransform);
            mathutil_mtxA_tf_point(&stobj->prevPos, &bagPrevPos);
            effect.vel.x += bagPos.x - bagPrevPos.x;
            effect.vel.y += bagPos.y - bagPrevPos.y;
            effect.vel.z += bagPos.z - bagPrevPos.z;

            spread = stobj->model->boundSphereRadius - commonGma->modelEntries[PAPER_PIECE_DEEPGREEN].model->boundSphereRadius;

            // put mtxA in object space
            mathutil_mtxA_from_mtx(animGroups[stobj->animGroupId].transform);
            mathutil_mtxA_translate(&stobj->pos);

            // Reduce the amount of confetti in competition mode (causes lag?)
            if (gameMode == MD_GAME && modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
            {
                switch (modeCtrl.playerCount)
                {
                case 2:  confettiCount = 130; break;
                case 3:  confettiCount = 100; break;
                case 4:  confettiCount = 70;  break;
                default: confettiCount = 160; break;
                }
            }
            else
                confettiCount = 160;

            // Release confetti
            confettiPos.x = 0.0f;
            confettiPos.y = 0.0f;
            while (confettiCount > 0)
            {
                confettiPos.z = 0.5 * (spread * (1.0 + RAND_FLOAT()));
                mathutil_mtxA_rotate_y(rand() & 0x7FFF);
                mathutil_mtxA_rotate_x(rand() & 0x7FFF);
                mathutil_mtxA_tf_point(&confettiPos, &effect.pos);
                effect.rotX = rand() & 0x7FFF;
                effect.rotY = rand() & 0x7FFF;
                effect.rotZ = rand() & 0x7FFF;
                if (confettiCount & 1)
                    effect.cameras = cameraMask;
                else
                    effect.cameras = 0;
                spawn_effect(&effect);
                confettiCount--;
            }
        }
    }
}

static void close_goalbag(int time)
{
    int i;
    struct GoalBag *bag;
    struct Stobj *stobj;

    bag = goalBags;
    for (i = ARRAY_COUNT(goalBags); i > 0; i--, bag++)
    {
        if (bag->u_openTime <= time && bag->u_flags != 0)
        {
            bag->u_flags = 0;
            bag->open = 0.0f;
            bag->openSpeed = 0.0f;
            stobj = bag->stobj;
            stobj->boundSphereRadius = stobj->model->boundSphereRadius;
            stobj->u_model_origin = stobj->model->boundSphereCenter;
            stobj->state = 1;
        }
    }
}

static float func_8006FCD0(Point3d *arg0, float arg8)
{
    Point3d sp10;
    float var_f1;

    mathutil_mtxA_tf_point(arg0, &sp10);
    sp10.z += 0.1f + arg8;
    sp10.z = -sp10.z;
    if (sp10.z > 1.0f)
        return 1.0f;
    var_f1 = 0.0f;
    if (sp10.z > 0.0f)
        var_f1 = sp10.z;
    return var_f1;
}

static void func_8006FD44(struct GoalTape *tape)
{
    int i;
    struct GoalTapeSegment *seg;
    float y;
    u8 unused[8];

    tape->u_flags = 0;
    tape->u_breakTime = -1;
    y = tape->unk8;
    seg = tape->segments;
    for (i = 8; i > 0; i--, seg++)
    {
        seg->unk0.x = 1.75 * (((i - 1) / 7.0f) - 0.5f);
        seg->unk0.y = y;
        seg->unk0.z = 0.0f;
        seg->unk1C.x *= 0.05f;
        seg->unk1C.y *= 0.05f;
        seg->unk1C.z *= 0.05f;
        seg->unkC.x *= 0.25f;
        seg->unkC.y *= 0.25f;
        seg->unkC.z = 0.75f + 0.25f * seg->unkC.z;
        mathutil_vec_normalize_len(&seg->unkC);
        seg->unk28 = 6;
    }
    tape->segments[0].unk28 &= 0xFFFFFFFD;
    tape->segments[0].unk28 |= 1;
    tape->segments[7].unk28 &= 0xFFFFFFFB;
    tape->segments[7].unk28 |= 1;
}
