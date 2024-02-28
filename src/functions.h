#ifndef _SRC_FUNCTIONS_H_
#define _SRC_FUNCTIONS_H_

#include "types.h"

// ? main();
void initialize(void);
void init_gx(void);
void init_tv(void);
void init_vi(void);
void init_gx_2(void);
void init_dvd(void);
void init_heaps(void);
void init_rel(void);
void init_cache(void);
void init_cache_ptrs(void);

// ? title_screen_debug_menu();
void u_menu_input_debug(void);
void u_menu_input_notdebug(void);
void submode_dummy_func(void);

void u_reset_gamedata(void);
void u_store_gamedata();
void u_load_gamedata();

void mode_sel_func(void);
void func_800123DC(void);
void submode_sel_ngc_dest_func(void);
void func_80012434(int);
void func_80012510(void (*a)(void), void (*b)(void), BallCallback ballcb, CameraCallback cameracb);
void submode_sel_init_func(void);
void submode_sel_main_func(void);
void func_800125A4(void);

void clip_init_detail(Vec *, S16Vec *, float, float, float, float);
BOOL clip_z(Vec *a, float b);
BOOL test_sphere_in_frustum(Point3d *p, float radius);
BOOL test_scaled_sphere_in_frustum(Point3d *p, float radius, float scale);

void init_wait_vblank(void);
void beginframe_main(void);
void gpwait_main(void);
void syncwait_main(void);

int load_common_graphics(void);
void func_800249D4(void);
void u_free_minigame_graphics(void);

void debug_main(void);

void u_create_joints_from_skeleton(struct AnimJoint *a, struct Skeleton *b, u16 c);
void u_load_new_anim_into_joints(struct AnimJoint *a, u16 b);
// ? u_reset_channels_in_joints();
// ? u_load_channels_from_dat_into_joints();
// ? u_read_channel_from_dat();
int init_ape_model_info(char *, char *, char *, char *);
void adjust_motdat_pointers(struct MotDat *);
void adjust_motlabel_pointers(u32 *);
void adjust_motskl_pointers(struct SkeletonFileData *);
void adjust_motinfo_pointers(struct MotInfo *);
u16 u_get_motdat_keyframe_count(u16 index);

// motload_2.c
void u_interpolate_joint_motion(struct AnimJoint *a, const struct JointRotationSomething *b, const struct JointPositionSomething *c, float e, u32 d);
void u_mot_joint_start_anim(struct ApeAnimationThing *);
void mot_joint_800355FC(struct ApeAnimationThing *);

void u_update_skel_anim(struct ApeAnimationThing *);
void u_joint_tree_calc_some_matrix();
// ? u_joint_tree_calc_some_other_matrix();
// ? calc_some_rotation_mtx_from_vec();
// ? func_80035E7C();
void u_init_something_joints_from_something(struct Struct8003699C_child_sub *arg0, struct ApeAnimationThing *arg1, s32 arg2, u16 arg3);
void func_80035FDC(struct ApeAnimationThing *);
void u_animate_ape_hands(struct ApeAnimationThing *);
void func_80036510(struct Struct8003699C_child_sub *);
// ? func_80036544();
// ? func_800366F8();
// ? func_80036720();
void u_create_joints_from_hardcoded_arrays(struct AnimJoint *arg0, u16 arg1, u16 arg2);

float interpolate_keyframes(int count, struct Keyframe *frames, float time);

void draw_stage_preview(void);
void u_apply_func_to_nl_model_vertices(struct NlModel *model, void (*b)(struct NlVtxTypeB *), void (*c)(struct NlVtxTypeA *));
void u_apply_func_to_nl_disp_list_type_b(struct NlDispList *dl, void *end, void (*func)(struct NlVtxTypeB *));
void u_apply_func_to_nl_disp_list_type_a(struct NlDispList *dl, void *end, void (*func)(struct NlVtxTypeA *));
void func_80048084(struct NlModel *arg0, struct NlModel *arg1, float *arg2);
void func_80048420();

// ? func_80081D34();
// ? func_80081D64();
// ? func_80081D90();
// ? func_80081DBC();
// ? func_80081DEC();
// ? func_80081E1C();
// ? func_80081E4C();
// ? func_80081E58();
// ? func_80081E84();
// ? func_80081EB4();
// ? func_80081EC4();
// ? func_80081EF0();
// ? func_80081F20();
void func_80081F30(void);
void help_sprite_main(s8 *, struct Sprite *);
// ? draw_controls_info();
// ? draw_help_blurbs();

// ? set_ambient_light();
// ? draw_aiai_hair();
void u_something_with_eyes_blinking(struct Ape *);
// ? draw_eye();
// ? draw_baby_hand();
// ? draw_left_hand();
// ? draw_right_hand();
// ? func_800862F4();
// ? func_8008638C();
// ? assign_shape_colors_1();
// ? next_shape();
// ? draw_baby_head();
// ? u_facial_expression_model_distort();
// ? draw_head();
// ? draw_ear();

int lzs_decompress(u8 *, void *);

void mode_mini_func(void);
void set_minigame_callbacks(void (*arg0)(), void (*arg1)(), void (*ballCb)(struct Ball *), void (*cameraCb)(struct Camera *, struct Ball *));
void submode_mini_game_init_func(void);
void submode_mini_game_main_func(void);
void u_minigame_draw(void);
void func_80093B84(void);
void func_80093BB4(void);
void submode_mini_select_init_func(void);
void submode_mini_select_main_func(void);
void submode_mini_test0_init_func(void);
void submode_mini_test0_main_func(void);
void func_80093E28(void);
void submode_mini_ending_init_func(void);
void submode_mini_ending_main_func(void);
void u_draw_ending_viewer_text(void);

void func_80094750(int);
void func_800947B0(void);
int func_800947F8(void);
void func_80094840(void);
void *func_80094870(void);
void func_800948D0(struct Struct80094870 *);
void func_800948F4(void);
void poly_shadow_init(void);
int set_poly_shadow(struct PolyShadowUnit *);
void poly_shadow_draw(void);

void submode_mini_commend_init_func(void);
void submode_mini_commend_main_func(void);
void func_8009BEF8(void);
// ? func_8009BF74();
// ? func_8009C178();
// ? func_8009C5E4();
// ? func_8009CAE0();
// ? mini_commend_free_data();
// ? func_8009CD5C();
int func_8009D5D8(void);
// ? func_8009D5F4();
// ? func_8009D794();
int func_8009D7CC(void);
int func_8009D7E8(void);
void func_8009D7FC(int, Vec *);
void func_8009D98C(Vec *);
// ? func_8009DB40();
void func_8009DB6C(Mtx);
int func_8009DBB0(Vec *, struct Struct8009DBB0 *, Vec *);

// ? DEMOInitCaption();
// ? DEMOInitROMFont();
// ? DEMOSetROMFontSize();
// ? DrawFontChar();
// ? DEMORFPuts();
// ? DEMORFPrintf();
// ? DEMOGetRFTextWidth();
void func_800A5704(void);
void ev_view_init(void);
void ev_view_main(void);
void ev_view_dest(void);
void view_draw(void);
void view_draw_simple(void);
void view_create_text_sprites(void);
void view_destroy_text_sprites(void);
void view_init_stage_anim(void);
void view_animate_stage(void);
void func_800A66CC(void);
void func_800A66E4(void);
void func_800A66FC(void);
void func_800A671C(void);
void draw_items(void);
void draw_banana_shadows(void);
void draw_stage_geometry(void);
void draw_stage_objects(void);
void get_curr_stage_view_bounds(struct Sphere *);
void view_apply_camera(struct Camera *);
void submode_mini_ranking_init_func(void);
void submode_mini_ranking_main_func(void);
// ? func_800A7A8C();
// ? func_800A7B7C();
// ? func_800A82D0();
// ? func_800A83C4();
// ? func_800A85F8();
// ? func_800A8828();
// ? func_800A8C44();
// ? func_800A8FCC();
// ? func_800A916C();
// ? func_800AADC4();
// ? func_800AAE80();
// ? func_800AAF40();
// ? func_800AB174();
// ? func_800AB2A0();
// ? func_800AB444();
// ? func_800AB720();
// ? func_800AB830();
// ? func_800AB8D8();
// ? func_800AB974();
// ? func_800ABA10();
// ? func_800ABAE0();
// ? func_800ABCF4();
// ? func_800ABE80();
// ? func_800AC064();
// ? func_800AC248();
// ? func_800AC43C();

void avDVDErrorHandling(void);
int avDVDRead(DVDFileInfo *file, void *arg1, u32 arg2, int arg3);
void init_loading_gct(void);

void credits_finish(void);
void credits_init(void);
void credits_main(void);
void func_800B0B48(void);
// ? func_800B340C();
// ? func_800B34B4();
// ? func_800B361C();
// ? func_800B3784();
// ? func_800B38EC();
// ? func_800B3CC8();
// ? func_800B3DCC();
// ? func_800B3ECC();
// ? func_800B42EC();
// ? func_800B4710();
// ? func_800B4A50();
// ? func_800B5020();
// ? func_800B55E4();
// ? func_800B5B5C();
// ? func_800B5C54();

// mkbe.sel_stage.rel functions
void create_sel_stage_sprites(void);
void dummy_func_C90(void);
void dummy_func_C94(void);
void dummy_func_C98(void);
void sel_stage_draw(void);

// ? PPCMfmsr();
// ? PPCMtmsr();
// ? PPCMfhid0();
// ? PPCMfl2cr();
// ? PPCMtl2cr();
// ? PPCMtdec();
// ? PPCSync();
// ? PPCHalt();
// ? PPCMtmmcr0();
// ? PPCMtmmcr1();
// ? PPCMfpmc1();
// ? PPCMtpmc1();
// ? PPCMtpmc2();
// ? PPCMfpmc3();
// ? PPCMtpmc3();
// ? PPCMfpmc4();
// ? PPCMtpmc4();
// ? PPCMfhid2();
// ? PPCMthid2();
// ? PPCMtwpar();
// ? OSGetConsoleType();
// ? OSInit();
// ? OSExceptionInit();
// ? __OSDBINITSTART();
// ? __OSDBJUMPSTART();
// ? __OSDBJUMPEND();
// ? __OSGetExceptionHandler();
// ? __OSEVStart();
// ? __OSEVEnd();
// ? OSDefaultExceptionHandler();
// ? __OSPSInit();
// ? __OSGetDIConfig();
// ? OSInitAlarm();
// ? OSCreateAlarm();
// ? InsertAlarm();
// ? OSSetAlarm();
// ? OSSetAbsAlarm();
// ? OSSetPeriodicAlarm();
// ? OSCancelAlarm();
// ? DecrementerExceptionCallback();
// ? DecrementerExceptionHandler();
// ? DLInsert();
// ? OSAllocFromHeap();
// ? OSFreeToHeap();
// ? OSSetCurrentHeap();
// ? OSInitAlloc();
// ? OSCreateHeap();
// ? OSCheckHeap();
// ? OSGetArenaHi();
// ? OSGetArenaLo();
// ? OSSetArenaHi();
// ? OSSetArenaLo();
// ? __OSInitAudioSystem();
// ? __OSStopAudioSystem();
// ? DCEnable();
// ? DCInvalidateRange();
// ? DCFlushRange();
// ? DCStoreRange();
// ? DCFlushRangeNoSync();
// ? DCZeroRange();
// ? ICInvalidateRange();
// ? ICFlashInvalidate();
// ? ICEnable();
// ? __LCEnable();
// ? LCEnable();
// ? L2GlobalInvalidate();
// ? DMAErrorHandler();
// ? __OSCacheInit();
// ? __OSLoadFPUContext();
// ? __OSSaveFPUContext();
// ? OSSetCurrentContext();
// ? OSGetCurrentContext();
// ? OSSaveContext();
// ? OSLoadContext();
// ? OSGetStackPointer();
// ? OSClearContext();
// ? OSDumpContext();
// ? OSSwitchFPUContext();
// ? __OSContextInit();
// ? OSReport();
// ? OSPanic();
// ? OSSetErrorHandler();
// ? __OSUnhandledException();
// ? SetExiInterruptMask();
// ? EXIImm();
// ? EXIImmEx();
// ? EXIDma();
// ? EXISync();
// ? EXIClearInterrupts();
// ? EXISetExiCallback();
// ? EXIProbe();
// ? EXIProbeEx();
// ? EXIAttach();
// ? EXIDetach();
// ? EXISelect();
// ? EXIDeselect();
// ? EXIInterruptHandler();
// ? TCInterruptHandler();
// ? EXTInterruptHandler();
// ? EXIInit();
// ? EXILock();
// ? EXIUnlock();
// ? EXIGetState();
// ? EXIGetID();
// ? GetFontCode();
// ? Decode();
// ? OSGetFontEncode();
// ? ReadROM();
// ? ExpandFontSheet();
// ? OSInitFont();
// ? OSGetFontTexture();
// ? OSGetFontWidth();
// ? OSDisableInterrupts();
// ? OSEnableInterrupts();
// ? OSRestoreInterrupts();
// ? __OSSetInterruptHandler();
// ? __OSGetInterruptHandler();
// ? __OSInterruptInit();
// ? SetInterruptMask();
// ? __OSMaskInterrupts();
// ? __OSUnmaskInterrupts();
// ? __OSDispatchInterrupt();
// ? ExternalInterruptHandler();
// ? func_800C68DC();
// ? OSNotifyLink();
// ? OSSetStringTable();
// ? Relocate();
// ? OSLink();
// ? Undo();
// ? OSUnlink();
// ? __OSModuleInit();
// ? OSGetPhysicalMemSize();
// ? __OSUnlockAllMutex();
// ? Run();
// ? Callback();
// ? __OSReboot();
// ? OSRegisterResetFunction();
// ? Reset();
// ? __OSDoHotReset();
// ? OSResetSystem();
// ? __OSResetSWInterruptHandler();
// ? OSGetResetSwitchState();
// ? WriteSramCallback();
// ? WriteSram();
// ? __OSInitSram();
// ? __OSLockSram();
// ? __OSLockSramEx();
// ? UnlockSram();
// ? __OSUnlockSram();
// ? __OSUnlockSramEx();
// ? __OSSyncSram();
// ? __OSReadRom();
// ? OSGetSoundMode();
// ? OSSetSoundMode();
// ? SIBusy();
// ? CompleteTransfer();
// ? SIInterruptHandler();
// ? SIInit();
// ? __SITransfer();
// ? SIGetStatus();
// ? SISetCommand();
// ? SITransferCommands();
// ? SISetXY();
// ? SIEnablePolling();
// ? SIDisablePolling();
// ? SIGetResponse();
// ? AlarmHandler();
// ? SITransfer();
// ? __OSSystemCallVectorStart();
// ? __OSSystemCallVectorEnd();
// ? __OSInitSystemCall();
// ? __OSThreadInit();
// ? OSInitThreadQueue();
// ? OSGetCurrentThread();
// ? OSDisableScheduler();
// ? OSEnableScheduler();
// ? UnsetRun();
// ? __OSGetEffectivePriority();
// ? SetEffectivePriority();
// ? SelectThread();
// ? __OSReschedule();
// ? OSCancelThread();
// ? OSResumeThread();
// ? OSSuspendThread();
// ? OSSleepThread();
// ? OSWakeupThread();
// ? OSGetTime();
// ? OSGetTick();
// ? __OSGetSystemTime();
// ? GetDates();
// ? OSTicksToCalendarTime();
// ? InitializeUART();
// ? ReadUARTN();
// ? WriteUARTN();
// ? __init_user();
// ? __init_cpp();
// ? _ExitProcess();
// ? DBInit();
// ? __DBExceptionDestinationAux();
// ? __DBExceptionDestination();
// ? __DBIsExceptionMarked();
// ? DBPrintf();
// ? PSMTXIdentity();
// ? C_MTXScale();
// ? C_MTXQuat();
// ? C_MTXLookAt();
// ? C_MTXFrustum();
// ? C_MTXPerspective();
// ? C_MTXOrtho();
// ? PSVECNormalize();
// ? PSVECCrossProduct();
// ? __DVDInitWA();
// ? __DVDInterruptHandler();
// ? AlarmHandler_2();
// ? AlarmHandlerForTimeout();
// ? Read();
// ? SeekTwiceBeforeRead();
// ? DVDLowRead();
// ? DVDLowSeek();
// ? DVDLowWaitCoverClose();
// ? DVDLowReadDiskID();
// ? DVDLowStopMotor();
// ? DVDLowRequestError();
// ? DVDLowInquiry();
// ? DVDLowAudioStream();
// ? DVDLowRequestAudioStatus();
// ? DVDLowAudioBufferConfig();
// ? DVDLowReset();
// ? DVDLowBreak();
// ? DVDLowClearCallback();
// ? __DVDLowSetWAType();
// ? __DVDFSInit();
// ? DVDConvertPathToEntrynum();
// ? DVDFastOpen();
// ? DVDOpen();
// ? DVDClose();
// ? entryToPath();
// ? DVDGetCurrentDir();
// ? DVDChangeDir();
// ? DVDReadAsyncPrio();
// ? cbForReadAsync();
// ? DVDOpenDir();
// ? DVDReadDir();
// ? DVDCloseDir();
// ? DVDPrepareStreamAsync();
// ? cbForPrepareStreamAsync();
// ? DVDInit();
// ? stateReadingFST();
// ? cbForStateReadingFST();
// ? cbForStateError();
// ? stateTimeout();
// ? stateGettingError();
// ? CategorizeError();
// ? cbForStateGettingError();
// ? cbForUnrecoveredError();
// ? cbForUnrecoveredErrorRetry();
// ? stateGoToRetry();
// ? cbForStateGoToRetry();
// ? stateCheckID();
// ? stateCheckID3();
// ? stateCheckID2();
// ? cbForStateCheckID1();
// ? cbForStateCheckID2();
// ? cbForStateCheckID3();
// ? AlarmHandler_3();
// ? stateCoverClosed();
// ? stateCoverClosed_CMD();
// ? cbForStateCoverClosed();
// ? stateMotorStopeped();
// ? cbForStateMotorStopped();
// ? stateReady();
// ? stateBusy();
// ? cbForStateBusy();
// ? DVDReadAbsAsyncPrio();
// ? DVDReadAbsAsyncForBS();
// ? DVDReadDiskID();
// ? DVDPrepareStreamAbsAsync();
// ? DVDCancelStreamAsync();
// ? DVDStopStreamAtEndAsync();
// ? DVDGetStreamErrorStatusAsync();
// ? DVDGetStreamPlayAddrAsync();
// ? DVDReset();
// ? DVDGetDriveStatus();
// ? DVDCancelAsync();
// ? DVDCancel();
// ? cbForCancelSync();
// ? DVDGetCurrentDiskID();
// ? DVDCheckDisk();
// ? __DVDPrepareResetAsync();
// ? __DVDClearWaitingQueue();
// ? __DVDPushWaitingQueue();
// ? __DVDPopWaitingQueue();
// ? __DVDCheckWaitingQueue();
// ? __DVDDequeueWaitingQueue();
// ? __DVDStoreErrorCode();
// ? cb();
// ? __fstLoad();
// ? __VIRetraceHandler();
// ? getTiming();
// ? func_800CF02C();
// ? func_800CF034();
// ? func_800CF03C();
// ? func_800CF044();
// ? func_800CF04C();
// ? func_800CF054();
// ? __VIInit();
// ? VIInit();
// ? VIWaitForRetrace();
// ? setFbbRegs();
// ? setVerticalRegs();
// ? VIConfigure();
// ? VIFlush();
// ? VISetNextFrameBuffer();
// ? VISetBlack();
// ? getCurrentFieldEvenOdd();
// ? VIGetNextField();
// ? VIGetTvFormat();
// ? DoReset();
// ? PADProbeCallback();
// ? UpdateOrigin();
// ? PADOriginCallback();
// ? PADOriginUpdateCallback();
// ? PADFixCallback();
// ? PADResetCallback();
// ? PADReset();
// ? PADRecalibrate();
// ? PADInit();
// ? PADReceiveCheckCallback();
// ? PADRead();
// ? PADSetSamplingRate();
// ? PADControlAllMotors();
// ? PADControlMotor();
// ? PADSetSpec();
// ? SPEC0_MakeStatus();
// ? SPEC1_MakeStatus();
// ? SPEC2_MakeStatus();
// ? PADSetAnalogMode();
// ? OnReset();
// ? AIRegisterDMACallback();
// ? AIInitDMA();
// ? AIStartDMA();
// ? AIRegisterStreamCallback();
// ? AIGetStreamSampleCount();
// ? AIResetStreamSampleCount();
// ? AISetStreamTrigger();
// ? AISetStreamPlayState();
// ? AIGetStreamPlayState();
// ? AISetDSPSampleRate();
// ? AIGetDSPSampleRate();
// ? AISetStreamSampleRate();
// ? __AI_set_stream_sample_rate();
// ? AIGetStreamSampleRate();
// ? AISetStreamVolLeft();
// ? AIGetStreamVolLeft();
// ? AISetStreamVolRight();
// ? AIGetStreamVolRight();
// ? AIInit();
// ? __AISHandler();
// ? __AIDHandler();
// ? __AICallbackStackSwitch();
// ? __AI_SRC_INIT();
// ? ARRegisterDMACallback();
// ? ARGetDMAStatus();
// ? ARStartDMA();
// ? ARInit();
// ? ARGetBaseAddress();
// ? ARGetSize();
// ? __ARHandler();
// ? __ARChecksize();
// ? __ARQServiceQueueLo();
// ? __ARQCallbackHack();
// ? __ARQInterruptServiceRoutine();
// ? ARQInit();
// ? ARQPostRequest();
// ? DSPCheckMailToDSP();
// ? DSPCheckMailFromDSP();
// ? DSPReadMailFromDSP();
// ? DSPSendMailToDSP();
// ? DSPInit();
// ? DSPAddTask();
// ? __DSP_debug_printf();
// ? __DSPHandler();
// ? __DSP_exec_task();
// ? __DSP_boot_task();
// ? __DSP_insert_task();
// ? __DSP_remove_task();
// ? __CARDDefaultApiCallback();
// ? __CARDSyncCallback();
// ? __CARDExtHandler();
// ? __CARDExiHandler();
// ? __CARDTxHandler();
// ? __CARDUnlockedHandler();
// ? __CARDReadNintendoID();
// ? __CARDEnableInterrupt();
// ? __CARDReadStatus();
// ? __CARDClearStatus();
// ? TimeoutHandler();
// ? Retry();
// ? UnlockedCallback();
// ? __CARDStart();
// ? __CARDReadSegment();
// ? __CARDWritePage();
// ? __CARDEraseSector();
// ? CARDInit();
// ? __CARDSetDiskID();
// ? __CARDGetControlBlock();
// ? __CARDPutControlBlock();
// ? CARDGetResultCode();
// ? CARDFreeBlocks();
// ? __CARDSync();
// ? func_800D64B8();
// ? bitrev();
// ? ReadArrayUnlock();
// ? DummyLen();
// ? __CARDUnlock();
// ? InitCallback();
// ? DoneCallback();
// ? BlockReadCallback();
// ? __CARDRead();
// ? BlockWriteCallback();
// ? __CARDWrite();
// ? __CARDGetFatBlock();
// ? WriteCallback();
// ? EraseCallback();
// ? __CARDAllocBlock();
// ? __CARDFreeBlock();
// ? __CARDUpdateFatBlock();
// ? __CARDGetDirBlock();
// ? WriteCallback_2();
// ? EraseCallback_2();
// ? __CARDUpdateDir();
// ? __CARDCheckSum();
// ? VerifyID();
// ? VerifyDir();
// ? VerifyFAT();
// ? __CARDVerify();
// ? CARDCheckExAsync();
// ? CARDCheckAsync();
// ? CARDProbeEx();
// ? DoMount();
// ? __CARDMountCallback();
// ? CARDMountAsync();
// ? CARDMount();
// ? DoUnmount();
// ? CARDUnmount();
// ? FormatCallback();
// ? __CARDFormatRegionAsync();
// ? __CARDCompareFileName();
// ? __CARDAccess();
// ? __CARDIsPublic();
// ? __CARDGetFileNo();
// ? CARDFastOpen();
// ? CARDOpen();
// ? CARDClose();
// ? __CARDIsOpened();
// ? CreateCallbackFat();
// ? CARDCreateAsync();
// ? __CARDSeek();
// ? ReadCallback();
// ? CARDReadAsync();
// ? CARDCancel();
// ? WriteCallback_3();
// ? EraseCallback_3();
// ? CARDWriteAsync();
// ? DeleteCallback();
// ? CARDFastDeleteAsync();
// ? CARDDeleteAsync();
// ? CARDDelete();
// ? UpdateIconOffsets();
// ? CARDGetStatus();
// ? CARDSetStatusAsync();
// ? CARDRenameAsync();
// ? CARDRename();
// ? HIOReadMailbox();
// ? HIOWriteMailbox();
// ? HIORead();
// ? HIOWrite();
// ? __GXDefaultTexRegionCallback();
// ? __GXDefaultTlutRegionCallback();
// ? GXInit();
// ? GXCPInterruptHandler();
// ? GXInitFifoBase();
// ? GXInitFifoPtrs();
// ? GXInitFifoLimits();
// ? GXSetCPUFifo();
// ? GXSetGPFifo();
// ? GXSaveCPUFifo();
// ? __GXSaveCPUFifoAux();
// ? GXGetGPStatus();
// ? GXGetFifoPtrs();
// ? __GXFifoInit();
// ? __GXFifoReadEnable();
// ? __GXFifoReadDisable();
// ? __GXFifoLink();
// ? __GXWriteFifoIntEnable();
// ? __GXWriteFifoIntReset();
// ? GXGetCPUFifo();
// ? GXGetGPFifo();
// ? __GXXfVtxSpecs();
// ? GXSetVtxDesc();
// ? GXSetVtxDescv();
// ? __GXSetVCD();
// ? GXGetVtxDesc();
// ? GXGetVtxDescv();
// ? GXClearVtxDesc();
// ? GXSetVtxAttrFmt();
// ? GXSetVtxAttrFmtv();
// ? __GXSetVAT();
// ? GXGetVtxAttrFmt();
// ? func_800DEB2C();
// ? func_800DEB70();
// ? func_800DEB94();
// ? func_800DEBB8();
// ? func_800DEBE0();
// ? func_800DEC08();
// ? func_800DEC30();
// ? func_800DEC58();
// ? func_800DEC80();
// ? func_800DECA8();
// ? func_800DECD0();
// ? GXGetVtxAttrFmtv();
// ? GXInvalidateVtxCache();
// ? GXSetTexCoordGen2();
// ? GXSetNumTexGens();
// ? GXFlush();
// ? GXSetDrawSync();
// ? GXSetDrawDone();
// ? GXDrawDone();
// ? GXPokeAlphaMode();
// ? GXPokeAlphaRead();
// ? GXPokeAlphaUpdate();
// ? GXPokeBlendMode();
// ? GXPokeColorUpdate();
// ? GXPokeDstAlpha();
// ? GXPokeDither();
// ? GXPokeZMode();
// ? GXSetDrawSyncCallback();
// ? GXTokenInterruptHandler();
// ? GXSetDrawDoneCallback();
// ? GXFinishInterruptHandler();
// ? __GXPEInit();
void __GXSetDirtyState(void);
// ? GXBegin();
// ? __GXSendFlushPrim();
// ? GXSetLineWidth();
// ? GXSetPointSize();
// ? GXEnableTexOffsets();
// ? GXSetCullMode();
// ? GXSetCoPlanar();
// ? __GXSetGenMode();
// ? GXAdjustForOverscan();
// ? GXSetDispCopySrc();
// ? GXSetTexCopySrc();
// ? GXSetDispCopyDst();
// ? GXSetTexCopyDst();
// ? GXSetDispCopyFrame2Field();
// ? GXSetCopyClamp();
// ? GXSetDispCopyYScale();
// ? GXSetCopyClear();
// ? GXSetCopyFilter();
// ? GXSetDispCopyGamma();
// ? GXCopyDisp();
// ? GXCopyTex();
// ? GXClearBoundingBox();
// ? GXInitLightAttnK();
// ? GXInitLightSpot();
// ? GXInitLightDistAttn();
// ? GXInitLightPos();
// ? GXGetLightPos();
// ? GXInitLightDir();
// ? GXInitLightColor();
// ? GXGetLightColor();
// ? GXLoadLightObjImm();
// ? GXSetChanAmbColor();
// ? GXSetChanMatColor();
// ? GXSetNumChans();
// ? GXSetChanCtrl();
// ? GXGetTexBufferSize();
// ? __GetImageTileCount();
// ? GXInitTexObj();
// ? GXInitTexObjLOD();
// ? GXInitTexObjWrapMode();
// ? GXGetTexObjData();
// ? GXGetTexObjWidth();
// ? GXGetTexObjHeight();
// ? GXGetTexObjFmt();
// ? GXGetTexObjWrapS();
// ? GXGetTexObjWrapT();
// ? GXGetTexObjMipMap();
// ? GXLoadTexObjPreLoaded();
// ? GXLoadTexObj();
// ? GXInitTexCacheRegion();
// ? GXInitTlutRegion();
// ? GXInvalidateTexAll();
// ? GXSetTexRegionCallback();
// ? GXSetTlutRegionCallback();
// ? __SetSURegs();
// ? __GXSetSUTexSize();
// ? GXSetTevIndirect();
// ? GXSetIndTexMtx();
// ? GXSetIndTexCoordScale();
// ? GXSetIndTexOrder();
// ? GXSetNumIndStages();
// ? GXSetTevDirect();
// ? __GXUpdateBPMask();
// ? __GXFlushTextureState();


// ? GXDrawCylinder();
// ? GXDrawSphere();
// ? GXDrawCubeFace();
// ? GXDrawCube();
// ? __GXSetRange();
// ? GXBeginDisplayList();
// ? GXEndDisplayList();
// ? GXCallDisplayList();
// ? GXSetProjection();
// ? GXGetProjectionv();
// ? WriteMTXPS4x3();
// ? WriteMTXPS3x3from3x4();
// ? WriteMTXPS4x2();
// ? GXLoadPosMtxImm();
// ? GXLoadNrmMtxImm();
// ? GXSetCurrentMtx();
// ? GXLoadTexMtxImm();
// ? GXSetViewportJitter();
// ? GXSetViewport();
// ? GXSetScissor();
// ? GXSetScissorBoxOffset();
// ? GXSetClipMode();
// ? __GXSetMatrixIndex();
// ? GXClearGPMetric();
// ? GXInitXfRasMetric();
// ? GXReadXfRasMetric();
// ? PERFResetAllMemMetrics();
// ? PERFGetAllMemMetrics();
// ? PERFTokenCallback();
// ? PERFInit();
// ? PERFSetEvent();
// ? PERFStartFrame();
// ? PERFEndFrame();
// ? PERFEventStart();
// ? PERFEventEnd();
// ? PERFStartAutoSample();
// ? PERFEndAutoSample();
// ? PERFTimerCallback();
// ? PERFStartAutoSampling();
// ? PERFStopAutoSampling();
// ? __PERFDrawInit();
// ? PERFPreDraw();
// ? DrawBWBar();
// ? DrawKey();
// ? PERFDumpScreen();
// ? PERFPostDraw();
// ? PERFSetDrawBWBarKey();

// ? __DTKCallbackForStreamStatus();
// ? __DTKCallbackForRun();
// ? __DTKCallbackForPreparePaused();
// ? __DTKCallbackForPlaylist();
// ? __DTKCallbackForAIInterrupt();
// ? __DTKCallbackForStop();
// ? __DTKCallbackForNextTrack();
// ? __DTKCallbackForPrevTrack();
// ? DTKInit();
// ? DTKQueueTrack();
// ? DTKRemoveTrack();
// ? DTKSetRepeatMode();
// ? DTKSetState();
// ? DTKNextTrack();
// ? DTKPrevTrack();
// ? DTKGetState();
// ? DTKSetVolume();
// ? __va_arg();
// ? __destroy_global_chain();
// ? __unregister_fragment();
// ? __register_fragment();
// ? __cvt_fp2unsigned();
// ? __div2u();
// ? __div2i();
// ? __mod2u();
// ? __mod2i();
// ? __shl2i();
// ? __shr2u();
// ? __shr2i();
// ? __cvt_sll_flt();
// ? __cvt_dbl_usll();
// ? GetR2__Fv();
// ? exit();
// ? __num2dec();
// ? __flush_buffer();
// ? __prep_buffer();
// ? puts();
// ? __put_char();
// ? __kill_critical_regions();
// ? tolower();
// ? fwrite();
// ? wcstombs();
// ? memcmp();
// ? memchr();
// ? memmove();
// ? __copy_longs_rev_unaligned();
// ? __copy_longs_unaligned();
// ? __copy_longs_rev_aligned();
// ? __copy_longs_aligned();
// ? __stdio_atexit();
// ? sprintf();
// ? vsprintf();
// ? vprintf();
// ? printf();
// ? __StringWrite();
// ? __FileWrite();
// ? __pformatter();
// ? float2str();
// ? round_decimal();
// ? longlong2str();
// ? long2str();
// ? parse_format();
// ? srand();
// ? rand();
// ? strchr();
// ? strncmp();
// ? strcmp();
// ? strncat();
// ? strncpy();
// ? strcpy();
// ? strlen();
// ? func_80106B80();
// ? func_80106B88();
// ? func_80106C20();
// ? fwide();
// ? fabs__Fd();
// ? scalbn();
// ? __ieee754_rem_pio2();
// ? __fpclassifyd__Fd();
// ? __kernel_cos();
// ? __kernel_rem_pio2();
// ? __kernel_sin();
// ? copysign();
// ? cos();
// ? floor();
// ? frexp();
// ? ldexp();
// ? sin();
// ? fabsf__Ff();
// ? atanf();
// ? atan__Ff();
// ? _inv_sqrtf();
// ? acosf();
// ? tanf();
// ? cos__Ff();
// ? sin__Ff();
// ? cosf();
// ? sinf();
// ? powf();

// ? AmcEXIImm();
// ? AmcEXISync();
// ? AmcEXIClearInterrupts();
// ? AmcEXISetExiCallback();
// ? AmcEXISelect();
// ? AmcEXIDeselect();
// ? AmcDebugIntHandler();
// ? AmcEXIEnableInterrupts();
// ? AmcEXIInit();
// ? EXI2_CallBack();
// ? EXI2_Init();
// ? EXI2_EnableInterrupts();
// ? EXI2_Poll();
// ? EXI2_ReadN();
// ? EXI2_WriteN();
// ? EXI2_Reserve();
// ? EXI2_Unreserve();
// ? DBGEXIImm();
// ? DBGReadMailbox();
// ? DBGRead();
// ? DBGWrite();
// ? DBGReadStatus();
// ? MWCallback();
// ? DBGHandler();
// ? DBInitComm();
// ? DBInitInterrupts();
// ? DBQueryData();
// ? DBRead();
// ? DBWrite();
// ? DBOpen();
// ? DBClose();
// ? AMC_IsStub();
// ? Hu_IsStub();

#endif
