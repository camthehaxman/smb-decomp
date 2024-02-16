#ifndef _SRC_BALL_H_
#define _SRC_BALL_H_

#include <dolphin/types.h>
#include <dolphin/mtx.h>

struct PhysicsBall;

enum
{
    BALL_FLAG_00 = 1 << 0,
    BALL_FLAG_01 = 1 << 1,
    BALL_FLAG_02 = 1 << 2,
    BALL_FLAG_03 = 1 << 3,
    BALL_FLAG_INVISIBLE = 1 << 4,
    BALL_FLAG_05 = 1 << 5,
    BALL_FLAG_06 = 1 << 6,
    BALL_FLAG_07 = 1 << 7,
    BALL_FLAG_08 = 1 << 8,
    BALL_FLAG_REVERSE_GRAVITY = 1 << 9,
    BALL_FLAG_IGNORE_GRAVITY = 1 << 10,
    BALL_FLAG_11 = 1 << 11,
    BALL_FLAG_GOAL = 1 << 12,  // monkey dances and cannot move horizontally
    BALL_FLAG_13 = 1 << 13,
    BALL_FLAG_14 = 1 << 14,
    BALL_FLAG_TIMEOVER = 1 << 15,  // monkey does a sad animation
    BALL_FLAG_16 = 1 << 16,
    BALL_FLAG_17 = 1 << 17,
    BALL_FLAG_18 = 1 << 18,
    BALL_FLAG_19 = 1 << 19,
    BALL_FLAG_20 = 1 << 20,
    BALL_FLAG_21 = 1 << 21,
    BALL_FLAG_22 = 1 << 22,
    BALL_FLAG_23 = 1 << 23,
    BALL_FLAG_24 = 1 << 24,
    BALL_FLAG_25 = 1 << 25,
    BALL_FLAG_26 = 1 << 26,
    BALL_FLAG_27 = 1 << 27,
    BALL_FLAG_28 = 1 << 28,
    BALL_FLAG_29 = 1 << 29,
    BALL_FLAG_30 = 1 << 30,
    BALL_FLAG_31 = 1 << 31,
};

enum
{
    BALL_STATE_0,
    BALL_STATE_1,
    BALL_STATE_READY_MAIN = 2,
    BALL_STATE_3,
    BALL_STATE_4,
    BALL_STATE_GOAL_INIT = 5,  // ball slows to a stop after entering the goal
    BALL_STATE_GOAL_MAIN = 6,
    BALL_STATE_REPLAY_INIT_1,
    BALL_STATE_REPLAY_MAIN_1 = 8,
    BALL_STATE_REPLAY_INIT_2,
    BALL_STATE_REPLAY_MAIN_2 = 10,
    BALL_STATE_11,
    BALL_STATE_12,
    BALL_STATE_13,
    BALL_STATE_14,
    BALL_STATE_15,
    BALL_STATE_16,
    BALL_STATE_17,
    BALL_STATE_18,
    BALL_STATE_19,
    BALL_STATE_20,
    BALL_STATE_DEMO_INIT = 21,
    BALL_STATE_22,  // minigame
    BALL_STATE_23,  // minigame
    BALL_STATE_24,  // minigame
    BALL_STATE_25,  // minigame
    BALL_STATE_26,  // minigame
    BALL_STATE_27,
    BALL_STATE_28,
};

struct Ball_child
{
    u8 filler0[0x14];
    u32 unk14;
    u8 filler18[0x1CE - 0x18];
    s16 unk1CE;
};

struct Ball
{
    u8 unk0;
    s8 unk1;
    /*0x02*/ s8 lives;
    /*0x03*/ s8 state;
    /*0x04*/ Vec pos;
    /*0x10*/ Vec prevPos;
    /*0x1C*/ Vec vel;
    /*0x28*/ s16 rotX;
    /*0x2A*/ s16 rotY;
    /*0x2C*/ s16 rotZ;
    /*0x2E*/ s8 playerId;
    /*0x2F*/ s8 rank;  // rank in competition mode
    Mtx unk30;
    s16 unk60;
    s16 unk62;
    s16 unk64;
    /*0x66*/ s16 oldModelId;  // id of Naomi model
    /*0x68*/ float currRadius;
    /*0x6C*/ float accel;
    /*0x70*/ float restitution;
    /*0x74*/ float modelScale;
    /*0x78*/ s32 bananas;
    /*0x7C*/ s32 score;
    s32 unk80;
    Vec unk84;
    s16 unk90;
    s16 unk92;
    /*0x94*/ u32 flags;
    Quaternion unk98;
    Quaternion unkA8;
    Vec unkB8;
    float unkC4;
    Mtx unkC8;
    float speed;
    /*0xFC*/ struct Ape *ape;
    u32 unk100;
    Vec unk104;
    float unk110;
    Vec unk114;
    u32 unk120;
    s16 unk124;
    /*0x126*/ s16 winStreak;  // number of consecutive competition mode wins
    s16 unk128;
    s16 unk12A;
    u8 filler12C[0x130-0x12C];
    float unk130;
    u32 unk134;
    s32 bananaBonus;
    s32 unk13C;
    /*0x140*/ float targetRadius;  // radius that the ball grows/shrinks to?
    struct Ball_child *unk144;  // guessing this is the same type as unkFC?
    u8 unk148;
    u8 filler14A[0x14A - 0x149];
    /*0x14A*/ u8 colorId;
    u8 unk14B;
    u8 filler14C[2];
    s16 unk14E;
    Vec unk150;  // positiom ball entered goal?
    /*0x15C*/ float u_opacity[4];  // something to do with ball-ball intersection (competition mode)
    u8 filler16C[0x1A4-0x16C];
};

// runs 'code' for each active ball
#define BALL_FOREACH(code) \
    { \
        struct Ball *ball; \
        struct Ball *ballBackup_; \
        s8 *status; \
        int i_; \
        ballBackup_ = currentBall; \
        ball = ballInfo; \
        status = g_poolInfo.playerPool.statusList; \
        for (i_ = 0; i_ < g_poolInfo.playerPool.count; i_++, ball++, status++) \
        { \
            if (*status == STAT_NORMAL) \
            { \
                currentBall = ball; \
                { code } \
            } \
        } \
        currentBall = ballBackup_; \
    }

extern struct Ball ballInfo[8];
extern struct Ball *currentBall;
extern void (*minigameRelBallCallback)(struct Ball *);
extern s32 apeThreadNo[16];
extern s32 playerCharacterSelection[MAX_PLAYERS];
extern s32 playerControllerIDs[4];
extern s32 lbl_80206BE0[4];

extern s16 clearHemisphereOutsideParts[];
extern s16 clearHemisphereInsideParts[];
extern s16 coloredBallPartModelIDs[][9];

void func_8003699C(struct Ape *a);
float func_80036CAC(struct Ape *a);
void func_80036EB8(struct Ape *a);
// ? func_80037098();
void u_choose_ape_anim(struct Ape *a, float b);
void func_8003765C(struct Ape *a);
void func_80037718();
void func_80037B1C(struct Ball *);
void ball_set_ape_flags_80037B20(void);
void ev_ball_init(void);
struct Ape *ape_get_by_type(int a, enum Character character, void (*c)(struct Ape *, int));
void ev_ball_main(void);
void func_80038528(struct Ball *);
void ev_ball_dest(void);
void ball_draw(void);
void u_ball_shadow_something_1(void);
void u_ball_shadow_something_2(void);
void give_bananas(int bananas);
void set_ball_target(int, Vec *, float);
// ? u_ball_init_1();
void u_ball_init_2(struct Ball *);
void ball_func_0(struct Ball *);
void ball_func_1(struct Ball *);
void ball_func_ready_main(struct Ball *);
void ball_func_3(struct Ball *);
void ball_func_4(struct Ball *);
void ball_func_goal_init(struct Ball *);
void ball_func_goal_main(struct Ball *);
void ball_func_replay_init(struct Ball *);
void ball_func_replay_main(struct Ball *);
void ball_func_11(struct Ball *);
void ball_func_12(struct Ball *);
void ball_func_13(struct Ball *);
void ball_func_14(struct Ball *);
void ball_func_15(struct Ball *);
void ball_func_17(struct Ball *);
void ball_func_16(struct Ball *);
void ball_func_18(struct Ball *);
void ball_func_19(struct Ball *);
void ball_func_20(struct Ball *);
void ball_func_demo_init(struct Ball *);
void ball_func_mini(struct Ball *);
void ball_func_27(struct Ball *);
void ball_func_28(struct Ball *);
void handle_ball_linear_kinematics(struct Ball *, struct PhysicsBall *, int);
void handle_ball_linear_kinematics_ignore_collision(struct Ball *, struct PhysicsBall *, int);
void update_ball_ape_transform(struct Ball *, struct PhysicsBall *, int);
// ? ball_8003BBF4();
void handle_ball_rotational_kinematics(struct Ball *, struct PhysicsBall *, int);
void ball_set_type(struct Ball *, int);
void ball_set_strongcoli_efc(struct Ball *);
void init_physball_from_ball(struct Ball *, struct PhysicsBall *b);
void set_ball_pos_and_vel_from_physball(struct Ball *, struct PhysicsBall *b);
void ball_ape_yang(struct Ball *);
void ball_effect(void);
void ball_sound(struct Ball *);
void ball_set_highspeed_efc(struct Ball *);
void animate_ball_size_change(struct Ball *);

#endif
