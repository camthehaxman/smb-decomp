#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <dolphin.h>

#include "src/types.h"
#include "src/mathutil.h"
#include "src/global.h"
#include "src/light.h"
#include "src/sound.h"

#define puts(...)

struct Light lbl_801F3A08;
struct Color3f lbl_801F39FC;

s32 CARDCancel(CARDFileInfo *fileInfo){puts("CARDCancel is a stub");return 0;}
s32 CARDCheckAsync(s32 chan, CARDCallback callback){puts("CARDCheckAsync is a stub");return 0;}
s32 CARDClose(CARDFileInfo *fileInfo){puts("CARDClose is a stub");return 0;}
s32 CARDCreateAsync(s32 chan, char *fileName, u32 size, CARDFileInfo *fileInfo, CARDCallback callback){puts("CARDCreateAsync is a stub");return 0;}
s32 CARDDelete(s32 chan, char *fileName){puts("CARDDelete is a stub");return 0;}
s32 CARDDeleteAsync(s32 chan, char *fileName, CARDCallback callback){puts("CARDDeleteAsync is a stub");return 0;}
s32 CARDFastDeleteAsync(s32 chan, s32 fileNo, CARDCallback callback){puts("CARDFastDeleteAsync is a stub");return 0;}
s32 CARDFastOpen(s32 chan, s32 fileNo, CARDFileInfo *fileInfo){puts("CARDFastOpen is a stub");return 0;}
s32 CARDFreeBlocks(s32 chan, s32 *byteNotUsed, s32 *filesNotUsed){puts("CARDFreeBlocks is a stub");return 0;}
s32 CARDGetResultCode(s32 chan){puts("CARDGetResultCode is a stub");return 0;}
s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat){puts("CARDGetStatus is a stub");return 0;}
void CARDInit(){puts("CARDInit is a stub");}
s32 CARDMount(s32 chan, void *workArea, CARDCallback detachCallback){puts("CARDMount is a stub");return 0;}
s32 CARDMountAsync(s32 chan, void *workArea, CARDCallback detachCallback,
                   CARDCallback attachCallback){puts("CARDMountAsync is a stub");return 0;}
s32 CARDOpen(s32 chan, char *fileName, CARDFileInfo *fileInfo){puts("CARDOpen is a stub");return 0;}
s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectorSize){puts("CARDProbeEx is a stub");return 0;}
s32 CARDReadAsync(CARDFileInfo* fileInfo, void *addr, s32 length, s32 offset, CARDCallback callback){puts("CARDReadAsync is a stub");return 0;}
s32 CARDRename(s32 chan, char *oldName, char *newName){puts("CARDRename is a stub");return 0;}
s32 CARDRenameAsync(s32 chan, char *oldName, char *newName, CARDCallback callback){puts("CARDRenameAsync is a stub");return 0;}
s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDStat *stat, CARDCallback callback){puts("CARDSetStatusAsync is a stub");return 0;}
s32 CARDUnmount(s32 chan){puts("CARDUnmount is a stub");return 0;}
s32 CARDWriteAsync(CARDFileInfo* fileInfo, void *addr, s32 length, s32 offset, CARDCallback callback){puts("CARDWriteAsync is a stub");return 0;}

void C_VECNormalize(const VecPtr src, VecPtr unit)
{
    f32 mag = (src->x * src->x) + (src->y * src->y) + (src->z * src->z);
    mag = 1.0f / sqrtf(mag);
    unit->x = src->x * mag;
    unit->y = src->y * mag;
    unit->z = src->z * mag;
}

void C_VECCrossProduct(const VecPtr a, const VecPtr b, VecPtr out)
{
    Vec tmp;
    tmp.x = (a->y * b->z) - (a->z * b->y);
    tmp.y = (a->z * b->x) - (a->x * b->z);
    tmp.z = (a->x * b->y) - (a->y * b->x);
    out->x = tmp.x;
    out->y = tmp.y;
    out->z = tmp.z;
}

void C_MTXLookAt(Mtx m, Point3dPtr camPos, VecPtr camUp, Point3dPtr target)
{
    Vec vv0, vv1, vv2;
    vv0.x = camPos->x - target->x;
    vv0.y = camPos->y - target->y;
    vv0.z = camPos->z - target->z;
    C_VECNormalize(&vv0, &vv0);
    C_VECCrossProduct(camUp, &vv0, &vv1);
    C_VECNormalize(&vv1, &vv1);
    C_VECCrossProduct(&vv0, &vv1, &vv2);
    m[0][0] = vv1.x;
    m[0][1] = vv1.y;
    m[0][2] = vv1.z;
    m[0][3] = -(camPos->x * vv1.x + camPos->y * vv1.y + camPos->z * vv1.z);
    m[1][0] = vv2.x;
    m[1][1] = vv2.y;
    m[1][2] = vv2.z;
    m[1][3] = -(camPos->x * vv2.x + camPos->y * vv2.y + camPos->z * vv2.z);
    m[2][0] = vv0.x;
    m[2][1] = vv0.y;
    m[2][2] = vv0.z;
    m[2][3] = -(camPos->x * vv0.x + camPos->y * vv0.y + camPos->z * vv0.z);
}

void C_MTXScale(Mtx m, f32 xS, f32 yS, f32 zS){*(volatile int*)0 = 0;puts("C_MTXScale is a stub");}
void DCFlushRange(void *addr, u32 nBytes){puts("DCFlushRange is a stub");}
void DCInvalidateRange(void *addr, u32 nBytes){puts("DCInvalidateRange is a stub");}
void DCStoreRange(void *addr, u32 nBytes){puts("DCStoreRange is a stub");}
s32 DVDCancel(DVDCommandBlock *block){puts("DVDCancel is a stub");return 0;}
void DVDReadAbsAsyncForBS(void *a, struct bb2struct *b, int c, int d, void (*e)()){puts("DVDReadAbsAsyncForBS is a stub");}
void DVDReadDiskID(void *a, DVDDiskID *b, void (*c)()){puts("DVDReadDiskID is a stub");}
void DVDReset(){puts("DVDReset is a stub");}
BOOL EXIDeselect(int chan){puts("EXIDeselect is a stub");return FALSE;}
BOOL EXIDma(int chan, void *buffer, s32 size, int d, int e){puts("EXIDma is a stub");return FALSE;}
BOOL EXIImm(int chan, u32 *b, int c, int d, int e){puts("EXIImm is a stub");return FALSE;}
BOOL EXILock(int chan, int b, int c){puts("EXILock is a stub");return FALSE;}
BOOL EXISelect(int chan, int b, int c){puts("EXISelect is a stub");return FALSE;}
BOOL EXISync(int chan){puts("EXISync is a stub");return FALSE;}
BOOL EXIUnlock(int chan){puts("EXIUnlock is a stub");return FALSE;}
void LCEnable(){puts("LCEnable is a stub");}
void OSClearContext(OSContext *context){puts("OSClearContext is a stub");}
BOOL OSDisableInterrupts(){puts("OSDisableInterrupts is a stub");return FALSE;}
void OSDumpContext(OSContext *context){puts("OSDumpContext is a stub");}
OSThread* OSGetCurrentThread(){puts("OSGetCurrentThread is a stub");return 0;}
u16 OSGetFontEncode(){puts("OSGetFontEncode is a stub");return 0;}
char * OSGetFontTexture(char *string, void **image, s32 *x, s32 *y, s32 *width){puts("OSGetFontTexture is a stub");return 0;}
char * OSGetFontWidth(char *string, s32 *width){puts("OSGetFontWidth is a stub");return 0;}
BOOL OSGetResetButtonState(){puts("OSGetResetButtonState is a stub");return FALSE;}
u32 OSGetStackPointer(){puts("OSGetStackPointer is a stub");return 0;}
BOOL OSInitFont(OSFontHeader *fontData){puts("OSInitFont is a stub");return FALSE;}
BOOL OSLink(OSModuleInfo *newModule, void *bss){puts("OSLink is a stub");return FALSE;}
void OSLoadContext(OSContext *context){puts("OSLoadContext is a stub");}
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu){puts("OSResetSystem is a stub");}
BOOL OSRestoreInterrupts(BOOL level){puts("OSRestoreInterrupts is a stub");return FALSE;}
s32 OSResumeThread(OSThread* thread){puts("OSResumeThread is a stub");return 0;}
void OSSetCurrentContext(OSContext *context){puts("OSSetCurrentContext is a stub");}
void OSSetStringTable(void *a){puts("OSSetStringTable is a stub");}
s32 OSSuspendThread(OSThread* thread){puts("OSSuspendThread is a stub");return 0;}
void OSTicksToCalendarTime(OSTime ticks, OSCalendarTime *td){puts("OSTicksToCalendarTime is a stub");}
BOOL OSUnlink(OSModuleInfo *oldModule){puts("OSUnlink is a stub");return FALSE;}
void OSWakeupThread(OSThreadQueue *queue){puts("OSWakeupThread is a stub");}
void PERFDumpScreen(){puts("PERFDumpScreen is a stub");}
void PERFEndFrame(){puts("PERFEndFrame is a stub");}
void PERFEventEnd(PERFId id){puts("PERFEventEnd is a stub");}
void PERFEventStart(PERFId id){puts("PERFEventStart is a stub");}
u32 PERFInit(u32 numSamples, u32 numFramesHistory, u32 numTypes,
              PERFAllocator allocator, PERFDeallocator deallocator, PERFDrawCallback initDraw){puts("PERFInit is a stub");return 0;}
void PERFPostDraw(){puts("PERFPostDraw is a stub");}
void PERFPreDraw(){puts("PERFPreDraw is a stub");}
void PERFSetDrawBWBarKey(BOOL tf){puts("PERFSetDrawBWBarKey is a stub");}
void PERFSetEvent(PERFId id, char *name, PerfType type){puts("PERFSetEvent is a stub");}
void PERFStartAutoSampling(f32 msInterval){puts("PERFStartAutoSampling is a stub");}
void PERFStartFrame(){puts("PERFStartFrame is a stub");}
void PERFStopAutoSampling(){puts("PERFStopAutoSampling is a stub");}
void PPCHalt(){puts("PPCHalt is a stub");}
void PSMTXIdentity(Mtx m){puts("PSMTXIdentity is a stub");}
void VIConfigure(GXRenderModeObj *rm){puts("VIConfigure is a stub");}
u32 VIGetNextField(){puts("VIGetNextField is a stub");return 0;}
void VISetBlack(BOOL black){puts("VISetBlack is a stub");}
void VISetNextFrameBuffer(void *fb){puts("VISetNextFrameBuffer is a stub");}
void VIWaitForRetrace(){puts("VIWaitForRetrace is a stub");}
s32 __CARDFormatRegionAsync(int a, int b){puts("__CARDFormatRegionAsync is a stub");return 0;}
void __GXSetSUTexSize(){puts("__GXSetSUTexSize is a stub");}
void __GXSetVAT(){puts("__GXSetVAT is a stub");}
void __GXSetVCD(){puts("__GXSetVCD is a stub");}
void __GXUpdateBPMask(){puts("__GXUpdateBPMask is a stub");}
u32 __OSGetDIConfig(){puts("__OSGetDIConfig is a stub");return 0;}
__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler){puts("__OSSetInterruptHandler is a stub");return 0;}
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask){puts("__OSUnmaskInterrupts is a stub");return 0;}
volatile u16 *__cpReg;
volatile u32 *__piReg;
void __sync(){puts("__sync is a stub");}
void credits_finish(){puts("credits_finish is a stub");}
void credits_init(){puts("credits_init is a stub");}
void credits_main(){puts("credits_main is a stub");}
struct GX *gx;
u8 lbl_801D5A20[0x1FC] = {0}; // TODO
float lbl_802F16B8;
u32 lbl_802F1DF8;
s8 lbl_802F2060;
u32 lbl_802F2150;
void *lbl_802F2154;
s32 lbl_802F21E8;
float lbl_802F21EC;
float lbl_802F21F0 = 0;
u32 lbl_802F22C8 = 0;
s16 mathutil_atan(float angle)
{
    return atanf(angle) * 65536.0f / (2.0f * PI);
}
s16 mathutil_atan2(double a, float b)
{
    return atan2f(a, b) * 65536.0f / (2.0f * PI);
}
void mathutil_init(){puts("mathutil_init is a stub");}
void mathutil_mtxA_invert(){puts("mathutil_mtxA_invert is a stub");}
void mathutil_mtxA_rigid_invert(){puts("mathutil_mtxA_rigid_invert is a stub");}
void mathutil_sin_cos_v(int a, float b[2])
{
    b[0] = mathutil_sin(a);
    b[1] = mathutil_cos(a);
}
void submode_mini_ranking_init_func(){puts("submode_mini_ranking_init_func is a stub");}
void submode_mini_ranking_main_func(){puts("submode_mini_ranking_main_func is a stub");}
void u_get_replay_info(){puts("u_get_replay_info is a stub");}
void u_play_sound(int a){puts("u_play_sound is a stub");}
u32 AIGetStreamSampleCount(){puts("AIGetStreamSampleCount is a stub");return 0;}
void AIInit(u8*){puts("AIInit is a stub");}
void AIResetStreamSampleCount(){puts("AIResetStreamSampleCount is a stub");}
void AISetStreamSampleRate(u32){puts("AISetStreamSampleRate is a stub");}
u32 ARInit(u32 *,u32){puts("ARInit is a stub");return 0;}
void ARQInit(){puts("ARQInit is a stub");}
u32 DTKGetState(){puts("DTKGetState is a stub");return 0;}
void DTKInit(){puts("DTKInit is a stub");}
void DTKNextTrack(){puts("DTKNextTrack is a stub");}
void DTKPrevTrack(){puts("DTKPrevTrack is a stub");}
u32 DTKQueueTrack(char *fileName, DTKTrack *track, u32 eventMask, DTKCallback callback){puts("DTKQueueTrack is a stub");return 0;}
u32 DTKRemoveTrack(DTKTrack*){puts("DTKRemoveTrack is a stub");return 0;}
void DTKSetRepeatMode(u32){puts("DTKSetRepeatMode is a stub");}
void DTKSetState(u32){puts("DTKSetState is a stub");}
void DTKSetVolume(int,int){puts("DTKSetVolume is a stub");}
u32 OSGetSoundMode(){puts("OSGetSoundMode is a stub");return 0;}
void OSSetSoundMode(u32){puts("OSSetSoundMode is a stub");}
//void PADControlAllMotors(const u32*){puts("PADControlAllMotors is a stub");}
void PADControlMotor(s32,u32){puts("PADControlMotor is a stub");}
float camPerspAngle;
s32 camPerspChange;
void func_8002FC90(int, int){puts("func_8002FC90 is a stub");}
void func_800B0B48(){puts("func_800B0B48 is a stub");}
void sndActive(){puts("sndActive is a stub");}
void sndAuxCallbackChorus(){puts("sndAuxCallbackChorus is a stub");}
void sndAuxCallbackPrepareChorus(){puts("sndAuxCallbackPrepareChorus is a stub");}
void sndAuxCallbackPrepareReverbHI(){puts("sndAuxCallbackPrepareReverbHI is a stub");}
void sndAuxCallbackReverbHI(){puts("sndAuxCallbackReverbHI is a stub");}
void sndFXCheck(){puts("sndFXCheck is a stub");}
void sndFXCtrl(){puts("sndFXCtrl is a stub");}
void sndFXCtrl14(){puts("sndFXCtrl14 is a stub");}
void sndFXKeyOff(){puts("sndFXKeyOff is a stub");}
void sndFXStartParaInfo(){puts("sndFXStartParaInfo is a stub");}
void sndMasterVolume(){puts("sndMasterVolume is a stub");}
void sndOutputMode(){puts("sndOutputMode is a stub");}
void sndPopGroup(){puts("sndPopGroup is a stub");}
void sndPushGroup(){puts("sndPushGroup is a stub");}
void sndSetAuxProcessingCallbacks(){puts("sndSetAuxProcessingCallbacks is a stub");}
void sndSetHooks(){puts("sndSetHooks is a stub");}
void sndStreamAllocEx(){puts("sndStreamAllocEx is a stub");}
void sndStreamFree(){puts("sndStreamFree is a stub");}
void sndVolume(){puts("sndVolume is a stub");}
