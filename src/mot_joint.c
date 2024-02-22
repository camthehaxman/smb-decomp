#include <float.h>
#include <math.h>
#include <stddef.h>

#include <dolphin.h>

#include "global.h"
#include "mathutil.h"

struct InterpolateStuff
{
    float time;
    float val1;
    float val2;
};

static void u_interp_pos_motion(struct AnimJoint *, struct AnimJoint *, u32, float);
static void u_interp_rot_motion(struct AnimJoint *, struct AnimJoint *, const struct JointRotationSomething *, u32, float);
static float interpolate_channel_keyframes(struct MotionChannel *, float);
static float u_crazy_interpolation_stuff(struct InterpolateStuff *a, struct InterpolateStuff *b, float c);
static void read_channel_keyframe_values(struct MotionChannel *, float *, float *, float *);
static void seek_channel_next_keyframe(struct MotionChannel *);
static void seek_channel_prev_keyframe(struct MotionChannel *);

void u_interpolate_joint_motion(struct AnimJoint *joints, const struct JointRotationSomething *rotInfo, const struct JointPositionSomething *posInfo, float t, u32 xflip)
{
    u32 flags;
    struct AnimJoint *joint = joints;

    if (rotInfo == NULL || posInfo == NULL)
        xflip = 0;

    flags = joint->flags;
    while (flags != 0)
    {
        flags &= ~(3 << 14);
        joint->flags = flags;
        if (flags & (1 << 2))
        {
            if (xflip)
                u_interp_pos_motion(joint, &joints[posInfo->jointIdx], xflip, t);
            else
                u_interp_pos_motion(joint, joint, xflip, t);
            posInfo++;
        }
        if (flags & JOINT_FLAG_HAS_ROTATION_MTX)
        {
            if (xflip)
                u_interp_rot_motion(joint, &joints[rotInfo->jointIdx], rotInfo, xflip, t);
            else
                u_interp_rot_motion(joint, joint, rotInfo, xflip, t);
            rotInfo++;
        }
        joint++;
        flags = joint->flags;
    }
}

// updates joint2's position using joint1's channels
static void u_interp_pos_motion(struct AnimJoint *joint1, struct AnimJoint *joint2, u32 xflip, float t)
{
    struct MotionChannel *chan;
    int unused;

    // x channel
    chan = &joint1->channels[0];
    if (chan->keyframeCount != 0)
    {
        joint2->u_motionPos.x = interpolate_channel_keyframes(chan, t);
        if (xflip)
            joint2->u_motionPos.x = -joint2->u_motionPos.x;
    }
    else
        joint2->u_motionPos.x = 0.0f;

    // y channel
    chan++;
    if (chan->keyframeCount != 0)
        joint2->u_motionPos.y = interpolate_channel_keyframes(chan, t);
    else
        joint2->u_motionPos.y = 0.0f;

    // z channel
    chan++;
    if (chan->keyframeCount != 0)
        joint2->u_motionPos.z = interpolate_channel_keyframes(chan, t);
    else
        joint2->u_motionPos.z = 0.0f;

}

// updates joint2's rotation using joint1's channels
static void u_interp_rot_motion(struct AnimJoint *joint1, struct AnimJoint *joint2, const struct JointRotationSomething *c, u32 d, float t)
{
    float radToS16;
    struct MotionChannel *chan;

    mathutil_mtxA_from_identity();
    radToS16 = 10430.3779296875f;

    // z rotation channel
    chan = &joint1->channels[5];
    if (chan->keyframeCount != 0)
    {
        float val = interpolate_channel_keyframes(chan, t);
        if (d != 0)
            val = c->unk18 + val * c->unkC;
        mathutil_mtxA_rotate_z((s16)(radToS16 * val));
    }

    // y rotation channel
    chan--;
    if (chan->keyframeCount != 0)
    {
        float val = interpolate_channel_keyframes(chan, t);
        if (d != 0)
            val = c->unk14 + val * c->unk8;
        mathutil_mtxA_rotate_y((s16)(radToS16 * val));
    }

    // x rotation channel
    chan--;
    if (chan->keyframeCount != 0)
    {
        float val = interpolate_channel_keyframes(chan, t);
        if (d != 0)
            val = c->unk10 + val * c->unk4;
        mathutil_mtxA_rotate_x((s16)(radToS16 * val));
    }

    mathutil_mtxA_sq_to_mtx(joint2->u_motRotation);
}

// Gets the value of the channel at time t
static float interpolate_channel_keyframes(struct MotionChannel *chan, float t)
{
    float ret;
    float dummy1;
    float dummy2;
    struct InterpolateStuff sp2C;
    struct InterpolateStuff sp20;
    struct MotionChannel prev;
    u8 keyframeCount = chan->keyframeCount;
    u8 keyframe = chan->currKeyframe;
    u8 type = 0;

    // Find which keyframe we are on
    while (keyframe < keyframeCount)
    {
        float frameT = *chan->times;

        if (fabs(frameT - t) < FLT_EPSILON)
        {
            type = 1;
            break;
        }
        else if (frameT > t)
        {
            if (keyframe != 0)
                type = 3;
            else
                type = 2;
            break;
        }
        keyframe++;
        seek_channel_next_keyframe(chan);
    }

    switch (type)
    {
    default:
        if (chan->currKeyframe < keyframeCount)
            seek_channel_prev_keyframe(chan);
        // fall through
    case 1:
    case 2:
        read_channel_keyframe_values(chan, &dummy1, &dummy2, &ret);
        break;
    case 3:
        sp20.time = *chan->times;
        read_channel_keyframe_values(chan, &sp20.val2, &dummy2, &sp20.val1);

        prev.times = chan->times;
        prev.valueCounts = chan->valueCounts;
        prev.values = chan->values;
        seek_channel_prev_keyframe(&prev);

        sp2C.time = *prev.times;
        read_channel_keyframe_values(&prev, &dummy1, &sp2C.val2, &sp2C.val1);

        ret = u_crazy_interpolation_stuff(&sp2C, &sp20, t);
        break;
    }
    chan->currKeyframe = keyframe;
    return ret;
}

static float u_crazy_interpolation_stuff(struct InterpolateStuff *a, struct InterpolateStuff *b, float c)
{
    float unkx, unkx2, dt, z1, f5, f0;

    z1 = a->val2;
    dt = b->time - a->time;
    unkx = (c - a->time) / dt;
    unkx2 = unkx * unkx;
    dt *= (1.0 / 30.0);
    f0 = unkx2 - unkx;
    f5 = unkx * unkx2 - unkx2;
    return a->val1
         + (f5 + f5 - unkx2) * (a->val1 - b->val1)
         + dt * (f5 * (z1 + b->val2) - z1 * f0);
}

// Reads up to 3 values from the motion channel
static void read_channel_keyframe_values(struct MotionChannel *chan, float *val1, float *val2, float *val3)
{
    switch (*chan->valueCounts)
    {
    default:
    case 0:
        *val1 = *val2 = *val3 = 0.0f;
        break;
    case 1:
        *val1 = *val2 = 0.0f;
        *val3 = chan->values[0];
        break;
    case 2:
        *val1 = *val2 = chan->values[0];
        *val3 = chan->values[1];
        break;
    case 3:
        *val1 = chan->values[0];
        *val2 = chan->values[1];
        *val3 = chan->values[2];
        break;
    }
}

// Advances channel pointers to the next keyframe
static void seek_channel_next_keyframe(struct MotionChannel *chan)
{
    chan->times++;
    chan->values += *chan->valueCounts;
    chan->valueCounts++;
}

// Moves channel pointers back to the previous keyframe
static void seek_channel_prev_keyframe(struct MotionChannel *chan)
{
    chan->times--;
    chan->valueCounts--;
    chan->values -= *chan->valueCounts;
}

void u_mot_joint_start_anim(struct ApeAnimationThing *a)
{
    a->u_keyframeCount = u_get_motdat_keyframe_count(a->u_animId);
    u_load_new_anim_into_joints(a->joints, a->u_animId);
}

void mot_joint_800355FC(struct ApeAnimationThing *a)
{
    struct AnimJoint *joints;

    mathutil_mtxA_from_identity();
    mathutil_mtxA_to_mtx(a->unk54);
    u_update_skel_anim(a);
    joints = a->joints;
    u_joint_tree_calc_some_matrix(joints, joints);
}
