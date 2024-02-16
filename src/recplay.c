/**
 * recplay.c - Manages recording and playback of replays
 */
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "camera.h"
#include "course.h"
#include "event.h"
#include "info.h"
#include "input.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "polydisp.h"
#include "pool.h"
#include "recplay.h"
#include "recplay_cmpr.h"
#include "stage.h"
#include "window.h"
#include "world.h"

#define MAX_FRAMES 720

struct ReplayInfo g_recplayInfo;

// Assignment of replay slots for each player
struct PlayerRecordingStatus
{
    s16 replayIndex;  // index of the replay in the s_replays array
    s16 isRecording;  // 1 if currently recording, 0 if not recording
};
static struct PlayerRecordingStatus s_recordingStatus[8];

struct Replay
{
    struct ReplayHeader header;
    s16 unk18;
    s16 frameCount;  // number of frames in the replay
    s16 unk1C;
    s16 goalEntered;
    s16 unk20;
    Vec unk24;
    s32 unk30;
    s16 currFrame;  // current frame being recorded
    s16 time;  // time length of the replay in frames
    struct ReplayBallFrame ballFrames[MAX_FRAMES];
    struct ReplayWorldFrame worldFrames[MAX_FRAMES];
};

// Array of loaded replays
// The first 7 entries here are available for single player use, while the last 4 are assigned to
// each player during competition mode (see submode_game_play_init_func in game.c)
static struct Replay s_replays[11];

static u32 s_dummyVar;  // set to the size of the Replay struct, but never actually used

static s32 s_builtinReplayFilesCount;
static char **s_builtinReplayFileNames;  // pointer to array of strings containing the filenames of built-in replays
static s32 s_recordingStopTimer;  // a timer that is decremented each frame. Recording automaticallystops when it reaches zero.

static struct
{
    s32 replayIndex;
    u32 unk4;
} lbl_802F1F78;

static void func_8004A820(void);
static void end_grading_play(void);
static void grading_play(void);
static float func_8004ABD8(void);
static void u_reset_replay_floor_counts(void);
static void increment_replay_floor_counts(struct Replay *);
static void decrement_replay_floor_counts(struct Replay *);
static int get_replay_floor_count(struct Replay *);
static float calc_replay_grade_adjustment(struct Replay *);
static int u_get_highest_grade_replay(void);
static int dummy_return_true(struct Replay *);

// Initialize at power-on
void recplay_init(void)
{
    int i;
    int bufLen;
    char **pFileName;
    struct Replay *replay;
    int temp_r3;
    int nItems;
    char *buffer;
    DVDFileInfo file;
    DVDDir dir;
    DVDDirEntry dirent;
    static u32 s_totalSize = sizeof(s_replays);

    if (s_totalSize == 0)  // s_totalSize is always non-zero
        return;

    // read the recdata.bin file into s_replays
    // This fills the entire array
    DVDChangeDir("recdata");
    if (DVDOpen("recdata.bin", &file))
    {
        size_t size = MIN(file.length, sizeof(s_replays));
        avDVDRead(&file, s_replays, OSRoundUp32B(size), 0);
        DVDClose(&file);
    }
    DVDChangeDir("/test");

    s_dummyVar = sizeof(struct Replay);

    if (!(dipSwitches & DIP_DEBUG))
    {
        replay = s_replays;
        for (i = ARRAY_COUNT(s_replays); i > 0; i--, replay++)
        {
            if (!dummy_return_true(replay))
                memset(replay, 0, sizeof(*replay));  // never happens
        }
    }

    temp_r3 = u_get_highest_grade_replay();
    if (temp_r3 >= 0)
    {
        replay = &s_replays[temp_r3];
        memcpy(&s_replays[6], replay, sizeof(s_replays[6]));
        memset(replay, 0, sizeof(*replay));
    }
    u_reset_replay_floor_counts();

    s_recordingStatus[0].replayIndex = 0;
    s_recordingStatus[0].isRecording = 0;
    s_recordingStatus[1].replayIndex = 0;
    s_recordingStatus[1].isRecording = 0;
    s_recordingStatus[2].replayIndex = 0;
    s_recordingStatus[2].isRecording = 0;
    s_recordingStatus[3].replayIndex = 0;
    s_recordingStatus[3].isRecording = 0;

    if (DVDOpenDir("recdata", &dir) != 0)
    {
        // Scan for built-in replay files
        nItems = 0;
        bufLen = 0;
        while (DVDReadDir(&dir, &dirent) != 0)
        {
            if (!dirent.isDir)
            {
                nItems++;
                bufLen += strlen(dirent.name) + 1;
            }
        }
        s_builtinReplayFilesCount = nItems;
        buffer = OSAlloc(OSRoundUp32B(bufLen));
        s_builtinReplayFileNames = OSAlloc(OSRoundUp32B(nItems * sizeof(*s_builtinReplayFileNames)));

        // Get names of all built-in replay files
        dir.location = dir.entryNum + 1;
        pFileName = s_builtinReplayFileNames;
        while (DVDReadDir(&dir, &dirent) != 0)
        {
            if (!dirent.isDir)
            {
                strcpy(buffer, dirent.name);
                *pFileName++ = buffer;
                buffer += strlen(dirent.name) + 1;
            }
        }
        DVDCloseDir(&dir);
    }
    func_8004AFC0();
}

void recplay_load_builtin_replays(void)
{
    struct Replay *replay;
    int temp_r3_3;
    u32 temp_r28;
    int count;
    int fileIdx;
    struct Replay *toLoad;
    void *fileBuf;
    u32 fileSize;
    DVDFileInfo file;
    int i;

    if (s_builtinReplayFileNames == NULL || s_builtinReplayFilesCount == 0)
        return;

    count = (rand() & 0x7FFF) % 6 + 1;  // choose a random number replays to load from the file (up to 6)
    DVDChangeDir("recdata");
    fileIdx = (rand() & 0x7FFF) % s_builtinReplayFilesCount;  // choose a random file to load
    if (DVDOpen(s_builtinReplayFileNames[fileIdx], &file))
    {
        fileSize = MIN(file.length, count * sizeof(struct Replay));
        fileSize = OSRoundUp32B(fileSize);
        fileBuf = OSAllocFromHeap(__OSCurrHeap, fileSize);
        if (fileBuf != NULL)
        {
            avDVDRead(&file, fileBuf, fileSize, 0);
            toLoad = fileBuf;

            for (; count > 0; count--, toLoad++)
            {
                if (toLoad->header.grade != 0.0 && toLoad->header.stageId != 0)
                {
                    replay = &s_replays[recplay_find_new_index()];
                    decrement_replay_floor_counts(replay);
                    memcpy(replay, toLoad, sizeof(*replay));
                    increment_replay_floor_counts(replay);
                }
            }
            OSFree(fileBuf);

            temp_r3_3 = u_get_highest_grade_replay();
            if (temp_r3_3 >= 0)
            {
                replay = &s_replays[temp_r3_3];
                memcpy(&s_replays[6], replay, sizeof(s_replays[6]));
                memset(replay, 0, sizeof(*replay));
            }
        }

        DVDClose(&file);

    }

    DVDChangeDir("/test");

    // Penalize the built-in replays slightly so that the player's own replays are preferred
    replay = s_replays;
    for (i = 11; i > 0; i--, replay++)
        replay->header.grade *= 0.92f;
}

void recplay_init_first(void)
{
    int i;
    struct Replay *replay;

    replay = s_replays;
    for (i = 11; i > 0; i--, replay++)
    {
        if (replay->header.flags & 0x8000)
            replay->header.flags &= ~0x8000;
        if (!(dipSwitches & DIP_DEBUG))
            replay->header.grade *= 0.92f;
    }
    cmp_recplay_init_first();
}

void ev_recplay_init(void)
{
    s_recordingStopTimer = 0;
    dummy_8004AFD4();
}

void ev_recplay_main(void)
{
    struct Ball *ball;
    int i;
    struct PlayerRecordingStatus *recstat;

    if (debugFlags & 0xA)
        return;

    if (s_recordingStopTimer > 0)
    {
        s_recordingStopTimer--;
        if (s_recordingStopTimer == 0)
            recplay_stop_recording();
    }

    // Record ball and world frame for each player

    recstat = s_recordingStatus;
    ball = ballInfo;
    for (i = 4; i > 0; i--, recstat++, ball++)
    {
        modeCtrl.gameType;  // needed to match

        if ((s16)(int)recstat->isRecording)
        {
            struct Replay *replay;
            int frame;
            struct ReplayBallFrame *ballFrame;
            struct ReplayWorldFrame *worldFrame;

            replay = &s_replays[recstat->replayIndex];
            frame = replay->currFrame;

            ballFrame = &replay->ballFrames[frame];
            ballFrame->pos = ball->pos;
            ballFrame->rotX = ball->rotX;
            ballFrame->rotY = ball->rotY;
            ballFrame->rotZ = ball->rotZ;
            ballFrame->unk12 = (32767.0f * ball->unk114.x);
            ballFrame->unk14 = (32767.0f * ball->unk114.y);
            ballFrame->unk16 = (32767.0f * ball->unk114.z);
            ballFrame->unk18 = ball->flags;
            ballFrame->unk1C = ball->unk130;

            worldFrame = &replay->worldFrames[frame];
            worldFrame->rotX = currentWorld->xrot;
            worldFrame->rotZ = currentWorld->zrot;

            replay->frameCount++;
            replay->unk30 = stageInfo.unk0;
            if (++frame > MAX_FRAMES - 1)
                frame -= MAX_FRAMES;
            replay->currFrame = frame;
            if (replay->time < MAX_FRAMES)
                replay->time++;
            grading_play();
        }
    }
    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL
     || modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        recplay_cmpr_record_frame();
}

void ev_recplay_dest(void)
{
    if (s_recordingStopTimer > 0)
    {
        recplay_stop_recording();
        s_recordingStopTimer = 0;
    }
    recplay_cmpr_dest();
}

// Computes an index in the s_replays array that a new replay can use.
// If there are no free slots, then the replay with the lowest grade is
// replaced.
int recplay_find_new_index(void)
{
    float minGrade;
    int i;
    struct Replay *replay;
    int index;

    replay = s_replays;
    index = 0;
    minGrade = replay->header.grade;
    for (i = 6; i > 0; i--, replay++)
    {
        float grade = replay->header.grade;

        if (grade == 0.0)  // no replay
        {
            index = 6 - i;
            break;
        }
        else
        {
            grade *= calc_replay_grade_adjustment(replay);
            if (grade < minGrade)
            {
                index = 6 - i;
                minGrade = grade;
            }
        }
    }
    return index;
}

void u_recplay_reset_recording_state(void)
{
    struct PlayerRecordingStatus *recstat;
    int i;

    recplay_stop_recording();
    recstat = s_recordingStatus;
    for (i = 0; i < 4; i++, recstat++)
        recstat->replayIndex = -1;
}

// Assigns the specified replay recording slot to the player
void recplay_set_player_replay_index(int playerId, int replayIndex)
{
    s_recordingStatus[playerId].replayIndex = replayIndex;
}

// Starts recording for all players that have an assigned replay slot
void recplay_start_recording(void)
{
    int i;
    struct Replay *replay;
    struct PlayerRecordingStatus *recstat;
    int isPractice = (modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE);

    recstat = s_recordingStatus;
    for (i = 4; i > 0; i--, recstat++)
    {
        if (recstat->replayIndex >= 0)
        {
            replay = &s_replays[recstat->replayIndex];
            if (replay->header.floorNum != 0)
                decrement_replay_floor_counts(replay);
            replay->currFrame = 0;
            replay->time = 0;
            replay->unk18 = infoWork.timerCurr;
            replay->frameCount = 0;
            replay->header.flags = 0x8000;
            replay->header.stageId = currStageId;
            if (!isPractice)
            {
                replay->header.difficulty = modeCtrl.difficulty;
                replay->header.floorNum = infoWork.currFloor;
            }
            else
            {
                replay->header.difficulty = lbl_8027CE24[0].unk2;
                replay->header.floorNum = lbl_8027CE24[0].unk0;
            }
            replay->header.character = playerCharacterSelection[4 - i];
            replay->header.playerName[0] = 0;
            replay->unk30 = stageInfo.unk0;
            if (infoWork.flags & INFO_FLAG_BONUS_STAGE)
                replay->header.flags |= REPLAY_FLAG_BONUS_STAGE;
            if (!isPractice)
            {
                if (modeCtrl.courseFlags & COURSE_FLAG_EXTRA)
                    replay->header.flags |= REPLAY_FLAG_EXTRA;
                if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
                    replay->header.flags |= REPLAY_FLAG_MASTER;
            }
            else
            {
                if (lbl_8027CE24[0].unk4 & COURSE_FLAG_EXTRA)
                    replay->header.flags |= REPLAY_FLAG_EXTRA;
                if (lbl_8027CE24[0].unk4 & COURSE_FLAG_MASTER)
                    replay->header.flags |= REPLAY_FLAG_MASTER;
            }
            increment_replay_floor_counts(replay);
            recstat->isRecording = 1;
            func_8004A820();
        }
    }

    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL
     || modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        recplay_cmpr_start_recording();
}

// Stops recording for all players that currently are recording
void recplay_stop_recording(void)
{
    int i;
    struct PlayerRecordingStatus *recstat;
    struct Replay *replay;

    s_recordingStopTimer = 0;

    recstat = s_recordingStatus;
    for (i = 4; i > 0; i--, recstat++)
    {
        if (recstat->replayIndex >= 0)
        {
            if (recstat->isRecording == 1)
            {
                replay = &s_replays[recstat->replayIndex];
                if (replay->frameCount < 720)
                    replay->header.flags |= REPLAY_FLAG_4;
                else
                    replay->header.flags &= ~REPLAY_FLAG_4;
                end_grading_play();
                replay->header.grade = func_8004ABD8();
            }
            recstat->isRecording = 0;
        }
    }

    if (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL
     || modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE)
        recplay_cmpr_stop_recording();
}

void recplay_set_recording_stop_timer(int timer)
{
    s_recordingStopTimer = timer;
    if (timer == 0)
        recplay_stop_recording();
}

// Records that the player reached a goal
void recplay_record_goal(int playerId)
{
    struct PlayerRecordingStatus *recstat;
    struct Replay *replay;

    recstat = &s_recordingStatus[playerId];
    if (recstat->isRecording == 1)
    {
        replay = &s_replays[recstat->replayIndex];
        replay->header.flags |= REPLAY_FLAG_GOAL;
        replay->unk1C = infoWork.unk1C;
        replay->unk24 = infoWork.unk10;
        replay->goalEntered = infoWork.goalEntered;
        replay->unk20 = infoWork.unkE;
        recplay_cmpr_record_goal();
    }
}

// Records that the player fell off the stage
void recplay_record_fallout(int playerId)
{
    struct PlayerRecordingStatus *recstat;
    struct Replay *replay;

    recstat = &s_recordingStatus[playerId];
    if (recstat->isRecording == 1)
    {
        replay = &s_replays[recstat->replayIndex];
        replay->header.flags |= REPLAY_FLAG_FALLOUT;
        replay->unk1C = infoWork.timerCurr;
        recplay_cmpr_record_fallout();
    }
}

// Records that the player ran out of time
void recplay_record_timeover(int playerId)
{
    struct PlayerRecordingStatus *recstat;
    struct Replay *replay;

    recstat = &s_recordingStatus[playerId];
    if (recstat->isRecording == 1)
    {
        replay = &s_replays[recstat->replayIndex];
        replay->header.flags |= REPLAY_FLAG_TIME_OVER;
        recplay_cmpr_record_timeover();
    }
}

// Records that the player cleared a bonus stage
void recplay_record_bonus_clear(int playerId)
{
    struct PlayerRecordingStatus *recstat;
    struct Replay *replay;

    recstat = &s_recordingStatus[playerId];
    if (recstat->isRecording == 1)
    {
        replay = &s_replays[recstat->replayIndex];
        replay->header.flags |= REPLAY_FLAG_BONUS_CLEAR;
        replay->unk1C = infoWork.timerCurr;
        recplay_cmpr_record_bonus_clear();
    }
}

void recplay_record_player_name(char *playerName)
{
    BOOL hasSymbol;
    volatile char *symbols;
    char *p;
    int i;
    struct Replay *replay;

    // Don't allow special characters in player name
    hasSymbol = FALSE;
    for (p = playerName; *p != 0; p++)
    {
        for (symbols = "#$%@^"; *symbols != 0; symbols++)
        {
            if (*symbols == *p)
            {
                hasSymbol = TRUE;
                playerName = "";
                break;
            }
        }

        if (hasSymbol)
            break;
    }

    replay = s_replays;
    for (i = 11; i > 0; i--, replay++)
    {
        if (replay->header.flags & REPLAY_FLAG_15)
        {
            strncpy(replay->header.playerName, playerName, 4);
            replay->header.playerName[3] = 0;
        }
    }
    recplay_cmpr_record_player_name(playerName);
}

void func_80049514(int replayIndex)
{
    struct Replay *replay;

    if (replayIndex == 11)
    {
        func_8004B70C();
        return;
    }
    replay = &s_replays[replayIndex];
    infoWork.timerMax = replay->unk18;
    infoWork.timerCurr = replay->unk18;
    infoWork.flags &= ~(INFO_FLAG_GOAL|INFO_FLAG_TIMEOVER|INFO_FLAG_FALLOUT|INFO_FLAG_05|INFO_FLAG_BONUS_STAGE|INFO_FLAG_BONUS_CLEAR|INFO_FLAG_10);
    infoWork.currFloor = replay->header.floorNum;
    if (replay->header.flags & REPLAY_FLAG_GOAL)
    {
        infoWork.flags |= INFO_FLAG_TIMER_PAUSED|INFO_FLAG_05;
        infoWork.timerCurr = replay->unk1C;
        infoWork.unk1C = replay->unk1C;
        infoWork.unk10 = replay->unk24;
        infoWork.goalEntered = replay->goalEntered;
        infoWork.unkE = replay->unk20;
    }
    if (replay->header.flags & REPLAY_FLAG_BONUS_STAGE)
        infoWork.flags |= INFO_FLAG_BONUS_STAGE;
    playerCharacterSelection[0] = replay->header.character;
    modeCtrl.difficulty = replay->header.difficulty;
    modeCtrl.courseFlags &= ~(COURSE_FLAG_EXTRA|COURSE_FLAG_MASTER);
    if (replay->header.flags & REPLAY_FLAG_EXTRA)
        modeCtrl.courseFlags |= COURSE_FLAG_EXTRA;
    if (replay->header.flags & REPLAY_FLAG_MASTER)
        modeCtrl.courseFlags |= COURSE_FLAG_MASTER;
}

float recplay_get_time(int replayIndex)
{
    struct Replay *replay;

    if (replayIndex == 11)
        return recplay_cmpr_get_time();
    replay = &s_replays[replayIndex];
    return ((int)replay->time != 0) ? replay->time - 1 : 0;
}

// Gets the ball's position and rotation at the specified time in the replay
void recplay_get_ball_frame(int replayIndex, struct ReplayBallFrame *out, float time)
{
    float temp_f30;
    float temp_f31;
    float var_f1;
    int temp_r3;
    int temp_r5;
    s32 var_r3;
    s32 var_r3_2;
    s32 var_r6;
    struct Ball *ball;
    struct Replay *replay;
    struct ReplayBallFrame sp64;
    struct ReplayBallFrame sp44;
    struct ReplayBallFrame curr;
    Point3d sp18;
    u8 unused[4];

    ball = currentBall;
    if (replayIndex == 11)
    {
        recplay_cmpr_get_ball_frame(time, out);
        return;
    }
    replay = &s_replays[replayIndex];

    // If there are no frames, just copy the ball's current position
    if ((s16)(int)replay->time == 0)
    {
        out->pos = ball->pos;
        out->rotX = ball->rotX;
        out->rotY = ball->rotY;
        out->rotZ = ball->rotZ;
        return;
    }
    if (time < 0.0)
    {
        var_r3 = replay->currFrame - 1;
        if (var_r3 < 0)
            var_r3 += 720;
        *out = replay->ballFrames[var_r3];
        return;
    }
    if (replayIndex == 11)
        var_f1 = recplay_cmpr_get_time();
    else
        var_f1 = ((s16)replay->time != 0) ? replay->time - 1 : 0;
    if (var_f1 <= time)
    {
        int var_r4 = replay->currFrame - replay->time;
        while (var_r4 < 0)
            var_r4 += 720;
        *out = replay->ballFrames[var_r4];
        return;
    }

    temp_r5 = time;

    temp_f31 = time - temp_r5;
    temp_f30 = 1.0 - temp_f31;

    var_r3_2 = replay->currFrame - 1 - temp_r5;
    if (var_r3_2 < 0)
        var_r3_2 += 720;
    var_r6 = var_r3_2 - 1;
    if (var_r6 < 0)
        var_r6 += 720;

    sp64 = replay->ballFrames[var_r3_2];
    sp44 = replay->ballFrames[var_r6];

    curr.pos.x = (sp64.pos.x * temp_f30) + (sp44.pos.x * temp_f31);
    curr.pos.y = (sp64.pos.y * temp_f30) + (sp44.pos.y * temp_f31);
    curr.pos.z = (sp64.pos.z * temp_f30) + (sp44.pos.z * temp_f31);

    curr.rotX = sp64.rotX * temp_f30 + sp44.rotX * temp_f31;
    curr.rotY = sp64.rotY * temp_f30 + sp44.rotY * temp_f31;
    curr.rotZ = sp64.rotZ * temp_f30 + sp44.rotZ * temp_f31;

    sp18.x = sp64.unk12 * temp_f30 + sp44.unk12 * temp_f31;
    sp18.y = sp64.unk14 * temp_f30 + sp44.unk14 * temp_f31;
    sp18.z = sp64.unk16 * temp_f30 + sp44.unk16 * temp_f31;
    mathutil_vec_set_len(&sp18, &sp18, 32767.0f);

    curr.unk12 = sp18.x;
    curr.unk14 = sp18.y;
    curr.unk16 = sp18.z;
    curr.unk18 = sp64.unk18;
    curr.unk1C = sp64.unk1C * temp_f30 + sp44.unk1C * temp_f31;
    *out = curr;
}

// Gets the world tilt angles at the specified time in the replay
void recplay_get_world_frame(int replayIndex, struct ReplayWorldFrame *out, float time)
{
    struct ReplayWorldFrame sp20;
    struct ReplayWorldFrame sp1C;
    struct ReplayWorldFrame curr;
    float temp_f5;
    float temp_f6;
    float var_f1;
    int temp_r5;
    s32 var_r3;
    s32 var_r3_2;
    s32 var_r5;
    struct Replay *replay;
    struct World *world;
    u8 unused[4];

    world = currentWorld;
    if (replayIndex == 11)
    {
        recplay_cmpr_get_world_frame(time, out);
        return;
    }
    replay = &s_replays[replayIndex];
    if ((s16)(int)replay->time == 0)
    {
        out->rotX = (s16) world->xrot;
        out->rotZ = (s16) world->zrot;
        return;
    }
    if (time < 0.0)
    {
        var_r3 = replay->currFrame - 1;
        if (var_r3 < 0)
            var_r3 += 720;
        *out = replay->worldFrames[var_r3];
        return;
    }
    if (replayIndex == 11)
        var_f1 = recplay_cmpr_get_time();
    else
        var_f1 = ((s16)replay->time != 0) ? replay->time - 1 : 0;
    if (var_f1 <= time)
    {
        int var_r4 = replay->currFrame - replay->time;
        while (var_r4 < 0)
            var_r4 += 720;
        *out = replay->worldFrames[var_r4];
        return;
    }

    temp_r5 = time;
    temp_f5 = time - temp_r5;
    temp_f6 = 1.0 - temp_f5;

    var_r3_2 = replay->currFrame - 1 - temp_r5;
    if (var_r3_2 < 0)
        var_r3_2 += 720;
    var_r5 = var_r3_2 - 1;
    if (var_r5 < 0)
        var_r5 += 720;

    sp20 = replay->worldFrames[var_r3_2];
    sp1C = replay->worldFrames[var_r5];

    curr.rotX = sp20.rotX * temp_f6 + sp1C.rotX * temp_f5;
    curr.rotZ = sp20.rotZ * temp_f6 + sp1C.rotZ * temp_f5;
    *out = curr;
}

float recplay_get_info_timer(int replayIndex, float arg1)
{
    float var_f1;
    struct Replay *replay;

    if (replayIndex == 11)
        return recplay_cmpr_get_info_timer(arg1);
    replay = &s_replays[replayIndex];
    var_f1 = arg1 + (float)(replay->unk18 - replay->frameCount);
    if (replay->header.flags & REPLAY_FLAG_GOAL)
    {
        if (var_f1 < replay->unk1C)
            var_f1 = replay->unk1C;
    }
    return var_f1;
}

void recplay_get_header(int replayIndex, struct ReplayHeader *header)
{
    struct Replay *replay;

    if (replayIndex == 11)
    {
        recplay_cmpr_get_header(header);
        return;
    }
    replay = &s_replays[replayIndex];
    *header = replay->header;
}

float recplay_get_stage_timer(float arg0, int replayIndex)
{
    struct Replay *replay;

    if (replayIndex == 11)
        return func_8004C254(arg0);
    replay = &s_replays[replayIndex];
    return replay->unk30 - arg0;
}

#pragma force_active on
void u_replay_test_init(void)
{
    if (debugFlags & 0xA)
        return;

    u_clear_buffers_2_and_5();
    gameSubmodeRequest = SMD_TEST_REPLAY_MAIN;
    event_finish_all();
    func_80044920();
    load_stage(loadingStageIdRequest);
    event_start(EVENT_STAGE);
    event_start(EVENT_WORLD);
    event_start(EVENT_BALL);
    event_start(EVENT_STOBJ);
    event_start(EVENT_INFO);
    event_start(EVENT_ITEM);
    event_start(EVENT_OBJ_COLLISION);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    event_start(EVENT_EFFECT);
    event_start(EVENT_REND_EFC);
    event_start(EVENT_BACKGROUND);
    g_recplayInfo.u_playerId = 0;
    lbl_802F1F78.replayIndex = g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId];
    lbl_802F1F78.unk4 = 0;
}

void u_replay_test_main(void)
{
    struct Ball *currBall = currentBall;
    struct Replay *replay;
    int temp_r0_2;
    struct ReplayHeader header;
    u8 unused[0xC];

    if (debugFlags & 0xA)
        return;

    u_clear_buffers_2_and_5();
    if ((controllerInfo[0].pressed.button & PAD_BUTTON_LEFT)
     || ((controllerInfo[0].held.button & PAD_BUTTON_LEFT) && (controllerInfo[0].held.button & PAD_TRIGGER_R)))
    {
        if (lbl_802F1F78.replayIndex > 0)
            lbl_802F1F78.replayIndex--;
    }
    if ((controllerInfo[0].pressed.button & PAD_BUTTON_RIGHT)
     || ((controllerInfo[0].held.button & PAD_BUTTON_RIGHT) && (controllerInfo[0].held.button & PAD_TRIGGER_R)))
    {
        if (lbl_802F1F78.replayIndex < 11)
            lbl_802F1F78.replayIndex++;
    }

    if (lbl_802F1F78.replayIndex < 11)
        replay = &s_replays[lbl_802F1F78.replayIndex];
    else
        replay = NULL;

    temp_r0_2 = lbl_802F1F78.replayIndex;
    if (temp_r0_2 == 11)
        recplay_cmpr_get_header(&header);
    else
        header = s_replays[temp_r0_2].header;

    window_set_cursor_pos(8, 8);
    window_printf_2(" REPLAY TEST\n\n");
    if (lbl_802F1F78.replayIndex != 11)
        window_printf_2("     ID: %03d\n", lbl_802F1F78.replayIndex);
    else
    {
        window_set_text_color(WINDOW_COLOR_RED);
        window_printf_2("     Record for Save\n");
        window_set_text_color(WINDOW_COLOR_WHITE);
    }

    if (recplay_get_time(lbl_802F1F78.replayIndex) == 0.0f)
    {
        window_set_text_color(WINDOW_COLOR_RED);
        window_printf_2("       :NO DATA\n");
        window_set_text_color(WINDOW_COLOR_WHITE);
    }
    else
    {
        window_printf_2("  STAGE: %03d(Lib No.%03d)\n", header.floorNum, header.stageId);
        window_printf_2(" COURSE: %3d\n", header.difficulty);
        if (controllerInfo[0].held.button & PAD_BUTTON_Y)
        {
            window_printf_2("  MONKY: %3d\n", header.character);
            window_printf_2("   NAME: %s\n", header.playerName);
            window_printf_2("  GRADE: %f\n", header.grade);
            if (replay != NULL)
            {
                window_printf_2("\n");
                window_printf_2("Same Kind Data Count : %d\n", get_replay_floor_count(replay));
                window_printf_2("Adjust Coeff : %f\n", calc_replay_grade_adjustment(replay));
            }
        }
    }
    if (currBall->state == 4)
        infoWork.flags &= ~(INFO_FLAG_REPLAY|INFO_FLAG_11);

    switch (lbl_802F1F78.unk4)
    {
    case 0:
        if (recplay_get_time(lbl_802F1F78.replayIndex) > 0.0)
        {
            if ((controllerInfo[0].pressed.button & PAD_BUTTON_A)
             && !(controllerInfo[0].pressed.button & PAD_BUTTON_B))
            {
                float var_f1_4;
                int temp_r0_7;

                g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId] = lbl_802F1F78.replayIndex;
                currStageId = header.stageId;
                modeCtrl.difficulty = header.difficulty;
                event_finish(EVENT_EFFECT);
                func_80049514(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]);
                infoWork.flags |= INFO_FLAG_REPLAY;
                load_stage(currStageId);
                event_finish(EVENT_STAGE);
                event_finish(EVENT_STOBJ);
                event_finish(EVENT_ITEM);
                event_finish(EVENT_EFFECT);
                event_finish(EVENT_REND_EFC);
                event_finish(EVENT_BACKGROUND);
                event_finish(EVENT_BALL);
                event_start(EVENT_STAGE);
                event_start(EVENT_STOBJ);
                event_start(EVENT_ITEM);
                event_start(EVENT_EFFECT);
                event_start(EVENT_REND_EFC);
                event_start(EVENT_BACKGROUND);
                event_start(EVENT_BALL);
                BALL_FOREACH( ball->state = 9; )
                WORLD_FOREACH( world->state = 6; )
                camera_set_state_all(0x2C);
                g_recplayInfo.u_timeOffset = recplay_get_time(g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId]);
                var_f1_4 = g_recplayInfo.u_timeOffset;
                temp_r0_7 = g_recplayInfo.u_replayIndexes[g_recplayInfo.u_playerId];
                if (temp_r0_7 == 11)
                    var_f1_4 = func_8004C254(g_recplayInfo.u_timeOffset);
                else
                    var_f1_4 = s_replays[temp_r0_7].unk30 - var_f1_4;
                animate_anim_groups(var_f1_4);
            }
        }
        if (replay != NULL)
        {
            if ((controllerInfo[0].held.button & PAD_BUTTON_A)
             && (controllerInfo[0].held.button & PAD_TRIGGER_L))
            {
                BALL_FOREACH( ball->state = 0; )
                WORLD_FOREACH( world->state = 4; )
                if (header.floorNum != 0)
                    decrement_replay_floor_counts(replay);
                memset(replay, 0, sizeof(*replay));
            }
        }
        if ((controllerInfo[0].held.button & PAD_BUTTON_B)
         && (controllerInfo[0].held.button & PAD_TRIGGER_L))
        {
            lbl_802F1F78.unk4 = 1;
            modeCtrl.submodeTimer = 0;
            return;
        }
        break;
    case 1:
        window_set_cursor_pos(32, 30);
        switch (modeCtrl.submodeTimer)
        {
        case 0:
        case 1:
            window_printf_2("Converting to C Source format.");
            break;
        case 3:
        case 4:
            window_printf_2("Please Type [save_recplay] in DDD\n");
            window_printf_2(" to Make File recplay_data.h.\n");
            break;
        case 5:
            lbl_802F1F78.unk4 = 0;
            break;
        }
        modeCtrl.submodeTimer++;
        break;
    }
}

void func_8004A800(void)
{
    draw_normal_game_scene();
}
#pragma force_active reset

static struct
{
    float grade;
    Vec unk4;
    Vec unk10;
    s32 unk1C;
} someGradingStruct;

static void func_8004A820(void)
{
    someGradingStruct.grade = 0.0f;
    someGradingStruct.unk4 = currentBall->pos;
    someGradingStruct.unk10 = currentBall->vel;
    someGradingStruct.unk1C = 0;
}

static void end_grading_play(void)
{
    float temp_f7;
    float f1;
    float var_f0;
    struct Replay *replay;

    replay = &s_replays[s_recordingStatus[0].replayIndex];
    temp_f7 = (float)someGradingStruct.unk1C / (float)replay->frameCount;
    f1 = replay->frameCount + 1;
    var_f0 = someGradingStruct.grade;
    var_f0 *= 1.0 / f1;
    if (replay->header.flags & REPLAY_FLAG_GOAL)
    {
        var_f0 *= 2.0;
        var_f0 *= 1.0 + (float)(replay->unk18 - replay->unk1C) / (float)replay->unk18;
        if (replay->header.flags & 0x10)
            var_f0 *= 1.0 + (replay->frameCount - 360.0) / 720.0;
        var_f0 *= 1.0 + 0.5 * (1.0 - temp_f7);
        var_f0 *= 1.0 + 0.25 * (0.02 * replay->header.floorNum);
    }
    if (replay->header.flags & REPLAY_FLAG_FALLOUT)
    {
        var_f0 *= 0.25;
        var_f0 *= temp_f7;
    }
    if (replay->header.flags & REPLAY_FLAG_TIME_OVER)
        var_f0 *= 0.1;
    someGradingStruct.grade = var_f0;
}

#ifdef NONMATCHING
static void grading_play(void)
{
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float f1;
    float var_f31 = 0.0f;
    float temp_f30;
    float temp_f1_3;
    struct Ball *ball;

    ball = currentBall;
    sp2C = ball->pos;
    sp20.x = ball->pos.x - someGradingStruct.unk4.x;
    sp20.y = ball->pos.y - someGradingStruct.unk4.y;
    sp20.z = ball->pos.z - someGradingStruct.unk4.z;
    sp14 = sp20;
    temp_f30 = mathutil_vec_len(&sp14);
    sp8 = someGradingStruct.unk10;
    f1 = mathutil_vec_len(&sp8);
    var_f31 += 2.0 * (temp_f30 - f1);
    temp_f1_3 = sp14.x * sp8.x + sp14.y * sp8.y + sp14.z * sp8.z;
    !temp_f1_3;
    if (temp_f1_3 > 1.1920929e-7f)
        var_f31 += mathutil_sqrt(temp_f1_3) * 1.0;
    if (ball->flags & 1)
        someGradingStruct.unk1C++;
    someGradingStruct.grade += var_f31;
    someGradingStruct.unk4 = sp2C;
    someGradingStruct.unk10 = sp20;
}
#else
static asm void grading_play(void)
{
    nofralloc
#include "../asm/nonmatchings/grading_play.s"
}
#pragma peephole on
#endif

static float func_8004ABD8(void)
{
    return someGradingStruct.grade;
}

static s8 s_replayCountPerFloor[3][61];  // number of replays per floor in each difficulty
static s8 s_replayCountPer10Floors[3][7];  // number of replays per group of 10 floors in each difficulty

static void u_reset_replay_floor_counts(void)
{
    s32 i;
    struct Replay *replay;

    memset(s_replayCountPerFloor, 0, sizeof(s_replayCountPerFloor));
    memset(&s_replayCountPer10Floors, 0, sizeof(s_replayCountPer10Floors));

    replay = s_replays;
    for (i = ARRAY_COUNT(s_replays); i > 0; i--, replay++)
    {
        if (replay->header.floorNum != 0)
            increment_replay_floor_counts(replay);
    }
}

static void increment_replay_floor_counts(struct Replay *replay)
{
    s8 *perFloor = s_replayCountPerFloor[replay->header.difficulty];
    int index = replay->header.floorNum - 1;

    if (replay->header.flags & REPLAY_FLAG_MASTER)
        index += 60;
    else if (replay->header.flags & REPLAY_FLAG_EXTRA)
        index += 50;
    perFloor[index]++;
    s_replayCountPer10Floors[replay->header.difficulty][index / 10]++;
}

static void decrement_replay_floor_counts(struct Replay *replay)
{
    s8 *perFloor = s_replayCountPerFloor[replay->header.difficulty];
    int index = replay->header.floorNum - 1;

    if (replay->header.flags & REPLAY_FLAG_MASTER)
        index += 60;
    else if (replay->header.flags & REPLAY_FLAG_EXTRA)
        index += 50;
    perFloor[index]--;
    s_replayCountPer10Floors[replay->header.difficulty][index / 10]--;
}

// Returns the number of replays that are for this replay's floor
static int get_replay_floor_count(struct Replay *replay)
{
    s8 *perFloor = s_replayCountPerFloor[replay->header.difficulty];
    int index = replay->header.floorNum - 1;

    if (replay->header.flags & REPLAY_FLAG_MASTER)
        index += 60;
    else if (replay->header.flags & REPLAY_FLAG_EXTRA)
        index += 50;
    return perFloor[index];
}

// Computes a coefficient used by recplay_find_new_index to penalize
// replays that have the same floor or group of floors as other replays.
// The goal of this is to have more variety in the replays that are shown
// during the ADV sequence.
static float calc_replay_grade_adjustment(struct Replay *replay)
{
    s8 *perFloor = s_replayCountPerFloor[replay->header.difficulty];
    s8 *per10Floor = s_replayCountPer10Floors[replay->header.difficulty];
    int index = replay->header.floorNum - 1;
    float value;

    if (replay->header.flags & REPLAY_FLAG_MASTER)
        index += 60;
    else if (replay->header.flags & REPLAY_FLAG_EXTRA)
        index += 50;
    value = perFloor[index] + 0.2 * per10Floor[index / 10];
    if (value > 0.0)
        return 1.0 / value;
    return 1.0f;
}

static int u_get_highest_grade_replay(void)
{
    struct Replay *replay;
    int i;
    int var_r3 = -1;
    float var_f1 = -1.0f;

    replay = s_replays;
    for (i = 6; i > 0; i--, replay++)
    {
        if ((replay->header.flags & REPLAY_FLAG_GOAL) && (replay->header.flags & 0x10)
         && replay->header.grade > var_f1)
        {
            var_r3 = 6 - i;
            var_f1 = replay->header.grade;
        }
    }
    return var_r3;
}

static int dummy_return_true(struct Replay *unused)
{
    return TRUE;
}

static u32 lbl_801B89A0[] =
{
    0x0002136E,
    0x00076675,
    0x00096776,
    0,
};

static u32 lbl_801B89B0[] =
{
    0x00086877,
    0x00094E78,
    0x000B1B79,
    0x000C697A,
    0x000E6B7B,
    0x00136D7C,
    0x0017707D,
    0x0018717E,
    0x001A5F7F,
    0,
};

static u32 lbl_802F0A04[] =
{
    0,
};

#pragma force_active on
void *lbl_801B89D8[] = { lbl_801B89A0, lbl_801B89B0, lbl_802F0A04, NULL };
#pragma force_active reset
