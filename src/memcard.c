/**
 * memcard.c - Implements memory card loading and saving functionality
 */
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "course.h"
#include "event.h"
#include "input.h"
#include "mathutil.h"
#include "memcard.h"
#include "mode.h"
#include "name_entry.h"
#include "recplay.h"
#include "recplay_cmpr.h"
#include "sound.h"
#include "sprite.h"
#include "vibration.h"
#include "window.h"

struct StringEntry
{
    char *str;
    u32 unk4;
};

struct MemCardMessage
{
    struct StringEntry *lines;
    int numLines;
};

enum
{
    MC_STATUS_MOUNTED = (1 << 0),
    MC_STATUS_OPEN    = (1 << 1),
    MC_STATUS_SAVING  = (1 << 4),  // saving gamedata or replay
    MC_STATUS_LOADING = (1 << 5),  // loading gamdata or replay
    MC_STATUS_ERROR   = (1 << 9),
    MC_STATUS_REPLAY_FILE  = (1 << 12),  // loading/saving/deleting replay
    MC_STATUS_GAMEDATA_FILE = (1 << 13),  // loading/saving gamedata
    MC_STATUS_WRITE_IN_PROGRESS = (1 << 21),
};

enum
{
    MC_STATE_UNK1 = 1,
    MC_STATE_PROBE = 2,
    MC_STATE_MOUNT = 3,
    MC_STATE_CHECK_MOUNT_RESULT = 4,
    MC_STATE_VERIFY_FILESYSTEM = 5,
    MC_STATE_CHECK_VERIFY_FILESYSTEM_RESULT = 6,
    MC_STATE_OPEN_FILE = 8,
    MC_STATE_FORMAT_PROMPT = 9,
    MC_STATE_OVERWRITE_PROMPT = 13,
    MC_STATE_CHECK_CREATE_FILE_RESULT = 14,
    MC_STATE_GET_METADATA = 20,
    MC_STATE_CHECK_FREE_SPACE = 23,
    MC_STATE_CHECK_FREE_SPACE2 = 30,
    MC_STATE_ERROR = 0xFF,
};

// .bss
static CARDStat cardStat;
static struct MemcardInfo
{
    s32 fileNo;
    /*0x04*/ s32 sectorSize;
    /*0x08*/ u32 statusFlags;
    /*0x0C*/ struct MemCardMessage *msg;
    u8 unused10[4];
    /*0x14*/ char fileName[0x38-0x14];
    /*0x38*/ OSTime time;
    s16 unk40;
    u16 unk42;  // error message timer?
    u32 unk44;
    /*0x48*/ u32 fileSize;
    /*0x4C*/ u8 state;
    u8 unk4D;
    /*0x50*/ CARDFileInfo cardFileInfo;
    u8 unused64[0xC];
} memcardInfo;
static u8 cardWorkArea[0xA100];
static struct MemcardGameData lbl_802C4480;
static char strFmtBufferLine1[64];
static char strFmtBufferLine2[64];
static char strFmtBufferLine3[64];
static char lbl_802C4900[64];

static u8 lbl_802F21A8;
static struct MemcardContents *memcardContents;
static u8 memcardMode;
static u8 lbl_802F21B1;
static s8 lbl_802F21B2;
static u32 lbl_802F21B4;
static u8 replayFileIndex;
static u8 lbl_802F21B9;

struct ReplayFileHeader
{
    u16 crc;
    u16 replayFlags;
    u8 unk4;
    u8 difficulty;
    u8 floorNum;
    u8 character;
    u32 unk8;
    u32 timestamp;
};

static struct ReplayFileInfo
{
    struct ReplayFileHeader header;
    u32 fileSize;
    s8 fileNo;
} *replayFileInfo;
static u32 lbl_802F21C0;

// replay file data
static struct
{
    struct ReplayFileHeader header;
    /*0x0010*/ u8 bannerImg[0x1800];
    /*0x1810*/ u8 replayIcon[0x800];
    /*0x2010*/ char comment[32];  // "Super Monkey Ball"
    char unk2030[0x20];
    u8 compressedReplay[];  // compressed replay data follows this struct
} *memcardReplayData;
static u8 lbl_802F21C8;

static void func_800A4E70(void);
static void func_800A4F04(void);

#define BLOCK_SIZE 0x2000

#pragma force_active on

void memcard_set_mode(int mode)
{
    memcardMode = mode;
}

int memcard_is_write_in_progress(void)
{
    return (memcardInfo.statusFlags & MC_STATUS_WRITE_IN_PROGRESS);
}

void func_8009F4B8(struct ReplayFileInfo *replay, u32 b)
{
    replayFileInfo = replay;
    lbl_802F21C0 = b;
}

u8 func_8009F4C4(void)
{
    return lbl_802F21A8;
}

void func_8009F4CC(u8 a)
{
    lbl_802F21A8 = a;
}

void memcard_cancel_and_unmount(void)
{
    CARDCancel(&memcardInfo.cardFileInfo);

    if (memcardInfo.statusFlags & MC_STATUS_OPEN)
    {
        CARDClose(&memcardInfo.cardFileInfo);
        memcardInfo.statusFlags &= ~MC_STATUS_OPEN;
    }

    if (memcardInfo.statusFlags & MC_STATUS_MOUNTED)
    {
        CARDUnmount(0);
        memcardInfo.statusFlags &= ~MC_STATUS_MOUNTED;
    }
}

int sub_8009F554(void)
{
    return !(1 - lbl_802F21B9);
}

//lbl_801D4288 = start of .data section

static struct StringEntry strMemCardError[] =
{
    {"A Memory Card error has occurred.", 0},
};

static struct MemCardMessage msgMemCardError = {strMemCardError, ARRAY_COUNT(strMemCardError)};

static struct StringEntry strFmtBuffer[] =
{
    {strFmtBufferLine1, 0},
    {strFmtBufferLine2, 0},
    {strFmtBufferLine3, 0},
};

static struct MemCardMessage msgFmtBuffer = {strFmtBuffer, 0 /*why 0?*/};

static struct StringEntry strMemCardCantRead[] =
{
    {"Could not read Memory Card.", 0},
    {"Please see the Instruction Booklet for details.", 0},
};

static struct StringEntry strMemCardCantUse[] =
{
    {"This Memory Card cannot be used.", 0},
};

static struct StringEntry strMemCardNotInserted[] =
{
    {"Memory Card is not inserted.", 0},
    {"Please insert a Memory Card.", 0},
};

static struct StringEntry strMemCardNotInsertedAutosaveOff[] =
{
    {"There is no Memory Card in Slot A.", 0},
    {"The Autosave function will be set to Off.", 0},
};

static struct StringEntry strMemCardNotInsertedSlotA[] =
{
    {"Memory Card is not inserted.", 0},
    {"Please insert a Memory Card into Slot A.", 0},
};

static struct StringEntry strMemCardRemoved[] =
{
    {"The Memory Card was removed.", 0},
    {"The operation has been stopped.", 0},
};

static struct StringEntry strMemCardNotSupported[] =
{
    {"This Memory Card is not supported/recognized.", 0},
    {"Please insert a different Memory Card.", 0},
};

static struct StringEntry strMemCardFileDamagedPleaseFormat[] =
{
    {"The file on the Memory Card has been damaged.", 0},
    {"Please use the GameCube tm Memory Card Menu", 0},
    {"or the Save Function of the Game Data Option in this game", 0},
    {"to format your Memory Card.", 0},
};

static struct StringEntry strMemCardFileDamaged[] =
{
    {"The file on the Memory Card has been damaged.", 0},
};

static struct StringEntry strMemCardCantUse2[] =
{
    {"This Memory Card cannot be used.", 0},
};

static struct StringEntry stringEntry4[] =
{
    {"No Super Monkey Ball Game Data found on Memory Card.", 0},
};

static struct StringEntry strCantSaveFile[] =
{
    {"Cannot save data to the file.", 0},
};

static struct StringEntry strCantLoadFile[] =
{
    {"Cannot load file.", 0},
};

static struct StringEntry strCantReadFile[] =
{
    {"Cannot read file.", 0},
};

static struct StringEntry lbl_801D46B0[] =
{
    {"Not enough free blocks in Memory Card.", 0},
    {"There are %d free blocks,", 0},
    {"but %d blocks are required to save the data.", 0},
};

static struct StringEntry lbl_801D46E0[] =
{
    {"Not enough free blocks in Memory Card.", 0},
    {"There is %d free block,", 0},
    {"but %d blocks are required to save the data.", 0},
};

static struct StringEntry lbl_801D46F8[] =
{
    {"There are %d free blocks,", 0},
    {"but %d blocks are required to save the data.", 0},
    {"The Autosave function will be set to Off.", 0},
};

static struct StringEntry lbl_801D46F8_blah[] =
{
    {"There is %d free block,", 0},
    {"but %d blocks are required to save the data.", 0},
    {"The Autosave function will be set to Off.", 0},
};

static struct StringEntry lbl_801D4774[] =
{
    {"No free blocks in Memory Card.", 0},
    {"%d blocks are required to save the data.", 0},
};

static struct StringEntry lbl_801D4774_blah[] =
{
    {"No free blocks in Memory Card.", 0},
    {"%d blocks are required to save the data.", 0},
    {"The Autosave function will be set to Off.", 0},
};

static struct StringEntry lbl_801D4774_blah2[] =
{
    {"Not enough free blocks in Memory Card.", 0},
    {"%d blocks are required to save the data.", 0},
};

static struct StringEntry lbl_801D48A4[] =
{
    {"The data has been successfully saved,", 0},
    {"but there are not enough free blocks to save the data.", 0},
    {"To save Replay Data, at least 2 free blocks are required.", 0},
    {"Also, to save Game Data, or to save over old data,", 0},
    {"at least 3 free blocks are required.", 0},
};

static struct StringEntry lbl_801D4934[] =
{
    {"The data has been successfully saved,", 0},
    {"but there are not enough free blocks to save Game Data.", 0},
    {"To save Game Data, or to save over old data,", 0},
    {"at least 3 free blocks are required.", 0},
};

static struct StringEntry lbl_801D49D0[] =
{
    {"The data has been successfully saved,", 0},
    {"but there may not be enough free blocks to save Replay Data.", 0},
    {"Replay Data requires a maximum of 13 free blocks to save.", 0},
};

static struct StringEntry lbl_801D4A30[] =
{
    {"There are not enough free blocks", 0},
    {"to save a file in this Memory Card.", 0},
    {"To save Replay Data, at least 2 free blocks are required.", 0},
    {"Also, to save Game Data, or to save over old data,", 0},
    {"at least 3 free blocks are required.", 0},
};

static struct StringEntry lbl_801D4A80[] =
{
    {"There are not enough free blocks", 0},
    {"to save Game Data in this Memory Card.", 0},
    {"To save Game Data, or to save over old data,", 0},
    {"at least 3 free blocks are required.", 0},
};

static struct StringEntry lbl_801D4B10[] =
{
    {"There may not be enough free blocks to save Replay Data.", 0},
    {"Replay Data requires a maximum of 13 free blocks.", 0},
};

static struct StringEntry lbl_801D4B58[] =
{
    {"No Super Monkey Ball Game Data found on Memory Card.", 0},
    {"There are not enough free blocks in this Memory Card.", 0},
    {"To save Replay Data, at least 2 free blocks are required.", 0},
    {"Also, to save Game Data, or to save over old data,", 0},
    {"at least 3 free blocks are required.", 0},
};

static struct StringEntry lbl_801D4B58_blah[] =
{
    {"No Super Monkey Ball Game Data found on Memory Card.", 0},
    {"There are not enough free blocks", 0},
    {"to save Game Data in this Memory Card.", 0},
    {"To save Game Data, or to save over old data,", 0},
    {"at least 3 free blocks are required.", 0},
};

static struct StringEntry lbl_801D4C38[] =
{
    {"No Super Monkey Ball Game Data found on Memory Card.", 0},
    {"There may not be enough free blocks", 0},
    {"to save Replay Data in this Memory Card.", 0},
    {"To save Replay Data, a maximum of 13 free blocks is required.", 0},
};

static struct StringEntry strCantMakeMoreFiles[] =
{
    {"Cannot make any more files.", 0},
};

static struct StringEntry strSaveSuccessNoMoreFiles[] =
{
    {"The data has been succesfully saved.", 0},
    {"However, no more files can be made. ", 0},
    {"You will not be able to save the data next time.", 0},
};

static struct StringEntry strSaveFinished[] =
{
    {"Saving finished.", 0},
};

static struct StringEntry strLoadFinished[] =
{
    {"Loading finished.", 0},
};

static struct StringEntry strDeleteFinished[] =
{
    {"A file has been deleted.", 0},
};

static struct StringEntry strGameDataWrongVersion[] =
{
    {"The Game Data version is incorrect.", 0},
    {"Please try saving your game again.", 0},
};

static struct StringEntry strGameDataDamaged[] =
{
    {"The Game Data has been damaged.", 0},
    {"Please try saving your game again.", 0},
};

static struct StringEntry strReplayDataDamaged[] =
{
    {"The Replay Data has been damaged.", 0},
    {"Please delete the file.", 0},
};

static struct StringEntry strFormatInterrupted[] =
{
    {"Formatting has been interrupted.", 0},
};

static struct StringEntry strSaveInterrupted[] =
{
    {"Saving has been interrupted.", 0},
};

static struct StringEntry strLoadInterrupted[] =
{
    {"Loading has been interrupted.", 0},
};

static struct StringEntry strFileSizeChanged[] =
{
    {"The file size has been changed.", 0},
    {"Please delete the file from the menu, and try again.", 0},
};

static struct StringEntry strNoReplayData[] =
{
    {"No Replay Data found.", 0},
    {"In Normal mode and Practice Mode, during the Replay playback,", 0},
    {"if you access the Pause Menu and select Save Replay,", 0},
    {"you can save the Replay that you are viewing.", 0},
};

static struct MemCardMessage msgMemCardCantRead                = {strMemCardCantRead, ARRAY_COUNT(strMemCardCantRead)};
static struct MemCardMessage msgMemCardCantUse                 = {strMemCardCantUse, ARRAY_COUNT(strMemCardCantUse)};
static struct MemCardMessage msgMemCardNotInserted             = {strMemCardNotInserted, ARRAY_COUNT(strMemCardNotInserted)};
static struct MemCardMessage msgMemCardNotInsertedAutosaveOff  = {strMemCardNotInsertedAutosaveOff, ARRAY_COUNT(strMemCardNotInsertedAutosaveOff)};
static struct MemCardMessage msgMemCardNotInsertedSlotA        = {strMemCardNotInsertedSlotA, ARRAY_COUNT(strMemCardNotInsertedSlotA)};
static struct MemCardMessage msgMemCardRemoved                 = {strMemCardRemoved, ARRAY_COUNT(strMemCardRemoved)};
static struct MemCardMessage msgMemCardNotSupported            = {strMemCardNotSupported, ARRAY_COUNT(strMemCardNotSupported)};
static struct MemCardMessage msgMemCardDamaged                 = {strMemCardFileDamaged, ARRAY_COUNT(strMemCardFileDamaged)};
static struct MemCardMessage msgMemCardFileDamagedPleaseFormat = {strMemCardFileDamagedPleaseFormat, ARRAY_COUNT(strMemCardFileDamagedPleaseFormat)};
static struct MemCardMessage msgMemCardCantUse2                = {strMemCardCantUse2, ARRAY_COUNT(strMemCardCantUse2)};
static struct MemCardMessage lbl_802F14A0                      = {stringEntry4, ARRAY_COUNT(stringEntry4)};
static struct MemCardMessage msgCantSaveFile                   = {strCantSaveFile, ARRAY_COUNT(strCantSaveFile)};
static struct MemCardMessage msgCantLoadFile                   = {strCantLoadFile, ARRAY_COUNT(strCantLoadFile)};
static struct MemCardMessage msgCantReadFile                   = {strCantReadFile, ARRAY_COUNT(strCantReadFile)};
static struct MemCardMessage lbl_802F14C0                      = {lbl_801D46B0, ARRAY_COUNT(lbl_801D46B0)};
static struct MemCardMessage lbl_802F14C8                      = {lbl_801D46E0, ARRAY_COUNT(lbl_801D46E0)};
static struct MemCardMessage lbl_802F14D0                      = {lbl_801D46F8, ARRAY_COUNT(lbl_801D46F8)};
static struct MemCardMessage lbl_802F14D8                      = {lbl_801D46F8_blah, ARRAY_COUNT(lbl_801D46F8_blah)};
static struct MemCardMessage lbl_802F14E0                      = {lbl_801D4774, ARRAY_COUNT(lbl_801D4774)};
static struct MemCardMessage lbl_802F14E8                      = {lbl_801D4774_blah, ARRAY_COUNT(lbl_801D4774_blah)};
static struct MemCardMessage lbl_802F14F0                      = {lbl_801D4774_blah2, ARRAY_COUNT(lbl_801D4774_blah2)};
static struct MemCardMessage lbl_802F14F8                      = {lbl_801D48A4, ARRAY_COUNT(lbl_801D48A4)};
static struct MemCardMessage lbl_802F1500                      = {lbl_801D4934, ARRAY_COUNT(lbl_801D4934)};
static struct MemCardMessage lbl_802F1508                      = {lbl_801D49D0, ARRAY_COUNT(lbl_801D49D0)};
static struct MemCardMessage lbl_802F1510                      = {lbl_801D4A30, ARRAY_COUNT(lbl_801D4A30)};
static struct MemCardMessage lbl_802F1518                      = {lbl_801D4A80, ARRAY_COUNT(lbl_801D4A80)};
static struct MemCardMessage lbl_802F1520                      = {lbl_801D4B10, ARRAY_COUNT(lbl_801D4B10)};
static struct MemCardMessage lbl_802F1528                      = {lbl_801D4B58, ARRAY_COUNT(lbl_801D4B58)};
static struct MemCardMessage lbl_802F1530                      = {lbl_801D4B58_blah, ARRAY_COUNT(lbl_801D4B58_blah)};
static struct MemCardMessage lbl_802F1538                      = {lbl_801D4C38, ARRAY_COUNT(lbl_801D4C38)};
static struct MemCardMessage msgCantMakeMoreFiles              = {strCantMakeMoreFiles, ARRAY_COUNT(strCantMakeMoreFiles)};
static struct MemCardMessage msgSaveSuccessNoMoreFiles         = {strSaveSuccessNoMoreFiles, ARRAY_COUNT(strSaveSuccessNoMoreFiles)};
static struct MemCardMessage msgSaveFinished                   = {strSaveFinished, ARRAY_COUNT(strSaveFinished)};
static struct MemCardMessage msgLoadFinished                   = {strLoadFinished, ARRAY_COUNT(strLoadFinished)};
static struct MemCardMessage msgDeleteFinished                 = {strDeleteFinished, ARRAY_COUNT(strDeleteFinished)};
static struct MemCardMessage msgGameDataWrongVersion           = {strGameDataWrongVersion, ARRAY_COUNT(strGameDataWrongVersion)};
static struct MemCardMessage msgGameDataDamaged                = {strGameDataDamaged, ARRAY_COUNT(strGameDataDamaged)};
static struct MemCardMessage msgReplayDataDamaged              = {strReplayDataDamaged, ARRAY_COUNT(strReplayDataDamaged)};
static struct MemCardMessage msgFormatInterrupted              = {strFormatInterrupted, ARRAY_COUNT(strFormatInterrupted)};
static struct MemCardMessage msgSaveInterrupted                = {strSaveInterrupted, ARRAY_COUNT(strSaveInterrupted)};
static struct MemCardMessage msgLoadInterrupted                = {strLoadInterrupted, ARRAY_COUNT(strLoadInterrupted)};
static struct MemCardMessage msgFileSizeChanged                = {strFileSizeChanged, ARRAY_COUNT(strFileSizeChanged)};
static struct MemCardMessage msgNoReplayData                   = {strNoReplayData, ARRAY_COUNT(strNoReplayData)};

static struct StringEntry strPressBButton[] =
{
    {"Please press the p/BUTTON_B/ a/Button.", 0},
};

static struct StringEntry strPressBButtonNoSave[] =
{
    {"Please press the p/BUTTON_B/ a/Button to Continue without saving.", 0},
};

static struct StringEntry strMemCardNumFreeBlocks[] =
{
    {"There are %d free blocks on this Memory Card.", 0},
};

static struct StringEntry strMemCardNumFreeBlock[] =
{
    {"There is %d free block on this Memory Card.", 0},
};

static struct StringEntry strMemCardNoFreeBlocks[] =
{
    {"No free blocks found on this Memory Card.", 0},
};

static struct StringEntry strAccessMemCard[] =
{
    {"Accessing Memory Card.", 0},
};

static struct StringEntry strSavingReplay[] =
{
    {"Now saving Replay Data.", 0},
    {"Do not touch the Memory Card or the POWER Button.", 0},
};

static struct StringEntry strSavingGame[] =
{
    {"Now saving Game Data.", 0},
    {"Do not touch the Memory Card or the POWER Button.", 0},
};

static struct StringEntry strLoadingGame[] =
{
    {"Now loading Game Data.", 0},
    {"Do not touch the Memory Card or the POWER Button.", 0},
};

static struct StringEntry strMakeSelection[] =
{
    {"Please use the p/LEVER/ a/or the p/BUTTON_+/ a/to highlight a selection,", 0},
    {"and press the p/BUTTON_A/ a/Button to select.", 0},
};

static struct StringEntry strInsertMemcardSlotAPressA[] =
{
    {"Please insert a Memory Card into Slot A,", 0},
    {"then press the p/BUTTON_A/ a/Button.", 0},
    {"You may cancel by pressing the p/BUTTON_B/ a/Button.", 0},
};

static struct StringEntry strFormatPrompt[] =
{
    {"This will format your Memory Card.", 0},
    {"All data will be lost. Is it OK to format?", 0},
    {"", 0},
    {"", 0},
};

static struct StringEntry strFormatProgress[] =
{
    {"Memory Card being formatted.", 0},
    {"Do not touch the Memory Card or the POWER Button.", 0},
};

static struct StringEntry strOverwritePrompt[] =
{
    {"A Super Monkey Ball Game Data file already exists.", 0},
    {"Would you like to save over it?", 0},
    {"", 0},
    {"", 0},
};

static struct MemCardMessage msgPressBButton             = {strPressBButton,             ARRAY_COUNT(strPressBButton)};
static struct MemCardMessage msgPressBButtonNoSave       = {strPressBButtonNoSave,       ARRAY_COUNT(strPressBButtonNoSave)};
static struct MemCardMessage msgMemCardNumFreeBlocks     = {strMemCardNumFreeBlocks,     ARRAY_COUNT(strMemCardNumFreeBlocks)};
static struct MemCardMessage msgMemCardNumFreeBlock      = {strMemCardNumFreeBlock,      ARRAY_COUNT(strMemCardNumFreeBlock)};
static struct MemCardMessage msgMemCardNoFreeBlocks      = {strMemCardNoFreeBlocks,      ARRAY_COUNT(strMemCardNoFreeBlocks)};
static struct MemCardMessage msgAccessMemCard            = {strAccessMemCard,            ARRAY_COUNT(strAccessMemCard)};
static struct MemCardMessage msgSavingReplay             = {strSavingReplay,             ARRAY_COUNT(strSavingReplay)};
static struct MemCardMessage msgSavingGame               = {strSavingGame,               ARRAY_COUNT(strSavingGame)};
static struct MemCardMessage msgLoadingGame              = {strLoadingGame,              ARRAY_COUNT(strLoadingGame)};
static struct MemCardMessage msgMakeSelection            = {strMakeSelection,            ARRAY_COUNT(strMakeSelection)};
static struct MemCardMessage msgInsertMemcardSlotAPressA = {strInsertMemcardSlotAPressA, ARRAY_COUNT(strInsertMemcardSlotAPressA)};
static struct MemCardMessage msgFormatPrompt             = {strFormatPrompt,             ARRAY_COUNT(strFormatPrompt)};
static struct MemCardMessage msgFormatProgress           = {strFormatProgress,           ARRAY_COUNT(strFormatProgress)};
static struct MemCardMessage msgOverwritePrompt          = {strOverwritePrompt,          ARRAY_COUNT(strOverwritePrompt)};

static struct MemCardMessage *s_memcardMsgTable[] =
{
    &msgMemCardCantRead,
    &msgMemCardCantUse,
    &msgMemCardNotInserted,
    &msgMemCardNotInsertedAutosaveOff,
    &msgMemCardNotInsertedSlotA,
    &msgMemCardRemoved,
    &msgMemCardNotSupported,
    &msgMemCardDamaged,
    &msgMemCardFileDamagedPleaseFormat,
    &msgMemCardCantUse2,
    &lbl_802F14A0,
    &msgCantSaveFile,
    &msgCantLoadFile,
    &msgCantReadFile,
    &lbl_802F14C0,
    &lbl_802F14D0,
    &lbl_802F14E0,
    &lbl_802F14E8,
    &lbl_802F14F0,
    &lbl_802F14F8,
    &lbl_802F1500,
    &lbl_802F1508,
    &lbl_802F1510,
    &lbl_802F1518,
    &lbl_802F1520,
    &lbl_802F1528,
    &lbl_802F1530,
    &lbl_802F1538,
    &msgCantMakeMoreFiles,
    &msgSaveSuccessNoMoreFiles,
    &msgSaveFinished,
    &msgLoadFinished,
    &msgDeleteFinished,
    &msgGameDataWrongVersion,
    &msgGameDataDamaged,
    &msgReplayDataDamaged,
    &msgFormatInterrupted,
    &msgSaveInterrupted,
    &msgLoadInterrupted,
    &msgFileSizeChanged,
    &msgNoReplayData,
    &msgPressBButton,
    &msgPressBButtonNoSave,
    &msgMemCardNumFreeBlocks,
    &msgMemCardNoFreeBlocks,
    &msgAccessMemCard,
    &msgSavingReplay,
    &msgSavingGame,
    &msgLoadingGame,
    &msgMakeSelection,
    &msgInsertMemcardSlotAPressA,
    &msgFormatPrompt,
    &msgFormatProgress,
    &msgOverwritePrompt,
};

void init_gamedata_file(void)
{
    int i;
    DVDFileInfo file;
    OSCalendarTime calendarTime;
    void *buffer;

    // set banner and icon
    buffer = OSAlloc(sizeof(memcardContents->bannerAndIcon));
    if (buffer == NULL)
        OSPanic("memcard.c", 927, "cannot OSAlloc");
    if (DVDOpen("banner_and_icon.bin", &file) == 0)
        OSPanic("memcard.c", 931, "cannot open banner_and_icon.bin");
    if (avDVDRead(&file, buffer, sizeof(memcardContents->bannerAndIcon), 0) == 0)
        OSPanic("memcard.c", 935, "cannot read banner_and_icon.bin");
    memcpy(memcardContents->bannerAndIcon, buffer, sizeof(memcardContents->bannerAndIcon));
    OSFree(buffer);
    DVDClose(&file);

    // set comment
    cardStat.commentAddr = (u32)memcardContents->comment - (u32)memcardContents;
    strcpy(memcardContents->comment, "Super Monkey Ball");
    OSTicksToCalendarTime(memcardInfo.time, &calendarTime);
    sprintf(
        memcardContents->title,
        "GameData%02d-%02d-%02d %02d:%02d",
        calendarTime.mon + 1,
        calendarTime.mday,
        calendarTime.year % 100,
        calendarTime.hour,
        calendarTime.min);

    cardStat.iconAddr = (u32)memcardContents->bannerAndIcon - (u32)memcardContents;
    cardStat.bannerFormat = (cardStat.bannerFormat & ~0x3) | 2;

    // These loops match except for stack
    /*
    for (i = 0; i < 8; i++)
        cardStat.rotZKeyframeCount = (cardStat.rotZKeyframeCount & ~(3 << (2*i))) | (2 << (2*i));
    for (i = 0; i < 8; i++)
        cardStat.unk36 = (cardStat.unk36 & ~(3 << (2*i))) | ((1 << (2*i)) & 0x1FFF);
    */
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*0))) | (2 << (2*0));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*1))) | (2 << (2*1));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*2))) | (2 << (2*2));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*3))) | (2 << (2*3));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*4))) | (2 << (2*4));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*5))) | (2 << (2*5));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*6))) | (2 << (2*6));
    cardStat.iconFormat = (cardStat.iconFormat & ~(3 << (2*7))) | (2 << (2*7));

    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*0))) | ((1 << (2*0)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*1))) | ((1 << (2*1)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*2))) | ((1 << (2*2)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*3))) | ((1 << (2*3)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*4))) | ((1 << (2*4)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*5))) | ((1 << (2*5)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*6))) | ((1 << (2*6)) & 0x1FFF);
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(3 << (2*7))) | ((1 << (2*7)) & 0x1FFF);

    cardStat.bannerFormat = (cardStat.bannerFormat & ~(0x1<<2));
}

void init_replay_file_data(void)
{
    DVDFileInfo file;
    struct ReplayHeader replayHdr;
    char category[12];
    char replayFileName[68];
    OSCalendarTime calendarTime;

    void *buffer = OSAlloc(0x1800);
    if (buffer == NULL)
        OSPanic("memcard.c", 1014, "cannot OSAlloc");
    recplay_get_header(11, &replayHdr);

    // copy banner image
    if (DVDOpen("preview/96x32.tpl", &file) == 0)
        OSPanic("memcard.c", 1026, "cannot open replay banner image");
    if (avDVDRead(&file, buffer, 0x1800, (replayHdr.stageId - 1) * 0x1800) == 0)
        OSPanic("memcard.c", 1029, "cannot read replay banner image");
    memcpy(memcardReplayData->bannerImg, buffer, 0x1800);
    DVDClose(&file);

    // copy replay icon
    if (DVDOpen("replay_icon.bin", &file) == 0)
        OSPanic("memcard.c", 1040, "cannot open replay_icon.bin");
    if (avDVDRead(&file, buffer, 0x800, 0) == 0)
        OSPanic("memcard.c", 1043, "cannot read replay_icon.bin");
    memcpy(memcardReplayData->replayIcon, buffer, 0x800);
    DVDClose(&file);

    OSFree(buffer);

    cardStat.commentAddr = (u32)memcardReplayData->comment - (u32)memcardReplayData;
    strncpy(memcardReplayData->comment, "Super Monkey Ball", 32);

    if (replayHdr.flags & REPLAY_FLAG_MASTER)
    {
        strcpy(category, "Master");
    }
    else
    {
        if (replayHdr.flags & REPLAY_FLAG_EXTRA)
        {
            switch (replayHdr.difficulty)
            {
            case DIFFICULTY_BEGINNER: strcpy(category, "Beg.Ext"); break;
            case DIFFICULTY_ADVANCED: strcpy(category, "Adv.Ext"); break;
            case DIFFICULTY_EXPERT:   strcpy(category, "Exp.Ext"); break;
            }
        }
        else
        {
            switch (replayHdr.difficulty)
            {
            case DIFFICULTY_BEGINNER: strcpy(category, "Beg.FL"); break;
            case DIFFICULTY_ADVANCED: strcpy(category, "Adv.FL"); break;
            case DIFFICULTY_EXPERT:   strcpy(category, "Exp.FL"); break;
            }
        }
    }

    OSTicksToCalendarTime(memcardInfo.time, &calendarTime);
    sprintf(
        replayFileName,
        "%s%d %02d-%02d-%02d %02d:%02d",
        category,
        replayHdr.floorNum,
        calendarTime.mon + 1,
        calendarTime.mday,
        calendarTime.year % 100,
        calendarTime.hour,
        calendarTime.min);
    strncpy(memcardReplayData->unk2030, replayFileName, 32);

    cardStat.iconAddr = (u32)memcardReplayData->bannerImg - (u32)memcardReplayData;
    cardStat.bannerFormat = (cardStat.bannerFormat & ~0x3) | 2;
    cardStat.iconFormat = (cardStat.iconFormat & ~0x3) | 2;
    cardStat.iconSpeed = (cardStat.iconSpeed & ~0x3) | 3;
    cardStat.iconSpeed = (cardStat.iconSpeed & ~(0x3<<2));
    cardStat.bannerFormat = (cardStat.bannerFormat & ~(0x1<<2));

    memcardReplayData->header.replayFlags = replayHdr.flags;
    memcardReplayData->header.unk4 = replayHdr.stageId;
    memcardReplayData->header.difficulty = replayHdr.difficulty;
    memcardReplayData->header.floorNum = replayHdr.floorNum;
    memcardReplayData->header.character = replayHdr.character;
    memcardReplayData->header.unk8 = replayHdr.unk10;

    memcardReplayData->header.timestamp = OSTicksToSeconds((u64)memcardInfo.time);
}

void probe_memcard(void)
{
    s32 dummyMemSize;
    s32 result;

    result = CARDProbeEx(0, &dummyMemSize, &memcardInfo.sectorSize);
    if (result != CARD_RESULT_NOCARD)
    {
        if (result != CARD_RESULT_BUSY)
            memcardInfo.unk40 = 0;
        if (result < -1)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
        }
    }

    memcardInfo.statusFlags &= ~(1 << 2);
    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.statusFlags &= ~(1 << 2);
            if (memcardInfo.statusFlags & (1 << 6))
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.unk42 = 0;
                memcardInfo.msg = &msgMemCardNotInsertedAutosaveOff;
                memcardInfo.state = MC_STATE_ERROR;
            }
            else
            {
                memcardInfo.msg = &msgMemCardNotInsertedSlotA;
                memcardInfo.state = 1;
            }
        }
        else
            memcardInfo.statusFlags |= (1 << 2);
        break;
    case CARD_RESULT_WRONGDEVICE:
        memcardInfo.msg = &msgMemCardNotInserted;
        memcardInfo.state = (memcardInfo.statusFlags & (1 << 6)) ? MC_STATE_ERROR : MC_STATE_UNK1;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.sectorSize != 0x2000)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantUse2;
            memcardInfo.state = MC_STATE_ERROR;
        }
        else
        {
            // round up to multiple of sector size
            int sizeMask = memcardInfo.sectorSize - 1;
            memcardInfo.fileSize = (memcardInfo.unk44 + sizeMask) & ~sizeMask;
            memcardInfo.state = 3;
        }
        break;
    }
}

void mount_memcard(void)
{
    s32 result = CARDMountAsync(0, cardWorkArea, NULL, NULL);

    if (result < 0)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardNotSupported;
        memcardInfo.state = (memcardInfo.statusFlags & (1 << 6)) ? MC_STATE_ERROR : MC_STATE_UNK1;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_WRONGDEVICE:
        memcardInfo.msg = &msgMemCardNotInserted;
        memcardInfo.state = (memcardInfo.statusFlags & (1 << 6)) ? MC_STATE_ERROR : MC_STATE_UNK1;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 4;
        break;
    }
}

void check_mount_memcard_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_WRONGDEVICE:
        memcardInfo.msg = &msgMemCardNotInserted;
        memcardInfo.state = (memcardInfo.statusFlags & (1 << 6)) ? MC_STATE_ERROR : MC_STATE_UNK1;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_ENCODING:
    case CARD_RESULT_BROKEN:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        // fall through
    case CARD_RESULT_READY:
        memcardInfo.state = 5;
        memcardInfo.statusFlags |= MC_STATUS_MOUNTED;
        break;
    }
}

void verify_filesystem(void)
{
    s32 result = CARDCheckAsync(0, NULL);

    if (result < 0)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 6;
        break;
    }
}

void check_verify_filesystem_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_WRONGDEVICE:
        memcardInfo.msg = &msgMemCardNotInserted;
        memcardInfo.state = (memcardInfo.statusFlags & (1 << 6)) ? MC_STATE_ERROR : MC_STATE_UNK1;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_BROKEN:
    case CARD_RESULT_ENCODING:
        if ((memcardInfo.statusFlags & (MC_STATUS_SAVING | (1 << 7))) == (MC_STATUS_SAVING | (1 << 7)))
        {
            memcardInfo.msg = &msgMemCardDamaged;
            memcardInfo.state = MC_STATE_FORMAT_PROMPT;
            lbl_802F21B1 = 0;
            memcardInfo.statusFlags |= (1 << 10);
        }
        else
        {
            memcardInfo.unk42 = 0;
            memcardInfo.msg = &msgMemCardFileDamagedPleaseFormat;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardMode == MC_MODE_SAVE_REPLAY)
        {
            memcardInfo.unk40 = 0x4B0;
            memcardInfo.state = MC_STATE_CHECK_FREE_SPACE;
        }
        else if (memcardMode == MC_MODE_LIST_REPLAY)
            memcardInfo.state = 0x18;
        else if (memcardInfo.statusFlags & MC_STATUS_SAVING)
            memcardInfo.state = 0x21;
        else
            memcardInfo.state = 7;
        break;
    }
}

void open_memcard_file(void)
{
    s32 result = CARDOpen(0, memcardInfo.fileName, &memcardInfo.cardFileInfo);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        if (memcardInfo.statusFlags & MC_STATUS_SAVING)
        {
            if (memcardInfo.state == 0x22)
                memcardInfo.state = 7;
            else
            {
                memcardInfo.fileNo = -1;
                memcardInfo.unk40 = 0x4B0;
                memcardInfo.state = MC_STATE_CHECK_FREE_SPACE;
            }
        }
        else
            memcardInfo.state = 0x1F;
        break;
    case CARD_RESULT_NOPERM:
        if ((memcardInfo.statusFlags & (MC_STATUS_SAVING | (1 << 7))) == (MC_STATUS_SAVING | (1 << 7)))
        {
            memcardInfo.msg = &msgCantSaveFile;
            memcardInfo.state = MC_STATE_FORMAT_PROMPT;
            lbl_802F21B1 = 0;
            memcardInfo.statusFlags |= (1 << 10);
        }
        else
        {
            memcardInfo.msg = &msgCantLoadFile;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_BROKEN:
        if ((memcardInfo.statusFlags & (MC_STATUS_SAVING | (1 << 7))) == (MC_STATUS_SAVING | (1 << 7)))
        {
            memcardInfo.msg = &msgMemCardDamaged;
            memcardInfo.state = MC_STATE_FORMAT_PROMPT;
            lbl_802F21B1 = 0;
            memcardInfo.statusFlags |= (1 << 10);
        }
        else
        {
            memcardInfo.unk42 = 0;
            memcardInfo.msg = &msgMemCardFileDamagedPleaseFormat;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        if ((memcardInfo.statusFlags & (MC_STATUS_SAVING | MC_STATUS_GAMEDATA_FILE)) == (MC_STATUS_SAVING | MC_STATUS_GAMEDATA_FILE))
        {
            memcardInfo.fileNo = memcardInfo.cardFileInfo.fileNo;
            CARDClose(&memcardInfo.cardFileInfo);
            if (memcardInfo.state == 0x22)
                memcardInfo.state = 0x23;
            else
            {
                memcardInfo.unk40 = 0x4B0;
                memcardInfo.state = MC_STATE_CHECK_FREE_SPACE;
                memcardInfo.statusFlags |= (1 << 16);
            }
        }
        else
        {
            memcardInfo.statusFlags |= MC_STATUS_OPEN;
            memcardInfo.state = 0xF;
        }
        break;
    }
}

void check_card_free_space(void)
{
    s32 freeBytes;
    s32 freeFiles;
    s32 result = CARDFreeBlocks(0, &freeBytes, &freeFiles);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BROKEN:
        memcardInfo.msg = &msgMemCardFileDamagedPleaseFormat;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        else
        {
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.state == MC_STATE_CHECK_FREE_SPACE)
        {
            if (freeFiles < 1)
            {
                memcardInfo.msg = &msgCantMakeMoreFiles;
                memcardInfo.state = MC_STATE_ERROR;
            }
            else if (memcardMode == MC_MODE_LOAD_GAMEDATA_2)
            {
                freeBytes /= BLOCK_SIZE;
                if (freeBytes < 13)
                {
                    memcardInfo.statusFlags |= (1 << 19) | (1 << 20);
                    memcardInfo.unk4D = freeBytes;
                    memcardInfo.unk42 = 0;
                    if (freeBytes < 2)
                    {
                        if (memcardInfo.statusFlags & (1 << 18))
                            memcardInfo.msg = &lbl_802F1510;
                        else
                            memcardInfo.msg = &lbl_802F1528;
                    }
                    else if (freeBytes < 3)
                    {
                        if (memcardInfo.statusFlags & (1 << 18))
                            memcardInfo.msg = &lbl_802F1518;
                        else
                            memcardInfo.msg = &lbl_802F1530;
                    }
                    else
                    {
                        if (memcardInfo.statusFlags & (1 << 18))
                            memcardInfo.msg = &lbl_802F1520;
                        else
                            memcardInfo.msg = &lbl_802F1538;
                    }
                }
                else
                {
                    if (memcardInfo.statusFlags & (1 << 18))
                    {
                        memcardInfo.unk42 = 0;
                        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
                        memcardInfo.statusFlags |= (1 << 3);
                    }
                    else
                        memcardInfo.msg = &lbl_802F14A0;
                }
                memcardInfo.statusFlags &= ~(1 << 17);
                memcardInfo.state = MC_STATE_ERROR;
            }
            else if (freeBytes < memcardInfo.fileSize)
            {
                if (freeBytes == 0)
                {
                    if (memcardMode == MC_MODE_SAVE_GAMEDATA_3)
                    {
                        sprintf(
                            msgFmtBuffer.lines[0].str,
                            lbl_802F14E8.lines[0].str,
                            freeBytes / BLOCK_SIZE);
                        sprintf(
                            msgFmtBuffer.lines[1].str,
                            lbl_802F14E8.lines[1].str,
                            memcardInfo.fileSize / BLOCK_SIZE);
                        sprintf(
                            msgFmtBuffer.lines[2].str,
                            lbl_802F14E8.lines[2].str);
                        msgFmtBuffer.lines[0].unk4 = lbl_802F14E8.lines[0].unk4;
                        msgFmtBuffer.lines[1].unk4 = lbl_802F14E8.lines[1].unk4;
                        msgFmtBuffer.lines[2].unk4 = lbl_802F14E8.lines[2].unk4;
                        msgFmtBuffer.numLines = lbl_802F14E8.numLines;
                    }
                    else
                    {
                        sprintf(
                            msgFmtBuffer.lines[0].str,
                            lbl_802F14E0.lines[0].str);
                        sprintf(
                            msgFmtBuffer.lines[1].str,
                            lbl_802F14E0.lines[1].str,
                            memcardInfo.fileSize / BLOCK_SIZE);
                        msgFmtBuffer.lines[0].unk4 = lbl_802F14E0.lines[0].unk4;
                        msgFmtBuffer.lines[1].unk4 = lbl_802F14E0.lines[1].unk4;
                        msgFmtBuffer.numLines = lbl_802F14E0.numLines;
                    }
                }
                else
                {
                    struct MemCardMessage *fmtMsg;

                    if (memcardMode == MC_MODE_SAVE_GAMEDATA_3)
                    {
                        if (freeBytes / BLOCK_SIZE == 1)
                            fmtMsg = &lbl_802F14D8;
                        else
                            fmtMsg = &lbl_802F14D0;
                        sprintf(
                            msgFmtBuffer.lines[0].str,
                            fmtMsg->lines[0].str,
                            freeBytes / BLOCK_SIZE);
                        sprintf(
                            msgFmtBuffer.lines[1].str,
                            fmtMsg->lines[1].str,
                            memcardInfo.fileSize / BLOCK_SIZE);
                        sprintf(
                            msgFmtBuffer.lines[2].str,
                            fmtMsg->lines[2].str);
                        msgFmtBuffer.lines[0].unk4 = fmtMsg->lines[0].unk4;
                        msgFmtBuffer.lines[1].unk4 = fmtMsg->lines[1].unk4;
                        msgFmtBuffer.lines[2].unk4 = fmtMsg->lines[2].unk4;
                        msgFmtBuffer.numLines = fmtMsg->numLines;
                    }
                    else
                    {
                        if (freeBytes / BLOCK_SIZE == 1)
                            fmtMsg = &lbl_802F14C8;
                        else
                            fmtMsg = &lbl_802F14C0;
                        sprintf(
                            msgFmtBuffer.lines[0].str,
                            fmtMsg->lines[0].str);
                        sprintf(
                            msgFmtBuffer.lines[1].str,
                            fmtMsg->lines[1].str,
                            freeBytes / BLOCK_SIZE);
                        sprintf(
                            msgFmtBuffer.lines[2].str,
                            fmtMsg->lines[2].str,
                            memcardInfo.fileSize / BLOCK_SIZE);
                        msgFmtBuffer.lines[0].unk4 = fmtMsg->lines[0].unk4;
                        msgFmtBuffer.lines[1].unk4 = fmtMsg->lines[1].unk4;
                        msgFmtBuffer.lines[2].unk4 = fmtMsg->lines[2].unk4;
                        msgFmtBuffer.numLines = fmtMsg->numLines;
                    }
                }
                memcardInfo.msg = &msgFmtBuffer;
                memcardInfo.state = MC_STATE_ERROR;
            }
            else
            {
                memcardInfo.unk42 = 0;
                memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
                memcardInfo.state = MC_STATE_OVERWRITE_PROMPT;
                if (memcardInfo.statusFlags & MC_STATUS_GAMEDATA_FILE
                 && ((memcardInfo.statusFlags & ((1 << 16) | (1 << 7))) == ((1 << 16) | (1 << 7))))
                {
                    lbl_802F21B1 = 0;
                    memcardInfo.statusFlags |= (1 << 10);
                }
            }
        }
        else
        {
            freeBytes /= BLOCK_SIZE;
            if (freeFiles < 1)
                memcardInfo.msg = &msgSaveSuccessNoMoreFiles;
            else
            {
                if (freeBytes < 13)
                {
                    memcardInfo.statusFlags |= (1 << 20);
                    memcardInfo.unk4D = freeBytes;
                    memcardInfo.unk42 = 0;
                    if (freeBytes < 2)
                        memcardInfo.msg = &lbl_802F14F8;
                    else if (freeBytes < 3)
                        memcardInfo.msg = &lbl_802F1500;
                    else
                        memcardInfo.msg = &lbl_802F1508;
                }
                else
                {
                    if (memcardMode == MC_MODE_SAVE_GAMEDATA_3)
                    {
                        memcardInfo.unk42 = 0;
                        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
                        memcardInfo.statusFlags &= ~(1 << 19);
                    }
                    else
                        memcardInfo.msg = &msgSaveFinished;
                }
            }
            memcardInfo.statusFlags &= ~(1 << 15);
            memcardInfo.statusFlags |= (1 << 3);
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    }
}

void create_memcard_file(void)
{
    s32 status = CARDCreateAsync(0, memcardInfo.fileName, memcardInfo.fileSize, &memcardInfo.cardFileInfo, NULL);

    if (status < 0)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (status)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOENT:
        memcardInfo.msg = &msgCantMakeMoreFiles;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_INSSPACE:
        sprintf(msgFmtBuffer.lines[0].str, lbl_802F14F0.lines[0].str);
        sprintf(msgFmtBuffer.lines[1].str, lbl_802F14F0.lines[1].str);
        msgFmtBuffer.lines[0].unk4 = lbl_802F14F0.lines[0].unk4;
        msgFmtBuffer.lines[1].unk4 = lbl_802F14F0.lines[1].unk4;
        msgFmtBuffer.numLines = lbl_802F14F0.numLines;
        memcardInfo.msg = &msgFmtBuffer;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = MC_STATE_CHECK_CREATE_FILE_RESULT;
        break;
    }
}

void check_create_memcard_file_result(void)
{
    s32 status = CARDGetResultCode(0);

    if (status != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (status < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (status)
    {
    case CARD_RESULT_BROKEN:
    case CARD_RESULT_NAMETOOLONG:
    case CARD_RESULT_FATAL_ERROR:
    default:
        printf("fatal: %d\n", (int)status);
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOENT:
        memcardInfo.msg = &msgCantMakeMoreFiles;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_INSSPACE:
        sprintf(msgFmtBuffer.lines[0].str, lbl_802F14F0.lines[0].str);
        sprintf(msgFmtBuffer.lines[1].str, lbl_802F14F0.lines[1].str);
        msgFmtBuffer.lines[0].unk4 = lbl_802F14F0.lines[0].unk4;
        msgFmtBuffer.lines[1].unk4 = lbl_802F14F0.lines[1].unk4;
        msgFmtBuffer.numLines = lbl_802F14F0.numLines;
        memcardInfo.msg = &msgFmtBuffer;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_READY:
        memcardInfo.statusFlags |= MC_STATUS_OPEN | (1 << 11);
        memcardInfo.state = 0x13;
        break;
    }
}

void write_memcard_file(void *data)
{
    s32 result = CARDWriteAsync(
        &memcardInfo.cardFileInfo,
        data,
        memcardInfo.fileSize,
        0,
        NULL);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_LIMIT:
        memcardInfo.msg = &msgFileSizeChanged;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x12;
        break;
    }
}

void check_write_memcard_file_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_NOPERM:
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_LIMIT:
        memcardInfo.msg = &msgFileSizeChanged;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_CANCELED:
        memcardInfo.msg = &msgSaveInterrupted;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        memcardInfo.state = 0x15;
        break;
    }
}

void set_memcard_file_metadata(void)
{
    s32 result = CARDSetStatusAsync(0, memcardInfo.cardFileInfo.fileNo, &cardStat, 0);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x16;
        break;
    }
}

void check_set_memcard_file_metadata_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_NOFILE:
    case CARD_RESULT_NOPERM:
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.statusFlags & MC_STATUS_GAMEDATA_FILE)
        {
            memcardInfo.statusFlags |= (1 << 22);
            if (memcardInfo.statusFlags & (1 << 16))
                memcardInfo.state = 11;
            else
                memcardInfo.state = 31;
        }
        else
        {
            memcardInfo.unk40 = 0x4B0;
            memcardInfo.state = 30;
        }
        break;
    }
}

void read_memcard_file(void *buffer)
{
    s32 result = CARDReadAsync(&memcardInfo.cardFileInfo, buffer, memcardInfo.fileSize, 0, NULL);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_LIMIT:
        memcardInfo.msg = &msgFileSizeChanged;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x10;
        break;
    }
}

void check_read_memcard_file_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_NOFILE:
    case CARD_RESULT_NOPERM:
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_LIMIT:
        memcardInfo.msg = &msgFileSizeChanged;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_CANCELED:
        memcardInfo.msg = &msgLoadInterrupted;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.statusFlags & MC_STATUS_REPLAY_FILE)
        {
            if (memcardReplayData->header.crc == mathutil_calc_crc16(memcardInfo.fileSize - 2, (u8 *)memcardReplayData + 2)
             && recplay_cmpr_set_data(memcardReplayData->compressedReplay) != 0)
            {
                memcardInfo.statusFlags |= (1 << 3);
                memcardInfo.state = MC_STATE_ERROR;
            }
            else
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.statusFlags |= (1 << 14);
                memcardInfo.msg = &msgReplayDataDamaged;
                memcardInfo.state = MC_STATE_ERROR;
            }
        }
        else if (memcardInfo.statusFlags & MC_STATUS_SAVING)
            memcardInfo.state = 0x13;
        else
        {
            u8 *r4 = (u8 *)memcardContents + 2;
            if (memcardContents->crc != mathutil_calc_crc16(0x5C04 - (r4 - (u8 *)memcardContents), r4))
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.statusFlags |= (1 << 14);
                memcardInfo.msg = &msgGameDataDamaged;
                memcardInfo.state = MC_STATE_ERROR;
            }
            else
            {
                if (memcardContents->version != 0x16)
                {
                    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                    memcardInfo.statusFlags |= MC_STATUS_ERROR;
                    memcardInfo.msg = &msgGameDataWrongVersion;
                    memcardInfo.state = MC_STATE_ERROR;
                }
                else
                    memcardInfo.state = 0x25;
            }
        }
        break;
    }
}

void get_memcard_file_metadata(void)
{
    s32 result = CARDGetStatus(0, memcardInfo.cardFileInfo.fileNo, &cardStat);

    if (result != -1)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
        memcardInfo.msg = &lbl_802F14A0;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOPERM:
        memcardInfo.msg = &msgCantReadFile;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.statusFlags & MC_STATUS_SAVING)
            memcardInfo.state = 0x11;
        else
        {
            if ((memcardInfo.statusFlags & (1 << 6)) == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
            }
            memcardInfo.msg = &msgLoadFinished;
            memcardInfo.state = MC_STATE_ERROR;
            memcardInfo.statusFlags |= (1 << 3);
            memcardInfo.statusFlags &= ~(1 << 17);
        }
        break;
    }
}

void format_memcard(void)
{
    extern s32 __CARDFormatRegionAsync(s32 chan, CARDCallback callback);
    s32 result = __CARDFormatRegionAsync(0, 0);  // calling private SDK function?

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 10;
        break;
    }
}

void check_format_memcard_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != -1)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_IOERROR:
        memcardInfo.msg = &msgMemCardCantUse;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        memcardInfo.state = MC_STATE_OVERWRITE_PROMPT;
        break;
    }
}

void get_memcard_file_metadata_2(void)
{
    struct ReplayFileInfo *replay;
    int result = CARDGetStatus(0, lbl_802F21B2, &cardStat);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (strncmp((char *)cardStat.gameName, "GMBE", 4) == 0
         && strncmp((char *)cardStat.company, "8P", 2) == 0
         && strncmp((char *)cardStat.fileName, "smkb", 4) == 0)
        {
            replay = &replayFileInfo[replayFileIndex];
            replay->fileNo = lbl_802F21B2;
            replay->fileSize = cardStat.length;
            replayFileIndex++;
        }
        // fall through
    case CARD_RESULT_NOFILE:
    case CARD_RESULT_NOPERM:
        if (++lbl_802F21B2 >= 0x7F)
        {
            replay = &replayFileInfo[replayFileIndex];
            replay->fileNo = -1;
            if (replayFileIndex == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.msg = &msgNoReplayData;
                memcardInfo.state = MC_STATE_ERROR;
                memcardInfo.statusFlags &= ~(1 << 17);
            }
            else
                memcardInfo.state = 0x1A;
        }
        else
        {
            memcardInfo.unk40 = 0x4B0;
            memset(&cardStat, 0, sizeof(cardStat));
        }
        break;
    }
}

void replay_list_open_and_read(void)
{
    struct ReplayFileInfo *replay = &replayFileInfo[replayFileIndex];
    s32 result;

    if (lbl_802F21C8 == 0)
    {
        result = CARDFastOpen(0, replay->fileNo, &memcardInfo.cardFileInfo);

        if (result != CARD_RESULT_BUSY)
            memcardInfo.unk40 = 0;
        if (result < -1)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
        }

        switch (result)
        {
        case CARD_RESULT_FATAL_ERROR:
        default:
            memcardInfo.msg = &msgMemCardError;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_NOCARD:
            memcardInfo.msg = &msgMemCardRemoved;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_NOFILE:
            memcardInfo.msg = &lbl_802F14A0;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_NOPERM:
            memcardInfo.msg = &msgCantLoadFile;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_BUSY:
            if (memcardInfo.unk40 == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.msg = &msgMemCardCantRead;
                memcardInfo.state = MC_STATE_ERROR;
            }
            break;
        case CARD_RESULT_BROKEN:
            memcardInfo.msg = &msgMemCardFileDamagedPleaseFormat;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_READY:
            memcardInfo.statusFlags |= MC_STATUS_OPEN;
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            lbl_802F21C8 = 1;
            memcardInfo.unk40 = 0x4B0;
            break;
        }
    }
    else if (lbl_802F21C8 == 1)
    {
        if ((memcardReplayData = OSAlloc(replay->fileSize)) == NULL)
            OSPanic("memcard.c", 2506, "cannot OSAlloc");
        result = CARDReadAsync(&memcardInfo.cardFileInfo, memcardReplayData, replay->fileSize, 0, NULL);

        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;

        switch (result)
        {
        case CARD_RESULT_FATAL_ERROR:
            memcardInfo.msg = &msgMemCardError;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_LIMIT:
            memcardInfo.msg = &msgFileSizeChanged;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_NOCARD:
            memcardInfo.msg = &msgMemCardRemoved;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        default:
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            memcardInfo.unk40 = 0x4B0;
            lbl_802F21C8 = 2;
            break;
        }
    }
    else
    {
        result = CARDGetResultCode(0);

        if (result != CARD_RESULT_BUSY)
            memcardInfo.unk40 = 0;
        if (result < -1)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
        }

        switch (result)
        {
        case CARD_RESULT_NOPERM:
        case CARD_RESULT_FATAL_ERROR:
        default:
            memcardInfo.msg = &msgMemCardError;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_IOERROR:
            memcardInfo.msg = &msgMemCardCantUse;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_LIMIT:
            memcardInfo.msg = &msgFileSizeChanged;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_CANCELED:
            memcardInfo.msg = &msgLoadInterrupted;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_NOCARD:
            memcardInfo.msg = &msgMemCardRemoved;
            memcardInfo.state = MC_STATE_ERROR;
            break;
        case CARD_RESULT_BUSY:
            if (memcardInfo.unk40 == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.msg = &msgMemCardCantRead;
                memcardInfo.state = MC_STATE_ERROR;
            }
            break;
        case CARD_RESULT_READY:
            CARDClose(&memcardInfo.cardFileInfo);
            if (mathutil_calc_crc16(replay->fileSize - 2, (u8 *)memcardReplayData + 2) != memcardReplayData->header.crc)
            {
                replay->header.replayFlags = 0x100;
                replay->header.unk4 = 1;
                replay->header.difficulty = 0;
                replay->header.floorNum = 0;
                replay->header.character = 0;
                replay->header.unk8 = 0;
                replay->header.timestamp = 0;
            }
            else
            {
                replay->header.replayFlags = memcardReplayData->header.replayFlags;
                replay->header.unk4 = memcardReplayData->header.unk4;
                replay->header.difficulty = memcardReplayData->header.difficulty;
                replay->header.floorNum = memcardReplayData->header.floorNum;
                replay->header.character = memcardReplayData->header.character;
                replay->header.unk8 = memcardReplayData->header.unk8;
                replay->header.timestamp = memcardReplayData->header.timestamp;
            }
            OSFree(memcardReplayData);
            memcardReplayData = NULL;
            if (replayFileIndex == 0)
            {
                memcardInfo.statusFlags |= (1 << 3);
                memcardInfo.state = MC_STATE_ERROR;
                memcardInfo.statusFlags &= ~(1 << 17);
            }
            else
            {
                lbl_802F21C8 = 0;
                replayFileIndex--;
                memcardInfo.unk40 = 0x4B0;
            }
            break;
        }
    }
}

void open_replay_file(void)
{
    s32 result = CARDFastOpen(0, replayFileInfo[lbl_802F21C0].fileNo, &memcardInfo.cardFileInfo);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
        memcardInfo.msg = &lbl_802F14A0;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOPERM:
        memcardInfo.msg = &msgCantLoadFile;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_BROKEN:
        memcardInfo.msg = &msgMemCardFileDamagedPleaseFormat;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_READY:
        memcardInfo.statusFlags |= MC_STATUS_OPEN;
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.state = 15;
        break;
    }
}

void delete_memcard_file(int fileNo)
{
    s32 result;

    if (fileNo < 0)
        result = CARD_RESULT_NOFILE;
    else
        result = CARDFastDeleteAsync(0, fileNo, NULL);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        if (memcardInfo.state == 11)
            memcardInfo.state = 12;
        else
            memcardInfo.state = 0x24;
        break;
    }
}

void check_delete_memcard_file_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
        memcardInfo.msg = &lbl_802F14A0;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOPERM:
        memcardInfo.msg = &msgCantLoadFile;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.statusFlags & MC_STATUS_GAMEDATA_FILE)
        {
            if (memcardInfo.state == 0x24)
                memcardInfo.state = 7;
            else
                memcardInfo.state = 31;
        }
        else
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgDeleteFinished;
            memcardInfo.state = MC_STATE_ERROR;
            memcardInfo.statusFlags |= (1 << 3);
        }
        break;
    }
}

void delete_gamedata_file(void)
{
    s32 result = CARDDeleteAsync(0, "super_monkey_ball.000", NULL);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x26;
        break;
    }
}

void check_delete_gamedata_file_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOPERM:
        memcardInfo.msg = &msgCantLoadFile;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_NOFILE:
    case CARD_RESULT_READY:
        if (memcardInfo.statusFlags & (1 << 6))
        {
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            memcardInfo.unk40 = 0x4B0;
            memcardInfo.state = MC_STATE_CHECK_FREE_SPACE;
            memcardInfo.statusFlags |= (1 << 18);
        }
        else
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgLoadFinished;
            memcardInfo.state = MC_STATE_ERROR;
            memcardInfo.statusFlags |= (1 << 3);
            memcardInfo.statusFlags &= ~(1 << 17);
        }
        break;
    }
}

void rename_gamedata_file(void)
{
    s32 result = CARDRenameAsync(0, "super_monkey_ball.000", "super_monkey_ball.sys", NULL);

    memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
    memcardInfo.statusFlags |= MC_STATUS_ERROR;

    switch (result)
    {
    case CARD_RESULT_FATAL_ERROR:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
        if (memcardMode == MC_MODE_LOAD_GAMEDATA_2)
        {
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            memcardInfo.unk40 = 0x4B0;
            memcardInfo.state = MC_STATE_CHECK_FREE_SPACE;
        }
        else
        {
            memcardInfo.msg = &lbl_802F14A0;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    default:
        memcardInfo.unk42 = 0;
        memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 32;
        break;
    }
}

void check_rename_gamedata_file_result(void)
{
    s32 result = CARDGetResultCode(0);

    if (result != CARD_RESULT_BUSY)
        memcardInfo.unk40 = 0;
    if (result < -1)
    {
        memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
        memcardInfo.statusFlags |= MC_STATUS_ERROR;
    }

    switch (result)
    {
    case CARD_RESULT_EXIST:
    case CARD_RESULT_NAMETOOLONG:
    case CARD_RESULT_FATAL_ERROR:
    default:
        memcardInfo.msg = &msgMemCardError;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOCARD:
        memcardInfo.msg = &msgMemCardRemoved;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_NOFILE:
        if (memcardInfo.statusFlags & MC_STATUS_LOADING)
        {
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            if (memcardInfo.statusFlags & (1 << 6))
            {
                memcardInfo.unk40 = 0x4B0;
                memcardInfo.state = MC_STATE_CHECK_FREE_SPACE;
            }
            else
                memcardInfo.state = 15;
        }
        else
        {
            memcardInfo.msg = &lbl_802F14A0;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_NOPERM:
        memcardInfo.msg = &msgCantLoadFile;
        memcardInfo.state = MC_STATE_ERROR;
        break;
    case CARD_RESULT_BUSY:
        if (memcardInfo.unk40 == 0)
        {
            memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
            memcardInfo.statusFlags |= MC_STATUS_ERROR;
            memcardInfo.msg = &msgMemCardCantRead;
            memcardInfo.state = MC_STATE_ERROR;
        }
        break;
    case CARD_RESULT_READY:
        if (memcardInfo.statusFlags & MC_STATUS_LOADING)
            memcardInfo.state = 7;
        else if ((memcardInfo.statusFlags & ((1 << 16) | (1 << 6))) == ((1 << 16) | (1 << 6)))
        {
            memcardInfo.state = MC_STATE_ERROR;
            memcardInfo.statusFlags &= ~(1 << 15);
            memcardInfo.statusFlags |= (1 << 3);
        }
        else
        {
            if (memcardInfo.statusFlags & (1 << 6))
                memcardInfo.statusFlags |= (1 << 19);
            memcardInfo.unk40 = 0x4B0;
            memcardInfo.state = MC_STATE_CHECK_FREE_SPACE2;
        }
        break;
    }
}

// gamedata load state machine
void load_sequence(void)
{
    switch (memcardInfo.state)
    {
    case MC_STATE_UNK1:
        if ((g_currPlayerButtons[2] & PAD_BUTTON_A)
         || !(memcardInfo.statusFlags & (1 << 7)))
        {
            memcardInfo.unk40 = 0x3C;
            memcardInfo.state = MC_STATE_PROBE;
        }
        break;
    case MC_STATE_PROBE:
        probe_memcard();
        break;
    case MC_STATE_MOUNT:
        if ((memcardContents = OSAlloc(memcardInfo.fileSize)) == NULL)
            OSPanic("memcard.c", 3062, "cannot OSAlloc");
        memset(memcardContents, 0, memcardInfo.fileSize);
        memcardInfo.statusFlags |= (1 << 17);
        mount_memcard();
        break;
    case MC_STATE_CHECK_MOUNT_RESULT:
        check_mount_memcard_result();
        break;
    case MC_STATE_VERIFY_FILESYSTEM:
        verify_filesystem();
        break;
    case MC_STATE_CHECK_VERIFY_FILESYSTEM_RESULT:
        check_verify_filesystem_result();
        break;
    case 7:
        memcardInfo.unk40 = 0x4B0;
        if (memcardInfo.state == 7)  // always true
            memcardInfo.state = MC_STATE_OPEN_FILE;
        else
            memcardInfo.state = 0x22;
        // fall through
    case MC_STATE_OPEN_FILE:
        open_memcard_file();
        break;
    case 0x1F:
        rename_gamedata_file();
        break;
    case 0x20:
        check_rename_gamedata_file_result();
        break;
    case MC_STATE_CHECK_FREE_SPACE:
        check_card_free_space();
        break;
    case 0xF:
        read_memcard_file(memcardContents);
        break;
    case 0x10:
        check_read_memcard_file_result();
        break;
    case 0x25:
        delete_gamedata_file();
        break;
    case 0x26:
        check_delete_gamedata_file_result();
        break;
    case MC_STATE_ERROR:
        break;
    default:
        printf("stat: %d\n", memcardInfo.state);
        OSPanic("memcard.c", 3143, "load_sequence\n");
        break;
    }
}

void save_sequence(void)
{
    u8 *r4;

    switch (memcardInfo.state)
    {
    case MC_STATE_UNK1:
        if ((g_currPlayerButtons[2] & PAD_BUTTON_A)
         || !(memcardInfo.statusFlags & (1 << 7)))
        {
            memcardInfo.unk40 = 0x3C;
            memcardInfo.state = MC_STATE_PROBE;
        }
        break;
    case MC_STATE_PROBE:
        probe_memcard();
        break;
    case MC_STATE_MOUNT:
        if ((memcardContents = OSAlloc(memcardInfo.fileSize)) == NULL)
            OSPanic("memcard.c", 3178, "cannot OSAlloc");
        memset(memcardContents, 0, memcardInfo.fileSize);
        mount_memcard();
        break;
    case MC_STATE_CHECK_MOUNT_RESULT:
        check_mount_memcard_result();
        break;
    case MC_STATE_VERIFY_FILESYSTEM:
        verify_filesystem();
        break;
    case MC_STATE_CHECK_VERIFY_FILESYSTEM_RESULT:
        check_verify_filesystem_result();
        break;
    case 0x21:
        memcardInfo.unk40 = 0x4B0;
        if (memcardInfo.state == 7)  // always false
            memcardInfo.state = MC_STATE_OPEN_FILE;
        else
            memcardInfo.state = 0x22;
        // fall through
    case 0x22:
        open_memcard_file();
        break;
    case 0x23:
        delete_memcard_file(memcardInfo.fileNo);
        break;
    case 0x24:
        check_delete_memcard_file_result();
        break;
    case 7:
        strcpy(memcardInfo.fileName, "super_monkey_ball.sys");
        memcardInfo.unk40 = 0x4B0;
        if (memcardInfo.state == 7)  // always true
            memcardInfo.state = MC_STATE_OPEN_FILE;
        else
            memcardInfo.state = 0x22;
        // fall through
    case MC_STATE_OPEN_FILE:
        open_memcard_file();
        break;
    case 9:
        if ((g_currPlayerButtons[2] & PAD_BUTTON_LEFT)
         || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_LEFT))
        {
            if (lbl_802F21B1 == 0)
                u_play_sound_1(0x6C);
            lbl_802F21B1 = 1;
        }
        if ((g_currPlayerButtons[2] & PAD_BUTTON_RIGHT)
         || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_RIGHT))
        {
            if (lbl_802F21B1 == 1)
                u_play_sound_1(0x6C);
            lbl_802F21B1 = 0;
        }
        if (g_currPlayerButtons[2] & PAD_BUTTON_A)
        {
            u_play_sound_1(0x6A);
            memcardInfo.statusFlags &= ~(1 << 10);
            if (lbl_802F21B1 == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.msg = &msgFormatInterrupted;
                memcardInfo.state = MC_STATE_ERROR;
            }
            else
            {
                memcardInfo.statusFlags |= MC_STATUS_WRITE_IN_PROGRESS;
                format_memcard();
            }
        }
        break;
    case 0xA:
        check_format_memcard_result();
        if (memcardInfo.state != 10)
            memcardInfo.statusFlags &= ~MC_STATUS_WRITE_IN_PROGRESS;
        break;
    case MC_STATE_CHECK_FREE_SPACE:
        check_card_free_space();
        break;
    case 0xD:
        if (memcardInfo.statusFlags & (1 << 10))
        {
            if ((g_currPlayerButtons[2] & PAD_BUTTON_LEFT)
             || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_LEFT))
            {
                if (lbl_802F21B1 == 0)
                    u_play_sound_1(0x6C);
                lbl_802F21B1 = 1;
            }
            if ((g_currPlayerButtons[2] & PAD_BUTTON_RIGHT)
             || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_RIGHT))
            {
                if (lbl_802F21B1 == 1)
                    u_play_sound_1(0x6C);
                lbl_802F21B1 = 0;
            }
            if (!(g_currPlayerButtons[2] & PAD_BUTTON_A))
                break;
            u_play_sound_1(0x6A);
            memcardInfo.statusFlags &= ~(1 << 10);
            if (lbl_802F21B1 == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.msg = &msgSaveInterrupted;
                memcardInfo.state = MC_STATE_ERROR;
                break;
            }
        }
        strcpy(memcardInfo.fileName, "super_monkey_ball.000");
        memcardInfo.statusFlags |= MC_STATUS_WRITE_IN_PROGRESS | (1 << 15);
        create_memcard_file();
        break;
    case MC_STATE_CHECK_CREATE_FILE_RESULT:
        check_create_memcard_file_result();
        break;
    case 0x13:
        memset(&cardStat, 0, sizeof(cardStat));
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x14;
        break;
    case 0x14:
        get_memcard_file_metadata();
        break;
    case 0x11:
        init_gamedata_file();
        func_800A4E70();
        memcardContents->version = 0x16;
        r4 = (u8 *)memcardContents + 2;
        memcardContents->crc = mathutil_calc_crc16(0x5C04 - (r4 - (u8 *)memcardContents), r4);
        write_memcard_file(memcardContents);
        break;
    case 0x12:
        check_write_memcard_file_result();
        break;
    case 0x15:
        set_memcard_file_metadata();
        break;
    case 0x16:
        check_set_memcard_file_metadata_result();
        break;
    case 0xB:
        delete_memcard_file(memcardInfo.fileNo);
        break;
    case 0xC:
        check_delete_memcard_file_result();
        break;
    case 0x1F:
        rename_gamedata_file();
        break;
    case 0x20:
        check_rename_gamedata_file_result();
        if (memcardInfo.state != 0x20)
            memcardInfo.statusFlags &= ~MC_STATUS_WRITE_IN_PROGRESS;
        break;
    case MC_STATE_CHECK_FREE_SPACE2:
        check_card_free_space();
        break;
    case MC_STATE_ERROR:
        break;
    default:
        printf("stat: %d\n", memcardInfo.state);
        OSPanic("memcard.c", 3392, "save_sequence\n");
        break;
    }
}

void replay_save_sequence(void)
{
    switch (memcardInfo.state)
    {
    case 1:
        if (g_currPlayerButtons[2] & PAD_BUTTON_A)
        {
            u_play_sound_1(0x6A);
            memcardInfo.unk40 = 0x3C;
            memcardInfo.state = 2;
        }
        break;
    case 2:
        probe_memcard();
        break;
    case 3:
        if ((memcardReplayData = OSAlloc(memcardInfo.fileSize)) == NULL)
            OSPanic("memcard.c", 3427, "cannot OSAlloc");
        memset(memcardReplayData, 0, memcardInfo.fileSize);
        mount_memcard();
        break;
    case 4:
        check_mount_memcard_result();
        break;
    case 5:
        verify_filesystem();
        break;
    case 6:
        check_verify_filesystem_result();
        break;
    case 9:
        if ((g_currPlayerButtons[2] & PAD_BUTTON_LEFT)
         || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_LEFT))
        {
            if (lbl_802F21B1 == 0)
                u_play_sound_1(0x6C);
            lbl_802F21B1 = 1;
        }
        if ((g_currPlayerButtons[2] & PAD_BUTTON_RIGHT)
         || (g_currPlayerAnalogButtons[2] & PAD_BUTTON_RIGHT))
        {
            if (lbl_802F21B1 == 1)
                u_play_sound_1(0x6C);
            lbl_802F21B1 = 0;
        }
        if (g_currPlayerButtons[2] & PAD_BUTTON_A)
        {
            u_play_sound_1(0x6A);
            memcardInfo.statusFlags &= ~(1 << 10);
            if (lbl_802F21B1 == 0)
            {
                memcardInfo.unk42 = (memcardInfo.statusFlags & (1 << 6)) ? 0xB4 : 0;
                memcardInfo.statusFlags |= MC_STATUS_ERROR;
                memcardInfo.msg = &msgFormatInterrupted;
                memcardInfo.state = MC_STATE_ERROR;
            }
            else
            {
                memcardInfo.statusFlags |= MC_STATUS_WRITE_IN_PROGRESS;
                format_memcard();
            }
        }
        break;
    case 0xA:
        check_format_memcard_result();
        if (memcardInfo.state != 10)
            memcardInfo.statusFlags &= ~MC_STATUS_WRITE_IN_PROGRESS;
        break;
    case MC_STATE_CHECK_FREE_SPACE:
        check_card_free_space();
        break;
    case 0xD:
        memcardInfo.statusFlags |= MC_STATUS_WRITE_IN_PROGRESS;
        create_memcard_file();
        break;
    case MC_STATE_CHECK_CREATE_FILE_RESULT:
        check_create_memcard_file_result();
        break;
    case 0x13:
        memcardInfo.statusFlags |= (1 << 15);
        memset(&cardStat, 0, sizeof(cardStat));
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = MC_STATE_GET_METADATA;
        break;
    case MC_STATE_GET_METADATA:
        get_memcard_file_metadata();
        break;
    case 0x11:
        init_replay_file_data();
        recplay_cmpr_get_data(memcardReplayData->compressedReplay);
        memcardReplayData->header.crc = mathutil_calc_crc16(memcardInfo.fileSize - 2, (u8 *)memcardReplayData + 2);
        write_memcard_file(memcardReplayData);
        break;
    case 0x12:
        check_write_memcard_file_result();
        break;
    case 0x15:
        set_memcard_file_metadata();
        break;
    case 0x16:
        check_set_memcard_file_metadata_result();
        if (memcardInfo.state != 0x16)
            memcardInfo.statusFlags &= ~MC_STATUS_WRITE_IN_PROGRESS;
        break;
    case MC_STATE_CHECK_FREE_SPACE2:
        check_card_free_space();
        break;
    case MC_STATE_ERROR:
        break;
    default:
        printf("stat: %d\n", memcardInfo.state);
        OSPanic("memcard.c", 3561, "replay_save_sequence\n");
        break;
    }
}

void replay_list_sequence(void)
{
    switch (memcardInfo.state)
    {
    case 1:
        if (g_currPlayerButtons[2] & PAD_BUTTON_A)
        {
            u_play_sound_1(0x6A);
            memcardInfo.unk40 = 0x3C;
            memcardInfo.state = 2;
        }
        break;
    case 2:
        probe_memcard();
        break;
    case 3:
        memcardInfo.statusFlags |= (1 << 17);
        mount_memcard();
        break;
    case 4:
        check_mount_memcard_result();
        break;
    case 5:
        verify_filesystem();
        break;
    case 6:
        check_verify_filesystem_result();
        break;
    case 0x18:
        lbl_802F21B2 = 0;
        replayFileIndex = 0;
        memset(&cardStat, 0, sizeof(cardStat));
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x19;
        break;
    case 0x19:
        get_memcard_file_metadata_2();
        break;
    case 0x1A:
        lbl_802F21C8 = 0;
        replayFileIndex--;
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x1B;
        break;
    case 0x1B:
        replay_list_open_and_read();
        break;
    case MC_STATE_ERROR:
        break;
    default:
        printf("stat: %d\n", memcardInfo.state);
        OSPanic("memcard.c", 3641, "replay_list_sequence\n");
        break;
    }
}

void replay_load_sequence(void)
{
    switch (memcardInfo.state)
    {
    case 0x1C:
        memcardInfo.fileSize = memcardInfo.unk44;
        if ((memcardReplayData = OSAlloc(memcardInfo.fileSize)) == NULL)
            OSPanic("memcard.c", 3663, "cannot OSAlloc");
        memcardInfo.unk40 = 0x4B0;
        memcardInfo.state = 0x1D;
        break;
    case 0x1D:
        open_replay_file();
        break;
    case 0xF:
        read_memcard_file(memcardReplayData);
        break;
    case 0x10:
        check_read_memcard_file_result();
        break;
    case MC_STATE_ERROR:
        break;
    default:
        printf("stat: %d\n", memcardInfo.state);
        OSPanic("memcard.c", 3691, "replay_load_sequence\n");
        break;
    }
}

#pragma force_active off

// inline
void replay_delete_sequence(void)
{
    switch (memcardInfo.state)
    {
    case 0xB:
        delete_memcard_file(replayFileInfo[lbl_802F21C0].fileNo);
        break;
    case 0xC:
        check_delete_memcard_file_result();
        break;
    case MC_STATE_ERROR:
        break;
    default:
        printf("stat: %d\n", memcardInfo.state);
        OSPanic("memcard.c", 3721, "replay_delete_sequence\n");
        break;
    }
}

void memcard_init(void)
{
    lbl_802F21A8 = 0;
}

void ev_memcard_init(void)
{
    memcardInfo.state = 1;
    memcardInfo.msg = NULL;
    memcardInfo.fileNo = -1;
    lbl_802F21B2 = 0;
    memcardInfo.time = OSGetTime();

    if (memcardMode != MC_MODE_DELETE_REPLAY)
    {
        if (memcardMode == MC_MODE_SAVE_GAMEDATA_1
         || memcardMode == MC_MODE_SAVE_GAMEDATA_3
         || memcardMode == MC_MODE_SAVE_REPLAY)
            memcardInfo.statusFlags |= MC_STATUS_SAVING;
        else
            memcardInfo.statusFlags |= MC_STATUS_LOADING;
    }

    if (memcardMode == MC_MODE_LOAD_GAMEDATA_2
     || memcardMode == MC_MODE_SAVE_GAMEDATA_3)
        memcardInfo.statusFlags |= (1 << 6);
    else
        memcardInfo.statusFlags |= (1 << 7);

    if (memcardMode == MC_MODE_LOAD_REPLAY
     || memcardMode == MC_MODE_SAVE_REPLAY
     || memcardMode == MC_MODE_LIST_REPLAY
     || memcardMode == MC_MODE_DELETE_REPLAY)
        memcardInfo.statusFlags |= MC_STATUS_REPLAY_FILE;
    else
        memcardInfo.statusFlags |= MC_STATUS_GAMEDATA_FILE;

    if (memcardInfo.statusFlags & MC_STATUS_REPLAY_FILE)
    {
        sprintf(memcardInfo.fileName, "smkb%08x%08x",
            (unsigned int)((u64)memcardInfo.time >> 32),
            (unsigned int)(memcardInfo.time));
    }
    else if (memcardInfo.statusFlags & MC_STATUS_SAVING)
        strcpy(memcardInfo.fileName, "super_monkey_ball.000");
    else
        strcpy(memcardInfo.fileName, "super_monkey_ball.sys");

    if (!(memcardInfo.statusFlags & MC_STATUS_REPLAY_FILE))
        memcardInfo.unk44 = 0x5C04;
    else if (memcardMode == MC_MODE_SAVE_REPLAY)
        memcardInfo.unk44 = recplay_cmpr_calc_compressed_size() + 0x2050;
    else if (memcardMode == MC_MODE_LOAD_REPLAY)
        memcardInfo.unk44 = replayFileInfo[lbl_802F21C0].fileSize;
    else
        memcardInfo.unk44 = 0x200;

    if (memcardMode == MC_MODE_LOAD_REPLAY)
        memcardInfo.state = 0x1C;
    else if (memcardMode == MC_MODE_DELETE_REPLAY)
        memcardInfo.state = 0xB;

    if (memcardMode == MC_MODE_LOAD_GAMEDATA_2)
    {
        if ((memcardContents = OSAlloc(sizeof(*memcardContents))) == NULL)
            OSPanic("memcard.c", 3835, "cannot OSAlloc");
        func_800A4E70();
        memcpy(&lbl_802C4480, &memcardContents->gameData, sizeof(lbl_802C4480));
        OSFree(memcardContents);
        memcardContents = NULL;
    }
    if (!(memcardInfo.statusFlags & (1 << 6)))
        g_currPlayerButtons[2] = 0;
}

void ev_memcard_main(void)
{
    if ((memcardInfo.statusFlags & (1 << 7))
     && memcardInfo.state == 1
     && (g_currPlayerButtons[2] & PAD_BUTTON_B))
    {
        u_play_sound_1(0x6B);
        memcardInfo.state = MC_STATE_ERROR;
        memcardInfo.statusFlags |= (1 << 8);
    }
    if (memcardInfo.state == MC_STATE_ERROR && !(memcardInfo.statusFlags & MC_STATUS_ERROR))
    {
        memcardInfo.statusFlags &= ~MC_STATUS_WRITE_IN_PROGRESS;
        event_finish(0);
        return;
    }
    if (memcardInfo.statusFlags & MC_STATUS_ERROR)
    {
        memcardInfo.statusFlags &= ~((1 << 15) | (1 << 17) | MC_STATUS_WRITE_IN_PROGRESS);
        if (g_currPlayerButtons[2] & PAD_BUTTON_B)
        {
            u_play_sound_1(0x6B);
            memcardInfo.unk42 = 0;
            memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            g_currPlayerButtons[2] = 0;
        }
        else
        {
            if (memcardInfo.unk42 == 0)
                return;
            memcardInfo.unk42--;
            if (memcardInfo.unk42 == 0)
                memcardInfo.statusFlags &= ~MC_STATUS_ERROR;
            else
                return;
        }
    }

    if (memcardInfo.unk40 > 0)
        memcardInfo.unk40--;
    switch (memcardMode)
    {
    case MC_MODE_LOAD_GAMEDATA_0:
    case MC_MODE_LOAD_GAMEDATA_2:
        load_sequence();
        break;
    case MC_MODE_SAVE_GAMEDATA_1:
    case MC_MODE_SAVE_GAMEDATA_3:
        save_sequence();
        break;
    case MC_MODE_LOAD_REPLAY:
        replay_load_sequence();
        break;
    case MC_MODE_SAVE_REPLAY:
        replay_save_sequence();
        break;
    case MC_MODE_LIST_REPLAY:
        replay_list_sequence();
        break;
    case MC_MODE_DELETE_REPLAY:
        replay_delete_sequence();
        break;
    default:
        printf("memcard_mode:%d\n", memcardMode);
        OSPanic("memcard.c", 3952, "Memcard.");
        break;
    }
}

void ev_memcard_dest(void)
{
    if (memcardInfo.statusFlags & (1 << 3))
        lbl_802F21B9 = 1;
    else
    {
        lbl_802F21B9 = 0;
        if (memcardMode == MC_MODE_SAVE_GAMEDATA_3)
            lbl_802F21A8 = 0;
    }

    CARDCancel(&memcardInfo.cardFileInfo);
    if (memcardInfo.statusFlags & MC_STATUS_OPEN)
    {
        CARDClose(&memcardInfo.cardFileInfo);
        memcardInfo.statusFlags &= ~MC_STATUS_OPEN;
    }

    if (memcardInfo.statusFlags & MC_STATUS_MOUNTED)
    {
        if (memcardMode != MC_MODE_LIST_REPLAY
         && memcardMode != MC_MODE_DELETE_REPLAY
         && memcardMode != MC_MODE_LOAD_REPLAY)
        {
            CARDUnmount(0);
            memcardInfo.statusFlags &= ~MC_STATUS_MOUNTED;
        }
    }

    if (memcardContents != NULL
     && !(memcardInfo.statusFlags & (MC_STATUS_SAVING | (1 << 8) | MC_STATUS_REPLAY_FILE | (1 << 14)))
     && memcardContents->version == 0x16)
        func_800A4F04();
    memcardInfo.state = 0;
    memcardInfo.statusFlags &= 3;
    if (memcardContents != NULL)
    {
        OSFree(memcardContents);
        memcardContents = NULL;
    }
    if (memcardReplayData != NULL)
    {
        OSFree(memcardReplayData);
        memcardReplayData = NULL;
    }

    lbl_802F21B4 = 0;
    replayFileInfo = NULL;
}

NLsprarg lbl_801D5724 =
{
    0x4B,
    320.0f,
    240.0f,
    1.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0,  // unk28
    0.649999976158f,
    -1,
    10,
    0xFF000000,
    0,
};

void draw_memcard_msg(struct MemCardMessage *msg, float x, float y)
{
    int i;
    int r27;
    float f2;
    float f30;
    float f1;

    reset_text_draw_settings();
    set_text_font(0xB3);
    func_80071B50(0x200000);
    set_text_scale(0.65f, 0.8f);

    for (i = 0, f30 = 0.0f, r27 = msg->numLines; i < msg->numLines; i++)
    {
        float lineWidth = u_get_text_width(msg->lines[i].str);
        if (lineWidth > f30)
            f30 = lineWidth;
    }
    if (r27 > 0)
        f2 = 0.800000011920929 * (24.0 * r27 + 8.0 * (r27 - 1));
    else
        f2 = 0.800000011920929 * (24.0 * 1 + 8.0 * (1 - 1));

    f1 = f30;
    f1 += 31.19999885559082;
    f2 += 38.40000057220459;
    lbl_801D5724.x = x;
    lbl_801D5724.y = y;
    {
        float var1 = f1;
        float var2 = f2;
        var1 *= 0.125;
        var2 *= 0.125;
        lbl_801D5724.zm_x = var1;
        lbl_801D5724.zm_y = var2;
        lbl_801D5724.trnsl = 0.65 + 0.1 * lbl_802F1ECC;
    }
    nlSprPut(&lbl_801D5724);
    for (i = 0; i < msg->numLines; i++)
    {
        float param1 = x - 0.5 * u_get_text_width(msg->lines[i].str);
        float param2 = 0.800000011920929 * (32.0 * i)
            + ((y - 9.600000143051147) - 0.800000011920929 * (16.0 * (msg->numLines - 1)));
        set_text_pos(param1, param2);
        sprite_puts(msg->lines[i].str);
    }
}

// not used. needed here to force float constants into the right order
static void memcard_dummy(void)
{
    draw_memcard_msg(NULL, 320.0f, 240.0f);
}

static struct StringEntry lbl_802F1698 = { lbl_802C4900, 0 };

void memcard_draw_ui(void)
{
    u32 color;
    u32 intensity;

    reset_text_draw_settings();
    set_text_font(0xB3);
    func_80071B1C(0.008f);
    if (memcardInfo.statusFlags & MC_STATUS_ERROR)
    {
        if (memcardInfo.msg != NULL)
            draw_memcard_msg(memcardInfo.msg, 320.0f, 240.0f);
        if (memcardInfo.unk42 == 0 || memcardInfo.statusFlags & (1 << 19))
        {
            if ((memcardInfo.statusFlags & MC_STATUS_SAVING)
             && !(memcardInfo.statusFlags & (1 << 22)))
                draw_memcard_msg(&msgPressBButtonNoSave, 320.0f, 380.0f);
            else
                draw_memcard_msg(&msgPressBButton, 320.0f, 380.0f);
        }
        if (memcardInfo.statusFlags & (1 << 20))
        {
            if (memcardInfo.unk4D == 0)
                draw_memcard_msg(&msgMemCardNoFreeBlocks, 320.0f, 100.0f);
            else
            {
                struct MemCardMessage sp8 = { &lbl_802F1698, 0 };
                struct MemCardMessage *r29;

                if (memcardInfo.unk4D == 1)
                    r29 = &msgMemCardNumFreeBlock;
                else
                    r29 = &msgMemCardNumFreeBlocks;
                sprintf(sp8.lines[0].str, r29->lines[0].str, memcardInfo.unk4D);
                if (memcardInfo.unk4D > 9)
                    sp8.lines[0].unk4 = r29->lines[0].unk4 + 1;
                else
                    sp8.lines[0].unk4 = r29->lines[0].unk4;
                sp8.numLines = r29->numLines;
                draw_memcard_msg(&sp8, 320.0f, 100.0f);
            }
        }
        reset_text_draw_settings();
        return;
    }
    else if (memcardInfo.statusFlags & (1 << 2))
    {
        draw_memcard_msg(&msgAccessMemCard, 320.0f, 240.0f);
        return;
    }
    else if (memcardInfo.statusFlags & (1 << 6))
    {
        if (memcardInfo.statusFlags & (1 << 15))
        {
            set_text_pos(100.0f, 340.0f);
            draw_memcard_msg(&msgSavingGame, 320.0f, 240.0f);
        }
        reset_text_draw_settings();
        return;
    }
    else if (memcardInfo.statusFlags & (1 << 10))
        draw_memcard_msg(&msgMakeSelection, 320.0f, 380.0f);
    if (memcardInfo.state == 1)
        draw_memcard_msg(&msgInsertMemcardSlotAPressA, 320.0f, 240.0f);
    if (memcardInfo.state == MC_STATE_FORMAT_PROMPT)
    {
        draw_memcard_msg(&msgFormatPrompt, 320.0f, 240.0f);

        intensity = ((float)abs((int)(float)(globalAnimTimer % 60) - 30.0) / 30.0) * 255.0;
        color = RGBA(intensity, intensity, intensity, 0);

        set_text_scale(1.5f, 1.5f);
        set_text_pos(242.0f, lbl_802F1EB0 + 0x101);
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        else
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(color);
        }
        sprite_puts("Yes ");
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(color);
        }
        else
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        sprite_puts("No");
        set_text_pos(240.0f, lbl_802F1EB0 + 0xFF);
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(127, 127, 127, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        else
        {
            set_text_mul_color(RGBA(255, 255, 255, 0));
            set_text_add_color(color);
        }
        sprite_puts("Yes ");
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(255, 255, 255, 0));
            set_text_add_color(color);
        }
        else
        {
            set_text_mul_color(RGBA(127, 127, 127, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        sprite_puts("No");
    }
    if (memcardInfo.state == 10)
        draw_memcard_msg(&msgFormatProgress, 320.0f, 240.0f);
    if (memcardInfo.state == MC_STATE_OVERWRITE_PROMPT && (memcardInfo.statusFlags & (1 << 10)))
    {
        draw_memcard_msg(&msgOverwritePrompt, 320.0f, 240.0f);

        intensity = ((float)abs((int)(float)(globalAnimTimer % 60) - 30.0) / 30.0) * 255.0;
        color = RGBA(intensity, intensity, intensity, 0);

        set_text_scale(1.5f, 1.5f);
        set_text_pos(242.0f, lbl_802F1EB0 + 0x101);
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        else
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(color);
        }
        sprite_puts("Yes ");
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(color);
        }
        else
        {
            set_text_mul_color(RGBA(0, 0, 0, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        sprite_puts("No");
        set_text_pos(240.0f, lbl_802F1EB0 + 0xFF);
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(127, 127, 127, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        else
        {
            set_text_mul_color(RGBA(255, 255, 255, 0));
            set_text_add_color(color);
        }
        sprite_puts("Yes ");
        if (lbl_802F21B1 == 0)
        {
            set_text_mul_color(RGBA(255, 255, 255, 0));
            set_text_add_color(color);
        }
        else
        {
            set_text_mul_color(RGBA(127, 127, 127, 0));
            set_text_add_color(RGBA(0, 0, 0, 0));
        }
        sprite_puts("No");
    }
    if (memcardInfo.statusFlags & (1 << 15))
    {
        if (memcardInfo.statusFlags & MC_STATUS_REPLAY_FILE)
            draw_memcard_msg(&msgSavingReplay, 320.0f, 240.0f);
        else
            draw_memcard_msg(&msgSavingGame, 320.0f, 240.0f);
    }
    if (memcardInfo.statusFlags & (1 << 17))
        draw_memcard_msg(&msgLoadingGame, 320.0f, 240.0f);
    reset_text_draw_settings();
}

void func_800A4CEC(void)
{
    s32 result;
    CARDFileInfo cardFile;

    // unmount card
    while (CARDGetResultCode(0) == CARD_RESULT_BUSY)
        ;
    while (CARDUnmount(0) == CARD_RESULT_BUSY)
        ;

    // mount card
    while ((result = CARDMount(0, cardWorkArea, NULL)) == CARD_RESULT_BUSY)
        ;
    if (result != CARD_RESULT_READY)
        goto error;  // could not mount

    while ((result = CARDOpen(0, "super_monkey_ball.000", &cardFile)) == CARD_RESULT_BUSY)
        ;
    if (result != CARD_RESULT_READY)
        goto error;

    while ((result = CARDOpen(0, "super_monkey_ball.sys", &cardFile)) == CARD_RESULT_BUSY)
        ;
    if (result == CARD_RESULT_READY)
    {
        while (CARDDelete(0, "super_monkey_ball.000") == CARD_RESULT_BUSY)
            ;
    }
    else if (result == CARD_RESULT_NOFILE)
    {
        while (CARDRename(0, "super_monkey_ball.000", "super_monkey_ball.sys") == CARD_RESULT_BUSY)
            ;
    }

error:
    while (CARDUnmount(0) == -1)
        ;
}

#pragma force_active on

void func_800A4DF0(void)
{
    if ((memcardContents = OSAlloc(sizeof(*memcardContents))) == NULL)
        OSPanic("memcard.c", 4462, "cannot OSAlloc");
    memcpy(&memcardContents->gameData, &lbl_802C4480, sizeof(memcardContents->gameData));
    func_800A4F04();
    OSFree(memcardContents);
    memcardContents = NULL;
}

extern u8 lbl_801D5A20[];
extern u32 lbl_802F22C8;

static void func_800A4E70(void)
{
    memcardContents->gameData.unk4E = lbl_802F21A8;
    save_input_data(memcardContents);
    u_store_gamedata(memcardContents);
    store_sound_settings(memcardContents);
    save_course_completion_data(memcardContents);
    memcardContents->gameData.unkAC = modeCtrl.splitscreenMode;
    memcpy(memcardContents->gameData.unkB0, lbl_801D5A20, sizeof(memcardContents->gameData.unkB0));
    memcardContents->gameData.unkAD = vibration_get_cont_enable_mask();
    func_800AFC1C(memcardContents);
    memcardContents->gameData.unk3BC = lbl_802F22C8;
}

static void func_800A4F04(void)
{
    lbl_802F21A8 = memcardContents->gameData.unk4E;
    load_input_data(memcardContents);
    u_load_gamedata(memcardContents);
    load_sound_settings(memcardContents);
    load_course_completion_data(memcardContents);
    modeCtrl.splitscreenMode = memcardContents->gameData.unkAC;
    memcpy(lbl_801D5A20, memcardContents->gameData.unkB0, sizeof(memcardContents->gameData.unkB0));
    vibration_set_cont_enable_mask(memcardContents->gameData.unkAD);
    func_800AFC4C(memcardContents);
    lbl_802F22C8 = memcardContents->gameData.unk3BC;
}
