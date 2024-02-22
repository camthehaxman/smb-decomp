#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "event.h"
#include "info.h"
#include "item.h"
#include "mathutil.h"
#include "mode.h"
#include "obj_collision.h"
#include "pool.h"
#include "stage.h"
#include "stcoli.h"
#include "stobj.h"
#include "window.h"

static void do_object_collision(void);

void ev_obj_collision_init(void) {}

void ev_obj_collision_main(void)
{
    if (!(debugFlags & 0xA))
        do_object_collision();
}

void ev_obj_collision_dest(void) {}

static void do_object_collision(void)
{
    Vec sp70;
    struct Ball *ballBackup = currentBall;
    int i;
    struct Ball *ball;
    s8 *status;

    ball = ballInfo;
    status = g_poolInfo.playerPool.statusList;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
    {
        currentBall = ball;
        if (*status != STAT_NULL && *status != STAT_FREEZE)
        {
            struct PhysicsBall physBall;

            init_physball_from_ball(ball, &physBall);

            // Collision with stage objects
            if (eventInfo[EVENT_STOBJ].state == EV_STATE_RUNNING)
            {
                int j;
                s8 *status = g_poolInfo.stobjPool.statusList;
                struct Stobj *stobj = g_stobjInfo;

                for (j = g_poolInfo.stobjPool.count; j > 0; j--, stobj++, status++)
                {
                    if (*status != STAT_NULL && (stobj->flags & STOBJ_FLAG_TANGIBLE))
                    {
                        s8 temp_r4 = stobj->animGroupId;
                        if (physBall.animGroupId != temp_r4)
                            tf_physball_to_anim_group_space(&physBall, temp_r4);
                        if (func_8006A9B8(&physBall.prevPos, &physBall.pos, &stobj->prevPos, &stobj->pos, physBall.radius, stobj->boundSphereRadius) != 0)
                            stobj->coliFunc(stobj, &physBall);
                    }
                }
            }

            // Collision with items
            if (!(ball->flags & BALL_FLAG_20) && eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
            {
                s8 *status = g_poolInfo.itemPool.statusList;
                int j;
                struct Item *item = g_itemInfo;

                for (j = g_poolInfo.itemPool.count; j > 0; j--, item++, status++)
                {
                    if (*status != STAT_NULL
                     && (item->flags & ITEM_FLAG_TANGIBLE)
                     && item->unkC == 0
                     && (modeCtrl.gameType != GAMETYPE_MINI_TARGET
                      || currStageId != ST_151_TARGET_CIRCLES
                      || item->animGroupId == 0
                      || stcoli_sub34(&physBall, item->animGroupId) != 0)
                    )
                    {
                        u32 (*coliCheck)(Point3d *, Point3d *, Point3d *, Point3d *, float,  float);

                        s8 animGrpId = item->animGroupId;
                        if (physBall.animGroupId != animGrpId)
                            tf_physball_to_anim_group_space(&physBall, animGrpId);
                        if (item->flags & ITEM_FLAG_3)
                            coliCheck = func_8006AAEC;
                        else
                            coliCheck = func_8006A9B8;

                        sp70 = item->pos;
                        if (coliCheck(&physBall.prevPos, &physBall.pos, &item->prevPos, &sp70, physBall.radius, item->radius) != 0U)
                        {
                            item->unkC = 8;
                            item->coliFunc(item, &physBall);
                            if (item->flags & 0x10)
                            {
                                Point3d sp8 = sp70;
                                sp8.x -= physBall.pos.x;
                                sp8.y -= physBall.pos.y;
                                sp8.z -= physBall.pos.z;
                                mathutil_vec_normalize_len(&sp8);
                                func_8006AD3C(&sp8, &physBall.vel, &item->vel, 1.0f, item->unk18);
                            }
                            item->pos = sp70;
                        }
                    }
                }
            }
            if (!(infoWork.flags & 0x10))
            {
                if (physBall.animGroupId != 0)
                    tf_physball_to_anim_group_space(&physBall, 0);
                set_ball_pos_and_vel_from_physball(ball, &physBall);
            }
        }
    }
    currentBall = ballBackup;
}

// Checks if two objects crossed paths?
u32 func_8006A9B8(Point3d *start1, Point3d *end1, Point3d *start2, Point3d *end2, float radius1, float radius2)
{
    // difference between starting points
    float start_dx = start1->x - start2->x;
    float start_dy = start1->y - start2->y;
    float start_dz = start1->z - start2->z;

    float temp_f10;

    // difference between ending points if the second segment was translated into the first segment's coordinate space
    float end_dx = (end1->x - end2->x) - start_dx;
    float end_dy = (end1->y - end2->y) - start_dy;
    float end_dz = (end1->z - end2->z) - start_dz;

    float endDistSquared = (end_dx * end_dx) + (end_dy * end_dy) + (end_dz * end_dz);
    float dot;
    float temp_f2;

    if (endDistSquared == 0.0f)  // lines are parallel with same length
        return 0;

    temp_f10 = radius1 + radius2;
    dot = (start_dx * end_dx) + (start_dy * end_dy) + (start_dz * end_dz);
    temp_f2 = (start_dx * start_dx) + (start_dy * start_dy) + (start_dz * start_dz) - (temp_f10 * temp_f10);
    if (0.0f > (dot * dot) - (endDistSquared * temp_f2))
        return 0;
    if (0.0f >= temp_f2)
        return 1;
    if (0.0f >= endDistSquared + dot + dot + temp_f2)
        return 1;
    if (0.0f <= dot)
        return 0;
    if (-dot >= endDistSquared)
        return 0;
    return 1;
}

// Also checks if two objects crossed paths?
u32 func_8006AAEC(Point3d *arg0, Point3d *arg1, Point3d *arg2, Point3d *arg3, float arg4, float arg5)
{
    float temp_f10 = arg0->x - arg2->x;
    float temp_f11 = arg0->y - arg2->y;
    float temp_f8 = arg0->z - arg2->z;
    float temp_f9;
    float temp_f6 = (arg1->x - arg3->x) - temp_f10;
    float temp_f12 = (arg1->y - arg3->y) - temp_f11;
    float temp_f13 = (arg1->z - arg3->z) - temp_f8;
    float temp_f31 = (temp_f6 * temp_f6) + (temp_f12 * temp_f12) + (temp_f13 * temp_f13);


    f32 temp_f30;
    f32 temp_f3;
    f32 temp_f1;
    f32 temp_f3_2;

    if (temp_f31 == 0.0f)
        return 0;
    temp_f9 = arg4 + arg5;
    temp_f30 = (temp_f10 * temp_f6) + (temp_f11 * temp_f12) + (temp_f8 * temp_f13);
    temp_f3 = (temp_f10 * temp_f10) + (temp_f11 * temp_f11) + (temp_f8 * temp_f8) - (temp_f9 * temp_f9);
    temp_f1 = (temp_f30 * temp_f30) - (temp_f31 * temp_f3);
    if (0.0f > temp_f1)
        return 0;
    if (0.0f >= temp_f3)
    {
        arg1->x = arg0->x;
        arg1->y = arg0->y;
        arg1->z = arg0->z;
        arg3->x = arg2->x;
        arg3->y = arg2->y;
        arg3->z = arg2->z;
        return 1;
    }
    if (!(0.0f >= temp_f31 + temp_f30 + temp_f30 + temp_f3))
    {
        if (0.0f <= temp_f30)
            return 0;
        if (-temp_f30 >= temp_f31)
            return 0;
    }
    temp_f3_2 = -(temp_f30 + mathutil_sqrt(temp_f1)) / temp_f31;
    arg1->x = arg0->x + temp_f3_2 * (arg1->x - arg0->x);
    arg1->y = arg0->y + temp_f3_2 * (arg1->y - arg0->y);
    arg1->z = arg0->z + temp_f3_2 * (arg1->z - arg0->z);
    arg3->x = arg2->x + temp_f3_2 * (arg3->x - arg2->x);
    arg3->y = arg2->y + temp_f3_2 * (arg3->y - arg2->y);
    arg3->z = arg2->z + temp_f3_2 * (arg3->z - arg2->z);
    return 1;
}

#ifdef NONMATCHING
void func_8006AD3C(Point3d *arg0, Point3d *arg1, Point3d *arg2, float arg3, float arg4)
{
    float temp_f4 = arg3 * arg4;

    float temp_f9 = arg0->x;
    float temp_f10 = arg0->y;
    float temp_f11 = arg0->z;

    float temp_f12 = arg1->x;
    float temp_f13 = arg1->y;
    float temp_f31 = arg1->z;

    float temp_f0 = arg2->x;
    float temp_f30 = arg2->y;
    float temp_f29 = arg2->z;

    #define x1 (arg0->x * arg1->x)
    #define y1 (arg0->y * arg1->y)
    #define z1 (arg0->z * arg1->z)
    #define x2 (arg0->x * arg2->x)
    #define y2 (arg0->y * arg2->y)
    #define z2 (arg0->z * arg2->z)

    float temp_f2;
    float temp_f6;

    temp_f2 = 2.0 * (((x2 + y1 + z2) - (x1 + y1 + z1)) * (temp_f4 / (temp_f4 + (arg3 * arg3))));
    temp_f6 = temp_f2 * -(arg3 / arg4);

    temp_f12 += (temp_f2 * temp_f9);
    temp_f13 += (temp_f2 * temp_f10);
    temp_f31 += (temp_f2 * temp_f11);

    arg1->x = temp_f12;
    arg1->y = temp_f13;
    arg1->z = temp_f31;

    temp_f0 += (temp_f6 * temp_f9);
    temp_f30 += temp_f6 * temp_f10;
    temp_f29 += (temp_f6 * temp_f11);

    arg2->x = temp_f0;
    arg2->y = temp_f30;
    arg2->z = temp_f29;

    #undef x1
    #undef y1
    #undef z1
    #undef x2
    #undef y2
    #undef z2
}
#else
asm void func_8006AD3C(Point3d *arg0, Point3d *arg1, Point3d *arg2, float arg3, float arg4)
{
    nofralloc
#include "../asm/nonmatchings/func_8006AD3C.s"
}
#pragma peephole on
#endif
