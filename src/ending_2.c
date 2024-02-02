#include <stddef.h>
#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "ending.h"
#include "gxutil.h"
#include "gma.h"
#include "light.h"
#include "mathutil.h"
#include "ord_tbl.h"

void lbl_800C00F4(struct Camera *, struct Ball *);
void func_800C013C(struct Camera *camera, struct Ball *ball);
void func_800C0354(struct Camera *camera, struct Ball *ball);

void (*lbl_801E3248[])(struct Camera *, struct Ball *) =
{
    func_800C013C,
    func_800C0354,
    NULL,
    NULL,
};

void func_800C008C(void)
{
    minigameRelCameraCallback = lbl_800C00F4;
    CAMERA_FOREACH(
        camera->state = 0x4A;
        camera->unk1F = -1;
    )
}

void func_800C00F0(void) {}

void lbl_800C00F4(struct Camera *camera, struct Ball *ball)
{
    if (camera->unk1F >= 0)
        lbl_801E3248[camera->unk1F](camera, ball);
}

void func_800C013C(struct Camera *camera, struct Ball *ball)
{
    struct Ape *temp_r27;
    struct Ape *temp_r31;
    struct SomeBigEndingStruct_sub *temp_r30;
    struct SomeBigEndingStruct_sub3 *temp;
    Mtx sp10;

    if (gamePauseStatus & 0xA)
        return;
    if (endingInfo.unk8 == NULL)
        return;
    temp_r30 = &endingInfo.unk8->unkAA54;
    camera_clear(camera);
    camera->flags |= 8;
    if (endingInfo.unk8 == NULL)
        return;
    switch (temp_r30->unk0)
    {
    case 2:
        temp = &endingInfo.unk8->unkA804[temp_r30->unk1C];
        temp_r31 = temp->unk0;
        mathutil_mtxA_from_quat(&temp_r31->unk60);
        mathutil_mtxA_to_mtx(sp10);
        mathutil_mtxA_from_translate(&temp_r31->unk30);
        mathutil_mtxA_scale_s(temp_r31->modelScale);
        mathutil_mtxA_translate(&temp_r31->unk3C);
        mathutil_mtxA_mult_right(sp10);
        mathutil_mtxA_mult_right(temp_r31->unk0->joints[temp_r30->unk1E].transformMtx);
        mathutil_mtxA_tf_point(&temp_r30->unk20, &camera->eye);
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    default:
        camera->eye = temp_r30->unk4;
        camera->eyeVel = temp_r30->unk10;
        break;
    }
    switch (temp_r30->unk2)
    {
    case 2:
        temp = &endingInfo.unk8->unkA804[temp_r30->unk44];
        temp_r31 = temp->unk0;
        mathutil_mtxA_from_quat(&temp_r31->unk60);
        mathutil_mtxA_to_mtx(sp10);
        mathutil_mtxA_from_translate(&temp_r31->unk30);
        mathutil_mtxA_scale_s(temp_r31->modelScale);
        mathutil_mtxA_translate(&temp_r31->unk3C);
        mathutil_mtxA_mult_right(sp10);
        mathutil_mtxA_mult_right(temp_r31->unk0->joints[temp_r30->unk46].transformMtx);
        mathutil_mtxA_tf_point(&temp_r30->unk48, &camera->lookAt);
        camera->eyeVel.x = 0.0f;
        camera->eyeVel.y = 0.0f;
        camera->eyeVel.z = 0.0f;
        break;
    default:
        camera->lookAt = temp_r30->unk2C;
        camera->lookAtVel = temp_r30->unk38;
        break;
    }
    camera->unk1F = 1;
    camera->unk26 = 0;
    func_800C0354(camera, ball);
}

void func_800C0354(struct Camera *arg0, struct Ball *ball)
{
    Vec sp7C;
    Vec sp70;
    Vec sp64;
    Point3d sp58;
    Vec sp4C;
    Vec sp40;
    Mtx sp10;
    struct Ape *temp_r29;
    struct SomeBigEndingStruct_sub *temp_r31;
    struct SomeBigEndingStruct_sub3 *temp;

    if (gamePauseStatus & 0xA)
        return;
    if (endingInfo.unk8 == NULL)
        return;

    temp_r31 = &endingInfo.unk8->unkAA54;
    sp70 = arg0->eye;
    sp64 = arg0->lookAt;
    switch (temp_r31->unk0)
    {
    case 0:
        arg0->eye = temp_r31->unk4;
        break;
    case 1:
        sp58 = temp_r31->unk4;
        sp4C = arg0->eye;
        sp40 = arg0->eyeVel;
        sp40.x += 0.5f * ((0.05f * (sp58.x - sp4C.x)) - sp40.x);
        sp40.y += 0.5f * ((0.05f * (sp58.y - sp4C.y)) - sp40.y);
        sp40.z += 0.5f * ((0.05f * (sp58.z - sp4C.z)) - sp40.z);
        arg0->eye.x += sp40.x;
        arg0->eye.y += sp40.y;
        arg0->eye.z += sp40.z;
        break;
    case 2:
        temp = &endingInfo.unk8->unkA804[temp_r31->unk1C];
        temp_r29 = temp->unk0;
        mathutil_mtxA_from_quat(&temp_r29->unk60);
        mathutil_mtxA_to_mtx(sp10);
        mathutil_mtxA_from_translate(&temp_r29->unk30);
        mathutil_mtxA_scale_s(temp_r29->modelScale);
        mathutil_mtxA_translate(&temp_r29->unk3C);
        mathutil_mtxA_mult_right(sp10);
        mathutil_mtxA_mult_right(temp_r29->unk0->joints[temp_r31->unk1E].transformMtx);
        mathutil_mtxA_tf_point(&temp_r31->unk20, &sp58);
        sp4C = arg0->eye;
        sp40 = arg0->eyeVel;
        sp40.x += 0.5f * ((0.05f * (sp58.x - sp4C.x)) - sp40.x);
        sp40.y += 0.5f * ((0.05f * (sp58.y - sp4C.y)) - sp40.y);
        sp40.z += 0.5f * ((0.05f * (sp58.z - sp4C.z)) - sp40.z);
        arg0->eye.x += sp40.x;
        arg0->eye.y += sp40.y;
        arg0->eye.z += sp40.z;
        break;
    }
    switch (temp_r31->unk2)
    {
    case 0:
        arg0->lookAt = temp_r31->unk2C;
        break;
    case 1:
        sp58 = temp_r31->unk2C;
        sp4C = arg0->lookAt;
        sp40 = arg0->lookAtVel;
        sp40.x += 0.2f * ((0.1f * (sp58.x - sp4C.x)) - sp40.x);
        sp40.y += 0.2f * ((0.1f * (sp58.y - sp4C.y)) - sp40.y);
        sp40.z += 0.2f * ((0.1f * (sp58.z - sp4C.z)) - sp40.z);
        arg0->lookAt.x += sp40.x;
        arg0->lookAt.y += sp40.y;
        arg0->lookAt.z += sp40.z;
        break;
    case 2:
        temp = &endingInfo.unk8->unkA804[temp_r31->unk44];
        temp_r29 = temp->unk0;
        mathutil_mtxA_from_quat(&temp_r29->unk60);
        mathutil_mtxA_to_mtx(sp10);
        mathutil_mtxA_from_translate(&temp_r29->unk30);
        mathutil_mtxA_scale_s(temp_r29->modelScale);
        mathutil_mtxA_translate(&temp_r29->unk3C);
        mathutil_mtxA_mult_right(sp10);
        mathutil_mtxA_mult_right(temp_r29->unk0->joints[temp_r31->unk46].transformMtx);
        mathutil_mtxA_tf_point(&temp_r31->unk48, &sp58);
        sp4C = arg0->lookAt;
        sp40 = arg0->lookAtVel;
        sp40.x += 0.2f * ((0.1f * (sp58.x - sp4C.x)) - sp40.x);
        sp40.y += 0.2f * ((0.1f * (sp58.y - sp4C.y)) - sp40.y);
        sp40.z += 0.2f * ((0.1f * (sp58.z - sp4C.z)) - sp40.z);
        arg0->lookAt.x += sp40.x;
        arg0->lookAt.y += sp40.y;
        arg0->lookAt.z += sp40.z;
        break;
    }
    arg0->eyeVel.x = arg0->eye.x - sp70.x;
    arg0->eyeVel.y = arg0->eye.y - sp70.y;
    arg0->eyeVel.z = arg0->eye.z - sp70.z;
    arg0->lookAtVel.x = arg0->lookAt.x - sp64.x;
    arg0->lookAtVel.y = arg0->lookAt.y - sp64.y;
    arg0->lookAtVel.z = arg0->lookAt.z - sp64.z;
    sp7C.x = arg0->lookAt.x - arg0->eye.x;
    sp7C.y = arg0->lookAt.y - arg0->eye.y;
    sp7C.z = arg0->lookAt.z - arg0->eye.z;
    arg0->rotY = mathutil_atan2(sp7C.x, sp7C.z) - 0x8000;
    arg0->rotX = mathutil_atan2(sp7C.y, mathutil_sqrt(mathutil_sum_of_sq_2(sp7C.x, sp7C.z)));
    arg0->rotZ = 0;
}

void func_800C09B0(void) {}

void func_800C09B4(void) {}

void effect_ending_ballfrag_init(struct Effect *arg0)
{
    arg0->timer = 0xE10;
    arg0->cameras = 0xFFFF;
}

void effect_ending_ballfrag_main(struct Effect *arg0)
{
    Vec spC;
    float temp_f2;

    if (!(endingInfo.unk4 & 8))
    {
        mathutil_mtxA_from_rotate_y(arg0->rotY);
        mathutil_mtxA_rotate_x(arg0->rotX);
        mathutil_mtxA_rotate_z(arg0->rotZ);
        mathutil_mtxA_rigid_inv_tf_vec(&arg0->vel, &spC);
        temp_f2 = mathutil_vec_dot_prod(&arg0->unk88, &spC);
        if (temp_f2 < 0.0f)
        {
            temp_f2 *= -0.3f;
            spC.x += temp_f2 * arg0->unk88.x;
            spC.y += temp_f2 * arg0->unk88.y;
            spC.z += temp_f2 * arg0->unk88.z;
        }
        else
        {
            temp_f2 *= -0.05f;
            spC.x += temp_f2 * arg0->unk88.x;
            spC.y += temp_f2 * arg0->unk88.y;
            spC.z += temp_f2 * arg0->unk88.z;
        }
        spC.x *= 0.99f;
        spC.y *= 0.99f;
        spC.z *= 0.99f;
        mathutil_mtxA_tf_vec(&spC, &arg0->vel);
        arg0->vel.y += -0.004899996f;
        arg0->pos.x += arg0->vel.x;
        arg0->pos.y += arg0->vel.y;
        arg0->pos.z += arg0->vel.z;
        arg0->rotX = (0.96f * arg0->rotX) + (6144.0f * arg0->vel.y);
        arg0->rotY = (0.96f * arg0->rotY) + (6144.0f * arg0->vel.z);
        arg0->rotZ = (0.96f * arg0->rotZ) + (6144.0f * arg0->vel.x);
    }
}

struct MyNode
{
    struct OrdTblNode node;
    u32 unk8;
    struct Effect *effect;
};

void lbl_800C0C94(struct MyNode *);

void effect_ending_ballfrag_draw(struct Effect *effect)
{
    struct MyNode *temp_r3;
    struct OrdTblNode *temp_r30;

    mathutil_mtxA_from_mtxB();
    temp_r30 = ord_tbl_get_entry_for_pos(&effect->pos);
    temp_r3 = ord_tbl_alloc_node(0x10U);
    temp_r3->node.drawFunc = (OrdTblDrawFunc)lbl_800C0C94;
    temp_r3->unk8 = peek_light_group();
    temp_r3->effect = effect;
    ord_tbl_insert_node(temp_r30, &temp_r3->node);
}

void lbl_800C0C94(struct MyNode *node)
{
    struct Effect *effect;
    struct GMAModelEntry *modelEntries;

    load_light_group_cached(node->unk8);
    effect = node->effect;
    mathutil_mtxA_from_mtxB_translate(&effect->pos);
    mathutil_mtxA_rotate_y(effect->rotY);
    mathutil_mtxA_rotate_x(effect->rotX);
    mathutil_mtxA_rotate_z(effect->rotZ);
    u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
    modelEntries = commonGma->modelEntries;
    if (effect->unkA0 < 0)
    {
        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_culled_sort_none(modelEntries[clearHemisphereInsideParts[0]].model);
        avdisp_set_z_mode(1, 3, 1);
        avdisp_draw_model_culled_sort_none(modelEntries[clearHemisphereOutsideParts[0]].model);
    }
    else
    {
        s16 *r30 = coloredBallPartModelIDs[effect->unkA0];

        avdisp_set_z_mode(1, 3, 0);
        avdisp_draw_model_unculled_sort_none(modelEntries[r30[0]].model);
        avdisp_draw_model_unculled_sort_none(modelEntries[r30[6]].model);
        avdisp_set_z_mode(1, 3, 1);
        avdisp_draw_model_unculled_sort_none(modelEntries[r30[3]].model);
    }
}

void effect_ending_ballfrag_destroy(struct Effect *effect) {}

struct UnkStrct
{
    u8 filler0[0x38];
};

void func_800C0DC0(s32 *arg0, s32 *arg1)
{
    struct UnkStrct sp10;
    int r3;

    func_800C0F94(&sp10);
    r3 = func_800C122C(&sp10, 0);
    func_800C1418(&sp10, arg0, arg1, r3);
}
