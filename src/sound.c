#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>
#include <musyx/musyx.h>

#include "global.h"
#include "adv.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "event.h"
#include "load.h"
#include "mathutil.h"
#include "mode.h"
#include "sound.h"
#include "spline.h"
#include "stage.h"
#include "window.h"

struct Struct801FE498
{
    s16 unk0;
    s16 unk2;
    s8 unk4;
    s8 unk5;
    s8 unk6;
    s8 unk7;
    u32 unk8;
};

static void func_8002A34C(void);
static void func_8002A964(struct Struct801FE498 *arg0);
static s8 func_8002BB20(int arg0, Vec *arg1, s8 *arg2, s8 *arg3);
static void func_8002CEB8(int);
static void lbl_8002D420(s32 result, DVDFileInfo *fileInfo);
static void lbl_8002D538(s32 result, DVDFileInfo *fileInfo);

const struct SoundGroupDesc g_soundGroupDesc[] =
{
    // name          unused      index   basename
    { "GRPse04",          0,  3,     0, "allse"  },
    { "GRPse01",          4,  0,     0, "allse"  },
    { "GRPse02",         85,  1,     0, "allse"  },
    { "GRPse03",        114,  2,     0, "allse"  },
    { "GRPnar",         126,  7,     0, "allse"  },
    { "GRPboy",         178,  4,     0, "allse"  },
    { "GRPgirl",        243,  6,     0, "allse"  },
    { "GRPbaby",        310,  5,     0, "allse"  },
    { "GRPgoli",        370,  8,     0, "allse"  },
    { "GRPbil_set",     411,  1,     1, "bil"    },
    { "GRPbow_set",     476,  2,     2, "bowl"   },
    { "GRPfight_set",   539,  6,     3, "fight"  },
    { "GRPgolf_set",    626,  3,     4, "golf"   },
    { "GRPrace_set",    671,  4,     5, "race"   },
    { "GRPtarget_set",  730,  5,     6, "target" },
    { "GRPcomn_set",    831,  7,     7, "comn"   },
    { "GRPending",      973,  8,     8, "ending" },
    { "GRPstream",      985, -1,     0, "allse"  },
    { "GRPend",        1060, -2,     0, "allse"  },
};

const struct SoundDesc g_soundDesc[] =
{
    {   3, "GRPse04",                               1,  0,  -1 },
    { 209, "SND_SEB_SE_COIN_SEPCM_2",               7,  0,   0 },
    { 210, "SND_SEB_SE_COIN_SEPCM_3",               7,  0,   1 },
    { 205, "SND_SEB_SE_START_SEPCM",                7,  0,   2 },
    {   0, "GRPse01",                               1,  0,  -1 },
    {  -1, "SND_SNGA_SNG_BNK0_FX_TEST",             0,  0,  -1 },
    { 174, "SND_SEB_SE_BALL_SEPCM",                 5,  8,  53 },
    {  -1, "DMY_CODE",                              7,  0, 101 },
    { 175, "SND_SEB_SE_BALL_SEPCM_1",               6,  0, 105 },
    {  -1, "DMY_CODE",                              0,  0,  50 },
    {  -1, "DMY_CODE",                              7,  0, 107 },
    { 176, "SND_SEB_SE_BALL_SEPCM_2",               3,  8,  47 },
    { 177, "SND_SEB_SE_BALL_SEPCM_3",               5,  8,  54 },
    {  -1, "DMY_CODE",                              7,  0, 102 },
    { 178, "SND_SEB_SE_BALL_SEPCM_4",               3,  8,  19 },
    { 179, "SND_SEB_SE_BALL_SEPCM_5",               0,  0, 103 },
    {  -1, "DMY_CODE",                              0,  0,  46 },
    { 180, "SND_SEB_SE_BALL_SEPCM_6",               0,  0,  64 },
    { 181, "SND_SEB_SE_BALL_SEPCM_7",               5,  8,  55 },
    { 184, "SND_SEB_SE_BALL_SEPCM_8",               3,  8,  18 },
    { 185, "SND_SEB_SE_BALL_SEPCM_9",               3,  8,  20 },
    { 186, "SND_SEB_SE_BALL_SEPCM_10",              0,  0,  -1 },
    { 197, "SND_SEB_SE_BALL_SEPCM_18",              0,  0,  -1 },
    { 198, "SND_SEB_SE_BALL_SEPCM_19",              0,  0,  -1 },
    { 199, "SND_SEB_SE_BALL_SEPCM_20",              0,  0,  -1 },
    { 200, "SND_SEB_SE_BALL_SEPCM_21",              0,  0,  -1 },
    { 201, "SND_SEB_SE_BALL_SEPCM_22",              0,  0,  -1 },
    { 202, "SND_SEB_SE_BALL_SEPCM_23",              5,  8,  56 },
    { 204, "SND_SEB_SE_BALL_SEPCM_25",              0,  0,  -1 },
    { 182, "SND_SEB_SE_BALL_SEPCM_11",              0,  0,  -1 },
    { 183, "SND_SEB_SE_BALL_SEPCM_12",              0,  0,  -1 },
    { 187, "SND_SEB_SE_BALL_SEPCM_13",              0,  0,  -1 },
    { 188, "SND_SEB_SE_BALL_SEPCM_14",              0,  0,  -1 },
    { 189, "SND_SEB_SE_BALL_SEPCM_15",              0,  0,  -1 },
    { 190, "SND_SEB_SE_BALL_SEPCM_16",              0,  0,  -1 },
    { 191, "SND_SEB_SE_BALL_SEPCM_17",              0,  0,  -1 },
    { 194, "SND_SEB_SE_BALL_SEPCM_26",              0,  0,  -1 },
    { 195, "SND_SEB_SE_BALL_SEPCM_27",              0,  0,  -1 },
    { 196, "SND_SEB_SE_BALL_SEPCM_28",              0,  0,  -1 },
    { 192, "SND_SEB_SE_BALL_SEPCM_29",              0,  0,  -1 },
    { 193, "SND_SEB_SE_BALL_SEPCM_30",              0,  0,  -1 },
    { 203, "SND_SEB_SE_BALL_SEPCM_31",              0,  0,  -1 },
    { 208, "SND_SEB_SE_ETC_SEPCM",                  0,  0,  -1 },
    { 209, "SND_SEB_SE_ETC_SEPCM_2",                7,  0,  -1 },
    { 210, "SND_SEB_SE_ETC_SEPCM_3",                7,  0,  -1 },
    { 211, "SND_SEB_SE_ETC_SEPCM_5",               10,  0,  22 },
    {  -1, "DMY_CODE",                             10,  0, 104 },
    {  -1, "DMY_CODE",                              7,  0, 365 },
    { 213, "SND_SEB_SE_ETC_SEPCM_6",                0,  0,  -1 },
    { 214, "SND_SEB_SE_ETC_SEPCM_7",                0,  0,  15 },
    {  -1, "DMY_CODE",                              7,  0, 108 },
    {  -1, "DMY_CODE",                              7,  0, 361 },
    { 215, "SND_SEB_SE_ETC_SEPCM_8",                0,  0,  -1 },
    { 216, "SND_SEB_SE_ETC_SEPCM_9",                0,  0,  -1 },
    { 217, "SND_SEB_SE_ETC_SEPCM_10",               7,  0,   9 },
    {  -1, "DMY_CODE",                              7,  0, 111 },
    { 218, "SND_SEB_SE_ETC_SEPCM_11",               7,  0,  10 },
    { 226, "SND_SEB_SE_ETC_SEPCM_12",               0,  0,  -1 },
    { 227, "SND_SEB_SE_ETC_SEPCM_22",               0,  0,  -1 },
    { 228, "SND_SEB_SE_ETC_SEPCM_14",               9,  0,  -1 },
    {  -1, "SND_SEB_SE_ETC_SEPCM_OFF",              0,  0,  -1 },
    { 229, "SND_SEB_SE_ETC_SEPCM_15",               0,  0,  77 },
    { 239, "SND_SEB_SE_ETC_SEPCM_16",               0,  0,  78 },
    { 240, "SND_SEB_SE_ETC_SEPCM_17",               0,  0,  -1 },
    { 241, "SND_SEB_SE_ETC_SEPCM_18",               9,  0,  -1 },
    {  -1, "SND_SEB_SE_ETC_SEPCM_OFF_1",            0,  0,  -1 },
    { 242, "SND_SEB_SE_ETC_SEPCM_19",               9,  0,  -1 },
    {  -1, "SND_SEB_SE_ETC_SEPCM_OFF_2",            0,  0,  -1 },
    { 243, "SND_SEB_SE_ETC_SEPCM_20",               7,  0,  48 },
    {  -1, "DMY_CODE",                              7,  0, 112 },
    {  -1, "DMY_CODE",                              7,  0, 209 },
    { 244, "SND_SEB_SE_ETC_SEPCM_21",               0,  0,  16 },
    {  -1, "DMY_CODE",                              7,  0, 106 },
    {  -1, "DMY_CODE",                              7,  0, 110 },
    { 212, "SND_SEB_SE_ETC_SEPCM_24",               0,  0,  52 },
    {  -1, "DMY_CODE",                              0,  0,  68 },
    { 205, "SND_SEB_SE_JGL_SEPCM_2",                7,  0,  -1 },
    { 206, "SND_SEB_SE_JGL_SEPCM_3",               10,  2,   3 },
    {  -1, "DMY_CODE",                              7,  0, 354 },
    { 207, "SND_SEB_SE_JGL_SE034UP_1",             10,  0,  57 },
    { 271, "SND_SEB_SE_JGL_JGL01A_AIF_1",          13,  0, 285 },
    { 248, "SND_SE01_RUN01",                        4,  8,  45 },
    { 273, "SND_SE01_RUN02",                        4,  8,  -1 },
    { 274, "SND_SE01_RUN03",                        4,  8,  -1 },
    {  -1, "SND_SE01_FORPOP01_FPD",                 0,  0,  -1 },
    {   1, "GRPse02",                               1,  0,  -1 },
    { 260, "SND_SEB_SE_ETC_0_SEPCM_25",             0,  0,  -1 },
    { 219, "SND_SEB_SE_ETC_0_SEPCM_27",             0,  0,  -1 },
    { 220, "SND_SEB_SE_ETC_0_SEPCM_31",             0,  0,  -1 },
    { 221, "SND_SEB_SE_ETC_0_SEPCM_32",             0,  0,  -1 },
    { 222, "SND_SEB_SE_ETC_0_SEPCM_33",             0,  0,  -1 },
    { 223, "SND_SEB_SE_ETC_0_SEPCM_34",             0,  0,  66 },
    { 224, "SND_SEB_SE_ETC_0_SEPCM_35",             0,  0,  65 },
    { 225, "SND_SEB_SE_ETC_0_SEPCM_36",             0,  0,  67 },
    { 230, "SND_SEB_SE_ETC_0_SEPCM_37",             9,  0,  -1 },
    {  -1, "SND_SEB_SE_ETC_0_SEPCM_OFF_3",          0,  0,  -1 },
    { 231, "SND_SEB_SE_ETC_0_SEPCM_38",             9,  0,  -1 },
    {  -1, "SND_SEB_SE_ETC_0_SEPCM_OFF_4",          0,  0,  -1 },
    { 232, "SND_SEB_SE_ETC_0_SEPCM_39",             0,  0,  -1 },
    { 233, "SND_SEB_SE_ETC_0_SEPCM_40",             0,  0,  -1 },
    { 234, "SND_SEB_SE_ETC_0_SEPCM_41",             0,  0,  -1 },
    { 235, "SND_SEB_SE_ETC_0_SEPCM_42",             0,  0,  -1 },
    { 236, "SND_SEB_SE_ETC_0_SEPCM_43",             0,  0,  -1 },
    { 261, "SND_SEB_SE_ETC_0_SEPCM_44",             0,  0,  -1 },
    { 262, "SND_SEB_SE_ETC_0_SEPCM_45",             0,  0,  -1 },
    { 263, "SND_SEB_SE_ETC_0_SEPCM_46",             0,  0,  -1 },
    { 264, "SND_SEB_SE_ETC_0_SEPCM_47",             0,  0,  -1 },
    { 265, "SND_SEB_SE_ETC_0_SEPCM_48",             0,  0,  -1 },
    { 266, "SND_SEB_SE_ETC_0_SEPCM_49",             0,  0,  -1 },
    { 267, "SND_SEB_SE_ETC_0_SEPCM_50",             0,  0,  -1 },
    { 268, "SND_SEB_SE_ETC_0_SEPCM_51",             0,  0,  -1 },
    { 269, "SND_SEB_SE_ETC_0_SEPCM_52",            11,  0,  61 },
    { 270, "SND_SEB_SE_ETC_0_SEPCM_56",             0,  0,  60 },
    { 237, "SND_SEB_SE_ETC_0_SEPCM_13",             0,  0,  -1 },
    {   2, "GRPse03",                               1,  0,  -1 },
    { 257, "SND_SEB_RACKA_31_01_2",                 0,  0,  -1 },
    { 258, "SND_SEB_RACKA_C_0003_WAV_2",            0,  0,  -1 },
    { 259, "SND_SEB_RACKA_C_0004_WAV",              0,  0,  -1 },
    { 247, "SND_SEB_YUKA_SEPCM_1_1",                0,  0,  99 },
    { 246, "SND_SEB_YUKA_SEPCM_3_2",                0,  0, 100 },
    { 245, "SND_SEB_YUKA_SEPCM_5_3",                0,  0,  98 },
    { 161, "SFXse162",                             13,  0, 498 },
    { 162, "SFXse163",                             13,  0, 499 },
    { 187, "SFXse188",                              0,  0, 500 },
    { 188, "SFXse189",                              0,  0, 511 },
    { 160, "SFXse161",                              0,  0,  -1 },
    {   7, "GRPnar",                                1,  0,  -1 },
    { 251, "SND_SED_NAR_SEGA",                      8,  0,  33 },
    { 256, "SND_SED_NAR_TITTLE1",                   7,  0,  34 },
    { 272, "SND_SED_NAR_TITTLE2",                   7,  0,  -1 },
    {   0, "SND_SED_NAR_0",                         8,  0,  35 },
    {   1, "SND_SED_NAR_1",                         8,  0,  36 },
    {   2, "SND_SED_NAR_2",                         8,  0,  37 },
    {   3, "SND_SED_NAR_3",                         8,  0,  38 },
    {   4, "SND_SED_NAR_4",                         8,  0,  39 },
    {   5, "SND_SED_NAR_5",                         8,  0,  40 },
    {   6, "SND_SED_NAR_6",                         8,  0,  41 },
    {   7, "SND_SED_NAR_7",                         8,  0,  42 },
    {   8, "SND_SED_NAR_8",                         8,  0,  43 },
    {   9, "SND_SED_NAR_9",                         8,  0,  44 },
    { 170, "SND_SED_NAR_READY",                     8,  0,   4 },
    { 158, "SND_SED_NAR_GO",                        8,  0,   5 },
    { 159, "SND_SED_NAR_GO2",                       8,  0,  -1 },
    { 172, "SND_SED_NAR_RINGOUT",                   8,  0,  -1 },
    { 163, "SND_SED_NAR_HURRY",                     8,  0,   7 },
    { 100, "SND_SED_NAR_CLEAR",                     8,  0,  -1 },
    { 255, "SND_SED_NAR_TIMEOVER",                  8,  0,  11 },
    { 109, "SND_SED_NAR_GAMEOVER",                  7,  0,  13 },
    {  52, "SND_SED_NAR_BONUSFIN",                  8,  0,  69 },
    { 101, "SND_SED_NAR_CLEAR_FLOOR",               8,  0,  -1 },
    { 102, "SND_SED_NAR_CLEAR_GAME",                8,  0,  -1 },
    { 103, "SND_SED_NAR_CONGLA",                    8,  0,  70 },
    { 104, "SND_SED_NAR_CONTINUE",                  7,  0,  71 },
    { 108, "SND_SED_NAR_FALLOUT",                   8,  0,  12 },
    { 160, "SND_SED_NAR_GOAL",                      8,  0,   8 },
    {  -1, "DMY_CODE",                             14,  0, 366 },
    { 166, "SND_SED_NAR_PERFECT",                   8,  0,  72 },
    { 167, "SND_SED_NAR_PRESS",                     8,  0,  73 },
    { 171, "SND_SED_NAR_READY2",                    8,  0,  -1 },
    { 252, "SND_SED_NAR_SEL_LV",                    8,  0,  74 },
    { 253, "SND_SED_NAR_SEL_MK",                    8,  0,  75 },
    { 254, "SND_SED_NAR_THANKYOU",                  8,  0,  -1 },
    { 105, "SND_SED_NAR_FALL00",                   10,  0,  21 },
    { 106, "SND_SED_NAR_FALL01",                    0,  0,  -1 },
    { 107, "SND_SED_NAR_FALL02",                    0,  0,  -1 },
    { 164, "SND_SED_NAR_N4_ES",                     0,  0,   6 },
    { 165, "SND_SED_NAR_OP_TRN",                    2,  8,  17 },
    { 173, "SND_SED_NAR_RST5",                      0,  0,  -1 },
    { 161, "SND_SED_NAR_HAMMER1",                   0,  0,  -1 },
    { 162, "SND_SED_NAR_HAMMER2",                   0,  0,  -1 },
    { 168, "SND_SED_NAR_RAIN1",                     9,  0,  -1 },
    {  -1, "SND_SED_NAR_RAIN1_OFF",                 0,  0,  -1 },
    { 169, "SND_SED_NAR_RAIN2",                     9,  0,  76 },
    {  -1, "SND_SED_NAR_RAIN2_OFF",                 0,  0,  -1 },
    {  95, "SND_SED_NAR_BOYM_START2",              15,  0,  49 },
    {  96, "SND_SED_NAR_BOYM_START3",              15,  0,  79 },
    { 147, "SND_SED_NAR_GIRLM_GOAL3",              15,  0,  79 },
    {  50, "SND_SED_NAR_BABYM_START3",             15,  0,  79 },
    {   4, "GRPboy",                                1,  0,  -1 },
    {  94, "SND_SED_VO1_BOYM_START1",              17,  0,  31 },
    {  95, "SND_SED_VO1_BOYM_START2",              15,  0,  -1 },
    {  96, "SND_SED_VO1_BOYM_START3",              15,  0,  -1 },
    {  -1, "DMY_CODE",                             19,  0,  80 },
    {  97, "SND_SED_VO1_BOYM_START4",              15,  0,  -1 },
    {  98, "SND_SED_VO1_BOYM_TIMEOVER1",           15,  0,  62 },
    {  -1, "DMY_CODE",                             19,  0, 556 },
    {  99, "SND_SED_VO1_BOYM_TIMEOVER2",           15,  0,  28 },
    {  -1, "DMY_CODE",                             19,  0, 300 },
    {  89, "SND_SED_VO1_BOYM_OCHISOU1",            15,  0,  -1 },
    {  90, "SND_SED_VO1_BOYM_OCHISOU2",            15,  0,  -1 },
    {  91, "SND_SED_VO1_BOYM_OCHISOU3",            15,  0,  -1 },
    {  70, "SND_SED_VO1_BOYM_BGET",                15,  0,  -1 },
    {  68, "SND_SED_VO1_BOYM_BBGET1",              15,  0,  -1 },
    {  69, "SND_SED_VO1_BOYM_BBGET2",              18,  0,  32 },
    {  72, "SND_SED_VO1_BOYM_COLI1",               16,  8,  -1 },
    {  73, "SND_SED_VO1_BOYM_COLI2",               16,  8,  -1 },
    {  74, "SND_SED_VO1_BOYM_COLI3",               16,  8,  24 },
    {  75, "SND_SED_VO1_BOYM_COLI4",               16,  8,  25 },
    {  76, "SND_SED_VO1_BOYM_COLI5",               16,  8,  23 },
    {  77, "SND_SED_VO1_BOYM_COLI6",               16,  8,  -1 },
    {  78, "SND_SED_VO1_BOYM_COLI7",               16,  8,  26 },
    {  92, "SND_SED_VO1_BOYM_QUEST1",              15,  0,  -1 },
    {  67, "SND_SED_VO1_BOYM_ANGLY",               15,  0,  -1 },
    {  88, "SND_SED_VO1_BOYM_LAUGH",               15,  0,  -1 },
    {  79, "SND_SED_VO1_BOYM_CRY",                 15,  0, 109 },
    {  84, "SND_SED_VO1_BOYM_GOAL1",               15,  0,  -1 },
    {  85, "SND_SED_VO1_BOYM_GOAL2",               15,  0,  -1 },
    {  86, "SND_SED_VO1_BOYM_GOAL3",               15,  0,  -1 },
    {  80, "SND_SED_VO1_BOYM_FALL1",               15,  0,  -1 },
    {  81, "SND_SED_VO1_BOYM_FALL2",               15,  0,  -1 },
    {  82, "SND_SED_VO1_BOYM_FALL3",               15,  0,  -1 },
    {  83, "SND_SED_VO1_BOYM_FALL4",               15,  0,  -1 },
    {  87, "SND_SED_VO1_BOYM_HELLO",               15,  0,  -1 },
    {  71, "SND_SED_VO1_BOYM_BYE",                 15,  0,  -1 },
    {  93, "SND_SED_VO1_BOYM_SEEYOU",              15,  0,  -1 },
    {  66, "SND_SED_VO1_BOYH_START1",              15,  0,  30 },
    {  59, "SND_SED_VO1_BOYH_OCHISOU1",            16,  8,  58 },
    {  -1, "DMY_CODE",                             16,  7,  87 },
    {  -1, "DMY_CODE",                             16,  7,  90 },
    {  -1, "DMY_CODE",                             16,  7,  97 },
    {  60, "SND_SED_VO1_BOYH_OCHISOU2",            16,  8,  59 },
    {  -1, "DMY_CODE",                             16,  7,  91 },
    {  61, "SND_SED_VO1_BOYH_OCHISOU3",            16,  8,  63 },
    {  -1, "DMY_CODE",                             16,  7,  92 },
    {  62, "SND_SED_VO1_BOYH_OCHISOU5",            16,  8,  83 },
    {  -1, "DMY_CODE",                             16,  7,  93 },
    {  63, "SND_SED_VO1_BOYH_OCHISOU7",            16,  8,  84 },
    {  -1, "DMY_CODE",                             16,  7,  94 },
    {  64, "SND_SED_VO1_BOYH_OCHISOU8",            16,  8,  85 },
    {  -1, "DMY_CODE",                             16,  7,  95 },
    {  65, "SND_SED_VO1_BOYH_OCHISOU9",            16,  8,  86 },
    {  -1, "DMY_CODE",                             16,  7,  96 },
    {  58, "SND_SED_VO1_BOYH_LAUGH2",              15,  0,  82 },
    {  -1, "DMY_CODE",                             19,  0, 301 },
    {  55, "SND_SED_VO1_BOYH_GOAL1",               15,  0,  89 },
    {  -1, "DMY_CODE",                             19,  0, 304 },
    {  56, "SND_SED_VO1_BOYH_GOAL2",               15,  0,  88 },
    {  -1, "DMY_CODE",                             19,  0, 303 },
    {  57, "SND_SED_VO1_BOYH_GOAL3",               15,  0,  27 },
    {  -1, "DMY_CODE",                             19,  0, 302 },
    {  53, "SND_SED_VO1_BOYH_FALL1",               15,  0,  -1 },
    {  54, "SND_SED_VO1_BOYH_FALL1L",              15,  0,  81 },
    {  -1, "DMY_CODE",                             15,  0,  29 },
    {   6, "GRPgirl",                               1,  0,  -1 },
    { 156, "SND_SED_VO2_GIRLM_START",              15,  0,  -1 },
    { 157, "SND_SED_VO2_GIRLM_TIMEOVER",           15,  0,  62 },
    {  -1, "DMY_CODE",                             19,  1, 556 },
    { 150, "SND_SED_VO2_GIRLM_OCHISOU1",           15,  0,  -1 },
    { 151, "SND_SED_VO2_GIRLM_OCHISOU2",           15,  0,  -1 },
    { 152, "SND_SED_VO2_GIRLM_OCHISOU3",           15,  0,  -1 },
    { 124, "SND_SED_VO2_GIRLM_BGET1",              15,  0,  -1 },
    { 125, "SND_SED_VO2_GIRLM_BGET2",              15,  0,  -1 },
    { 126, "SND_SED_VO2_GIRLM_BGET3",              17,  0,  31 },
    { 127, "SND_SED_VO2_GIRLM_BGET4",              15,  0,  -1 },
    { 128, "SND_SED_VO2_GIRLM_BGET5",              15,  0,  -1 },
    { 120, "SND_SED_VO2_GIRLM_BBGET1",             15,  0,  82 },
    {  -1, "DMY_CODE",                             19,  1, 301 },
    { 121, "SND_SED_VO2_GIRLM_BBGET2",             18,  0,  32 },
    { 122, "SND_SED_VO2_GIRLM_BBGET3",             15,  0,  88 },
    {  -1, "DMY_CODE",                             19,  1, 303 },
    { 123, "SND_SED_VO2_GIRLM_BBGET4",             15,  0,  -1 },
    { 131, "SND_SED_VO2_GIRLM_COLI1",              16,  8,  23 },
    { 132, "SND_SED_VO2_GIRLM_COLI2",              16,  8,  -1 },
    { 133, "SND_SED_VO2_GIRLM_COLI3",              16,  8,  -1 },
    { 134, "SND_SED_VO2_GIRLM_COLI4",              16,  8,  26 },
    { 135, "SND_SED_VO2_GIRLM_COLI5",              16,  8,  -1 },
    { 136, "SND_SED_VO2_GIRLM_COLI6",              16,  8,  24 },
    { 137, "SND_SED_VO2_GIRLM_COLI7",              16,  8,  25 },
    { 138, "SND_SED_VO2_GIRLM_COLI8",              16,  8,  -1 },
    { 139, "SND_SED_VO2_GIRLM_COLI9",              16,  8,  -1 },
    { 153, "SND_SED_VO2_GIRLM_QUEST1",             15,  0,  -1 },
    { 154, "SND_SED_VO2_GIRLM_QUEST2",             15,  0,  -1 },
    { 118, "SND_SED_VO2_GIRLM_ANGLY1",             15,  0,  -1 },
    { 119, "SND_SED_VO2_GIRLM_ANGLY2",             15,  0,  -1 },
    { 140, "SND_SED_VO2_GIRLM_CRY1",               15,  0,  28 },
    {  -1, "DMY_CODE",                             15,  1, 109 },
    {  -1, "DMY_CODE",                             19,  1, 300 },
    { 145, "SND_SED_VO2_GIRLM_GOAL1",              15,  0,  89 },
    {  -1, "DMY_CODE",                             19,  1, 304 },
    { 146, "SND_SED_VO2_GIRLM_GOAL2",              15,  0,  -1 },
    { 147, "SND_SED_VO2_GIRLM_GOAL3",              19,  0,  80 },
    {  -1, "DMY_CODE",                             15,  1,  27 },
    {  -1, "DMY_CODE",                             19,  1, 302 },
    { 148, "SND_SED_VO2_GIRLM_GOAL4",              15,  0,  -1 },
    { 141, "SND_SED_VO2_GIRLM_FALL1",              15,  0,  -1 },
    { 144, "SND_SED_VO2_GIRLM_FALL2",              15,  0,  -1 },
    { 149, "SND_SED_VO2_GIRLM_HELLO",              15,  0,  -1 },
    { 129, "SND_SED_VO2_GIRLM_BYE1",               15,  0,  -1 },
    { 130, "SND_SED_VO2_GIRLM_BYE2",               15,  0,  -1 },
    { 155, "SND_SED_VO2_GIRLM_SEEYOU",             15,  0,  -1 },
    { 117, "SND_SED_VO2_GIRLH_START1",             15,  0,  30 },
    { 112, "SND_SED_VO2_GIRLH_OCHISOU1",           16,  8,  58 },
    {  -1, "DMY_CODE",                             15,  1,  86 },
    {  -1, "DMY_CODE",                             16,  8,  90 },
    {  -1, "DMY_CODE",                             16,  8,  94 },
    { 113, "SND_SED_VO2_GIRLH_OCHISOU2",           16,  8,  59 },
    {  -1, "DMY_CODE",                             15,  1,  87 },
    {  -1, "DMY_CODE",                             16,  8,  91 },
    {  -1, "DMY_CODE",                             16,  8,  95 },
    { 114, "SND_SED_VO2_GIRLH_OCHISOU3",           16,  8,  63 },
    {  -1, "DMY_CODE",                             15,  1,  85 },
    {  -1, "DMY_CODE",                             16,  8,  92 },
    {  -1, "DMY_CODE",                             16,  8,  96 },
    { 115, "SND_SED_VO2_GIRLH_OCHISOU5",           16,  8,  84 },
    {  -1, "DMY_CODE",                             16,  8,  93 },
    {  -1, "DMY_CODE",                             16,  8,  97 },
    { 116, "SND_SED_VO2_GIRLH_OCHISOU8",           15,  0,  83 },
    { 110, "SND_SED_VO2_GIRLH_FALL2",              15,  0,  -1 },
    { 111, "SND_SED_VO2_GIRLH_FALL2L",             15,  0,  81 },
    {  -1, "DMY_CODE",                             15,  1,  29 },
    {   5, "GRPbaby",                               1,  0,  -1 },
    {  49, "SND_SED_VO3_BABYM_START2",             15,  0,  -1 },
    {  50, "SND_SED_VO3_BABYM_START3",             19,  0,  80 },
    {  51, "SND_SED_VO3_BABYM_TIMEOVER",           15,  0,  -1 },
    {  44, "SND_SED_VO3_BABYM_OCHISOU1",           15,  0,  -1 },
    {  45, "SND_SED_VO3_BABYM_OCHISOU2",           15,  0,  -1 },
    {  46, "SND_SED_VO3_BABYM_OCHISOU3",           15,  0,  -1 },
    {  27, "SND_SED_VO3_BABYM_BGET1",              17,  0,  31 },
    {  28, "SND_SED_VO3_BABYM_BGET2",              15,  0,  -1 },
    {  29, "SND_SED_VO3_BABYM_BGET3",              15,  0,  -1 },
    {  24, "SND_SED_VO3_BABYM_BBGET1",             15,  0,  -1 },
    {  25, "SND_SED_VO3_BABYM_BBGET2",             18,  0,  32 },
    {  26, "SND_SED_VO3_BABYM_BBGET3",             15,  0,  82 },
    {  -1, "DMY_CODE",                             19,  2, 301 },
    {  30, "SND_SED_VO3_BABYM_COLI1",              16,  8,  -1 },
    {  31, "SND_SED_VO3_BABYM_COLI2",              16,  8,  24 },
    {  32, "SND_SED_VO3_BABYM_COLI4",              16,  8,  25 },
    {  33, "SND_SED_VO3_BABYM_COLI5",              16,  8,  -1 },
    {  34, "SND_SED_VO3_BABYM_COLI6",              16,  8,  -1 },
    {  47, "SND_SED_VO3_BABYM_QUEST1",             15,  0,  -1 },
    {  48, "SND_SED_VO3_BABYM_QUEST2",             15,  0,  -1 },
    {  23, "SND_SED_VO3_BABYM_ANGLY",              15,  0,  -1 },
    {  43, "SND_SED_VO3_BABYM_LAUGH",              15,  0,  -1 },
    {  35, "SND_SED_VO3_BABYM_CRY1",               15,  0, 109 },
    {  36, "SND_SED_VO3_BABYM_CRY2",               15,  0,  28 },
    {  -1, "DMY_CODE",                             19,  2, 300 },
    {  41, "SND_SED_VO3_BABYM_GOAL1",              15,  0,  27 },
    {  -1, "DMY_CODE",                             19,  2, 302 },
    {  42, "SND_SED_VO3_BABYM_GOAL2",              15,  0,  88 },
    {  -1, "DMY_CODE",                             19,  2, 303 },
    {  37, "SND_SED_VO3_BABYM_FALL1",              15,  0,  -1 },
    {  38, "SND_SED_VO3_BABYM_FALL2",              15,  0,  -1 },
    {  39, "SND_SED_VO3_BABYM_FALL3",              15,  0,  -1 },
    {  40, "SND_SED_VO3_BABYM_FALL4",              15,  0,  -1 },
    {  21, "SND_SED_VO3_BABYH_START1",             15,  0,  30 },
    {  22, "SND_SED_VO3_BABYH_TIMEOVER3",          15,  0,  62 },
    {  -1, "DMY_CODE",                             19,  2, 556 },
    {  15, "SND_SED_VO3_BABYH_OCHISOU1",           16,  8,  58 },
    {  -1, "DMY_CODE",                             16,  9,  90 },
    {  -1, "DMY_CODE",                             16,  9,  97 },
    {  16, "SND_SED_VO3_BABYH_OCHISOU2",           16,  8,  59 },
    {  -1, "DMY_CODE",                             15,  2,  87 },
    {  -1, "DMY_CODE",                             16,  9,  91 },
    {  17, "SND_SED_VO3_BABYH_OCHISOU3",           16,  8,  63 },
    {  -1, "DMY_CODE",                             15,  2,  86 },
    {  -1, "DMY_CODE",                             16,  9,  92 },
    {  18, "SND_SED_VO3_BABYH_OCHISOU4",           16,  8,  83 },
    {  -1, "DMY_CODE",                             16,  9,  93 },
    {  -1, "DMY_CODE",                             16,  9,  94 },
    {  19, "SND_SED_VO3_BABYH_OCHISOU6",           16,  8,  84 },
    {  -1, "DMY_CODE",                             16,  9,  95 },
    {  20, "SND_SED_VO3_BABYH_OCHISOU7",           16,  8,  85 },
    {  -1, "DMY_CODE",                             16,  9,  96 },
    {  10, "SND_SED_VO3_BABYH_BGET4",              16,  8,  23 },
    {  11, "SND_SED_VO3_BABYH_COLI4",              16,  8,  26 },
    {  14, "SND_SED_VO3_BABYH_LAUGH1",             15,  0,  89 },
    {  -1, "DMY_CODE",                             19,  2, 304 },
    {  12, "SND_SED_VO3_BABYH_FALL1",              15,  0,  -1 },
    {  13, "SND_SED_VO3_BABYH_FALL1L",             15,  0,  81 },
    {  -1, "DMY_CODE",                             15,  2,  29 },
    {   8, "GRPgoli",                               1,  0,  -1 },
    { 275, "SFXgoli_1up_basic_macro",              15,  0,  82 },
    {  -1, "DMY_CODE",                             19,  3, 301 },
    { 276, "SFXgoli_bumper1_basic_macro",          16,  8,  90 },
    {  -1, "DMY_CODE",                             16, 10,  96 },
    {  -1, "DMY_CODE",                             16, 10,  58 },
    { 277, "SFXgoli_bumper2_basic_macro",          16,  8,  91 },
    {  -1, "DMY_CODE",                             16, 10,  97 },
    {  -1, "DMY_CODE",                             16, 10,  59 },
    { 278, "SFXgoli_bumper3_basic_macro",          16,  8,  92 },
    {  -1, "DMY_CODE",                             16, 10,  63 },
    {  -1, "DMY_CODE",                             16, 10,  86 },
    { 279, "SFXgoli_bumper4_basic_macro",          16,  8,  93 },
    {  -1, "DMY_CODE",                             16, 10,  83 },
    {  -1, "DMY_CODE",                             16, 10,  87 },
    { 280, "SFXgoli_bumper5_basic_macro",          16,  8,  94 },
    {  -1, "DMY_CODE",                             16, 10,  84 },
    { 281, "SFXgoli_bumper6_basic_macro",          16,  8,  95 },
    {  -1, "DMY_CODE",                             16, 10,  85 },
    { 282, "SFXgoli_conte_basic_macro",            19,  0,  80 },
    { 283, "SFXgoli_fall1_basic_macro",            15,  0,  29 },
    { 284, "SFXgoli_fall2_basic_macro",            15,  0,  81 },
    { 285, "SFXgoli_gameover_basic_macro",         15,  0,  62 },
    {  -1, "DMY_CODE",                             19,  3, 556 },
    { 286, "SFXgoli_get1_basic_macro",             17,  0,  31 },
    { 287, "SFXgoli_goal_basic_macro",             15,  0,  27 },
    {  -1, "DMY_CODE",                             19,  3, 302 },
    { 288, "SFXgoli_goal2_basic_macro",            15,  0,  88 },
    {  -1, "DMY_CODE",                             19,  3, 303 },
    { 289, "SFXgoli_goal3_basic_macro",            15,  0,  89 },
    {  -1, "DMY_CODE",                             19,  3, 304 },
    { 290, "SFXgoli_goal4_basic_macro",            15,  0,  -1 },
    { 291, "SFXgoli_husaget1_basic_macro",         18,  0,  32 },
    { 292, "SFXgoli_select_basic_macro",           15,  0,  79 },
    { 293, "SFXgoli_shock1_basic_macro",           16,  8,  23 },
    { 294, "SFXgoli_shock2_basic_macro",           16,  8,  24 },
    { 295, "SFXgoli_shock3_basic_macro",           16,  8,  25 },
    { 296, "SFXgoli_shock4_basic_macro",           16,  8,  26 },
    { 297, "SFXgoli_start_basic_macro",            15,  0,  30 },
    { 298, "SFXgoli_timeover_basic_macro",         15,  0,  28 },
    {  -1, "DMY_CODE",                             19,  3, 300 },
    {   1, "GRPbil_set",                            1,  0,  -1 },
    { 299, "SFXcushion01_basic_macro",              0,  0, 113 },
    { 300, "SFXcushion02_basic_macro",              0,  0, 114 },
    { 301, "SFXcushion03_basic_macro",              0,  0, 115 },
    { 302, "SFXhit01_basic_macro",                  0,  0, 116 },
    { 303, "SFXhit02_basic_macro",                  0,  0, 117 },
    { 304, "SFXhit03_basic_macro",                  0,  0, 118 },
    { 305, "SFXhit04_basic_macro",                  0,  0, 119 },
    { 306, "SFXhit05_basic_macro",                  0,  0, 120 },
    { 307, "SFXhit06_basic_macro",                  0,  0, 121 },
    { 308, "SFXhit07_basic_macro",                  0,  0, 122 },
    { 309, "SFXhit08_basic_macro",                  0,  0, 123 },
    { 310, "SFXhit09_basic_macro",                  0,  0, 124 },
    { 311, "SFXhit10_basic_macro",                  0,  0, 125 },
    { 312, "SFXhit11_basic_macro",                  0,  0, 126 },
    { 313, "SFXpoket01_basic_macro",                0,  0, 127 },
    { 314, "SFXpoket02_basic_macro",                0,  0, 128 },
    { 315, "SFXpoket03_basic_macro",                0,  0, 129 },
    { 316, "SFXshot01_basic_macro",                 0,  0, 130 },
    { 317, "SFXshot02_basic_macro",                 0,  0, 131 },
    { 318, "SFXshot03_basic_macro",                 0,  0, 132 },
    { 319, "SFXshot04_basic_macro",                 0,  0, 133 },
    { 320, "SFXbillose_basic_macro",               13,  0, 134 },
    { 321, "SFXbilwin_basic_macro",                13,  0, 135 },
    { 322, "SFXapplaudb_basic_macro",               0,  0, 136 },
    { 323, "SFXbilstr_basic_macro",                13,  0, 137 },
    { 324, "SFXtoogoodb_basic_macro",               0,  0, 138 },
    { 325, "SFXcushion04_basic_macro",              0,  0, 139 },
    { 326, "SFXcushion05_basic_macro",              0,  0, 140 },
    { 327, "SFXshot_moresoft_basic_macro",          0,  0, 141 },
    { 328, "SFXshot_soft_basic_macro",              0,  0, 142 },
    { 329, "SFXapplaudbbig_basic_macro",            0,  0, 333 },
    { 330, "SFXapplaudbnocl_basic_macro",           0,  0, 334 },
    { 331, "SFXapplaudbnol_basic_macro",            0,  0, 335 },
    { 332, "SFXbill_foul_l_basic_macro",           13,  0, 336 },
    { 333, "SFXbill_foul_r_basic_macro",           13,  0, 337 },
    { 334, "SFXbill_lose_l_basic_macro",           13,  0, 338 },
    { 335, "SFXbill_lose_r_basic_macro",           13,  0, 339 },
    { 336, "SFXbill_senkou_l_basic_macro",         13,  0, 340 },
    { 337, "SFXbill_senkou_r_basic_macro",         13,  0, 341 },
    { 338, "SFXbill_win_l_basic_macro",            13,  0, 342 },
    { 339, "SFXbill_win_r_basic_macro",            13,  0, 343 },
    { 340, "SFXboob_basic_macro",                   0,  0, 344 },
    { 341, "SFXcushion06_basic_macro",              0,  0, 345 },
    { 342, "SFXshotshift_basic_macro",              0,  0, 346 },
    { 343, "SFX1st_match_basic_macro",              8,  0, 367 },
    { 344, "SFX2nd_match_basic_macro",              8,  0, 368 },
    { 345, "SFX3rd_match_basic_macro",              8,  0, 369 },
    { 346, "SFXbreak_ace_01_basic_macro",           8,  0, 370 },
    { 347, "SFXbreak_ace_02_basic_macro",           8,  0, 371 },
    { 348, "SFXbreak_ace_03_basic_macro",           8,  0, 372 },
    { 349, "SFXbreak_shot_basic_macro",             8,  0, 373 },
    { 350, "SFXcanon_shot_basic_macro",             8,  0, 374 },
    { 351, "SFXcombination_shot_basic_macro",       8,  0, 375 },
    { 352, "SFXfinal_match_basic_macro",            8,  0, 376 },
    { 353, "SFXfoul_basic_macro",                   8,  0, 377 },
    { 354, "SFXfree_ball_basic_macro",              8,  0, 378 },
    { 355, "SFXfree_drop_basic_macro",              8,  0, 379 },
    { 356, "SFXkiss_shot_basic_macro",              8,  0, 380 },
    { 357, "SFXpocket_01_basic_macro",              8,  0, 381 },
    { 358, "SFXpocket_02_basic_macro",              8,  0, 382 },
    { 359, "SFXpocket_03_basic_macro",              8,  0, 383 },
    { 360, "SFXpocket_04_basic_macro",              8,  0, 384 },
    { 361, "SFXsafety_basic_macro",                 8,  0, 385 },
    { 362, "SFXspecial_match_basic_macro",          8,  0, 386 },
    {   2, "GRPbow_set",                            1,  0,  -1 },
    { 363, "SFXballpin_heavy_basic_macro",          0,  0, 146 },
    { 364, "SFXballpin_light_basic_macro",          0,  0, 148 },
    { 365, "SFXballpin_normal_basic_macro",         0,  0, 147 },
    { 366, "SFXhit_after01_basic_macro",            0,  0,  -1 },
    { 367, "SFXpinfloor_heavy_basic_macro",         0,  0, 149 },
    { 368, "SFXpinfloor_light_basic_macro",         0,  0,  -1 },
    { 369, "SFXpinfloor_normal02_basic_macro",      0,  0, 150 },
    { 370, "SFXpinfloor_normal_basic_macro",        0,  0, 151 },
    { 371, "SFXpinpin01_basic_macro",               0,  0,  -1 },
    { 372, "SFXpinpin02_basic_macro",               0,  0, 145 },
    { 373, "SFXpinpin03_basic_macro",               0,  0,  -1 },
    { 374, "SFXpinpin04_basic_macro",               0,  0, 144 },
    { 375, "SFXpinpin05_basic_macro",               0,  0,  -1 },
    { 376, "SFXpinpin06_basic_macro",               0,  0, 143 },
    { 377, "SFXroll_basic_macro",                   9,  0, 154 },
    { 378, "SFXstrike01_basic_macro",               0,  0,  -1 },
    { 379, "SFXstrike02_basic_macro",               0,  0,  -1 },
    { 380, "SFXstrike03_basic_macro",               0,  0,  -1 },
    { 381, "SFXstrike04_basic_macro",               0,  0,  -1 },
    { 382, "SFXthrow01_basic_macro",                3,  8, 153 },
    { 383, "SFXscorechange_basic_macro",           12,  0, 152 },
    { 384, "SFXgutter_basic_macro",                 3,  8, 155 },
    { 385, "SFXmachine1_basic_macro",               0,  0,  -1 },
    { 386, "SFXmachine2_basic_macro",               0,  0,  -1 },
    { 387, "SFXgutterbuzzer_basic_macro",          13,  0,  -1 },
    { 388, "SFXnyuinyui_basic_macro",              12,  0, 156 },
    { 389, "SFXsparebuzzer_basic_macro",           13,  0,  -1 },
    { 390, "SFXstrikebuzzer_basic_macro",          13,  0,  -1 },
    { 391, "SFXcourse_basic_macro",                 0,  0,  -1 },
    { 392, "SFXdecision_basic_macro",               0,  0,  -1 },
    { 393, "SFXmachine3_basic_macro",               9,  0,  -1 },
    { 394, "SFXmachine4_basic_macro",               0,  0,  -1 },
    { 395, "SFXmachine5_basic_macro",               0,  0,  -1 },
    { 396, "SFXbowl_miss_l_basic_macro",           13,  0, 159 },
    { 397, "SFXbowl_miss_r_basic_macro",           13,  0, 160 },
    { 398, "SFXbowl_normal_l_basic_macro",         13,  0, 165 },
    { 399, "SFXbowl_normal_r_basic_macro",         13,  0, 166 },
    { 400, "SFXbowl_perfect_l_basic_macro",        13,  0, 167 },
    { 401, "SFXbowl_perfect_r_basic_macro",        13,  0, 168 },
    { 402, "SFXbowl_spare_l_basic_macro",          13,  0, 161 },
    { 403, "SFXbowl_spare_r_basic_macro",          13,  0, 162 },
    { 404, "SFXbowl_strike_l_basic_macro",         13,  0, 163 },
    { 405, "SFXbowl_strike_r_basic_macro",         13,  0, 164 },
    { 406, "SFXbowl_turkey_l_basic_macro",         13,  0, 169 },
    { 407, "SFXbowl_turkey_r_basic_macro",         13,  0, 170 },
    { 408, "SFXpinpinhigh_basic_macro",             0,  0,  -1 },
    { 409, "SFXtick_basic_macro",                   0,  0, 157 },
    { 410, "SFXtock_basic_macro",                   0,  0, 158 },
    { 411, "SFXfirework1_basic_macro",              0,  0, 171 },
    { 412, "SFXfirework2_basic_macro",              0,  0, 172 },
    { 413, "SFXdouble_basic_macro",                 8,  0, 454 },
    { 414, "SFXexcellent_basic_macro",              8,  0, 455 },
    { 415, "SFXgutter_basic_macro_1",               8,  0, 456 },
    { 416, "SFXmiss_basic_macro",                   8,  0, 457 },
    { 417, "SFXpunch_out_01_basic_macro",           8,  0,  -1 },
    { 418, "SFXpunch_out_02_basic_macro",           8,  0, 458 },
    { 419, "SFXspare_basic_macro",                  8,  0, 459 },
    { 420, "SFXstrike_basic_macro",                 8,  0, 460 },
    { 421, "SFXtry_again_basic_macro",              8,  0, 461 },
    { 422, "SFXturkey_basic_macro",                 8,  0, 462 },
    { 423, "SFXclear2_basic_macro",                 8,  0, 463 },
    { 424, "SFXperfect2_basic_macro",               8,  0, 464 },
    {   6, "GRPfight_set",                          1,  0,  -1 },
    { 636, "SFXboxdrop_basic_macro",                0,  0, 250 },
    { 637, "SFXelectricalshock01_basic_macro",     10,  0,  -1 },
    { 638, "SFXelectricalshock02_basic_macro",     10,  0,  -1 },
    { 639, "SFXgong_bell04_basic_macro",            0,  0, 251 },
    { 640, "SFXgong_bell05_basic_macro",            0,  0,  -1 },
    { 641, "SFXgong_bell_a_basic_macro",            0,  0, 252 },
    { 642, "SFXitemboxbreak_basic_macro",           0,  0, 253 },
    { 643, "SFXitemboxbroke_basic_macro",           0,  0, 254 },
    { 644, "SFXpunch04_basic_macro",               10,  0, 255 },
    { 645, "SFXpunch05_basic_macro",               10,  0, 256 },
    { 646, "SFXpunch06_basic_macro",               10,  0, 257 },
    { 647, "SFXpunch07_basic_macro",               10,  0, 258 },
    { 648, "SFXpunch08_basic_macro",               10,  0, 259 },
    { 649, "SFXpunch_longbig01_basic_macro",       10,  0, 260 },
    { 650, "SFXpunch_longbig02_basic_macro",       10,  0,  -1 },
    { 651, "SFXspring01_basic_macro",              10,  0, 261 },
    { 652, "SFXspring02_basic_macro",              10,  0, 297 },
    { 653, "SFXswing01_basic_macro",               10,  0, 262 },
    { 654, "SFXtouchdown01_basic_macro",           10,  0,  -1 },
    { 655, "SFXtouchdown03_basic_macro",           10,  0,  -1 },
    { 656, "SFXspinpunch_basic_macro",              9,  0, 263 },
    { 657, "SFXbigpunch_basic_macro",              10,  0, 264 },
    { 658, "SFXrevive_basic_macro",                 0,  0, 265 },
    { 659, "SFXstar_basic_macro",                  10,  0, 305 },
    { 660, "SFXfight_win_l_basic_macro",           13,  0, 313 },
    { 661, "SFXfight_win_r_basic_macro",           13,  0, 314 },
    { 662, "SFXfight_01_basic_macro",               8,  0, 387 },
    { 663, "SFXfight_02_basic_macro",               8,  0,  -1 },
    { 664, "SFXfinal_round_01_basic_macro",        14,  0, 388 },
    { 665, "SFXfinal_round_02_basic_macro",         8,  0,  -1 },
    { 666, "SFXfinal_round_03_basic_macro",         8,  0,  -1 },
    { 667, "SFXfinal_round_04_basic_macro",         8,  0,  -1 },
    { 668, "SFXringout_01_basic_macro",             8,  0, 389 },
    { 669, "SFXringout_02_basic_macro",             8,  0,  -1 },
    { 670, "SFXround01_basic_macro",               14,  0, 390 },
    { 671, "SFXround02_basic_macro",               14,  0, 391 },
    { 672, "SFXround03_basic_macro",               14,  0, 392 },
    { 673, "SFXround04_basic_macro",               14,  0, 393 },
    { 674, "SFXround05_basic_macro",               14,  0, 394 },
    { 675, "SFXround06_basic_macro",               14,  0, 395 },
    { 676, "SFXround07_basic_macro",               14,  0, 396 },
    { 677, "SFXround08_basic_macro",               14,  0, 397 },
    { 678, "SFXround09_basic_macro",               14,  0, 398 },
    { 679, "SFXround10_basic_macro",               14,  0, 399 },
    { 680, "SFXround11_basic_macro",               14,  0, 400 },
    { 681, "SFXround12_basic_macro",               14,  0, 401 },
    { 682, "SFXround13_basic_macro",               14,  0, 402 },
    { 683, "SFXround14_basic_macro",               14,  0, 403 },
    { 684, "SFXround15_basic_macro",               14,  0, 404 },
    { 685, "SFXround16_basic_macro",               14,  0, 405 },
    { 686, "SFXround17_basic_macro",               14,  0, 406 },
    { 687, "SFXround18_basic_macro",               14,  0, 407 },
    { 688, "SFXround19_basic_macro",               14,  0, 408 },
    { 689, "SFXround20_basic_macro",               14,  0, 409 },
    { 690, "SFXsudden_death_01_basic_macro",        8,  0, 410 },
    { 691, "SFXsudden_death_02_basic_macro",        8,  0,  -1 },
    { 692, "SFXfight_01_basic_macro_1",            14,  0,  -1 },
    { 693, "SFXfight_02_basic_macro_1",            14,  0,  -1 },
    { 694, "SFXfinal_round_01_basic_macro_1",       8,  0,  -1 },
    { 695, "SFXfinal_round_02_basic_macro_1",       8,  0,  -1 },
    { 696, "SFXfinal_round_03_basic_macro_1",       8,  0,  -1 },
    { 697, "SFXfinal_round_04_basic_macro_1",       8,  0,  -1 },
    { 698, "SFXringout_01_basic_macro_1",           8,  0,  -1 },
    { 699, "SFXringout_02_basic_macro_1",           8,  0,  -1 },
    { 700, "SFXround01_basic_macro_1",              8,  0,  -1 },
    { 701, "SFXround02_basic_macro_1",              8,  0,  -1 },
    { 702, "SFXround03_basic_macro_1",              8,  0,  -1 },
    { 703, "SFXround04_basic_macro_1",              8,  0,  -1 },
    { 704, "SFXround05_basic_macro_1",              8,  0,  -1 },
    { 705, "SFXround06_basic_macro_1",              8,  0,  -1 },
    { 706, "SFXround07_basic_macro_1",              8,  0,  -1 },
    { 707, "SFXround08_basic_macro_1",              8,  0,  -1 },
    { 708, "SFXround09_basic_macro_1",              8,  0,  -1 },
    { 709, "SFXround10_basic_macro_1",              8,  0,  -1 },
    { 710, "SFXround11_basic_macro_1",              8,  0,  -1 },
    { 711, "SFXround12_basic_macro_1",              8,  0,  -1 },
    { 712, "SFXround13_basic_macro_1",              8,  0,  -1 },
    { 713, "SFXround14_basic_macro_1",              8,  0,  -1 },
    { 714, "SFXround15_basic_macro_1",              8,  0,  -1 },
    { 715, "SFXround16_basic_macro_1",              8,  0,  -1 },
    { 716, "SFXround17_basic_macro_1",              8,  0,  -1 },
    { 717, "SFXround18_basic_macro_1",              8,  0,  -1 },
    { 718, "SFXround19_basic_macro_1",              8,  0,  -1 },
    { 719, "SFXround20_basic_macro_1",              8,  0,  -1 },
    { 720, "SFXsudden_death_01_basic_macro_1",      8,  0,  -1 },
    { 721, "SFXsudden_death_02_basic_macro_1",      8,  0,  -1 },
    {   3, "GRPgolf_set",                           1,  0,  -1 },
    { 425, "SFXin1_basic_macro",                    0,  0, 183 },
    { 430, "SFXlawn_a_basic_macro",                 0,  0, 188 },
    { 431, "SFXlawn_b_basic_macro",                 0,  0, 189 },
    { 432, "SFXswisha_a_basic_macro",               0,  0, 190 },
    { 433, "SFXswish_b_basic_macro",                0,  0, 191 },
    { 434, "SFXtrip_basic_macro",                   0,  0, 192 },
    { 435, "SFXapplaud_basic_macro",                0,  0, 193 },
    { 436, "SFXapplaud_big_basic_macro",            0,  0, 194 },
    { 437, "SFXbird_basic_macro",                   0,  0, 195 },
    { 438, "SFXbird_loop_basic_macro",              9,  0, 196 },
    { 439, "SFXboo_basic_macro",                    0,  0, 197 },
    { 440, "SFXcheer_basic_macro",                  0,  0, 198 },
    { 446, "SFXputter_normal_basic_macro",          0,  0, 204 },
    { 447, "SFXputter_soft_basic_macro",            0,  0, 205 },
    { 448, "SFXscorechange02_basic_macro",          0,  0, 206 },
    { 449, "SFXtoogood1_basic_macro",               0,  0, 207 },
    { 450, "SFXtoogood2_basic_macro",               0,  0, 208 },
    { 451, "SFXawkward_basic_macro",                0,  0, 315 },
    { 452, "SFXflag_basic_macro",                   0,  0, 316 },
    { 453, "SFXafterob_basic_macro",                0,  0, 317 },
    { 454, "SFXgolf_birdie_l_basic_macro",         13,  0, 318 },
    { 455, "SFXgolf_birdie_r_basic_macro",         13,  0, 319 },
    { 456, "SFXgolf_bogie_l_basic_macro",          13,  0, 320 },
    { 457, "SFXgolf_bogie_r_basic_macro",          13,  0, 321 },
    { 458, "SFXgolf_giveup1_l_basic_macro",        13,  0, 322 },
    { 459, "SFXgolf_giveup1_r_basic_macro",        13,  0, 323 },
    { 460, "SFXgolf_giveup2_l_basic_macro",        13,  0, 324 },
    { 461, "SFXgolf_giveup2_r_basic_macro",        13,  0, 325 },
    { 462, "SFXgolf_holein1_l_basic_macro",        13,  0, 326 },
    { 463, "SFXgolf_holein1_r_basic_macro",        13,  0, 327 },
    { 464, "SFXgolf_ob_l_basic_macro",             13,  0, 328 },
    { 465, "SFXgolf_ob_r_basic_macro",             13,  0, 329 },
    { 466, "SFXgolf_par_l_basic_macro",            13,  0, 330 },
    { 467, "SFXgolf_par_r_basic_macro",            13,  0, 331 },
    { 468, "SFXpolehit_basic_macro",                0,  0, 332 },
    { 469, "SFXbirdie_basic_macro_1",               8,  0, 466 },
    { 470, "SFXbogey_basic_macro_1",                8,  0, 467 },
    { 471, "SFXdouble_bogey_basic_macro_1",         8,  0,  -1 },
    { 472, "SFXgive_up_basic_macro_1",              8,  0, 468 },
    { 473, "SFXhole_in_one_basic_macro_1",          8,  0, 469 },
    { 474, "SFXnice_on_basic_macro_1",              8,  0, 470 },
    { 475, "SFXnice_touch_basic_macro_1",           8,  0, 471 },
    { 476, "SFXob_basic_macro_1",                   8,  0, 472 },
    { 477, "SFXpar_basic_macro_1",                  8,  0, 473 },
    {   4, "GRPrace_set",                           1,  0,  -1 },
    { 478, "SFXbanana_basic_macro",                10,  0, 215 },
    { 479, "SFXbanana_big_basic_macro",            10,  0, 216 },
    { 480, "SFXdash_a_basic_macro",                10,  0, 210 },
    { 481, "SFXdash_b_basic_macro",                10,  0, 211 },
    { 482, "SFXdash_c_basic_macro",                10,  0, 212 },
    { 483, "SFXhit_basic_macro",                   10,  0, 217 },
    { 484, "SFXjumpstand_basic_macro",             10,  0, 218 },
    { 485, "SFXjumpstand_close_basic_macro",       10,  0, 219 },
    { 488, "SFXrecover_basic_macro",               10,  0, 220 },
    { 489, "SFXreverse_basic_macro",               10,  0, 221 },
    { 490, "SFXsignal1_basic_macro",               13,  0, 222 },
    { 491, "SFXsignal2_basic_macro",               13,  0, 223 },
    { 492, "SFXstick_basic_macro",                 10,  0, 224 },
    { 493, "SFXstick_close_basic_macro",           10,  0, 225 },
    { 494, "SFXthrow_basic_macro",                 10,  0, 226 },
    { 495, "SFXbrake_basic_macro",                 10,  0, 298 },
    { 496, "SFXcrack_basic_macro",                 10,  0,  -1 },
    { 497, "SFXcrackcrash_basic_macro",            10,  0,  -1 },
    { 498, "SFXrun_basic_macro",                   10,  0, 227 },
    { 499, "SFXsandrun_basic_macro",               10,  0, 228 },
    { 500, "SFXitem_hit_basic_macro",              10,  0, 299 },
    { 501, "SFXboxdrop_r_basic_macro",              0,  0, 306 },
    { 502, "SFXdrumcan_basic_macro",               10,  0, 307 },
    { 503, "SFXsignal_a_basic_macro",              13,  0, 308 },
    { 486, "SFXracelast_l_basic_macro",            13,  0, 356 },
    { 487, "SFXracelast_r_basic_macro",            13,  0, 357 },
    { 504, "SFXracetop_l_basic_macro",             13,  0, 358 },
    { 505, "SFXracetop_r_basic_macro",             13,  0, 359 },
    { 506, "SFXcure_basic_macro",                  10,  0, 360 },
    { 507, "SFXalright_01_basic_macro",             8,  0,  -1 },
    { 508, "SFXalright_02_basic_macro",             8,  0, 481 },
    { 509, "SFXalright_03_basic_macro",             8,  0,  -1 },
    { 510, "SFXbest_lap_basic_macro",               8,  0, 482 },
    { 511, "SFXbest_record_01_basic_macro",         8,  0, 483 },
    { 512, "SFXbest_record_02_basic_macro",         8,  0,  -1 },
    { 513, "SFXfastest_lap_basic_macro",            8,  0,  -1 },
    { 514, "SFXfinal_lap_01_basic_macro",           8,  0, 484 },
    { 515, "SFXfinal_lap_02_basic_macro",           8,  0,  -1 },
    { 516, "SFXkeep_on_01_basic_macro",             8,  0,  -1 },
    { 517, "SFXkeep_on_02_basic_macro",             8,  0,  -1 },
    { 518, "SFXkeep_on_03_basic_macro",             8,  0,  -1 },
    { 519, "SFXnew_record_01_basic_macro",          8,  0, 485 },
    { 520, "SFXnew_record_02_basic_macro",          8,  0,  -1 },
    { 521, "SFXtime_extension_basic_macro",         8,  0, 486 },
    { 522, "SFXgo_001_basic_macro",                 8,  0, 487 },
    { 523, "SFXgo_002_basic_macro",                 8,  0,  -1 },
    { 524, "SFXgo_003_basic_macro",                 8,  0,  -1 },
    { 525, "SFXready_001_basic_macro",              8,  0, 488 },
    { 526, "SFXready_002_basic_macro",              8,  0,  -1 },
    { 527, "SFXround01_basic_macro_1_1",            8,  0, 504 },
    { 528, "SFXround02_basic_macro_1_1",            8,  0, 505 },
    { 529, "SFXround03_basic_macro_1_1",            8,  0, 506 },
    { 530, "SFXround04_basic_macro_1_1",            8,  0, 507 },
    { 531, "SFXround05_basic_macro_1_1",            8,  0, 508 },
    { 532, "SFXround06_basic_macro_1_1",            8,  0, 509 },
    { 533, "SFXfinal_round_01_basic_macro_1_1",     8,  0,  -1 },
    { 534, "SFXfinal_round_03_basic_macro_1_1",     8,  0,  -1 },
    { 535, "SFXfinal_round_04_basic_macro_1_1",     8,  0,  -1 },
    {   5, "GRPtarget_set",                         1,  0,  -1 },
    { 536, "SFXblow1_basic_macro",                  0,  0, 229 },
    { 537, "SFXblow2_basic_macro",                  0,  0, 230 },
    { 538, "SFXblow3_basic_macro",                  0,  0, 231 },
    { 539, "SFXblow4_basic_macro",                  0,  0, 232 },
    { 540, "SFXbubble_baby01_basic_macro",          0,  0, 233 },
    { 541, "SFXbubble_baby02_basic_macro",          0,  0, 234 },
    { 542, "SFXbubble_boy01_basic_macro",           0,  0, 235 },
    { 543, "SFXbubble_boy02_basic_macro",           0,  0, 236 },
    { 544, "SFXbubble_girl01_basic_macro",          0,  0, 237 },
    { 545, "SFXbubble_girl02_basic_macro",          0,  0, 238 },
    { 546, "SFXfly_basic_macro",                    9,  0, 239 },
    { 547, "SFXopen_a_basic_macro",                 0,  0, 240 },
    { 548, "SFXopen_b_basic_macro",                 0,  0, 241 },
    { 549, "SFXtouchdown02_basic_macro",            0,  0, 242 },
    { 550, "SFXalart_basic_macro",                  9,  0, 243 },
    { 551, "SFXtarget_miss_l_basic_macro",         13,  0, 362 },
    { 552, "SFXsplash_ball_basic_macro",            0,  0, 245 },
    { 553, "SFXsplash_open_basic_macro",            0,  0, 246 },
    { 554, "SFXtarget_miss_r_basic_macro",         13,  0, 363 },
    { 555, "SFXcloud_basic_macro",                  9,  0, 248 },
    { 556, "SFXwave_small_basic_macro",             9,  0, 249 },
    { 557, "SFXaid_basic_macro",                   13,  0, 286 },
    { 558, "SFXbarbball_basic_macro",               0,  0, 287 },
    { 559, "SFXhindrance_basic_macro",             13,  0, 288 },
    { 560, "SFXhindrance2_basic_macro",            13,  0, 289 },
    { 561, "SFXmarkbig_basic_macro",                0,  0, 290 },
    { 562, "SFXpanel_basic_macro",                  0,  0, 291 },
    { 563, "SFXwindwall_basic_macro",               9,  0, 292 },
    { 564, "SFXtarget_score_hi_basic_macro",       13,  0, 311 },
    { 566, "SFXroulette_basic_macro",               0,  0, 310 },
    { 567, "SFXtarget_score_hi_l_basic_macro",      0,  0, 355 },
    { 568, "SFXheavydrop_basic_macro",             13,  0, 312 },
    { 569, "SFXtarget_score_hi_r_basic_macro",      0,  0, 348 },
    { 570, "SFXtarget_score_normal_l_basic_macro", 13,  0, 349 },
    { 571, "SFXtarget_score_normal_r_basic_macro", 13,  0, 350 },
    { 572, "SFXtarget_score_super_l_basic_macro",  13,  0, 351 },
    { 573, "SFXtarget_score_super_r_basic_macro",  13,  0, 352 },
    { 565, "SFXbond_basic_macro",                  13,  0, 309 },
    { 574, "SFXbubble_goli_basic_macro",           13,  0, 353 },
    { 575, "SFXkurukuru_basic_macro",               0,  0, 364 },
    { 576, "SFXalright_basic_macro",                8,  0, 411 },
    { 577, "SFXbomb_basic_macro_1",                 8,  0, 412 },
    { 578, "SFXbrake_ball_basic_macro",             8,  0, 413 },
    { 579, "SFXclowd_basic_macro",                  8,  0, 414 },
    { 580, "SFXdouble_score_basic_macro",           8,  0, 415 },
    { 581, "SFXgood_basic_macro",                   8,  0, 416 },
    { 582, "SFXmagnet_ball_basic_macro",            8,  0, 417 },
    { 583, "SFXno_wind_basic_macro",                8,  0, 418 },
    { 584, "SFXoh_no_01_basic_macro",               8,  0, 419 },
    { 585, "SFXoh_no_02_basic_macro",               8,  0, 420 },
    { 586, "SFXout_jp_basic_macro",                 8,  0, 421 },
    { 587, "SFXout_us_basic_macro",                 8,  0, 422 },
    { 588, "SFXsafe_jp_basic_macro",                8,  0, 423 },
    { 589, "SFXsafe_us_basic_macro",                8,  0, 424 },
    { 590, "SFXspike_ball_jp_basic_macro",          8,  0, 425 },
    { 591, "SFXspike_ball_us_basic_macro",          8,  0, 426 },
    { 592, "SFXstrong_wind_basic_macro",            8,  0, 427 },
    { 593, "SFXtriple_score_basic_macro",           8,  0, 428 },
    { 594, "SFXvery_good_basic_macro",              8,  0, 429 },
    { 595, "SFXexcellent2_basic_macro",             8,  0, 430 },
    { 596, "SFXmiss2_basic_macro",                  8,  0, 431 },
    { 597, "SFXalright_basic_macro_1",              8,  0, 432 },
    { 598, "SFXbomb_basic_macro_1_1",               8,  0, 433 },
    { 599, "SFXbrake_ball_basic_macro_1",           8,  0, 434 },
    { 600, "SFXclowd_basic_macro_1",                8,  0, 435 },
    { 601, "SFXdouble_score_basic_macro_1",         8,  0, 436 },
    { 602, "SFXgood_basic_macro_1",                 8,  0, 437 },
    { 603, "SFXmagnet_ball_basic_macro_1",          8,  0, 438 },
    { 604, "SFXno_wind_basic_macro_1",              8,  0, 439 },
    { 605, "SFXoh_no_01_basic_macro_1",             8,  0, 440 },
    { 606, "SFXoh_no_02_basic_macro_1",             8,  0, 441 },
    { 607, "SFXout_jp_basic_macro_1",               8,  0, 442 },
    { 608, "SFXout_us_basic_macro_1",               8,  0, 443 },
    { 609, "SFXsafe_jp_basic_macro_1",              8,  0, 444 },
    { 610, "SFXsafe_us_basic_macro_1",              8,  0, 445 },
    { 611, "SFXspike_ball_jp_basic_macro_1",        8,  0, 446 },
    { 612, "SFXspike_ball_us_basic_macro_1",        8,  0, 447 },
    { 613, "SFXstrong_wind_basic_macro_1",          8,  0, 448 },
    { 614, "SFXtriple_score_basic_macro_1",         8,  0, 449 },
    { 615, "SFXvery_good_basic_macro_1",            8,  0, 450 },
    { 616, "SFXexcellent2_basic_macro_1",           8,  0, 451 },
    { 617, "SFXmiss2_basic_macro_1",                8,  0, 452 },
    { 618, "SFXround01_basic_macro_2",              8,  0, 512 },
    { 619, "SFXround02_basic_macro_2",              8,  0, 513 },
    { 620, "SFXround03_basic_macro_2",              8,  0, 514 },
    { 621, "SFXround04_basic_macro_2",              8,  0, 515 },
    { 622, "SFXround05_basic_macro_2",              8,  0, 516 },
    { 623, "SFXround06_basic_macro_2",              8,  0, 517 },
    { 624, "SFXround07_basic_macro_1_1",            8,  0, 518 },
    { 625, "SFXround08_basic_macro_1_1",            8,  0, 519 },
    { 626, "SFXround09_basic_macro_1_1",            8,  0, 520 },
    { 627, "SFXround10_basic_macro_1_1",            8,  0, 521 },
    { 628, "SFXround11_basic_macro_1_1",            8,  0, 522 },
    { 629, "SFXround12_basic_macro_1_1",            8,  0, 523 },
    { 630, "SFXround13_basic_macro_1_1",            8,  0, 524 },
    { 631, "SFXround14_basic_macro_1_1",            8,  0, 525 },
    { 632, "SFXround15_basic_macro_1_1",            8,  0, 526 },
    { 633, "SFXfinal_round_01_basic_macro_2",       8,  0, 527 },
    { 634, "SFXfinal_round_03_basic_macro_2",       8,  0, 528 },
    { 635, "SFXfinal_round_04_basic_macro_2",       8,  0, 529 },
    {   7, "GRPcomn_set",                           1,  0,  -1 },
    { 722, "SFXball_bump_basic_macro",             10,  0, 266 },
    { 723, "SFXbomb_basic_macro",                  10,  0, 267 },
    {  -1, "DMY_CODE",                              0, 10, 296 },
    { 724, "SFXcamera_switch1_basic_macro",         0,  0, 268 },
    { 725, "SFXcamera_switch2_basic_macro",         0,  0, 269 },
    { 726, "SFXitem_basic_macro",                   0,  0, 270 },
    { 727, "SFXnormalitem_get_basic_macro",         0,  0, 271 },
    { 728, "SFXpowerscale1_basic_macro",            0,  0, 272 },
    { 729, "SFXpowerscale2_basic_macro",            0,  0, 273 },
    { 730, "SFXshot_power_basic_macro",             0,  0, 274 },
    { 731, "SFXspitem_get_basic_macro",             0,  0, 275 },
    { 732, "SFX1up_basic_macro",                    0,  0,  14 },
    { 733, "SFXfinishfly2_basic_macro",            10,  1, 294 },
    { 734, "SFXtapecut_basic_macro",               10,  0, 295 },
    { 735, "SFXball_bumpbig_basic_macro",          10,  0, 276 },
    {  -1, "DMY_CODE",                              0,  0, 347 },
    { 736, "SFXpower1_basic_macro",                 9,  0, 277 },
    { 737, "SFXpower2_basic_macro",                 9,  0, 278 },
    { 738, "SFXgauge1_basic_macro",                 9,  0, 279 },
    { 739, "SFXgauge2_basic_macro",                 9,  0, 280 },
    { 740, "SFXcancell_basic_macro",                0,  0, 281 },
    { 741, "SFXcomdecision_basic_macro",            0,  0, 282 },
    { 742, "SFXgauge3_basic_macro",                 9,  0, 283 },
    { 743, "SFXgauge4_basic_macro",                 9,  0, 284 },
    { 744, "SFXbirdc_basic_macro",                  0,  0, 502 },
    { 745, "SFXbirdc_loop_basic_macro",             9,  0, 503 },
    { 746, "SFXbonus_finish_basic_macro",           8,  0,  -1 },
    { 747, "SFXcount_00_basic_macro",               8,  0,  -1 },
    { 748, "SFXcount_01_basic_macro",               8,  0,  -1 },
    { 749, "SFXcount_02_basic_macro",               8,  0,  -1 },
    { 750, "SFXcount_03_basic_macro",               8,  0,  -1 },
    { 751, "SFXcount_04_basic_macro",               8,  0,  -1 },
    { 752, "SFXcount_05_basic_macro",               8,  0,  -1 },
    { 753, "SFXcount_06_basic_macro",               8,  0,  -1 },
    { 754, "SFXcount_07_basic_macro",               8,  0,  -1 },
    { 755, "SFXcount_08_basic_macro",               8,  0,  -1 },
    { 756, "SFXcount_09_basic_macro",               8,  0,  -1 },
    { 757, "SFXcount_10_basic_macro",               8,  0,  -1 },
    { 758, "SFXcount_11_basic_macro",               8,  0,  -1 },
    { 759, "SFXcount_12_basic_macro",               8,  0,  -1 },
    { 760, "SFXcount_13_basic_macro",               8,  0,  -1 },
    { 761, "SFXcount_14_basic_macro",               8,  0,  -1 },
    { 762, "SFXcount_15_basic_macro",               8,  0,  -1 },
    { 763, "SFXcount_16_basic_macro",               8,  0,  -1 },
    { 764, "SFXcount_17_basic_macro",               8,  0,  -1 },
    { 765, "SFXcount_18_basic_macro",               8,  0,  -1 },
    { 766, "SFXcount_19_basic_macro",               8,  0,  -1 },
    { 767, "SFXcount_20_basic_macro",               8,  0,  -1 },
    { 768, "SFXfallout_01_basic_macro",             8,  0,  -1 },
    { 769, "SFXfallout_02_basic_macro",             8,  0,  -1 },
    { 770, "SFXfallout_03_basic_macro",             8,  0,  -1 },
    { 771, "SFXfloor_clear_basic_macro",            8,  0,  -1 },
    { 772, "SFXgame_clear_basic_macro",             8,  0,  -1 },
    { 773, "SFXgame_over_basic_macro",              8,  0,  -1 },
    { 774, "SFXgo_01_basic_macro",                  8,  0,  -1 },
    { 775, "SFXgo_02_basic_macro",                  8,  0,  -1 },
    { 776, "SFXgo_03_basic_macro",                  8,  0,  -1 },
    { 777, "SFXgo_04_basic_macro",                  8,  0,  -1 },
    { 778, "SFXgoal_01_basic_macro",                8,  0,  -1 },
    { 779, "SFXgoal_02_basic_macro",                8,  0,  -1 },
    { 780, "SFXgoal_03_basic_macro",                8,  0,  -1 },
    { 781, "SFXgoal_04_basic_macro",                8,  0,  -1 },
    { 782, "SFXhurry_up_basic_macro",               8,  0,  -1 },
    { 783, "SFXready_01_basic_macro",               8,  0,  -1 },
    { 784, "SFXready_02_basic_macro",               8,  0,  -1 },
    { 785, "SFXstage_clear_basic_macro",            8,  0,  -1 },
    { 786, "SFXtime_over_basic_macro",              8,  0,  -1 },
    { 787, "SFXcontinue2_basic_macro",              8,  0,  -1 },
    { 788, "SFXenter_your_name_basic_macro",        8,  0,  -1 },
    { 789, "SFXsee_you_again_basic_macro",          8,  0,  -1 },
    { 790, "SFXthanx_for_playn_basic_macro",        8,  0, 576 },
    { 791, "SFXhow_many_players_basic_macro",       8,  0, 575 },
    { 792, "SFXselect_course_basic_macro",          8,  0, 569 },
    { 793, "SFXselect_game_basic_macro",            8,  0, 570 },
    { 794, "SFXselect_level_basic_macro",           8,  0, 571 },
    { 795, "SFXselect_mode_basic_macro",            8,  0, 572 },
    { 796, "SFXselect_stage_basic_macro",           8,  0, 573 },
    { 797, "SFXselect_your_monkey_basic_macro",     8,  0, 574 },
    { 798, "SFXadvanced_basic_macro",              14,  0, 535 },
    { 799, "SFXbiginner_basic_macro",              14,  0, 534 },
    { 800, "SFXcompetition_mode_basic_macro",      14,  0, 532 },
    { 801, "SFXexpert_basic_macro",                14,  0, 536 },
    { 802, "SFXextra_basic_macro",                 14,  0,  -1 },
    { 803, "SFXmain_game_basic_macro",             14,  0, 530 },
    { 804, "SFXmaster_basic_macro",                14,  0,  -1 },
    { 805, "SFXnormal_mode_basic_macro",           14,  0, 531 },
    { 806, "SFXpractice_mode_basic_macro",         14,  0, 533 },
    { 807, "SFXgrandprix_basic_macro",             14,  0, 544 },
    { 808, "SFXmonkey_fight_basic_macro",          14,  0, 541 },
    { 809, "SFXmonkey_target_basic_macro",         14,  0, 542 },
    { 810, "SFXmonky_race_basic_macro",            14,  0, 540 },
    { 811, "SFXonecourse_race_basic_macro",        14,  0, 543 },
    { 812, "SFXparty_game_basic_macro",            14,  0, 539 },
    { 813, "SFXtime_attack_basic_macro",           14,  0, 545 },
    { 814, "SFXchallenge_mode_basic_macro",        14,  0, 553 },
    { 815, "SFXmatch_play_basic_macro",            14,  0, 555 },
    { 816, "SFXmini_game_basic_macro",             14,  0, 546 },
    { 817, "SFXmonkey_billiards_basic_macro",      14,  0, 547 },
    { 818, "SFXmonkey_bowling_basic_macro",        14,  0, 548 },
    { 819, "SFXmonkey_golf_basic_macro",           14,  0, 549 },
    { 820, "SFXstandard_mode_basic_macro",         14,  0, 552 },
    { 821, "SFXstroke_play_basic_macro",           14,  0, 554 },
    { 822, "SFXtournament_mode_basic_macro",       14,  0, 551 },
    { 823, "SFXversus_mode_basic_macro",           14,  0, 550 },
    { 824, "SFXgamesettings_basic_macro",          14,  0, 557 },
    { 825, "SFXchamp_01_basic_macro",               8,  0,  -1 },
    { 826, "SFXchamp_02_basic_macro",               8,  0,  -1 },
    { 827, "SFXcongra_01_basic_macro",              8,  0,  -1 },
    { 828, "SFXcongra_02_basic_macro",              8,  0, 489 },
    { 829, "SFXcongra_03_basic_macro",              8,  0,  -1 },
    { 830, "SFXdraw_basic_macro",                   8,  0, 465 },
    { 831, "SFXfinish_01_basic_macro",              8,  0, 510 },
    { 832, "SFXfinish_02_basic_macro",              8,  0,  -1 },
    { 833, "SFXfinish_03_basic_macro",              8,  0,  -1 },
    { 834, "SFXlose_01_basic_macro",                8,  0,  -1 },
    { 835, "SFXlose_02_basic_macro",                8,  0, 583 },
    { 836, "SFXplayer1_basic_macro",                8,  0, 490 },
    { 837, "SFXplayer1win_basic_macro",             8,  0, 476 },
    { 838, "SFXplayer2_basic_macro",                8,  0, 491 },
    { 839, "SFXplayer2win_basic_macro",             8,  0, 477 },
    { 840, "SFXplayer3_basic_macro",                8,  0, 492 },
    { 841, "SFXplayer3win_basic_macro",             8,  0, 478 },
    { 842, "SFXplayer4_basic_macro",                8,  0, 493 },
    { 843, "SFXplayer4win_basic_macro",             8,  0, 479 },
    { 844, "SFXrank_in_basic_macro",                8,  0, 494 },
    { 845, "SFXresult_01_basic_macro",             14,  0, 480 },
    { 846, "SFXresult_02_basic_macro",              8,  0,  -1 },
    { 847, "SFXstart_basic_macro",                  8,  0, 501 },
    { 848, "SFXtoobad_01_basic_macro",              8,  0, 474 },
    { 849, "SFXtoobad_02_basic_macro",              8,  0, 475 },
    { 850, "SFXwin_01_basic_macro",                 8,  0, 495 },
    { 851, "SFXwinner_01_basic_macro",              8,  0,  -1 },
    { 852, "SFXyou_lose_03_basic_macro",            8,  0, 496 },
    { 853, "SFXyou_win_01_basic_macro",             8,  0, 497 },
    { 854, "SFXyou_win_02_basic_macro",             8,  0,  -1 },
    { 855, "SFXav_basic_macro",                     8,  0,  -1 },
    { 856, "SFXsmb_basic_macro",                    8,  0,  -1 },
    { 857, "SFXsega2_basic_macro",                  8,  0,  -1 },
    { 858, "SFXperfect3_basic_macro",               8,  0, 453 },
    { 859, "SFXpressstart_basic_macro",             8,  0,  -1 },
    { 860, "SFXringout_03_basic_macro",             8,  0,  -1 },
    {   8, "GRPending",                             1,  0,  -1 },
    { 861, "SFXdropstone_basic_macro",              0,  0, 558 },
    { 862, "SFXfly1_basic_macro",                   0,  0, 560 },
    { 863, "SFXfly2_basic_macro",                   0,  0, 562 },
    { 864, "SFXheavydrope_basic_macro",             0,  0, 564 },
    { 865, "SFXopen_c_basic_macro",                 0,  0, 563 },
    { 866, "SFXsit_basic_macro",                    0,  0, 561 },
    { 867, "SFXcharge_basic_macro",                 0,  0, 568 },
    { 868, "SFXdoorclose_basic_macro",              0,  0, 566 },
    { 869, "SFXdooropen_basic_macro",               0,  0, 565 },
    { 870, "SFXknock1_basic_macro",                 0,  0, 567 },
    { 871, "SFXbananadorop_basic_macro",            0,  0, 559 },
    {  -1, "GRPstream",                             1,  0,  -1 },
    {   0, "STRM_THEME_INT",                        0,  0,  -1 },
    {   1, "STRM_THEME_LP",                         0,  0,  -1 },
    {   2, "STRM_ADV_INT",                          0,  0,  -1 },
    {   3, "STRM_ADV_LP",                           0,  0,  -1 },
    {   4, "STRM_SEL_INT",                          0,  0,  -1 },
    {   5, "STRM_SEL_LP",                           0,  0,  -1 },
    {   6, "STRM_SEL2_INT",                         0,  0,  -1 },
    {   7, "STRM_SEL2_LP",                          0,  0,  -1 },
    {   8, "STRM_SEL_RACE_INT",                     0,  0,  -1 },
    {   9, "STRM_SEL_RACE_LP",                      0,  0,  -1 },
    {  10, "STRM_SEL_FIGHT_INT",                    0,  0,  -1 },
    {  11, "STRM_SEL_FIGHT_LP",                     0,  0,  -1 },
    {  12, "STRM_SEL_TARGET_INT",                   0,  0,  -1 },
    {  13, "STRM_SEL_TARGET_LP",                    0,  0,  -1 },
    {  14, "STRM_SEL_BILL_INT",                     0,  0,  -1 },
    {  15, "STRM_SEL_BILL_LP",                      0,  0,  -1 },
    {  16, "STRM_SEL_BOWL_INT",                     0,  0,  -1 },
    {  17, "STRM_SEL_BOWL_LP",                      0,  0,  -1 },
    {  18, "STRM_SEL_GOLF_INT",                     0,  0,  -1 },
    {  19, "STRM_SEL_GOLF_LP",                      0,  0,  -1 },
    {  20, "STRM_ST1_INT",                          0,  0,  -1 },
    {  21, "STRM_ST1_LP",                           0,  0,  -1 },
    {  22, "STRM_ST2_INT",                          0,  0,  -1 },
    {  23, "STRM_ST2_LP",                           0,  0,  -1 },
    {  24, "STRM_ST3_INT",                          0,  0,  -1 },
    {  25, "STRM_ST3_LP",                           0,  0,  -1 },
    {  26, "STRM_ST4_INT",                          0,  0,  -1 },
    {  27, "STRM_ST4_LP",                           0,  0,  -1 },
    {  28, "STRM_ST5_INT",                          0,  0,  -1 },
    {  29, "STRM_ST5_LP",                           0,  0,  -1 },
    {  30, "STRM_ST6_INT",                          0,  0,  -1 },
    {  31, "STRM_ST6_LP",                           0,  0,  -1 },
    {  32, "STRM_ST7_INT",                          0,  0,  -1 },
    {  33, "STRM_ST7_LP",                           0,  0,  -1 },
    {  34, "STRM_ST8_INT",                          0,  0,  -1 },
    {  35, "STRM_ST8_LP",                           0,  0,  -1 },
    {  36, "STRM_STB_INT",                          0,  0,  -1 },
    {  37, "STRM_STB_LP",                           0,  0,  -1 },
    {  38, "STRM_STM_INT",                          0,  0,  -1 },
    {  39, "STRM_STM_LP",                           0,  0,  -1 },
    {  40, "STRM_CNT_INT",                          0,  0,  -1 },
    {  41, "STRM_CNT_LP",                           0,  0,  -1 },
    {  42, "STRM_OVR_INT",                          0,  0,  -1 },
    {  43, "STRM_OVR_LP",                           0,  0,  -1 },
    {  44, "STRM_RACE_INT",                         0,  0,  -1 },
    {  45, "STRM_RACE_LP",                          0,  0,  -1 },
    {  46, "STRM_FIGHT_INT",                        0,  0,  -1 },
    {  47, "STRM_FIGHT_LP",                         0,  0,  -1 },
    {  48, "STRM_OMK_INT",                          0,  0,  -1 },
    {  49, "STRM_OMK_LP",                           0,  0,  -1 },
    {  50, "STRM_PILOT_INT",                        0,  0,  -1 },
    {  51, "STRM_PILOT_LP",                         0,  0,  -1 },
    {  52, "STRM_BILI_INT",                         0,  0,  -1 },
    {  53, "STRM_BILI_LP",                          0,  0,  -1 },
    {  54, "STRM_BWL_INT",                          0,  0,  -1 },
    {  55, "STRM_BWL_LP",                           0,  0,  -1 },
    {  56, "STRM_GLF_INT",                          0,  0,  -1 },
    {  57, "STRM_GLF_LP",                           0,  0,  -1 },
    {  58, "STRM_OMK2_INT",                         0,  0,  -1 },
    {  59, "STRM_OMK2_LP",                          0,  0,  -1 },
    {  60, "STRM_GREP_INT",                         0,  0,  -1 },
    {  61, "STRM_GREP_LP",                          0,  0,  -1 },
    {  62, "STRM_RESULT_INT",                       0,  0,  -1 },
    {  63, "STRM_RESULT_LP",                        0,  0,  -1 },
    {  64, "STRM_HYO_INT",                          0,  0,  -1 },
    {  65, "STRM_HYO_LP",                           0,  0,  -1 },
    {  66, "STRM_ENT_INT",                          0,  0,  -1 },
    {  67, "STRM_ENT_LP",                           0,  0,  -1 },
    {  68, "STRM_END_INT",                          0,  0,  -1 },
    {  69, "STRM_END_LP",                           0,  0,  -1 },
    {  70, "STRM_ROLL_INT",                         0,  0,  -1 },
    {  71, "STRM_ROLL_LP",                          0,  0,  -1 },
    {  72, "STRM_OPT_INT",                          0,  0,  -1 },
    {  73, "STRM_OPT_LP",                           0,  0,  -1 },
    {  -2, "GRPend",                                1,  0,  -1 },
};

struct Struct801B2A5C
{
    char *unk0;
    u32 unk4;
    u32 unk8;
    s8 unkC;
};

static struct Struct801B2A5C lbl_801B2A5C[] =
{
    { "theme",         0x91000000, 0x002CE400, -1 },
    { "theme",         0x91000000, 0x002CE400, -1 },
    { "adv_int",       0x91000000, 0x00055100,  3 },
    { "adv_lp",        0x91000000, 0x001AFE00,  3 },
    { "sel_int",       0x9B000000, 0x00014900,  5 },
    { "sel_lp",        0x9B000000, 0x000AE900,  5 },
    { "sel2_all",      0x8C000000, 0x00175700,  7 },
    { "sel2_lp",       0x8C000000, 0x0014C300,  7 },
    { "sel_race",      0x91000000, 0x0008B700,  9 },
    { "sel_race",      0x91000000, 0x0008B700,  9 },
    { "sel_Fight",     0xA0000000, 0x000DFC00, 11 },
    { "sel_Fight",     0xA0000000, 0x000DFC00, 11 },
    { "sel_target",    0x9B000000, 0x000CBD00, 13 },
    { "sel_target",    0x9B000000, 0x000CBD00, 13 },
    { "sel_billiard",  0xA0000000, 0x000D1F00, 15 },
    { "sel_billiard",  0xA0000000, 0x000D1F00, 15 },
    { "sel_bowling",   0x9B000000, 0x000A4800, 17 },
    { "sel_bowling",   0x9B000000, 0x000A4800, 17 },
    { "sel_golf",      0x9B000000, 0x000C2A00, 19 },
    { "sel_golf",      0x9B000000, 0x000C2A00, 19 },
    { "st1",           0x8C000000, 0x002ED100, 21 },
    { "st1",           0x8C000000, 0x002ED100, 21 },
    { "st2_all",       0x96000000, 0x00333F00, 23 },
    { "st2_lp",        0x96000000, 0x00318F00, 23 },
    { "st3_int",       0x9B000000, 0x00014E9C, 25 },
    { "st3_lp",        0x9B000000, 0x00353700, 25 },
    { "st4_int",       0x87000000, 0x000468A8, 27 },
    { "st4_lp",        0x87000000, 0x002D0100, 27 },
    { "st5_int",       0x96000000, 0x00003700, 29 },
    { "st5_lp",        0x96000000, 0x0040DA00, 29 },
    { "st6_int",       0x96000000, 0x00015E00, 31 },
    { "st6_lp",        0x96000000, 0x00391700, 31 },
    { "st7",           0x91000000, 0x003A0C00, 33 },
    { "st7",           0x91000000, 0x003A0C00, 33 },
    { "st8_int",       0xA0000000, 0x0004B000, 35 },
    { "st8_lp",        0xA0000000, 0x003BBE00, 35 },
    { "stb_int",       0x91000000, 0x00011E00, 37 },
    { "stb_lp",        0x91000000, 0x0034F100, 37 },
    { "stm_int",       0x78000000, 0x00028B00, 39 },
    { "stm_lp",        0x78000000, 0x0029A100, 39 },
    { "cnt",           0x7D000000, 0x000AC200, 41 },
    { "cnt",           0x7D000000, 0x000AC200, 41 },
    { "ovr",           0x96000000, 0x00029000, -1 },
    { "ovr",           0x96000000, 0x00029000, -1 },
    { "race",          0x91000000, 0x004CF000, 45 },
    { "race",          0x91000000, 0x004CF000, 45 },
    { "fight_int",     0x82000000, 0x0001E367, 47 },
    { "fight_lp",      0x82000000, 0x00353F00, 47 },
    { "omk",           0x8C000000, 0x000B9E00, 49 },
    { "omk",           0x8C000000, 0x000B9E00, 49 },
    { "tget_int",      0x8C000000, 0x0008B400, 51 },
    { "tget_lp",       0x8C000000, 0x00463F00, 51 },
    { "bili_int",      0x8C000000, 0x0003B800, 53 },
    { "bili_lp",       0x8C000000, 0x002DCE00, 53 },
    { "bwl_int",       0x8C000000, 0x00031900, 55 },
    { "bwl_lp",        0x8C000000, 0x002D4900, 55 },
    { "glf",           0x91000000, 0x00382B00, 57 },
    { "glf",           0x91000000, 0x00382B00, 57 },
    { "omk2",          0x87000000, 0x0007D400, 59 },
    { "omk2",          0x87000000, 0x0007D400, 59 },
    { "grep_int",      0x82000000, 0x00014600, 61 },
    { "grep_lp",       0x82000000, 0x00054A00, 61 },
    { "result",        0x9B000000, 0x00167300, 63 },
    { "result",        0x9B000000, 0x00167300, 63 },
    { "hyo",           0x73000000, 0x00137A00, 65 },
    { "hyo",           0x73000000, 0x00137A00, 65 },
    { "ent_int",       0x87000000, 0x0000CA00, 67 },
    { "ent_lp",        0x87000000, 0x00186200, 67 },
    { "end_int",       0x91000000, 0x00014D00, 69 },
    { "end_lp",        0x91000000, 0x00161500, 69 },
    { "roll_int",      0xBE000000, 0x00053A00, 71 },
    { "roll_lp",       0xBE000000, 0x00453100, 71 },
    { "opt",           0x91000000, 0x000DDA00, 73 },
    { "opt",           0x91000000, 0x000DDA00, 73 },
    { "themem",        0x01000000, 0x00000000, -1 },
    { "themem",        0x01000000, 0x00000000, -1 },
    { "advm_int",      0x01000000, 0x00000000,  3 },
    { "advm_lp",       0x01000000, 0x00000000,  3 },
    { "selm_int",      0x01000000, 0x00000000,  5 },
    { "selm_lp",       0x01000000, 0x00000000,  5 },
    { "sel2m_all",     0x01000000, 0x00000000,  7 },
    { "sel2m_lp",      0x01000000, 0x00000000,  7 },
    { "selm_race",     0x01000000, 0x00000000,  9 },
    { "selm_race",     0x01000000, 0x00000000,  9 },
    { "selm_Fight",    0x01000000, 0x00000000, 11 },
    { "selm_Fight",    0x01000000, 0x00000000, 11 },
    { "selm_target",   0x01000000, 0x00000000, 13 },
    { "selm_target",   0x01000000, 0x00000000, 13 },
    { "selm_billiard", 0x01000000, 0x00000000, 15 },
    { "selm_billiard", 0x01000000, 0x00000000, 15 },
    { "selm_bowling",  0x01000000, 0x00000000, 17 },
    { "selm_bowling",  0x01000000, 0x00000000, 17 },
    { "selm_golf",     0x01000000, 0x00000000, 19 },
    { "selm_golf",     0x01000000, 0x00000000, 19 },
    { "st1m",          0x01000000, 0x00000000, 21 },
    { "st1m",          0x01000000, 0x00000000, 21 },
    { "st2m_all",      0x01000000, 0x00000000, 23 },
    { "st2m_lp",       0x01000000, 0x00000000, 23 },
    { "st3m_int",      0x01000000, 0x00000000, 25 },
    { "st3m_lp",       0x01000000, 0x00000000, 25 },
    { "st4m_int",      0x01000000, 0x00000000, 27 },
    { "st4m_lp",       0x01000000, 0x00000000, 27 },
    { "st5m_int",      0x01000000, 0x00000000, 29 },
    { "st5m_lp",       0x01000000, 0x00000000, 29 },
    { "st6m_int",      0x01000000, 0x00000000, 31 },
    { "st6m_lp",       0x01000000, 0x00000000, 31 },
    { "st7m",          0x01000000, 0x00000000, 33 },
    { "st7m",          0x01000000, 0x00000000, 33 },
    { "st8m_int",      0x01000000, 0x00000000, 35 },
    { "st8m_lp",       0x01000000, 0x00000000, 35 },
    { "stbm_int",      0x01000000, 0x00000000, 37 },
    { "stbm_lp",       0x01000000, 0x00000000, 37 },
    { "stmm_int",      0x01000000, 0x00000000, 39 },
    { "stmm_lp",       0x01000000, 0x00000000, 39 },
    { "cntm",          0x01000000, 0x00000000, 41 },
    { "cntm",          0x01000000, 0x00000000, 41 },
    { "ovrm",          0x01000000, 0x00000000, -1 },
    { "ovrm",          0x01000000, 0x00000000, -1 },
    { "racem",         0x01000000, 0x00000000, 45 },
    { "racem",         0x01000000, 0x00000000, 45 },
    { "fightm_int",    0x01000000, 0x00000000, 47 },
    { "fightm_lp",     0x01000000, 0x00000000, 47 },
    { "omkm",          0x01000000, 0x00000000, 49 },
    { "omkm",          0x01000000, 0x00000000, 49 },
    { "tgetm_int",     0x01000000, 0x00000000, 51 },
    { "tgetm_lp",      0x01000000, 0x00000000, 51 },
    { "bilim_int",     0x01000000, 0x00000000, 53 },
    { "bilim_lp",      0x01000000, 0x00000000, 53 },
    { "bwlm_int",      0x01000000, 0x00000000, 55 },
    { "bwlm_lp",       0x01000000, 0x00000000, 55 },
    { "glfm",          0x01000000, 0x00000000, 57 },
    { "glfm",          0x01000000, 0x00000000, 57 },
    { "omk2m",         0x01000000, 0x00000000, 59 },
    { "omk2m",         0x01000000, 0x00000000, 59 },
    { "grepm_int",     0x01000000, 0x00000000, 61 },
    { "grepm_lp",      0x01000000, 0x00000000, 61 },
    { "resultm",       0x01000000, 0x00000000, 63 },
    { "resultm",       0x01000000, 0x00000000, 63 },
    { "hyom",          0x01000000, 0x00000000, 65 },
    { "hyom",          0x01000000, 0x00000000, 65 },
    { "entm_int",      0x01000000, 0x00000000, 67 },
    { "entm_lp",       0x01000000, 0x00000000, 67 },
    { "endm_int",      0x01000000, 0x00000000, 69 },
    { "endm_lp",       0x01000000, 0x00000000, 69 },
    { "rollm_int",     0x01000000, 0x00000000, 71 },
    { "rollm_lp",      0x01000000, 0x00000000, 71 },
    { "optm",          0x01000000, 0x00000000, 73 },
    { "optm",          0x01000000, 0x00000000, 73 },
};

static u32 s_aramBlocks[2];
static u8 sndEfcRev;
static u8 sndEfcCho;
static s16 lbl_802F1D3A;
static u8 lbl_802F1D3C;
static float lbl_802F1D40;
static float lbl_802F1D44;
static s32 lbl_802F1D48;
static float lbl_802F1D4C;
static float lbl_802F1D50;
static float lbl_802F1D54;
static s32 lbl_802F1D58;
static s8 lbl_802F1D5C[4];
static s8 lbl_802F1D60[4];
static s32 lbl_802F1D64;
static s32 lbl_802F1D68;
static s32 lbl_802F1D6C;
static s32 lbl_802F1D70;
static s32 lbl_802F1D74;
static float u_volumeRelated3;
static u8 *s_testStreamLeftBuffer;
static u8 *s_testStreamRightBuffer;
static u8 *lbl_802F1D84;
static u8 *lbl_802F1D88;
static u32 lbl_802F1D8C;
static u32 lbl_802F1D90;
static s32 lbl_802F1D94;
static s32 lbl_802F1D98;
static u32 lbl_802F1D9C;
static u32 lbl_802F1DA0;
static volatile s32 lbl_802F1DA4;
static volatile s32 lbl_802F1DA8;
static s32 lbl_802F1DAC;
static s32 lbl_802F1DB0;
static u32 lbl_802F1DB4;
static u32 lbl_802F1DB8;
static u32 lbl_802F1DBC;
static BOOL s_groupIsUsingCharaHeap;
static s32 lbl_802F1DC4;
static s32 lbl_802F1DC8;

static void *s_poolData[9];
static void *s_projData[9];
static void *s_sdirData[9];
static void *s_sampData[9];
static u32 s_poolDataSizes[9];
static u32 s_projDataSizes[9];
static u32 s_sdirDataSizes[9];
static u32 s_sampDataSizes[9];
static SND_AUX_REVERBHI lbl_801F8F38;
static struct
{
    u8 unk300[0x90];
    u32 unk390;
    u32 unk394;
    u32 unk398;
} lbl_801F9118;
static SND_VOICEID s_voiceIDs[4][0x425];
static u8 lbl_801FD404[4][0x425];
static struct Struct801FE498 lbl_801FE498[16];
static struct
{
    s8 unk0[4][20];
    struct { s32 unk0; u8 filler4[4]; } unk50[4];
} lbl_801FE558;
static float lbl_801FE5C8[4];
static DTKTrack s_dtkTracks[0x94];
static DVDFileInfo s_testStreamLeftFile;
static DVDFileInfo s_testStreamRightFile;
static struct
{
    u32 unk3C;
    u32 unk40;
    u32 unk44;
} lbl_80201490;
static int u_isSoundGroupDataLoaded[9];
static s32 s_loadedSoundGroupIDs[8];

s32 g_loadedSoundGroupsCount;
s32 lbl_802F1DFC;
int u_somePlayerId;
u8 g_soundSEVol;
u8 lbl_802F1DF4;
u32 g_soundAramTop;
u32 g_soundTotalBytesLoaded;
s32 u_someSoundId;  // always 1
s32 u_someSoundGroupId;  // always 0
const char *u_someSoundName;
const char *u_someSoundGroupName;
u8 g_soundBGMVol;
u8 u_volumeRelated2;
s32 g_soundMuted;
u32 lbl_802F1DD0;
s32 lbl_802F1DCC;

struct Struct802014E0 lbl_802014E0;
s8 lbl_80201500[0x428];

static void sound_error(const char *func, const char *msg, ...)
{
    va_list args;
    char buffer[0x200];

    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    printf("### Sound ERROR ### at ");
    printf(func);
    printf(buffer);
}

static void queue_dtk_tracks(s32 arg0, s32 mode)
{
    char fileName[0x100];
    int i;
    int r0;

    switch (mode)
    {
    case SND_OUTPUTMODE_MONO:
        sndOutputMode(SND_OUTPUTMODE_MONO);
        if (arg0 == 0)
        {
            for (i = 0; i < 0x4A; i++)
                DTKRemoveTrack(&s_dtkTracks[i]);
        }
        r0 = 0;
        for (i = 0x4A; i < 0x94; i++, r0++)
        {
            sprintf(fileName, "/test/snd/adp/%s.adp", lbl_801B2A5C[i].unk0);
            DTKQueueTrack(fileName, &s_dtkTracks[r0], 0, 0);
        }
        break;
    case SND_OUTPUTMODE_STEREO:
        break;
    case SND_OUTPUTMODE_SURROUND:
        sndOutputMode(SND_OUTPUTMODE_SURROUND);
        if (arg0 == 0)
        {
            for (i = 0; i < 0x4A; i++)
                DTKRemoveTrack(&s_dtkTracks[i]);
        }
        for (i = 0; i < 0x4A; i++)
        {
            sprintf(fileName, "/test/snd/adp/%s.adp", lbl_801B2A5C[i].unk0);
            DTKQueueTrack(fileName, &s_dtkTracks[i], 0, 0);
        }
        break;
    }
}

static void *ReadMusyXData(const char *fileName, u32 *sizeOut)
{
    struct File file;
    u32 size;
    void *buffer;

    if (!file_open(fileName, &file))
    {
        sound_error("ReadMusyXData\n", "Unable to open '%s'\n", fileName);
        return NULL;
    }
    size = file_size(&file);
    if (size == 0)
    {
        sound_error("ReadMusyXData\n", "Zero length file '%s'\n", fileName);
        return NULL;
    }
    *sizeOut = OSRoundUp32B(size);
    buffer = OSAlloc(*sizeOut);
    if (buffer == NULL)
    {
        sound_error("ReadMusyXData\n", "unable to allocate buffer '%s'\n", fileName);
        return NULL;
    }
    if (file_read(&file, buffer, *sizeOut, 0) <= 0)
    {
        sound_error("ReadMusyXData\n", "Failed to read data from '%s'\n", fileName);
        OSFree(buffer);
        return NULL;
    }
    file_close(&file);
    return buffer;
}

#define SOME_MACRO(a, b, c, d) \
    ((a) | (((b) & 0x7F) << 11) | (((c) & 0x7F) << 18) | (((d) & 0x7F) << 25))

#define SOME_MACRO2(a, b, c, d) \
    ((((b) & 0x7F) << 11) | (((c) & 0x7F) << 18) | (((d) & 0x7F) << 25) | (a))

static void play_stage_sounds(void)
{
    s8 sp15 = 0;
    s8 sp14 = 0;
    Vec sp8 = {0};
    s8 r3;

    if (!(advDemoInfo.flags & 0x100) && gameMode != MD_SEL)
    {
        if (((gameSubmode == SMD_SEL_NGC_MAIN) && lbl_801EEDA8.unk100 == 0.0f) || is_load_queue_not_empty())
        {
            if (lbl_802F1DC4 != 0)
            {
                SoundOff(lbl_80201490.unk3C);
                SoundOff(lbl_80201490.unk40);
                SoundOff(lbl_80201490.unk44);
                lbl_802F1DC4 = 0;
            }
            if (lbl_802F1DC8 != 0)
            {
                SoundOffID(0x1F7);
                lbl_802F1DC8 = 0;
            }
        }
        else
        {
            if (backgroundInfo.bgId == BG_TYPE_STORM_D || backgroundInfo.bgId == BG_TYPE_STM)
            {
                if (lbl_802F1DC4 == 0 && lbl_802F1D40 == 1.0)
                {
                    lbl_802F1DC4 = 1;
                    lbl_80201490.unk3C = u_play_sound_2(0x4C);
                }
                if ((int)stageInfo.u_stageTimer % 300 == 60)
                {
                    lbl_80201490.unk40 = u_play_sound_1_dupe(
                        SOME_MACRO(
                            0x4D,
                            ((int)stageInfo.u_stageTimer & 0x1F) - 0x23,
                            ((int)(59.0f + stageInfo.u_stageTimer) & 0x7F) - 0x40,
                            ((int)(77.0f + stageInfo.u_stageTimer) & 0x7F) - 0x40
                        )
                    );
                    SoundPitch(lbl_80201490.unk40, ((int)stageInfo.u_stageTimer * 0x1E) & 0x3FFF);
                }
                if ((int)stageInfo.u_stageTimer % 1000 == 200)
                {
                    lbl_80201490.unk44 = u_play_sound_1_dupe(
                        SOME_MACRO(
                            0x4E,
                            ((int)stageInfo.u_stageTimer & 0x1F) - 5,
                            ((int)(59.0f + stageInfo.u_stageTimer) & 0x7F) - 0x40,
                            ((int)(77.0f + stageInfo.u_stageTimer) & 0x7F) - 0x40
                        )
                    );
                    SoundPitch(lbl_80201490.unk40, ((int)stageInfo.u_stageTimer * 0x1E) & 0x3FFF);
                }
            }
            else if (lbl_802F1DC4 != 0)
            {
                SoundOff(lbl_80201490.unk3C);
                SoundOff(lbl_80201490.unk40);
                SoundOff(lbl_80201490.unk44);
                lbl_802F1DC4 = 0;
            }
            if (backgroundInfo.bgId == BG_TYPE_JUN
             && (modeCtrl.gameType == GAMETYPE_MAIN_NORMAL || modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION || modeCtrl.gameType == GAMETYPE_MAIN_PRACTICE))
            {
                if (lbl_802F1DC8 == 0 && lbl_802F1D40 == 1.0)
                {
                    lbl_802F1DC8 = 1;
                    u_play_sound_0(0x389F7);
                }
                if ((int)stageInfo.u_stageTimer % 1000 == 200)
                {
                    u_play_sound_0(
                        SOME_MACRO(
                            0x1F6,
                            ((int)stageInfo.u_stageTimer & 0x1F) - 0x28,
                            ((int)(59.0f + stageInfo.u_stageTimer) & 0x7F) - 0x40,
                            ((int)(77.0f + stageInfo.u_stageTimer) & 0x7F) - 0x40
                        )
                    );
                }
            }
            else if (lbl_802F1DC8 != 0)
            {
                SoundOffID(0xC4);
                lbl_802F1DC8 = 0;
            }

            if (stageInfo.u_stageTimer != 0.0)
            {
                switch (currStageId)
                {
                case ST_109_FACTORY:
                    if ((int)stageInfo.u_stageTimer % 180 == 60)
                    {
                        sp8.x = 0.0f;
                        sp8.y = 7.5f;
                        sp8.z = -17.2f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14);
                        u_play_sound_0(SOME_MACRO2(0x43, r3, sp15, sp14));
                    }
                    break;
                case ST_061_TWIN_ATTACKER:
                    if ((int)stageInfo.u_stageTimer % 75 == 0)
                    {
                        if ((int)stageInfo.u_stageTimer % 150 == 0)
                        {
                            sp8.x = -3.0f;
                            sp8.y = 0.25f;
                            sp8.z = 42.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x42, r3, sp15, sp14));
                            sp8.x = -3.0f;
                            sp8.y = 0.25f;
                            sp8.z = -15.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x42, r3, sp15, sp14));
                        }
                        else
                        {
                            sp8.x = 2.0f;
                            sp8.y = 0.25f;
                            sp8.z = 42.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x42, r3, sp15, sp14));
                            sp8.x = 2.0f;
                            sp8.y = 0.25f;
                            sp8.z = -15.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x42, r3, sp15, sp14));
                        }
                    }
                    break;
                case ST_038_SPIKY:
                    if ((int)stageInfo.u_stageTimer % 75 == 0)
                    {
                        if ((int)stageInfo.u_stageTimer % 150 == 0)
                        {
                            sp8.x = -3.0f;
                            sp8.y = 0.5f;
                            sp8.z = -9.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x41, r3, sp15, sp14));
                            sp8.x = -3.0f;
                            sp8.y = 0.5f;
                            sp8.z = 4.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x41, r3, sp15, sp14));
                        }
                        else
                        {
                            sp8.x = 3.0f;
                            sp8.y = 0.5f;
                            sp8.z = -9.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x41, r3, sp15, sp14));
                            sp8.x = 3.0f;
                            sp8.y = 0.5f;
                            sp8.z = 4.0f;
                            r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 2.0;
                            u_play_sound_0(SOME_MACRO2(0x41, r3, sp15, sp14));
                        }
                    }
                    break;
                case ST_023_JUMPIES:
                    if ((int)stageInfo.u_stageTimer % 150 == 90)
                    {
                        sp8.x = 0.0f;
                        sp8.y = 0.0f;
                        sp8.z = -3.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14);
                        r3 *= 0.333;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                        sp8.x = 0.0f;
                        sp8.y = 3.0f;
                        sp8.z = -15.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14);
                        r3 *= 0.333;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                        sp8.x = 0.0f;
                        sp8.y = 7.0f;
                        sp8.z = -26.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14);
                        r3 *= 0.333;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                    }
                    break;
                case ST_113_DAA_LOO_MAA:
                    if ((int)stageInfo.u_stageTimer % 150 == 0)
                    {
                        sp8.x = 0.0f;
                        sp8.y = 0.0f;
                        sp8.z = 0.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14);
                        r3 += 20;
                        u_play_sound_0(SOME_MACRO2(0x40, r3, sp15, sp14));
                    }
                    break;
                case ST_107_BREATHE:
                    if ((int)stageInfo.u_stageTimer % 0x78 == 1)
                    {
                        sp8.x = 16.0f;
                        sp8.y = 0.5f;
                        sp8.z = 16.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x43, r3, sp15, sp14));
                        sp8.x = -16.0f;
                        sp8.y = 0.5f;
                        sp8.z = -16.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x43, r3, sp15, sp14));
                        sp8.x = 18.0f;
                        sp8.y = 0.5f;
                        sp8.z = -18.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x43, r3, sp15, sp14));
                        sp8.x = -18.0f;
                        sp8.y = 0.5f;
                        sp8.z = 18.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x43, r3, sp15, sp14));
                    }
                    break;
                case ST_077_QUAKE:
                    if ((int)stageInfo.u_stageTimer % 480 == 150)
                    {
                        sp8.x = -16.0f;
                        sp8.y = 0.5f;
                        sp8.z = 16.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                        sp8.x = 16.0f;
                        sp8.y = 0.5f;
                        sp8.z = -16.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                        sp8.x = 12.0f;
                        sp8.y = 0.5f;
                        sp8.z = 12.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                        sp8.x = -12.0f;
                        sp8.y = 0.5f;
                        sp8.z = -12.0f;
                        r3 = func_8002BB20(modeCtrl.unk30, &sp8, &sp15, &sp14) / 4.0;
                        u_play_sound_0(SOME_MACRO2(0x44, r3, sp15, sp14));
                    }
                    break;
                }
            }
        }
    }
}

static void *u_alloc(u32 size)
{
    return OSAlloc(size);
}

static void u_free(void *ptr)
{
    OSFree(ptr);
}

void sound_init(void)
{
    SND_HOOKS hooks = { u_alloc, u_free };
    u8 unused[16];
    s32 i;
    s32 j;
    s32 i2;

    g_soundTotalBytesLoaded = 0;
    g_soundAramTop = 0;
    ARInit(s_aramBlocks, ARRAY_COUNT(s_aramBlocks));
    ARQInit();
    AIInit(NULL);
    AISetStreamSampleRate(AI_SAMPLERATE_48KHZ);
    sndSetHooks(&hooks);
    sndActive(0x30, 0x30, 0x30, 0, 0x700000);
    g_soundSEVol = 0x5A;
    lbl_802F1DF4 = 0x5A;
    sndVolume(0.01f * (127.0f * g_soundSEVol), 0, 0xFF);
    g_soundBGMVol = 0x50;
    u_volumeRelated2 = 0x50;
    DTKSetVolume(u_volumeRelated3 * (0.01f * g_soundBGMVol), u_volumeRelated3 * (0.01f * g_soundBGMVol));
    lbl_802F1D58 = 0;
    lbl_802F1D40 = 1.0f;
    lbl_802F1D44 = 1.0f;
    lbl_802F1D4C = 1.0f;
    lbl_802F1D50 = 1.0f;
    lbl_802F1D54 = 1.0f;
    for (i = 0; i < 9; i++)
        u_isSoundGroupDataLoaded[i] = FALSE;
    g_loadedSoundGroupsCount = 0;
    s_loadedSoundGroupIDs[0] = -1;
    s_loadedSoundGroupIDs[1] = -1;
    s_loadedSoundGroupIDs[2] = -1;
    s_loadedSoundGroupIDs[3] = -1;
    s_loadedSoundGroupIDs[4] = -1;
    s_loadedSoundGroupIDs[5] = -1;
    s_loadedSoundGroupIDs[6] = -1;
    s_loadedSoundGroupIDs[7] = -1;
    SoundGroupLoad(SOUND_GRPse01);
    SoundGroupLoad(SOUND_GRPcomn_set);
    lbl_801F8F38.tempDisableFX = 0;
    lbl_801F8F38.time = 3.0f;
    lbl_801F8F38.preDelay = 0.1f;
    lbl_801F8F38.damping = 0.6f;
    lbl_801F8F38.coloration = 0.9f;
    lbl_801F8F38.crosstalk = 0.0f;
    lbl_801F8F38.mix = 0.65f;
    lbl_801F9118.unk390 = 0xF;
    lbl_801F9118.unk394 = 0;
    lbl_801F9118.unk398 = 0x1F4;
    sndAuxCallbackPrepareReverbHI(&lbl_801F8F38);
    sndAuxCallbackPrepareChorus(&lbl_801F9118.unk300);

    sndSetAuxProcessingCallbacks(
        0,
        sndAuxCallbackReverbHI,
        &lbl_801F8F38,
        0xFF,
        0,
        sndAuxCallbackChorus,
        &lbl_801F9118.unk300,
        0xFF,
        0);
    sndEfcRev = 0;
    sndEfcCho = 0;
    sndMasterVolume(0x7F, 0, 1, 1);
    sndVolume(0.01f * (127.0f * g_soundSEVol), 0, 0xFF);

    for (i2 = 0; i2 < 4; i2++)
    {
        for (j = 0; j < 0x425; j++)
            s_voiceIDs[i2][j] = -1;
    }

    lbl_802F1D3A = -1;
    lbl_801FE558.unk50[0].unk0 = 0;
    lbl_801FE558.unk50[1].unk0 = 0;
    lbl_801FE558.unk50[2].unk0 = 0;
    lbl_801FE558.unk50[3].unk0 = 0;
    AISetStreamSampleRate(AI_SAMPLERATE_48KHZ);
    DTKInit();
    if (OSGetSoundMode() == OS_SOUND_MODE_MONO)
        queue_dtk_tracks(1, SND_OUTPUTMODE_MONO);
    else
        queue_dtk_tracks(1, SND_OUTPUTMODE_SURROUND);
    DTKSetRepeatMode(DTK_MODE_REPEAT1);
    lbl_802F1D64 = 0;
    lbl_802F1D68 = 0;
    lbl_802F1D6C = 0;
    lbl_802F1D70 = 0;
    lbl_802F1D74 = 1;
    lbl_802014E0.unk0 = -1;
    lbl_802F1DBC = 0;
    u_someSoundGroupId = 0;
    u_someSoundId = 1;
}

void sound_main(void)
{
    struct Struct801B2A5C *temp_r29 = &lbl_801B2A5C[lbl_802F1D64];
    u8 unused[8];

    lbl_802014E0.dtkState = DTKGetState();
    if (lbl_802014E0.dtkState != DTK_STATE_BUSY && lbl_802F1D70 == 0)
    {
        if (temp_r29->unkC != -1)
        {
            if (temp_r29->unk8 != 0 && AIGetStreamSampleCount() > temp_r29->unk8 + lbl_802F1DD0 * 256)
            {
                lbl_802F1D68 = temp_r29->unkC;
                lbl_802F1D6C = lbl_802F1D68 - lbl_802F1D64;
                lbl_802F1D70 = 1;
                lbl_802F1D74 = -1;
            }
        }
        else if (AIGetStreamSampleCount() > temp_r29->unk8)
            u_play_music(-1, 1);
    }

    if (lbl_802014E0.dtkState != DTK_STATE_BUSY)
    {
        if (lbl_802F1D70 > 0)
        {
            if (--lbl_802F1D70 == 0)
            {
                switch (lbl_802F1D74)
                {
                case -4:
                    DTKSetState(DTK_STATE_STOP);
                    AIResetStreamSampleCount();
                    lbl_802F1D74 = 0;
                    lbl_802F1D70 = 1;
                    lbl_802F1D44 = 1.0f;
                    lbl_802F1D40 = 1.0f;
                    lbl_802F1D48 = 1;
                    lbl_802F1D54 = lbl_802F1D4C;
                    lbl_802F1D50 = 1.0f;
                    lbl_802F1D58 = 0xF;
                    break;
                case -3:
                    if (lbl_802014E0.unk4 != 0 && lbl_802F1D64 != lbl_802F1D68)
                    {
                        lbl_802F1D44 = lbl_802F1D40;
                        u_play_music(0x1E, 2);
                        lbl_802F1D74 = -4;
                        lbl_802F1D70 = 0x1E;
                    }
                    else
                    {
                        lbl_802F1D74 = -1;
                        lbl_802F1D70 = 1;
                        lbl_802F1D44 = 1.0f;
                        lbl_802F1D40 = 1.0f;
                        lbl_802F1D48 = 1;
                        lbl_802F1D54 = lbl_802F1D4C;
                        lbl_802F1D50 = 1.0f;
                        lbl_802F1D58 = 0xF;
                    }
                    break;
                case -2:
                    func_8002CEB8(lbl_802F1D4C * (u_volumeRelated3 * lbl_802F1D40));
                    lbl_802F1D74 = 0;
                    break;
                case -1:
                    DTKSetState(DTK_STATE_STOP);
                    AIResetStreamSampleCount();
                    lbl_802F1D74 = 0;
                    lbl_802F1D70 = 1;
                    break;
                case 0:
                    if (lbl_802F1D6C > 0)
                    {
                        DTKNextTrack();
                        lbl_802F1D70 = 1;
                        lbl_802F1D64 += 1;
                        lbl_802F1D6C -= 1;
                    }
                    else if (lbl_802F1D6C < 0)
                    {
                        DTKPrevTrack();
                        lbl_802F1D70 = 1;
                        lbl_802F1D64 -= 1;
                        lbl_802F1D6C += 1;
                    }
                    else
                    {
                        DTKSetState(DTK_STATE_RUN);
                        AIResetStreamSampleCount();
                        lbl_802F1D74 = -2;
                        lbl_802F1D70 = 1;
                        u_volumeRelated3 = lbl_802014E0.unkC;
                        lbl_802014E0.unk4 = 0U;
                    }
                    break;
                case 1:
                    DTKSetState(DTK_STATE_STOP);
                    AIResetStreamSampleCount();
                    lbl_802F1D74 = 0;
                    lbl_802014E0.unk4 = 0U;
                    break;
                case 5:
                    if (lbl_802014E0.dtkState == DTK_STATE_RUN)
                        DTKSetState(DTK_STATE_PAUSE);
                    else if (lbl_802014E0.dtkState == DTK_STATE_PAUSE)
                        DTKSetState(DTK_STATE_RUN);
                    lbl_802F1D74 = 0;
                    break;
                }
            }
        }
    }
    if (lbl_802014E0.unk0 != -1 && lbl_802014E0.dtkState == DTK_STATE_RUN)
        lbl_802014E0.unk4++;
    u_someSoundGroupName = g_soundGroupDesc[u_someSoundGroupId].groupName;
    u_someSoundName = g_soundDesc[u_someSoundId].name;
}

void SoundGroupLoad(int groupId)
{
    char poolFileName[0x100];
    char projFileName[0x100];
    char sampFileName[0x100];
    char sdirFileName[0x100];
    u32 bytesRead;
    s32 temp_cr0_eq;
    OSHeapHandle heap;
    u32 temp_r7;
    const struct SoundGroupDesc *grpDesc;
    int i;

    if (g_loadedSoundGroupsCount == 8)
        return;
    grpDesc = &g_soundGroupDesc[groupId];
    if (u_isSoundGroupDataLoaded[grpDesc->index])
        return;

    // check if already loaded
    for (i = 0; i < 8; i++)
    {
        if (groupId == s_loadedSoundGroupIDs[i])
            return;
    }

    sprintf(poolFileName, "%s.pool", grpDesc->baseName);
    sprintf(projFileName, "%s.proj", grpDesc->baseName);
    sprintf(sampFileName, "%s.samp", grpDesc->baseName);
    sprintf(sdirFileName, "%s.sdir", grpDesc->baseName);

    DVDChangeDir("snd/mkb");
    s_groupIsUsingCharaHeap = FALSE;
    s_sampData[grpDesc->index] = ReadMusyXData(sampFileName, &bytesRead);
    if (s_sampData[grpDesc->index] == NULL)
    {
        u32 freeSpace = OSCheckHeap(mainHeap);
        if (freeSpace == -1U)
            sound_error("SoundGroupLoad\n", "RAM ERROR ! \"%s\"(0x%X) MainHeap error\n", grpDesc->baseName, bytesRead);
        else if (bytesRead < freeSpace)
            sound_error("SoundGroupLoad\n", "RAM ALLOC ERROR ! \"%s\"(0x%X) MainHeap:0x%X\n", grpDesc->baseName, bytesRead, freeSpace);
        else
            sound_error("SoundGroupLoad\n", "RAM SIZE OVER ! \"%s\"(0x%X) need 0x%X ... MainHeap:0x%X\n", grpDesc->baseName, bytesRead, bytesRead - freeSpace, freeSpace);
        s_groupIsUsingCharaHeap = TRUE;
        heap = OSSetCurrentHeap(charaHeap);
        s_sampData[grpDesc->index] = ReadMusyXData(sampFileName, &bytesRead);
        OSSetCurrentHeap(heap);
        if (s_sampData[grpDesc->index] == NULL)
        {
            freeSpace = OSCheckHeap(charaHeap);
            if (freeSpace == -1U)
                sound_error("SoundGroupLoad\n", "RAM ERROR ! \"%s\"(0x%X) CharaHeap error\n", grpDesc->baseName, bytesRead);
            else if (bytesRead < freeSpace)
                sound_error("SoundGroupLoad\n", "RAM ALLOC ERROR ! \"%s\"(0x%X) CharaHeap:0x%X\n", grpDesc->baseName, bytesRead, freeSpace);
            else
                sound_error("SoundGroupLoad\n", "RAM SIZE OVER ! \"%s\"(0x%X) need 0x%X ... CharaHeap:0x%X\n", grpDesc->baseName, bytesRead, bytesRead - freeSpace, freeSpace);
            DVDChangeDir("/test");
            return;
        }
        printf("--> but SUCCESS !! use CharaHeap\n");
    }

    s_sampDataSizes[grpDesc->index] = bytesRead;
    g_soundTotalBytesLoaded += s_sampDataSizes[grpDesc->index];

    temp_r7 = g_soundAramTop + s_sampDataSizes[grpDesc->index];
    if (temp_r7 >= 0x700000U)
    {
        sound_error("SoundGroupLoad\n", "ARAM SIZE OVER ! \"%s\"(0x%X) need 0x%X\n", grpDesc->baseName, s_sampDataSizes[grpDesc->index], temp_r7 + 0xFF900000);
        if (s_groupIsUsingCharaHeap)
            heap = OSSetCurrentHeap(charaHeap);
        OSFree(s_sampData[grpDesc->index]);
        if (s_groupIsUsingCharaHeap)
            OSSetCurrentHeap(heap);
        g_soundTotalBytesLoaded -= s_sampDataSizes[grpDesc->index];
        DVDChangeDir("/test");
        return;
    }

    s_poolData[grpDesc->index] = ReadMusyXData(poolFileName, &bytesRead);
    s_poolDataSizes[grpDesc->index] = bytesRead;
    g_soundTotalBytesLoaded += s_poolDataSizes[grpDesc->index];

    s_projData[grpDesc->index] = ReadMusyXData(projFileName, &bytesRead);
    s_projDataSizes[grpDesc->index] = bytesRead;
    g_soundTotalBytesLoaded += s_projDataSizes[grpDesc->index];

    s_sdirData[grpDesc->index] = ReadMusyXData(sdirFileName, &bytesRead);
    s_sdirDataSizes[grpDesc->index] = bytesRead;
    g_soundTotalBytesLoaded += s_sdirDataSizes[grpDesc->index];

    temp_cr0_eq = sndPushGroup(
        s_projData[grpDesc->index],
        (u16)grpDesc->unk8,
        s_sampData[grpDesc->index],
        s_sdirData[grpDesc->index],
        s_poolData[grpDesc->index]);
    g_soundAramTop += s_sampDataSizes[grpDesc->index];
    if (temp_cr0_eq == 0)
    {
        DVDChangeDir("/test");
        return;
    }
    if (s_groupIsUsingCharaHeap)
        heap = OSSetCurrentHeap(charaHeap);
    OSFree(s_sampData[grpDesc->index]);
    if (s_groupIsUsingCharaHeap)
        OSSetCurrentHeap(heap);

    g_soundTotalBytesLoaded -= s_sampDataSizes[grpDesc->index];
    u_isSoundGroupDataLoaded[grpDesc->index] = TRUE;
    DVDChangeDir("/test");
    s_loadedSoundGroupIDs[g_loadedSoundGroupsCount++] = groupId;
}

// Frees the most recently loaded sound group
void SoundGroupFree(void)
{
    const struct SoundGroupDesc *grpDesc;

    if (g_loadedSoundGroupsCount > 2)
    {
        grpDesc = &g_soundGroupDesc[s_loadedSoundGroupIDs[--g_loadedSoundGroupsCount]];
        sndPopGroup();
        g_soundAramTop -= s_sampDataSizes[grpDesc->index];
        OSFree(s_poolData[grpDesc->index]);
        g_soundTotalBytesLoaded -= s_poolDataSizes[grpDesc->index];
        OSFree(s_projData[grpDesc->index]);
        g_soundTotalBytesLoaded -= s_projDataSizes[grpDesc->index];
        OSFree(s_sdirData[grpDesc->index]);
        g_soundTotalBytesLoaded -= s_sdirDataSizes[grpDesc->index];
        u_isSoundGroupDataLoaded[grpDesc->index] = 0;
        s_loadedSoundGroupIDs[g_loadedSoundGroupsCount] = -1;
    }
}

#ifdef NONMATCHING
// https://decomp.me/scratch/8mIVf
void ev_sound_init(void)
{
    struct Struct801FE498 *ptr;
    int i;
    int j;
    int i2;

    SoundEffect(0, 0);
    ptr = lbl_801FE498;
    for (i = 0; i < 16; i++, ptr++)
    {
        ptr->unk0 = -1;
        ptr->unk2 = 0;
        ptr->unk4 = 0;
        ptr->unk5 = 0;
        ptr->unk6 = 0;
        ptr->unk7 = 0;
        ptr->unk8 = 0;
    }

    for (i2 = 0; i2 < 4; i2++)
    {
        for (j = 0; j < 20; j++)
            lbl_801FE558.unk0[i2][j] = 0;
    }

    lbl_802F1DC4 = 0;
    lbl_802F1DC8 = 0;
    lbl_802F1DFC = -1;
    u_somePlayerId = (u32) -1;

}
#else
asm void ev_sound_init(void)
{
    nofralloc
#include "../asm/nonmatchings/ev_sound_init.s"
}
#pragma peephole on
#endif

void ev_sound_main(void)
{
    int var_r24;
    int var_r23;

    if (lbl_802F1D58 > 0)
    {
        lbl_802F1D58--;
        lbl_802F1D4C = lbl_802F1D50 - ((lbl_802F1D58 / 60.0) * (lbl_802F1D50 - lbl_802F1D54));
        u_play_music((u32)(u_volumeRelated3 * lbl_802F1D40), 4);
    }
    if (lbl_802014E0.dtkState != DTK_STATE_BUSY)
    {
        if (lbl_802014E0.unk14 > 0)
        {
            if (!(debugFlags & 2))
                lbl_802014E0.unk14--;
            if (lbl_802014E0.unk10 == 2)
            {
                if (lbl_802014E0.unk14 == 0 && lbl_802F1D74 >= 0)
                {
                    double zero = 0.0;
                    lbl_802F1D40 = zero;
                    u_play_music(-1, 1);
                }
                else
                {
                    lbl_802F1D40 = ((float)*(s32 *)&lbl_802014E0.unk14 / (float)lbl_802014E0.unk18) * lbl_802F1D44;
                    u_play_music((u32)(u_volumeRelated3 * lbl_802F1D40), 4);
                }
            }
            else
            {
                if (lbl_802014E0.unk14 == 0)
                {
                    lbl_802F1D48 = 1;
                    lbl_802F1D40 = 1.0f;
                    u_play_music((u32)u_volumeRelated3, 4);
                }
                else
                {
                    lbl_802F1D40 = 1.0f - ((float)lbl_802014E0.unk14 / (float)lbl_802014E0.unk18);
                    u_play_music((u32)(u_volumeRelated3 * lbl_802F1D40), 4);
                }
            }
        }
    }

    for (var_r23 = 0; var_r23 < 4; var_r23++)
    {
        for (var_r24 = 0; var_r24 < 0x425; var_r24++)
        {
            if (s_voiceIDs[var_r23][var_r24] == -1U)
                continue;
            s_voiceIDs[var_r23][var_r24] = sndFXCheck(s_voiceIDs[var_r23][var_r24]);
            if (lbl_802F1D40 == 1.0 && lbl_802F1D48 == 0)
                continue;
            if (lbl_802F1D48 != 0)
            {
                sndFXCtrl(s_voiceIDs[var_r23][var_r24], 7, lbl_801FD404[var_r23][var_r24]);
                continue;
            }
            switch (g_soundDesc[var_r24].unk8)
            {
            case 4:
            case 9:
                sndFXCtrl(s_voiceIDs[var_r23][var_r24], 7, 0);
                break;
            default:
                sndFXCtrl(s_voiceIDs[var_r23][var_r24], 7, (s8)lbl_801FD404[var_r23][var_r24] * lbl_802F1D40);
                break;
            case 7:
            case 8:
            case 13:
            case 14:
            case 19:
                break;
            }
        }
    }

    if (lbl_802F1D48 != 0)
        lbl_802F1D48 = 0;
    if (g_soundSEVol != lbl_802F1DF4)
    {
        sndVolume(0.01f * (127.0f * g_soundSEVol), 0, 0xFF);
        lbl_802F1DF4 = g_soundSEVol;
    }
    if (g_soundBGMVol != u_volumeRelated2)
    {
        DTKSetVolume(
            u_volumeRelated3 * (0.01f * g_soundBGMVol),
            u_volumeRelated3 * (0.01f * g_soundBGMVol));
        u_volumeRelated2 = g_soundBGMVol;
    }
    if (!(debugFlags & 0xA))
    {
        int i;
        int j;
        int var_r21;
        struct Struct801FE498 *var_r22_2;

        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 20; j++)
            {
                if (lbl_801FE558.unk0[i][j] > 0)
                    lbl_801FE558.unk0[i][j]--;
            }
        }

        if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING)
            play_stage_sounds();
        func_8002A34C();

        var_r22_2 = lbl_801FE498;
        for (var_r21 = 0; var_r21 < 16; var_r21++, var_r22_2++)
        {
            if (var_r22_2->unk0 == -1)
                break;
            func_8002A964(var_r22_2);
            var_r22_2->unk0 = -1;
        }
    }
}

void ev_sound_dest(void)
{
    s32 j;
    s32 i;
    u8 unused[8];

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 0x425; j++)
        {
            if (s_voiceIDs[i][j] != -1U && g_soundDesc[j].unk8 != 7)
                sndFXKeyOff(s_voiceIDs[i][j]);
        }
    }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 0x425; j++)
            s_voiceIDs[i][j] = -1;
    }

    lbl_801FE558.unk50[0].unk0 = 0;
    lbl_801FE558.unk50[1].unk0 = 0;
    lbl_801FE558.unk50[2].unk0 = 0;
    lbl_801FE558.unk50[3].unk0 = 0;
    lbl_802F1DC4 = 0;
    lbl_802F1DC8 = 0;
}

static u8 lbl_802F07E4[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static u8 lbl_802F07EC[8] = { 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00 };
static u8 lbl_802F07F4[8] = { 0x00, 0x00, 0xC0, 0xC0, 0x3F, 0xC0, 0x00, 0x00 };
static u8 lbl_802F07FC[8] = { 0xC0, 0x00, 0x3F, 0x00, 0xC0, 0xC0, 0x3F, 0xC0 };
static u8 lbl_802F0804[8] = { 0xC0, 0xC0, 0x00, 0x00, 0x3F, 0xC0, 0x00, 0x00 };
static u8 lbl_802F080C[8] = { 0xC0, 0x00, 0x3F, 0x00, 0x00, 0xC0, 0x00, 0x00 };
static u8 lbl_802F0814[8] = { 0xC0, 0x00, 0x3F, 0x00, 0xC0, 0xC0, 0x00, 0x00 };

static u8 *lbl_801B3630[] =
{
    lbl_802F07E4,
    lbl_802F07EC,
    lbl_802F07F4,
    lbl_802F07FC,
    lbl_802F07E4,
    lbl_802F07EC,
    lbl_802F0804,
    lbl_802F07FC,
    lbl_802F07E4,
    lbl_802F07EC,
    lbl_802F080C,
    lbl_802F07FC,
    lbl_802F07E4,
    lbl_802F07EC,
    lbl_802F0814,
    lbl_802F07FC,
};

static s8 func_8002A22C(int arg0, int arg1)
{
    int temp_r4;

    if (func_8009D5D8() != 0)
        return 0;
    if (arg0 == 0)
    {
        if (lbl_802F1D5C[arg1] != 100)
            return lbl_802F1D5C[arg1];
    }
    else
    {
        if (lbl_802F1D60[arg1] != 100)
            return lbl_802F1D60[arg1];
    }

    if (modeCtrl.unk30 == 0)
        return 0;

    temp_r4 = (modeCtrl.unk30 - 1);
    temp_r4 += (modeCtrl.splitscreenMode * 4);
    if (arg0 == 0)
        return lbl_801B3630[temp_r4][arg1 * 2 + 0];
    else
        return lbl_801B3630[temp_r4][arg1 * 2 + 1];
}

#pragma dont_inline on
static float func_8002A324(int arg0)
{
    float temp_f1;

    temp_f1 = lbl_801FE5C8[arg0];
    if (temp_f1 == -1.0f)
        return 1.0f;
    else
        return temp_f1;
}
#pragma dont_inline reset

static float lbl_801B3670[] = { 1, 1, 1, 1, 0, 1, 1, 1, 1 };

static void func_8002A34C(void)
{
    int var_r3;
    float var_f31;
    float var_f30;
    int var_r27;
    int i;
    int var_r24;
    int var_r23;
    int var_r22;
    int j;
    float spC[4];
    s8 sp9;
    s8 sp8;

    var_r27 = -1;
    var_r24 = -1;
    var_r23 = -1;
    var_r22 = 0;
    if (modeCtrl.unk30 == 0)
        return;
    for (i = 0; i < 4; i++)
    {
        lbl_802F1D5C[i] = 100;
        lbl_802F1D60[i] = 100;
        lbl_801FE5C8[i] = -1.0f;
    }
    if (modeCtrl.gameType == GAMETYPE_MINI_RACE)
    {
        var_r27 = modeCtrl.unk30;
        var_r3 = 0;
        if (modeCtrl.unk30 == 3 && modeCtrl.splitscreenMode == SPLITSCREEN_4_SPLIT)
            var_r3 = 1;
        var_r24 = var_r3 ? 4 : modeCtrl.unk30;
        var_f31 = lbl_801B3670[modeCtrl.gameType];
        var_r23 = 3;
    }
    else if (modeCtrl.gameType == GAMETYPE_MINI_FIGHT)
    {
        var_f31 = lbl_801B3670[modeCtrl.gameType];
        var_r27 = 0;
        var_r24 = 1;
        var_r23 = 3;
    }

    if (var_r27 == -1)
        return;

    for (i = var_r27; i <= var_r23; i++)
    {
        var_f30 = 1.0e7f;

        for (j = 0; j < var_r24; j++)
        {
            float temp_f1;
            float var_f0;

            temp_f1 = mathutil_vec_distance(&cameraInfo[j].eye, &ballInfo[i].pos);
            var_f0 = 1.0f - ((temp_f1 * var_f31) / 100.0f);
            spC[j] = CLAMP(var_f0, 0.0f, 1.0f);
            if (temp_f1 > var_f30)
                continue;
            var_f30 = temp_f1;
            var_r22 = j;
        }
        lbl_801FE5C8[i] = spC[var_r22];
    }

    if (modeCtrl.gameType == 4)
    {
        for (i = var_r27; i <= var_r23; i++)
            func_8002B634(var_r22, &ballInfo[i].pos, &lbl_802F1D5C[i], &lbl_802F1D60[i]);
        return;
    }

    for (i = var_r27; i <= var_r23; i++)
    {
        lbl_802F1D5C[i] = 0;
        lbl_802F1D60[i] = 0;
        if (modeCtrl.unk30 == 1)
            func_8002B634(var_r22, &ballInfo[i].pos, &lbl_802F1D5C[i], &lbl_802F1D60[i]);
        else if (modeCtrl.unk30 == 2)
        {
            for (j = 0; j < var_r24; j++)
            {
                func_8002B634(j, &ballInfo[i].pos, &sp9, &sp8);
                lbl_802F1D5C[i] += sp9 * spC[j];
                lbl_802F1D60[i] += spC[j] * func_8002A22C(1, j);
            }
            lbl_802F1D5C[i] /= (float)var_r24;
            lbl_802F1D60[i] /= (float)var_r24;
        }
        else
        {
            for (j = 0; j < var_r24; j++)
            {
                lbl_802F1D5C[i] += spC[j] * func_8002A22C(0, j);
                lbl_802F1D60[i] += spC[j] * func_8002A22C(1, j);
            }
            lbl_802F1D5C[i] /= (float)var_r24;
            lbl_802F1D60[i] /= (float)var_r24;
        }
    }
}

static void func_8002A964(struct Struct801FE498 *arg0)
{
    int temp_r0;
    u8 temp_r31;
    int var_r5;
    s8 var_r9;
    s8 temp_r9;
    const struct SoundDesc *sndDesc;
    SND_PARAMETER_INFO sp1C;
    SND_PARAMETER spC[4];
    int temp;

    temp_r0 = g_soundDesc[arg0->unk0].unk0;
    sndDesc = &g_soundDesc[arg0->unk2];
    if (temp_r0 != -1)
    {
        if (arg0->unk5 > 63)
            arg0->unk5 = -(128 - arg0->unk5);
        if (arg0->unk6 > 63)
            arg0->unk6 = -(128 - arg0->unk6);
        temp_r9 = sndDesc->unk8;
        var_r5 = (temp_r9 == 7 || temp_r9 == 0xD || temp_r9 == 8 || temp_r9 == 14 || temp_r9 == 19);
        temp_r31 = var_r5 ? arg0->unk4 : arg0->unk4 * lbl_802F1D40;

        var_r9 = 0;
        spC[var_r9].ctrl = 0x83;
        spC[var_r9].paraData.value7 = arg0->unk6 + 0x40;
        var_r9++;
        if (arg0->unk7 != 0)
        {
            spC[var_r9].ctrl = 0x80;
            spC[var_r9].paraData.value14 = (s16) (((s8) arg0->unk7 << 0xC) + 0x1FFF);
            var_r9++;
        }
        if ((sndDesc->unk8 != 7) && (sndDesc->unk8 != 0xC))
        {
            spC[var_r9].ctrl = 0x5B;
            spC[var_r9].paraData.value7 = sndEfcRev;
            var_r9++;
            spC[var_r9].ctrl = 0x5D;
            spC[var_r9].paraData.value7 = sndEfcCho;
            var_r9++;
        }
        sp1C.numPara = var_r9;
        sp1C.paraArray = spC;
        temp = sndFXStartParaInfo(temp_r0, temp_r31, arg0->unk5 + 0x40, 0, &sp1C);
        s_voiceIDs[arg0->unk8][arg0->unk2] = temp;
        lbl_801FD404[arg0->unk8][arg0->unk2] = temp_r31;
    }
}

static s8 lbl_802F081C[4] = { 0, 1, -1, 2 };

static int PlayerAutoSurround(u32 *arg0, const struct SoundDesc *sndDesc, u32 arg2)
{
    float var_f2;
    int var_r3;
    int var_r3_2;
    s32 r26;

    r26 = (*arg0 >> 11) & 0x7F;
    if (r26 > 63)
        r26 = -(128 - r26);
    *arg0 &= ~(0x7F << 11);
    var_r3_2 = CLAMP(r26 + 100, 0, 127);
    *arg0 |= (var_r3_2 & 0x7F) << 11;

    if (modeCtrl.unk30 == 0)
        return 0;

    if (gameMode != MD_SEL)
    {
        switch (sndDesc->unk8)
        {
        case 2:
        case 3:
        case 6:
        case 10:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            r26 = (*arg0 >> 18) & 0x7F;
            *arg0 &= ~(0x7F << 18);
            var_r3_2 = func_8002A22C(0, arg2);
            if (var_r3_2 < 0)
                var_r3_2 += 128;
            *arg0 |= ((var_r3_2 + r26) & 0x7F) << 18;

            r26 = (*arg0 >> 25) & 0x7F;
            *arg0 &= ~(0x7F << 25);
            var_r3_2 = func_8002A22C(1, arg2);
            if (var_r3_2 < 0)
                var_r3_2 += 128;
            *arg0 |= (var_r3_2 + r26) << 25;

            r26 = (*arg0 >> 11) & 0x7F;
            *arg0 &= ~(0x7F << 11);
            var_f2 = lbl_801FE5C8[arg2] != -1.0f ? lbl_801FE5C8[arg2] : 1.0f;
            var_r3_2 = (r26 * var_f2 < 0.0f) ? 0.0f : (r26 * func_8002A324(arg2) > 127.0f ? 127.0f : r26 * func_8002A324(arg2));
            *arg0 |= (var_r3_2 & 0x7F) << 11;
            break;
        }
    }
    var_r3 = 0;
    switch (sndDesc->unk8)
    {
    case 2:
    case 3:
    case 6:
    case 10:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
        if (modeCtrl.unk30 > 1)
            var_r3 = lbl_802F081C[arg2];
        break;
    }
    return var_r3;
}

static int func_8002AE58(u32 *arg0, const struct SoundDesc *sndDesc, u32 arg2)
{
    int i;
    int j;
    const struct SoundDesc *var_r25;

    switch (sndDesc->unk8)
    {
    case 1:
        return 1;
    case 4:
    case 9:
    case 11:
        if (s_voiceIDs[arg2][*arg0 & 0x7FF] != -1U)
            return 1;
        break;
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
        if (s_voiceIDs[arg2][*arg0 & 0x7FF] != -1U)
        {
            sndFXKeyOff(s_voiceIDs[arg2][*arg0 & 0x7FF]);
            s_voiceIDs[arg2][*arg0 & 0x7FF] = -1U;
        }
        break;
    case 8:
    case 13:
    case 14:
        for (i = 0; i < 4; i++)
        {
            var_r25 = g_soundDesc;
            for (j = 0; j < 0x425; j++, var_r25++)
            {
                if (s_voiceIDs[i][j] != -1U)
                {
                    if (sndDesc->unk8 == 8 || sndDesc->unk8 == 14)
                    {
                        if (var_r25->unk8 != 8 && var_r25->unk8 != 14)
                            continue;
                    }
                    else
                    {
                        if (sndDesc->unk8 != var_r25->unk8)
                            continue;
                    }
                    sndFXKeyOff(s_voiceIDs[i][j]);
                    s_voiceIDs[i][j] = -1;
                }
            }
        }
    }
    return 0;
}

static int u_get_some_player_id(void)
{
    int var_r6 = u_somePlayerId;

    if (var_r6 != -1)
        u_somePlayerId = -1;
    else if (currentBall == NULL)
        return -1;
    else
        return currentBall->playerId;
    return var_r6;
}

int SoundReq(u32 soundParam)
{
    int i;
    int var_r0_2;
    int temp_r3_2;
    s32 playerId;
    struct Struct801FE498 *var_r29;
    const struct SoundDesc *sndDesc;

    sndDesc = &g_soundDesc[soundParam & 0x7FF];
    playerId = u_get_some_player_id();
    if (playerId == -1)
    {
        printf("SoundReq %s nowball is NULL. --> pid = 0\n", sndDesc->name);
        playerId = 0;
    }
    if (func_8002AE58(&soundParam, sndDesc, playerId) != 0)
        return -1;
    if (modeCtrl.gameType != 6 && lbl_801FE558.unk0[playerId][sndDesc->unk8] > 0)
        return -1;

    var_r29 = lbl_801FE498;
    for (i = 0; i < 16; i++, var_r29++)
    {
        if (var_r29->unk0 == -1)
        {
            temp_r3_2 = PlayerAutoSurround(&soundParam, sndDesc, playerId);
            var_r29->unk0 = soundParam & 0x7FF;
            var_r29->unk2 = (lbl_802F1D3A == -1) ? var_r29->unk0 : lbl_802F1D3A;
            var_r29->unk4 = (soundParam >> 11) & 0x7F;
            var_r29->unk5 = (soundParam >> 18) & 0x7F;
            var_r29->unk6 = (u8)(soundParam >> 25);
            var_r29->unk7 = temp_r3_2;
            var_r29->unk8 = playerId;
            if (sndDesc->unk8 != 0)
                lbl_801FE558.unk0[playerId][sndDesc->unk8] = sndDesc->unkA;
            lbl_802F1D3A = -1;
            var_r0_2 = var_r29->unk4 + lbl_80201500[var_r29->unk0];
            var_r29->unk4 = CLAMP(var_r0_2, 0, 127);
            return var_r29->unk2;
        }
    }
    lbl_802F1D3A = -1;
    printf("warning : SoundReq() cue over %s\n", sndDesc->name);
    return -1;
}

int SoundReqDirect(u32 soundParam)
{
    struct Struct801FE498 sp10;
    int var_r0;
    int temp_r3_2;
    struct Ball *temp_r3;
    s32 var_r31;
    const struct SoundDesc *sndDesc;

    sndDesc = &g_soundDesc[soundParam & 0x7FF];
    var_r31 = u_get_some_player_id();
    if (var_r31 == -1)
    {
        printf("SoundReqDirect %s ape is NULL. --> pid = 0\n", sndDesc->name);
        var_r31 = 0;
    }
    if (func_8002AE58(&soundParam, sndDesc, var_r31) != 0)
        return -1;
    temp_r3_2 = PlayerAutoSurround(&soundParam, sndDesc, var_r31);
    sp10.unk0 = soundParam & 0x7FF;
    sp10.unk2 = (lbl_802F1D3A == -1) ? sp10.unk0 : lbl_802F1D3A;
    sp10.unk4 = (soundParam >> 11) & 0x7F;
    sp10.unk5 = (soundParam >> 18) & 0x7F;
    sp10.unk6 = (u8)(soundParam >> 25);
    sp10.unk7 = temp_r3_2;
    sp10.unk8 = var_r31;
    lbl_802F1D3A = -1;
    var_r0 = sp10.unk4 + lbl_80201500[sp10.unk0];
    sp10.unk4 = CLAMP(var_r0, 0, 127);
    func_8002A964(&sp10);
    return sp10.unk2;
}

static int SoundReqID(u32 soundParam, s32 arg1)
{
    int var_r0;
    int var_r5;
    int soundId;
    int i;
    int j;
    int var_r30;
    s8 var_r8;
    const struct SoundDesc *sndDesc;
    s8 temp;

    soundId = soundParam & 0x7FF;
    sndDesc = g_soundDesc;
    var_r0 = -1;
    var_r30 = 0;
    var_r5 = 0;
    for (i = 0; i < 0x425; i++, var_r5++, sndDesc++)
    {
        if (sndDesc->unkC == soundId)
        {
            if (sndDesc->unk8 >= 15 && gameSubmode == SMD_ADV_RANKING_MAIN)
                return -1;
            if (sndDesc->unk8 >= 15)
            {
                var_r30++;
                if (lbl_802F1DFC != -1)
                {
                    if (lbl_802F1DFC + 1 != var_r30)
                        continue;
                    lbl_802F1DFC = -1;
                }
                else if (currentBall->ape == NULL)
                {
                    printf("SoundReqID %s ERROR !! ape is NULL.\n", sndDesc->name);
                    return -1;
                }
                else if (currentBall->ape->charaId + 1 != var_r30)
                    continue;
                if (var_r30 == 4)
                {
                    temp = (soundParam >> 11) & 0x7F;
                    soundParam &= ~(0x7F << 11);
                    temp += 20;
                    soundParam |= (temp & 0x7F) << 11;
                }
            }
            lbl_802F1D3A = var_r5;
            if ((u32)sndDesc->unk0 == -1U)
            {
                for (j = var_r5; j >= 0; j--, sndDesc--)
                {
                    if ((u32)sndDesc->unk0 != -1U)
                        break;
                    var_r5--;
                }
            }
            var_r8 = 0;
            switch (g_soundDesc[var_r5].unk0)
            {
            case 0xB2:
            case 0xB8:
            case 0xB9:
                var_r8 = 27;
                break;
            case 0xCE:
            case 0xCF:
                var_r8 = 10;
                break;
            }
            if (sndDesc->unk8 == 8 || sndDesc->unk8 == 14 || sndDesc->unk8 >= 15)
                var_r8 = 10;
            if (var_r8 != 0)
            {
                temp = (soundParam >> 11) & 0x7F;
                soundParam &= ~(0x7F << 11);
                temp += var_r8;
                soundParam |= (temp & 0x7F) << 11;
            }
            soundParam = (soundParam & ~0x7FF) | var_r5;
            switch (arg1)
            {
            case 2:
            case 0:
                var_r0 = SoundReq(soundParam);
                break;
            case 1:
            case 3:
                var_r0 = SoundReqDirect(soundParam);
                break;
            }
            return var_r0;
        }
    }
    return -1;
}

void u_play_sound_0(int soundParam)
{
    SoundReqID(soundParam, 0);
}

void u_play_sound_1(int soundParam)
{
    SoundReqID(soundParam, 1);
}

int u_play_sound_2(int soundParam)
{
    return SoundReqID(soundParam, 2);
}

int u_play_sound_1_dupe(int soundParam)
{
    return SoundReqID(soundParam, 1);
}

int func_8002B634(int arg0, Vec *arg1, s8 *arg2, s8 *arg3)
{
    struct Camera *temp_r26;
    Vec sp18;
    s16 temp_r31;
    s16 temp_r30;
    float dist;
    int ret;

    temp_r26 = &cameraInfo[arg0];
    sp18.x = arg1->x - temp_r26->eye.x;
    sp18.y = arg1->y - temp_r26->eye.y;
    sp18.z = arg1->z - temp_r26->eye.z;
    temp_r30 = mathutil_atan2(sp18.x, sp18.z) - 0x8000;
    temp_r31 = mathutil_atan2(sp18.y, mathutil_sqrt(mathutil_sum_of_sq_2(sp18.x, sp18.z)));
    temp_r30 = temp_r30 - temp_r26->rotY;
    temp_r31 = temp_r31 - temp_r26->rotX;

    dist = mathutil_vec_distance(&temp_r26->eye, arg1);
    ret = CLAMP(1.0f - (dist * lbl_801B3670[modeCtrl.gameType]) / 100.0f, 0.0f, 1.0f);
    if (abs(temp_r30) < 0x4000)
        *arg2 = 0.0039 * -temp_r30;
    else if (temp_r30 > 0)
        *arg2 = 0.0039 * -(0x8000 - temp_r30);
    else
        *arg2 = 0.0039 * -(-0x8000 - temp_r30);
    *arg3 = 0.0039 * -(abs(temp_r30) - 0x4000);
    if (modeCtrl.gameType == 6)
    {
        *arg2 = CLAMP(*arg2 * 10.0, -64.0, 63.0);
        *arg3 = CLAMP(*arg3 * 10.0, -64.0, 15.0);
    }
    else
    {
        *arg2 = CLAMP(*arg2 * 2.5, -64.0, 63.0);
        *arg3 = CLAMP(*arg3 * 2.5, -64.0, 15.0);
    }
    *arg3 = 0.6635 * (*arg3 + (0.00195 * temp_r31));
    return ret;
}

static s8 func_8002BB20(int arg0, Vec *arg1, s8 *arg2, s8 *arg3)
{
    float var_f31;
    float temp_f30;
    int i;
    int var_r28;
    s8 ret;
    float sp1C[4];
    s8 sp19;
    s8 sp18;

    var_r28 = arg0;
    ret = 0;
    temp_f30 = lbl_801B3670[modeCtrl.gameType];
    switch (modeCtrl.gameType)
    {
    case 2:
        break;
    case 1:
    case 3:
        if (arg0 == 3 && modeCtrl.splitscreenMode == 3)
            var_r28 = 4;
        break;
    }

    var_f31 = 1.0e7f;
    for (i = 0; i < var_r28; i++)
    {
        float dist = mathutil_vec_distance(&cameraInfo[i].eye, arg1);
        float var_f0 = 1.0f - ((dist * temp_f30) / 100.0f);
        sp1C[i] = CLAMP(var_f0, 0.0f, 1.0f);
        if (!(dist > var_f31))
        {
            var_f31 = dist;
            ret = sp1C[i];
        }
    }

    *arg2 = 0;
    *arg3 = 0;
    if (modeCtrl.unk30 == 1)
        func_8002B634(0, arg1, arg2, arg3);
    else if (modeCtrl.unk30 == 2)
    {
        for (i = 0; i < var_r28; i++)
        {
            func_8002B634(i, arg1, &sp19, &sp18);
            *arg2 += sp19 * sp1C[i];
            *arg3 += sp1C[i] * func_8002A22C(1, i);
        }
        *arg2 /= (float)var_r28;
        *arg3 /= (float)var_r28;
    }
    else
    {
        for (i = 0; i < var_r28; i++)
        {
            *arg2 += sp1C[i] * func_8002A22C(0, i);
            *arg3 += sp1C[i] * func_8002A22C(1, i);
        }
        *arg2 /= (float)var_r28;
        *arg3 /= (float)var_r28;
    }
    return ret;
}

void func_8002BFCC(u32 arg0, u32 arg1)
{
    u_somePlayerId = 0;
    SoundReqID(arg0 | 0x01000000, 0);
    u_somePlayerId = 0;
    SoundReqID(arg1 | 0xFC0000, 0);
}

static int get_some_id(const char *func, int arg1)
{
    int var_r3;

    if (g_soundDesc[arg1].unk8 == 13)
        return 0;
    else if (u_somePlayerId == -1 && currentBall == NULL)
    {
        printf("%s %s nowball is NULL. --> pid = 0\n", func, g_soundDesc[arg1].name);
        var_r3 = 0;
    }
    else if (u_somePlayerId != -1)
    {
        var_r3 = *(int *)&u_somePlayerId;
        u_somePlayerId = -1;
    }
    else
        var_r3 = currentBall->playerId;
    return var_r3;
}

void SoundOff(u16 arg0)
{
    int var_r3 = get_some_id("SoundOff", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
    {
        sndFXKeyOff(s_voiceIDs[var_r3][arg0 & 0xFFFF]);
        s_voiceIDs[var_r3][arg0 & 0xFFFF] = -1U;
    }
}

#pragma force_active on
void SoundVol(u16 arg0, u8 arg1)
{
    int var_r3 = get_some_id("SoundVol", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
        sndFXCtrl(s_voiceIDs[var_r3][arg0 & 0xFFFF], 7, arg1);
}
#pragma force_active reset

void SoundPan(u16 arg0, u8 arg1, u8 arg2)
{
    int var_r3 = get_some_id("SoundPan", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
    {
        sndFXCtrl(s_voiceIDs[var_r3][arg0 & 0xFFFF], 0x0A, arg1 + 64);
        sndFXCtrl(s_voiceIDs[var_r3][arg0 & 0xFFFF], 0x83, arg2 + 64);
    }
}

void SoundPitch(u16 arg0, u16 arg1)
{
    int var_r3 = get_some_id("SoundPitch", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
        sndFXCtrl14(s_voiceIDs[var_r3][arg0 & 0xFFFF], 0x80, arg1);
}

#pragma force_active on
void SoundDop(u16 arg0, u16 arg1)
{
    int var_r3 = get_some_id("SoundDop", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
        sndFXCtrl14(s_voiceIDs[var_r3][arg0 & 0xFFFF], 0x84, arg1);
}

char string_SoundMod[] = "SoundMod";

void SoundRev(u16 arg0, u8 arg1)
{
    int var_r3 = get_some_id("SoundRev", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
        sndFXCtrl(s_voiceIDs[var_r3][arg0 & 0xFFFF], 0x5B, arg1);
}

void SoundCho(u16 arg0, u8 arg1)
{
    int var_r3 = get_some_id("SoundCho", (u16)arg0);

    if (s_voiceIDs[var_r3][arg0 & 0xFFFF] != -1U)
        sndFXCtrl(s_voiceIDs[var_r3][arg0 & 0xFFFF], 0x5D, arg1);
}
#pragma force_active reset

static int SoundSearchID(int arg0)
{
    int i;
    int var_r9;
    const struct SoundDesc *sndDesc;

    sndDesc = g_soundDesc;
    var_r9 = 0;
    for (i = 0; i < 0x425; i++, sndDesc++)
    {
        if (sndDesc->unkC == arg0)
        {
            if (sndDesc->unk8 >= 0xF)
            {
                var_r9++;
                if (lbl_802F1DFC != -1)
                {
                    if (lbl_802F1DFC + 1 != var_r9)
                        continue;
                    lbl_802F1DFC = -1;
                }
                else
                {
                    if (currentBall->ape == NULL)
                    {
                        printf("SoundSearchID %s ERROR !! ape is NULL.\n", sndDesc->name);
                        return -1;
                    }
                    if (currentBall->ape->charaId + 1 != var_r9)
                        continue;
                }
            }
            return i;
        }
    }
    return -1;
}

void SoundOffID(int arg0_)
{
    int arg0 = SoundSearchID(arg0_);
    int var_r3 = get_some_id("SoundOffID", arg0);

    if (s_voiceIDs[var_r3][arg0] != -1U)
    {
        sndFXKeyOff(s_voiceIDs[var_r3][arg0]);
        s_voiceIDs[var_r3][arg0] = -1U;
    }
}

#pragma force_active on
char string_SoundVolID[] = "SoundVolID";
char string_SoundPanID[] = "SoundPanID";
char string_SoundPitchID[] = "SoundPitchID";
char string_SoundDopID[] = "SoundDopID";
char string_SoundModID[] = "SoundModID";
#pragma force_active reset

void SoundRevID(int arg0_, u8 arg1)
{
    int arg0 = SoundSearchID(arg0_);
    int var_r3 = get_some_id("SoundRevID", arg0);

    if (s_voiceIDs[var_r3][arg0] != -1U)
        sndFXCtrl(s_voiceIDs[var_r3][arg0], 0x5B, arg1);
}

void SoundChoID(int arg0_, u8 arg1)
{
    int arg0 = SoundSearchID(arg0_);
    int var_r3 = get_some_id("SoundChoID", arg0);

    if (s_voiceIDs[var_r3][arg0] != -1U)
        sndFXCtrl(s_voiceIDs[var_r3][arg0], 0x5D, arg1);
}

void SoundEffect(s8 arg0, s8 arg1)
{
    if (arg0 != -1)
        sndEfcRev = arg0;
    if (arg1 != -1)
        sndEfcCho = arg1;
}

struct Struct801B3938
{
    s32 unk0;
    s32 unk4;
};

static struct Struct801B3938 lbl_801B3938[][3] =
{
    {
        { -1, 0x00000051 },
        { -1, 0x00000052 },
        { -1, 0x00000053 },
    },
    {
        { -1, 0x00000051 },
        { -1, 0x00000052 },
        { -1, 0x00000053 },
    },
    {
        { -1, 0x00000051 },
        { -1, 0x00000052 },
        { -1, 0x00000053 },
    },
    {
        { -1, 0x00000051 },
        { -1, 0x00000052 },
        { -1, 0x00000053 },
    }
};

static struct Spline lbl_801B3998[] =
{
    {   0,   0.0f, 0.0f, 0.0f },
    { 127, 127.0f, 0.0f, 0.0f },
};

static struct Spline lbl_801B39B8[] =
{
    {   0, 127.0f, 0.0f, 0.0f },
    { 127, 127.0f, 0.0f, 0.0f },
};

static struct Spline lbl_801B39D8[] =
{
    {   0,   0.0f, 0.0f, 0.0f },
    { 127, 127.0f, 0.0f, 0.0f },
};

static struct Spline *lbl_801B39F8[] =
{
    lbl_801B3998,
    lbl_801B39B8,
    lbl_801B39D8,
};

static struct Spline lbl_801B3A04[] =
{
    {   0,  4608.0f, 0.0f, 0.0f },
    { 127, 16383.0f, 0.0f, 0.0f },
};

// SoundIcsReq?
void SoundIcsReq(u32 arg0, u8 arg1, s8 arg2)
{
    s8 var_r23;
    s8 var_r22;
    struct Struct801B3938 *ptr;
    u16 temp_r21_2;
    int i;
    float f28;
    u16 temp_r21;
    SND_PARAMETER_INFO sp14;
    SND_PARAMETER sp10;

    var_r23 = (arg0 >> 18) & 0x7F;
    var_r22 = (arg0 >> 25) & 0x7F;
    temp_r21 = (arg0 >> 0) & 0x7FF;
    !var_r23;
    if (var_r23 > 0x3F)
        var_r23 = -(0x80 - var_r23);
    if (var_r22 > 0x3F)
        var_r22 = -(0x80 - var_r22);

    lbl_802F1D3C = arg2;

    if (lbl_801FE558.unk50[temp_r21].unk0 == 0)
    {
        if (arg2 == 0 || arg1 == 0)
            return;

        if (modeCtrl.unk30 > 0)
        {
            var_r23 = func_8002A22C(0, currentBall->playerId);
            var_r22 = func_8002A22C(1, currentBall->playerId);
            arg2 *= lbl_801FE5C8[currentBall->playerId] != -1.0f ? lbl_801FE5C8[currentBall->playerId] : 1.0f;
        }

        f28 = arg2 / 127.0f;

        sp10.ctrl = 0x83;
        sp10.paraData.value7 = var_r22 + 0x40;
        sp14.numPara = 1;
        sp14.paraArray = &sp10;

        for (i = 0; i < 3; i++)
        {
            ptr = &lbl_801B3938[temp_r21][i];
            arg2 = f28 * calc_spline(arg1, lbl_801B39F8[i]);
            ptr->unk0 = sndFXStartParaInfo(g_soundDesc[ptr->unk4].unk0, 0x7FU, var_r23 + 0x40, 0U, &sp14);
            s_voiceIDs[temp_r21][ptr->unk4] = ptr->unk0;
            sndFXCtrl(ptr->unk0, 7, arg2);
        }
        lbl_801FE558.unk50[temp_r21].unk0 = 1;
    }
    else
    {
        if (arg2 == 0 || arg1 == 0)
        {
            for (i = 0; i < 3; i++)
            {
                ptr = &lbl_801B3938[temp_r21][i];
                sndFXKeyOff(ptr->unk0);
            }
            lbl_801FE558.unk50[temp_r21].unk0 = 0;
            return;
        }

        f28 = arg2 / 127.0f;
        temp_r21_2 = calc_spline(arg1, lbl_801B3A04);
        for (i = 0; i < 3; i++)
        {
            ptr = &lbl_801B3938[temp_r21][i];
            arg2 = f28 * calc_spline(arg1, lbl_801B39F8[i]);
            if (modeCtrl.unk30 > 0)
            {
                var_r23 = func_8002A22C(0, currentBall->playerId);
                var_r22 = func_8002A22C(1, currentBall->playerId);
                arg2 *= lbl_801FE5C8[currentBall->playerId] != -1.0f ? lbl_801FE5C8[currentBall->playerId] : 1.0f;
            }
            sndFXCtrl(ptr->unk0, 7, arg2);
            sndFXCtrl(ptr->unk0, 0xA, var_r23 + 0x40);
            sndFXCtrl(ptr->unk0, 0x83, var_r22 + 0x40);
            sndFXCtrl14(ptr->unk0, 0x84, temp_r21_2);
        }
    }
}

#pragma force_active on
void func_8002CEAC(void) {}

void func_8002CEB0(void) {}

void func_8002CEB4(void) {}
#pragma force_active reset

static void func_8002CEB8(int arg0)
{
    float temp_f0;
    u8 temp_f2;

    temp_f2 = (g_soundMuted != 0) ? 0 : arg0;
    temp_f0 = temp_f2 * (0.01f * g_soundBGMVol);
    DTKSetVolume(temp_f0, temp_f0);
}

void u_play_music(u32 arg0, s8 arg1)
{
    s32 hi = (arg0 >> 16) & 0xFFFF;
    u32 lo = arg0 & 0xFFFF;
    u8 var_r3;
    float vol;

    switch (arg1)
    {
    case 0:
        lbl_802014E0.unk0 = lo;
        if (hi > 255)
            hi = -(0x10000 - hi);
        hi += lbl_802F1DCC;
        lbl_802014E0.unkC =  CLAMP(hi + 0xFF, 0, 0xFF);
        lbl_802014E0.unk14 = 0;
        lbl_802014E0.unk18 = 0;
        lbl_802F1D68 = lo;
        lbl_802F1D6C = lbl_802F1D68 - lbl_802F1D64;
        lbl_802F1D74 = -3;
        lbl_802F1D70 = 1;
        break;
    case 1:
        lbl_802F1D70 = 1;
        lbl_802F1D74 = 1;
        lbl_802014E0.unk0 = -1;
        lbl_802014E0.unk14 = 0;
        lbl_802014E0.unk18 = 0;
        break;
    case 2:
        if (lbl_802014E0.unk14 <= 0)
        {
            lbl_802014E0.unk10 = 2;
            lbl_802014E0.unk14 = lo;
            lbl_802014E0.unk18 = lo;
        }
        break;
    case 3:
        if (lbl_802014E0.unk14 <= 0)
        {
            lbl_802014E0.unk10 = 3;
            lbl_802014E0.unk14 = lo;
            lbl_802014E0.unk18 = lo;
        }
        break;
    case 4:
        var_r3 = lo * lbl_802F1D4C;
        if (g_soundMuted != 0)
            var_r3 = 0;
        vol = var_r3 * (0.01f * g_soundBGMVol);
        DTKSetVolume(vol, vol);
        break;
    case 6:
        lbl_802014E0.unk0 = lo;
        if (hi > 255)
            hi = -(0x10000 - hi);
        lbl_802014E0.unkC = CLAMP(hi + 255, 0, 255);
        lbl_802014E0.unk14 = 0;
        lbl_802014E0.unk18 = 0;
        lbl_802F1D68 = lo;
        lbl_802F1D6C = lbl_802F1D68 - lbl_802F1D64;
        lbl_802F1D74 = -3;
        lbl_802F1D70 = 1;
        break;
    case 5:
        lbl_802F1D70 = 1;
        lbl_802F1D74 = 5;
        break;
    case 8:
        lbl_802F1D54 = lbl_802F1D4C;
        lbl_802F1D50 = 0.01f * lo;
        lbl_802F1D58 = 60;
        break;
    case 9:
        lbl_802F1D54 = lbl_802F1D4C;
        lbl_802F1D50 = 1.0f;
        lbl_802F1D58 = 60;
        break;
    case 10:
        lbl_802F1D44 = 0.01f * lo;
        lbl_802F1D40 = 0.01f * lo;
        if (lo == 100)
            lbl_802F1D48 = 1;
        var_r3 = lbl_802F1D4C * (u_volumeRelated3 * *(float *)&lbl_802F1D40);
        if (g_soundMuted != 0)
            var_r3 = 0;
        vol = var_r3 * (0.01f * g_soundBGMVol);
        DTKSetVolume(vol, vol);
        break;
    case 11:
        lbl_802F1D44 = 1.0f;
        lbl_802F1D40 = 1.0f;
        lbl_802F1D48 = 1;
        var_r3 = (u_volumeRelated3 * lbl_802F1D4C);
        if (g_soundMuted != 0)
            var_r3 = 0;
        vol = var_r3 * (0.01f * g_soundBGMVol);
        DTKSetVolume(vol, vol);
        break;
    }
}

static void lbl_8002D344(s32 result, DVDFileInfo *fileInfo)
{
    u8 *temp_r31;

    if (result != lbl_802F1DA4)
        OSReport("DVDReadAsync result %x\n", result);

    if (result < 0x8000)
    {
        memset(lbl_802F1D84, 0, lbl_802F1D94);
        lbl_802F1DAC = 0x8000 - result;
        temp_r31 = lbl_802F1D84 + lbl_802F1D94;
        OSReport("restL %d\n", lbl_802F1DAC);
        OSReport("extraL %d\n", lbl_802F1D94);
        DVDReadAsync(fileInfo, temp_r31, lbl_802F1DAC, 0, lbl_8002D420);
    }
}

static void lbl_8002D420(s32 result, DVDFileInfo *fileInfo)
{
    if (result != lbl_802F1DAC)
        OSReport("DVDReadAsync result %x\n", result);
}

static void lbl_8002D45C(s32 result, DVDFileInfo *fileInfo)
{
    u8 *temp_r31;

    if (result != lbl_802F1DA8)
        OSReport("DVDReadAsync result %x\n", result);

    if (result < 0x8000)
    {
        memset(lbl_802F1D88, 0, lbl_802F1D98);
        lbl_802F1DB0 = 0x8000 - result;
        temp_r31 = lbl_802F1D88 + lbl_802F1D98;
        OSReport("restR %d\n", lbl_802F1DB0);
        OSReport("extraR %d\n", lbl_802F1D98);
        DVDReadAsync(fileInfo, temp_r31, lbl_802F1DB0, 0, lbl_8002D538);
    }
}

static void lbl_8002D538(s32 result, DVDFileInfo *fileInfo)
{
    if (result != lbl_802F1DB0)
        OSReport("DVDReadAsync result %x\n", result);
}

static u32 lbl_8002D574(u8 *arg0, u32 arg1, int unused1, int unused2, s32 arg4)
{
    s32 temp_r5;
    s32 temp_r4;

    switch (arg4)
    {
    case 0:
        if (lbl_802F1D9C != 0 && arg1 < 0x4000)
            lbl_802F1D9C = 0;
        break;
    case 1:
        if (lbl_802F1DA0 != 0 && arg1 < 0x4000)
            lbl_802F1DA0 = 0;
        break;
    }
    if (arg1 >= 0x4000)
    {
        switch (arg4)
        {
        case 0:
            if (lbl_802F1D9C == 0)
            {
                temp_r4 = OSRoundUp32B(s_testStreamLeftFile.length - lbl_802F1D8C);
                if (temp_r4 > 0x8000)
                {
                    lbl_802F1DA4 = 0x8000;
                    DVDReadAsync(&s_testStreamLeftFile, arg0, lbl_802F1DA4, lbl_802F1D8C, lbl_8002D344);
                    lbl_802F1D8C += 0x8000;
                }
                else
                {
                    lbl_802F1DA4 = temp_r4;
                    temp_r5 = lbl_802F1DA4;
                    lbl_802F1D94 = (temp_r5 + lbl_802F1D8C) - s_testStreamLeftFile.length;
                    lbl_802F1D84 = (arg0 + temp_r5) - lbl_802F1D94;
                    DVDReadAsync(&s_testStreamLeftFile, arg0, temp_r5, lbl_802F1D8C, lbl_8002D344);
                    lbl_802F1D8C = 0x8000 - lbl_802F1DA4;
                }
                lbl_802F1D9C = 1;
            }
            break;
        case 1:
            if (lbl_802F1DA0 == 0)
            {
                temp_r4 = OSRoundUp32B(s_testStreamRightFile.length - lbl_802F1D90);
                if (temp_r4 > 0x8000)
                {
                    lbl_802F1DA8 = 0x8000;
                    DVDReadAsync(&s_testStreamRightFile, arg0, lbl_802F1DA8, lbl_802F1D90, lbl_8002D45C);
                    lbl_802F1D90 += 0x8000;
                }
                else
                {
                    lbl_802F1DA8 = temp_r4;
                    temp_r5 = lbl_802F1DA8;
                    lbl_802F1D98 = (temp_r5 + lbl_802F1D90) - s_testStreamRightFile.length;
                    lbl_802F1D88 = (arg0 + temp_r5) - lbl_802F1D98;
                    DVDReadAsync(&s_testStreamRightFile, arg0, temp_r5, lbl_802F1D90, lbl_8002D45C);
                    lbl_802F1D90 = 0x8000 - lbl_802F1DA8;
                }
                lbl_802F1DA0 = 1;
            }
            break;
        }
        return 0x4000;
    }
    return 0;
}

#pragma force_active on
// Starts the test song
void SoundStreamStart(void)
{
    BOOL intrEnabled;
    int i;
    u8 *ptr;

    if (lbl_802F1DBC == 0)
    {
        s_testStreamLeftBuffer = OSAlloc(0x10000);
        s_testStreamRightBuffer = OSAlloc(0x10000);
        ptr = s_testStreamLeftBuffer;
        for (i = 0; i < 0x10000; i++, ptr++)
            *ptr = 0;
        ptr = s_testStreamRightBuffer;
        for (i = 0; i < 0x10000; i++, ptr++)
            *ptr = 0;
        DCFlushRange(s_testStreamLeftBuffer, 0x10000);
        DCFlushRange(s_testStreamRightBuffer, 0x10000);
        if (DVDOpen("/test/snd/test/streamL.pcm", &s_testStreamLeftFile) == 0)
            sound_error("SoundStreamStart\n", "can't open data\n");
        if (DVDOpen("/test/snd/test/streamR.pcm", &s_testStreamRightFile) == 0)
            sound_error("SoundStreamStart\n", "can't open data\n");
        lbl_802F1D8C = 0;
        lbl_802F1D90 = 0;
        lbl_802F1D9C = 0;
        lbl_802F1DA0 = 0;
        intrEnabled = OSDisableInterrupts();
        lbl_802F1DB4 = sndStreamAllocEx(0xFF, s_testStreamLeftBuffer, 0x8000, 0xABE0, 0x7F, 0, 0, 0, 0, 0, 0, lbl_8002D574, 0, 0);
        lbl_802F1DB8 = sndStreamAllocEx(0xFF, s_testStreamRightBuffer, 0x8000, 0xABE0, 0x7F, 0x7F, 0, 0, 0, 0, 0, lbl_8002D574, 1, 0);
        OSRestoreInterrupts(intrEnabled);
        if (lbl_802F1DB4 == -1U)
            sound_error("SoundStreamStart\n", "can't allocate stream\n");
        if (lbl_802F1DB8 == -1U)
            sound_error("SoundStreamStart\n", "can't allocate stream\n");
        lbl_802F1DBC = 1;
    }
}

// Stops the test song
void SoundStreamEnd(void)
{
    if (lbl_802F1DBC != 0)
    {
        if (lbl_802F1DB4 != -1U)
            sndStreamFree(lbl_802F1DB4);
        if (lbl_802F1DB8 != -1U)
            sndStreamFree(lbl_802F1DB8);
        DVDClose(&s_testStreamLeftFile);
        DVDClose(&s_testStreamRightFile);
        if (s_testStreamLeftBuffer != NULL)
            OSFree(s_testStreamLeftBuffer);
        if (s_testStreamRightBuffer != NULL)
            OSFree(s_testStreamRightBuffer);
        lbl_802F1DBC = 0;
    }
}

void u_change_sound_mode(u32 mode)
{
    if (mode != OSGetSoundMode())
    {
        OSSetSoundMode(mode);
        if (mode == OS_SOUND_MODE_MONO)
            queue_dtk_tracks(0, SND_OUTPUTMODE_MONO);
        else
            queue_dtk_tracks(0, SND_OUTPUTMODE_SURROUND);
    }
}
#pragma force_active reset

void store_sound_settings(struct MemcardContents *mc)
{
    mc->gameData.bgmVolume = g_soundBGMVol;
    mc->gameData.seVolume  = g_soundSEVol;
}

void load_sound_settings(struct MemcardContents *mc)
{
    g_soundBGMVol = mc->gameData.bgmVolume;
    g_soundSEVol  = mc->gameData.seVolume;
}

void func_8002DB38(void)
{
    int j, i;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 0x425; j++)
        {
            if (s_voiceIDs[i][j] != -1U)
                sndFXCtrl(s_voiceIDs[i][j], 7, 0);
        }
    }
}

void func_8002DBC4(void)
{
    int j, i;

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 0x425; j++)
        {
            if (s_voiceIDs[i][j] != -1U)
                sndFXCtrl(s_voiceIDs[i][j], 7, lbl_801FD404[i][j]);
        }
    }
}
