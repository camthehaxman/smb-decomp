extern const DVDDiskID *__CARDDiskID;

void __CARDSyncCallback(s32 chan, s32 result);
s32 __CARDGetControlBlock(s32 chan, CARDControl** pcard);
s32 __CARDPutControlBlock(CARDControl* card, s32 result);
s32 __CARDSync(s32 chan);

BOOL __CARDCompareFileName(CARDDir* ent, const char* fileName);
s32 __CARDAccess(CARDDir* ent);

s32 __CARDUpdateDir(s32 chan, CARDCallback callback);
