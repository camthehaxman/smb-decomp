#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "camera.h"
#include "ending.h"
#include "mathutil.h"

static void lbl_800C00F4(struct Camera *, struct Ball *);
static void func_800C013C(struct Camera *camera, struct Ball *ball);
static void func_800C0354(struct Camera *camera, struct Ball *ball);

static void (*lbl_801E3248[])(struct Camera *, struct Ball *) =
{
    func_800C013C,
    func_800C0354,
    NULL,
    NULL,
};

void ending_camera_init(void)
{
    minigameRelCameraCallback = lbl_800C00F4;
    CAMERA_FOREACH(
        camera->state = 0x4A;
        camera->unk1F = -1;
    )
}

void ending_camera_emptyfunc1(void) {}

static void lbl_800C00F4(struct Camera *camera, struct Ball *ball)
{
    if (camera->unk1F >= 0)
        lbl_801E3248[camera->unk1F](camera, ball);
}

static void func_800C013C(struct Camera *camera, struct Ball *ball)
{
    struct Ape *temp_r27;
    struct Ape *temp_r31;
    struct SomeBigEndingStruct_sub *temp_r30;
    struct EndingSceneCharacter *chara;
    Mtx sp10;

    if (gamePauseStatus & 0xA)
        return;
    if (endingInfo.work == NULL)
        return;
    temp_r30 = &endingInfo.work->unkAA54;
    camera_clear(camera);
    camera->flags |= 8;
    if (endingInfo.work == NULL)
        return;
    switch (temp_r30->unk0)
    {
    case 2:
        chara = &endingInfo.work->characters[temp_r30->unk1C];
        temp_r31 = chara->ape;
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
        chara = &endingInfo.work->characters[temp_r30->unk44];
        temp_r31 = chara->ape;
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

static void func_800C0354(struct Camera *arg0, struct Ball *ball)
{
    Vec sp7C;
    Vec sp70;
    Vec sp64;
    Point3d sp58;
    Vec sp4C;
    Vec sp40;
    Mtx sp10;
    struct Ape *ape;
    struct SomeBigEndingStruct_sub *temp_r31;
    struct EndingSceneCharacter *chara;

    if (gamePauseStatus & 0xA)
        return;
    if (endingInfo.work == NULL)
        return;

    temp_r31 = &endingInfo.work->unkAA54;
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
        chara = &endingInfo.work->characters[temp_r31->unk1C];
        ape = chara->ape;
        mathutil_mtxA_from_quat(&ape->unk60);
        mathutil_mtxA_to_mtx(sp10);
        mathutil_mtxA_from_translate(&ape->unk30);
        mathutil_mtxA_scale_s(ape->modelScale);
        mathutil_mtxA_translate(&ape->unk3C);
        mathutil_mtxA_mult_right(sp10);
        mathutil_mtxA_mult_right(ape->unk0->joints[temp_r31->unk1E].transformMtx);
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
        chara = &endingInfo.work->characters[temp_r31->unk44];
        ape = chara->ape;
        mathutil_mtxA_from_quat(&ape->unk60);
        mathutil_mtxA_to_mtx(sp10);
        mathutil_mtxA_from_translate(&ape->unk30);
        mathutil_mtxA_scale_s(ape->modelScale);
        mathutil_mtxA_translate(&ape->unk3C);
        mathutil_mtxA_mult_right(sp10);
        mathutil_mtxA_mult_right(ape->unk0->joints[temp_r31->unk46].transformMtx);
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

void ending_camera_emptyfunc2(void) {}

void ending_camera_emptyfunc3(void) {}
