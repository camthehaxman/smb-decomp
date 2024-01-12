#define CARD_ATTR_PUBLIC  0x04u
#define CARD_ATTR_NO_COPY 0x08u
#define CARD_ATTR_NO_MOVE 0x10u
#define CARD_ATTR_GLOBAL  0x20u
#define CARD_ATTR_COMPANY 0x40u

#define CARD_FAT_AVAIL 0x0000u
#define CARD_FAT_CHECKSUM 0x0000u
#define CARD_FAT_CHECKSUMINV 0x0001u
#define CARD_FAT_CHECKCODE 0x0002u
#define CARD_FAT_FREEBLOCKS 0x0003u
#define CARD_FAT_LASTSLOT 0x0004u

#define CARD_WORKAREA_SIZE (5 * 8 * 1024)

#define CARD_SEG_SIZE 0x200
#define CARD_PAGE_SIZE 0x80

#define CARD_NUM_SYSTEM_BLOCK 5
#define CARD_SYSTEM_BLOCK_SIZE (8 * 1024u)

#define CARD_MAX_MOUNT_STEP (CARD_NUM_SYSTEM_BLOCK + 2)

#define __CARDIsValidBlockNo(card, blockNo) ((blockNo) >= CARD_NUM_SYSTEM_BLOCK && (blockNo) < (card)->cBlock)

#define TRUNC(n, a) (((u32)(n)) & ~((a)-1))
#define OFFSET(n, a) (((u32)(n)) & ((a)-1))

typedef struct CARDID
{
    u8 serial[32]; // flashID[12] + timebase[8] + counterBias[4] + language[4] + XXX[4]
    u16 deviceID;
    u16 size;
    u16 encode; // character set -- 0: S-JIS, 1: ANSI

    u8 padding[512 - 32 - 5 * 2];

    u16 checkSum;
    u16 checkSumInv;
} CARDID;

typedef struct CARDDirCheck
{
    u8 padding0[64 - 2 * 4];
    u16 padding1;
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

static CARDDirCheck *__CARDGetDirCheck(CARDDir *dir)
{
    return ((CARDDirCheck*)&(dir)[CARD_MAX_FILE]);
}

/* CARDBios.c */

extern CARDControl __CARDBlock[2];
extern DVDDiskID __CARDDiskNone;
extern const DVDDiskID *__CARDDiskID;

void __CARDDefaultApiCallback(s32 chan, s32 result);
void __CARDSyncCallback(s32 chan, s32 result);
void __CARDExtHandler(s32 chan, OSContext *context);
void __CARDExiHandler(s32 chan, OSContext *context);
void __CARDTxHandler(s32 chan, OSContext *context);
void __CARDUnlockedHandler(s32 chan, OSContext *context);
int __CARDReadNintendoID(s32 chan, u32 *id);
s32 __CARDEnableInterrupt(s32 chan, BOOL enable);
s32 __CARDReadStatus(s32 chan, u8 *status);
s32 __CARDClearStatus(s32 chan);
s32 __CARDStart(s32 chan, CARDCallback txCallback, CARDCallback exiCallback);
s32 __CARDReadSegment(s32 chan, CARDCallback callback);
s32 __CARDWritePage(s32 chan, CARDCallback callback);
s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback);
void __CARDSetDiskID(const DVDDiskID *id);
s32 __CARDGetControlBlock(s32 chan, CARDControl** pcard);
s32 __CARDPutControlBlock(CARDControl* card, s32 result);
s32 __CARDSync(s32 chan);

/* CARDDir.c */

CARDDir *__CARDGetDirBlock(CARDControl *);
s32 __CARDUpdateDir(s32 chan, CARDCallback callback);

/* CARDOpen.c */

BOOL __CARDCompareFileName(CARDDir* ent, const char* fileName);
s32 __CARDAccess(CARDDir* ent);
s32 __CARDIsPublic(CARDDir *ent);
s32 __CARDGetFileNo(CARDControl* card, const char* fileName, s32* pfileNo);
BOOL __CARDIsOpened(CARDControl *card, s32 fileNo);

/* CARDRdwr.c */

s32 __CARDRead(s32 chan, u32 addr, s32 length, void* dst, CARDCallback callback);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void* dst, CARDCallback callback);

/* CARDRead.c */

s32 __CARDSeek(CARDFileInfo *fileInfo, s32 length, s32 offset, CARDControl **pcard);

/* other */

s32 __CARDFormatRegionAsync(s32 chan, CARDCallback callback);
void *__CARDGetFatBlock(CARDControl *);
void __CARDUpdateSum(void* ptr, int length, u16* checksum, u16* checksumInv);
s32 __CARDFreeBlock(s32 chan, u16 nBlock, CARDCallback callback);
void __CARDMountCallback(s32 chan, s32 result);
void __CARDCheckSum(void* ptr, int length, u16* checkSum, u16* checkSumInv);
