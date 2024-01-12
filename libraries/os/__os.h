typedef struct OSSram
{
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} OSSram;

typedef struct OSSramEx
{
    u8 flashID[2][12];
    u32 wirelessKeyboardID;
    u16 wirelessPadID[4];
    u8 dvdErrorCode;
    u8 _padding0;
    u8 flashIDCheckSum[2];
    u16 gbs;
    u8 _padding1[2];
} OSSramEx;

OSSram *__OSLockSram(void);
OSSramEx *__OSLockSramEx(void);
BOOL __OSUnlockSram(BOOL commit);
BOOL __OSUnlockSramEx(BOOL commit);

u32 __OSGetDIConfig(void);
// TODO: EXI header
BOOL EXILock(int chan, int, int);
BOOL EXIUnlock(int chan);
BOOL EXISelect(int chan, int, int);
BOOL EXIDeselect(int chan);
BOOL EXIImm(int chan, u32 *, int, int, int);
BOOL EXIDma(int chan, void *buffer, s32 size, int, int);
BOOL EXISync(int chan);
