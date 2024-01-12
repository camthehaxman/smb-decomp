#define CARD_ATTR_PUBLIC  0x04u
#define CARD_ATTR_NO_COPY 0x08u
#define CARD_ATTR_NO_MOVE 0x10u
#define CARD_ATTR_GLOBAL  0x20u
#define CARD_ATTR_COMPANY 0x40u

extern CARDControl __CARDBlock[2];
extern DVDDiskID __CARDDiskNone;
extern const DVDDiskID *__CARDDiskID;

void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDReadStatus(s32 chan, u8 *status);
s32 __CARDClearStatus(s32 chan);
s32 __CARDReadSegment(s32 chan, CARDCallback callback);
s32 __CARDWritePage(s32 chan, CARDCallback callback);
s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback);
s32 __CARDGetControlBlock(s32 chan, CARDControl** pcard);
s32 __CARDPutControlBlock(CARDControl* card, s32 result);
s32 __CARDSync(s32 chan);

CARDDir *__CARDGetDirBlock(CARDControl *);
s32 __CARDUpdateDir(s32 chan, CARDCallback callback);

BOOL __CARDCompareFileName(CARDDir* ent, const char* fileName);
s32 __CARDAccess(CARDDir* ent);
s32 __CARDFormatRegionAsync();
void *__CARDGetFatBlock(CARDControl *);
void __CARDSetDiskID(const DVDDiskID *);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void* dst, CARDCallback callback);
void __CARDUpdateSum(void* ptr, int length, u16* checksum, u16* checksumInv);
