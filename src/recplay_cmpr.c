/**
 * recplay.c - Manages recording and playback of a compressed replay
 */
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "course.h"
#include "info.h"
#include "mathutil.h"
#include "mode.h"
#include "recplay.h"
#include "recplay_cmpr.h"
#include "world.h"

#define MAX_REPLAY_FRAMES 3840

static struct Struct80250B70
{
    s16 isRecording;
    Vec prevBallPos;
} recordingState;

// The compressed replay data that is read from or written to
static struct
{
    struct ReplayHeader header;
    s16 unk18;
    s16 frameCount;
    s16 unk1C;
    s16 goalEntered;
    s16 unk20;
    s16 unk22;
    Vec unk24;
    u32 unk30;
    u32 currFrameNum;
    Vec unk38;
    u8 frameData[24][MAX_REPLAY_FRAMES];
} s_replayData;

struct
{
    s16 unk0;
    u16 unk2;
    u8 filler4[4];
} lbl_802F1F88 ATTRIBUTE_ALIGN(8);

int lbl_802F1F80;

static void recplay_cmpr_get_ball_frame_int(int arg0, struct ReplayBallFrame *arg1);
static void recplay_cmpr_get_world_frame_int(int arg0, struct ReplayWorldFrame *arg1);

void func_8004AFC0(void)
{
    recordingState.isRecording = 0;
}

void cmp_recplay_init_first(void) {}

void dummy_8004AFD4(void) {}

// Records the current ball and world state into a compressed replay frame
void recplay_cmpr_record_frame(void)
{
    struct Ball *ball = &ballInfo[modeCtrl.currPlayer];
    struct World *world = &worldInfo[modeCtrl.currPlayer];
    s32 x, y, z;
    int var_r24_2;
    u32 temp;
    u32 temp_2;
    u32 frameNum;
    int var;

    switch (recordingState.isRecording)
    {
    case 1:
        break;
    default:
        return;
    }

    frameNum = s_replayData.currFrameNum;
    if (frameNum <= MAX_REPLAY_FRAMES)
    {
        // Compute difference from previous position of the ball
        // The ball is (hopefully) always going to move less than one unit in a single frame, so
        // this fractional value is scaled up and converted to a value suitable for storing in a
        // 16-bit signed integer
        x = (SHRT_MAX/2.0f) * (ball->pos.x - recordingState.prevBallPos.x);
        y = (SHRT_MAX/2.0f) * (ball->pos.y - recordingState.prevBallPos.y);
        z = (SHRT_MAX/2.0f) * (ball->pos.z - recordingState.prevBallPos.z);
        // Clamp the values to fit within the allowable range
        if (x < -SHRT_MAX)
            x = -SHRT_MAX;
        else if (x > SHRT_MAX)
            x = SHRT_MAX;
        if (y < -SHRT_MAX)
            y = -SHRT_MAX;
        else if (y > SHRT_MAX)
            y = SHRT_MAX;
        if (z < -SHRT_MAX)
            z = -SHRT_MAX;
        else if (z > SHRT_MAX)
            z = SHRT_MAX;

        // Store ball frame info

        s_replayData.frameData[0][frameNum] = x & 0xFF;
        x >>= 8;
        s_replayData.frameData[1][frameNum] = x & 0xFF;

        s_replayData.frameData[2][frameNum] = y & 0xFF;
        y >>= 8;
        s_replayData.frameData[3][frameNum] = y & 0xFF;

        s_replayData.frameData[4][frameNum] = z & 0xFF;
        z >>= 8;
        s_replayData.frameData[5][frameNum] = z & 0xFF;

        temp = (u16)ball->rotX;
        s_replayData.frameData[6][frameNum] = temp & 0xFF;
        temp >>= 8;
        s_replayData.frameData[7][frameNum] = temp & 0xFF;

        temp = (u16)ball->rotY;
        s_replayData.frameData[8][frameNum] = temp & 0xFF;
        temp >>= 8;
        s_replayData.frameData[9][frameNum] = temp & 0xFF;

        temp = (u16)ball->rotZ;
        s_replayData.frameData[10][frameNum] = temp & 0xFF;
        temp >>= 8;
        s_replayData.frameData[11][frameNum] = temp & 0xFF;

        s_replayData.frameData[12][frameNum] = 127.0f * ball->unk114.x;
        s_replayData.frameData[13][frameNum] = 127.0f * ball->unk114.y;
        s_replayData.frameData[14][frameNum] = 127.0f * ball->unk114.z;

        var_r24_2 = ball->unk130 * 127.0f;
        !var_r24_2;
        if (var_r24_2 < -127)
            var_r24_2 = -127;
        else if (var_r24_2 > 127)
            var_r24_2 = 127;
        s_replayData.frameData[15][frameNum] = (s8)var_r24_2;

        temp_2 = ball->flags;
        s_replayData.frameData[16][frameNum] = temp_2;
        temp_2 >>= 8;
        s_replayData.frameData[17][frameNum] = temp_2;
        temp_2 >>= 8;
        s_replayData.frameData[18][frameNum] = temp_2;
        temp_2 >>= 8;
        s_replayData.frameData[19][frameNum] = temp_2;

        temp_2 = (u16)world->xrot;
        s_replayData.frameData[20][frameNum] = temp_2;
        temp_2 >>= 8;
        s_replayData.frameData[21][frameNum] = temp_2;

        temp_2 = (u16)world->zrot;
        s_replayData.frameData[22][frameNum] = temp_2;
        temp_2 >>= 8;
        s_replayData.frameData[23][frameNum] = temp_2;

        // Update previous ball position for next frame
        var = (s_replayData.frameData[1][frameNum] << 8) | s_replayData.frameData[0][frameNum];
        recordingState.prevBallPos.x += (2.0f/SHRT_MAX) * (s16)var;
        var = (s_replayData.frameData[3][frameNum] << 8) | s_replayData.frameData[2][frameNum];
        recordingState.prevBallPos.y += (2.0f/SHRT_MAX) * (s16)var;
        var = (s_replayData.frameData[5][frameNum] << 8) | s_replayData.frameData[4][frameNum];
        recordingState.prevBallPos.z += (2.0f/SHRT_MAX) * (s16)var;

        s_replayData.frameCount++;
        s_replayData.unk22 = stageInfo.unk0;
        s_replayData.currFrameNum = frameNum + 1;
    }
}

void recplay_cmpr_dest(void)
{
    recplay_cmpr_stop_recording();
}

void recplay_cmpr_start_recording(void)
{
    struct Ball *ball;
    int practice;

    practice = modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE;
    ball = &ballInfo[modeCtrl.currPlayer];

    recordingState.prevBallPos = ball->pos;
    memset(&s_replayData, 0, sizeof(s_replayData));
    s_replayData.unk38 = ball->pos;
    s_replayData.unk30 = 0;
    s_replayData.currFrameNum = 0;
    s_replayData.unk18 = infoWork.timerCurr;
    s_replayData.frameCount = 0;
    s_replayData.header.flags = 0;
    s_replayData.header.stageId = currStageId;
    if (!practice)
    {
        s_replayData.header.difficulty = modeCtrl.difficulty;
        s_replayData.header.floorNum = infoWork.currFloor;
    }
    else
    {
        s_replayData.header.difficulty = lbl_8027CE24[0].unk2;
        s_replayData.header.floorNum = lbl_8027CE24[0].unk0;
    }
    s_replayData.header.character = playerCharacterSelection[ball->playerId];
    s_replayData.header.playerName[0] = 0;
    s_replayData.unk22 = stageInfo.unk0;
    if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
        s_replayData.header.flags |= REPLAY_FLAG_BONUS_STAGE;
    if (!practice)
    {
        if (modeCtrl.courseFlags & COURSE_FLAG_EXTRA)
            s_replayData.header.flags |= REPLAY_FLAG_EXTRA;
        if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
            s_replayData.header.flags |= REPLAY_FLAG_MASTER;
    }
    else
    {
        if (lbl_8027CE24[0].unk4 & COURSE_FLAG_EXTRA)
            s_replayData.header.flags |= REPLAY_FLAG_EXTRA;
        if (lbl_8027CE24[0].unk4 & COURSE_FLAG_MASTER)
            s_replayData.header.flags |= REPLAY_FLAG_MASTER;
    }
    s_replayData.header.unk10 = -1;
    recordingState.isRecording = 1;
}

void recplay_cmpr_stop_recording(void)
{
    recordingState.isRecording = 0;
}

void recplay_cmpr_record_goal(void)
{
    if (recordingState.isRecording == 1)
    {
        s_replayData.header.flags |= REPLAY_FLAG_GOAL;
        s_replayData.unk1C = infoWork.unk1C;
        s_replayData.unk24 = infoWork.unk10;
        s_replayData.goalEntered = infoWork.goalEntered;
        s_replayData.unk20 = infoWork.unkE;
    }
}

void recplay_cmpr_record_fallout(void)
{
    if (recordingState.isRecording == 1)
    {
        s_replayData.header.flags |= REPLAY_FLAG_FALLOUT;
        s_replayData.header.unk10 = ballInfo[modeCtrl.currPlayer].score - lbl_802F1F80;
        s_replayData.unk1C = infoWork.timerCurr;
    }
}

void recplay_cmpr_record_timeover(void)
{
    if (recordingState.isRecording == 1)
    {
        s_replayData.header.flags |= REPLAY_FLAG_TIME_OVER;
        s_replayData.header.unk10 = ballInfo[modeCtrl.currPlayer].score - lbl_802F1F80;
    }
}

void recplay_cmpr_record_score(void)
{
    s_replayData.header.unk10 = ballInfo[modeCtrl.currPlayer].score - lbl_802F1F80;
}

void recplay_cmpr_record_bonus_clear(void)
{
    if (recordingState.isRecording == 1)
    {
        s_replayData.header.flags |= 0x80;
        s_replayData.unk1C = infoWork.timerCurr;
    }
}

void recplay_cmpr_record_player_name(char *name)
{
    strncpy(s_replayData.header.playerName, name, sizeof(s_replayData.header.playerName));
    s_replayData.header.playerName[3] = 0;
}

void func_8004B70C(void)
{
    infoWork.timerMax = s_replayData.unk18;
    infoWork.timerCurr = s_replayData.unk18;
    infoWork.flags &= ~0x667;
    infoWork.currFloor = s_replayData.header.floorNum;
    if (s_replayData.header.flags & REPLAY_FLAG_GOAL)
    {
        infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_05;
        infoWork.timerCurr = s_replayData.unk1C;
        infoWork.unk1C = s_replayData.unk1C;
        infoWork.unk10 = s_replayData.unk24;
        infoWork.goalEntered = s_replayData.goalEntered;
        infoWork.unkE = s_replayData.unk20;
    }
    if (s_replayData.header.flags & REPLAY_FLAG_BONUS_STAGE)
        infoWork.flags |= INFO_FLAG_BONUS_STAGE;
    playerCharacterSelection[g_recplayInfo.u_playerId] = s_replayData.header.character;
    modeCtrl.difficulty = s_replayData.header.difficulty;
    modeCtrl.courseFlags &= ~(COURSE_FLAG_EXTRA|COURSE_FLAG_MASTER);
    if (s_replayData.header.flags & REPLAY_FLAG_EXTRA)
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
    if (s_replayData.header.flags & REPLAY_FLAG_MASTER)
        modeCtrl.courseFlags |= COURSE_FLAG_MASTER;
}

float recplay_cmpr_get_time(void)
{
    return s_replayData.frameCount;
}

void recplay_cmpr_get_ball_frame(float time, struct ReplayBallFrame *out)
{
    struct ReplayBallFrame prev;
    struct ReplayBallFrame next;
    struct ReplayBallFrame curr;
    Vec sp18;
    float nextFactor;
    float prevFactor;
    int timeInt;
    struct Ball *ball = currentBall;
    u8 unused[8];

    // If there are no frames, just copy the ball's current position
    if (s_replayData.frameCount == 0)
    {
        out->pos = ball->pos;
        out->rotX = ball->rotX;
        out->rotY = ball->rotY;
        out->rotZ = ball->rotZ;
        return;
    }

    // clamp time within bounds
    if (time < 0.0f)
        time = 0.0f;
    else if (s_replayData.frameCount <= time)
        time = s_replayData.frameCount;

    // split time value into integer and fractional parts
    timeInt = time;
    nextFactor = time - timeInt;
    prevFactor = 1.0 - nextFactor;

    recplay_cmpr_get_ball_frame_int(timeInt, &prev);
    recplay_cmpr_get_ball_frame_int(timeInt + 1, &next);

    // interpolate this frame between the previous and next frames
    curr.pos.x = (prev.pos.x * prevFactor) + (next.pos.x * nextFactor);
    curr.pos.y = (prev.pos.y * prevFactor) + (next.pos.y * nextFactor);
    curr.pos.z = (prev.pos.z * prevFactor) + (next.pos.z * nextFactor);
    curr.rotX = (prev.rotX * prevFactor) + (next.rotX * nextFactor);
    curr.rotY = (prev.rotY * prevFactor) + (next.rotY * nextFactor);
    curr.rotZ = (prev.rotZ * prevFactor) + (next.rotZ * nextFactor);
    sp18.x = (prev.unk12 * prevFactor) + (next.unk12 * nextFactor);
    sp18.y = (prev.unk14 * prevFactor) + (next.unk14 * nextFactor);
    sp18.z = (prev.unk16 * prevFactor) + (next.unk16 * nextFactor);
    mathutil_vec_set_len(&sp18, &sp18, 32767.0f);
    curr.unk12 = sp18.x;
    curr.unk14 = sp18.y;
    curr.unk16 = sp18.z;
    curr.unk18 = prev.unk18;
    curr.unk1C = (prev.unk1C * prevFactor) + (next.unk1C * nextFactor);
    *out = curr;
}

static void recplay_cmpr_get_ball_frame_int(int time, struct ReplayBallFrame *out)
{
    struct ReplayBallFrame ballFrame;
    u8 *var_r9;
    u8 *var_r10;
    u8 *var_r11;
    u8 *var_r12;
    u8 *var_r31;
    u8 *var_r30;
    int i;

    time = s_replayData.frameCount - 1 - time;
    if (time < 0)
        time = 0;
    ballFrame.pos = s_replayData.unk38;
    var_r9 = s_replayData.frameData[0];
    var_r10 = s_replayData.frameData[2];
    var_r11 = s_replayData.frameData[4];
    var_r12 = s_replayData.frameData[1];
    var_r31 = s_replayData.frameData[3];
    var_r30 = s_replayData.frameData[5];
    for (i = time + 1; i > 0 ; i--)
    {
        ballFrame.pos.x += 0.00006103702f * (s16)((*var_r12 << 8) | *var_r9);
        var_r12++;
        var_r9++;

        ballFrame.pos.y += 0.00006103702f * (s16)((*var_r31 << 8) | *var_r10);
        var_r31++;
        var_r10++;

        ballFrame.pos.z += 0.00006103702f * (s16)((*var_r30 << 8) | *var_r11);
        var_r30++;
        var_r11++;
    }

    ballFrame.rotX = s_replayData.frameData[6][time] | (s_replayData.frameData[7][time] << 8);
    ballFrame.rotY = s_replayData.frameData[8][time] | (s_replayData.frameData[9][time] << 8);
    ballFrame.rotZ = s_replayData.frameData[10][time] | (s_replayData.frameData[11][time] << 8);
    ballFrame.unk12 = (s8)s_replayData.frameData[12][time] << 8;
    ballFrame.unk14 = (s8)s_replayData.frameData[13][time] << 8;
    ballFrame.unk16 = (s8)s_replayData.frameData[14][time] << 8;
    ballFrame.unk18 = (s_replayData.frameData[19][time] << 24)
               | (s_replayData.frameData[18][time] << 16)
               | (s_replayData.frameData[17][time] << 8)
               | s_replayData.frameData[16][time];
    ballFrame.unk1C = 0.007874016f * (f32) (s8) s_replayData.frameData[15][time];

    *out = ballFrame;
}

void recplay_cmpr_get_world_frame(float arg8, struct ReplayWorldFrame *out)
{
    struct ReplayWorldFrame prev;
    struct ReplayWorldFrame next;
    struct ReplayWorldFrame curr;
    float temp_f2;
    float temp_f31;
    float temp_f30;
    s32 temp_r29;
    struct World *world;
    u8 unused[4];

    world = currentWorld;
    if (s_replayData.frameCount == 0)
    {
        out->rotX = world->xrot;
        out->rotZ = world->zrot;
        return;
    }
    if (arg8 < 0.0f)
        arg8 = 0.0f;
    else if (s_replayData.frameCount <= arg8)
        arg8 = s_replayData.frameCount;

    temp_r29 = arg8;
    temp_f2 = temp_r29;
    temp_f31 = arg8 - temp_f2;
    temp_f30 = 1.0 - temp_f31;
    recplay_cmpr_get_world_frame_int(temp_r29, &prev);
    recplay_cmpr_get_world_frame_int(temp_r29 + 1, &next);
    curr.rotX = (prev.rotX * temp_f30) + (next.rotX * temp_f31);
    curr.rotZ = (prev.rotZ * temp_f30) + (next.rotZ * temp_f31);
    *out = curr;
}

static void recplay_cmpr_get_world_frame_int(int time, struct ReplayWorldFrame *out)
{
    struct ReplayWorldFrame worldFrame;

    time = s_replayData.frameCount - 1 - time;
    if (time < 0)
        time = 0;
    worldFrame.rotX = s_replayData.frameData[20][time] | (s_replayData.frameData[21][time] << 8);
    worldFrame.rotZ = s_replayData.frameData[22][time] | (s_replayData.frameData[23][time] << 8);
    *out = worldFrame;
}

float recplay_cmpr_get_info_timer(float arg0)
{
    float var_f1;
    s16 temp_r0;

    var_f1 = arg0 + (s_replayData.unk18 - s_replayData.frameCount);
    if (s_replayData.header.flags & REPLAY_FLAG_GOAL)
    {
        if (var_f1 < s_replayData.unk1C)
            var_f1 = s_replayData.unk1C;
    }
    return var_f1;
}

float func_8004C254(float arg0)
{
    return s_replayData.unk22 - arg0;
}

void recplay_cmpr_get_header(struct ReplayHeader *header)
{
    *header = s_replayData.header;
}

#pragma force_active on
int func_8004C2C8(void)
{
    return s_replayData.unk1C;
}
#pragma force_active reset

struct RLEHeader
{
    u16 unk0;
    u16 flags;
    u32 size;
};

#define NO_COMPRESSION 1

static u32 compress_rle(const u8 *src, u8 *dest, u32 srcSize)
{
    u32 r5;
    u8 r6;
    u8 *destp;
    const u8 *srcp;
    int r9;
    int tooLarge;  // true if the compressed size is larger than the uncompressed size
    int i;
    u32 destPos;

    // skip over header
    destPos = sizeof(struct RLEHeader);
    destp = dest + sizeof(struct RLEHeader);

    r6 = src[0];
    if (dest != NULL)
        destp[1] = r6;
    r5 = 1;
    if (r6 == src[1])
        r9 = 1;
    else
        r9 = 0;
    tooLarge = FALSE;
    srcp = src + 1;
    for (i = srcSize - 1; i > 0; i--, srcp++)
    {
        if (r9)
        {
            if (*srcp == r6 && r5 < 0x7F)
            {
                r5++;
                continue;
            }
            if (dest != NULL)
                *destp = r5 | 0x80;
            destPos += 2;
            destp += 2;
            if (destPos >= srcSize)
            {
                tooLarge = TRUE;
                break;
            }
            r6 = *srcp;
            if (dest != 0)
                destp[1] = r6;
            r5 = 1;
            if (r6 != srcp[1])
                r9 = 0;
        }
        else if (*srcp == r6 || r5 == 0x7F)
        {
            if (dest != NULL)
                *destp = r5;
            destPos = r5 + destPos;
            destPos++;
#ifdef NONMATCHING
            destp += r5;
#else
            destp = (u8 *)r5 + (uintptr_t)destp;
#endif
            destp++;
            if (destPos >= srcSize)
            {
                tooLarge = TRUE;
                break;
            }
            r6 = *srcp;
            if (dest != NULL)
                destp[1] = r6;
            r5 = 1;
            if (r6 == srcp[1])
                r9 = 1;
        }
        else
        {
            r5++;
            if (destPos + (r5 + 1) >= srcSize)
            {
                tooLarge = TRUE;
                break;
            }
            r6 = *srcp;
            if (dest != NULL)
                destp[r5] = *srcp;
        }
    }

    if (!tooLarge)
    {
        if (r9 != 0)
        {
            if (dest != NULL)
                *destp = r5 | 0x80;
            destPos += 2;
            if (destPos >= srcSize)
                tooLarge = 1;
        }
        else
        {
            if (dest != NULL)
                *destp = r5;
            destPos = r5 + destPos;
            destPos++;
            if (destPos >= srcSize)
                tooLarge = 1;
        }
    }

    if (!tooLarge)
    {
        struct RLEHeader header;

        header.unk0 = 0;
        header.flags = 0;
        header.size = srcSize;
        if (dest != NULL)
            memcpy(dest, &header, sizeof(header));
    }
    else
    {
        struct RLEHeader header;

        header.unk0 = 0;
        header.flags = NO_COMPRESSION;
        destPos = srcSize + sizeof(struct RLEHeader);
        header.size = srcSize;
        if (dest != NULL)
        {
            memcpy(dest, &header, sizeof(header));
            memcpy(dest + sizeof(header), src, srcSize);
        }
    }
    return destPos;
}

static int decompress_rle(const u8 *src, u8 *dest, u32 destSize)
{
    struct RLEHeader header;
    u32 var_r3;
    int var_r5;
    u8 *destp;
    const u8 *srcp;
    u8 temp_r0;

    memcpy(&header, src, sizeof(header));
    if (header.unk0 != 0)
        return FALSE;
    if (header.size > destSize)
        return FALSE;
    if (header.flags & NO_COMPRESSION)
    {
        memcpy(dest, src + sizeof(struct RLEHeader), header.size);
        return TRUE;
    }

    destp = dest;
    srcp = src + sizeof(struct RLEHeader);
    var_r5 = header.size;
    while (var_r5 != 0)
    {
        temp_r0 = *srcp & 0x80;
        var_r3 = *srcp & 0x7F;
        srcp++;
        if (temp_r0)
        {
            // fill
            temp_r0 = *srcp;
            while (var_r3 != 0 && var_r5 > 0)
            {
                *destp = temp_r0;
                var_r3--;
                destp++;
                var_r5--;
            }
            srcp++;
        }
        else
        {
            // copy
            while (var_r3 != 0 && var_r5 > 0)
            {
                *destp = *srcp;
                var_r3--;
                destp++;
                var_r5--;
                srcp++;
            }
        }
    }
    return TRUE;
}

u32 recplay_cmpr_calc_compressed_size(void)
{
    return compress_rle((void *)&s_replayData, NULL, sizeof(s_replayData));
}

u32 recplay_cmpr_get_data(void *dest)
{
    if (dest == NULL)
        return 0;
    return compress_rle((void *)&s_replayData, dest, sizeof(s_replayData));
}

int recplay_cmpr_set_data(const void *src)
{
    return decompress_rle(src, (void *)&s_replayData, sizeof(s_replayData));
}

int func_8004C70C_sub(void)
{
    return (recordingState.isRecording == 0 && s_replayData.frameCount != 0);
}

int func_8004C70C(void)
{
    return (func_8004C70C_sub() && s_replayData.header.unk10 >= 0);
}

void func_8004C754(void)
{
    memset(&lbl_802F1F88, 0, sizeof(lbl_802F1F88));
}

void func_8004C780(void)
{
    lbl_802F1F88.unk0--;
    if (lbl_802F1F88.unk0 <= 0)
        lbl_802F1F88.unk2 = 0;
}

void func_8004C7A8(void)
{
    memset(&lbl_802F1F88, 0, sizeof(lbl_802F1F88));
}
