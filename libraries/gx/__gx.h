extern volatile u16 *__cpReg;
extern volatile u32 *__piReg;

#define GX_REG_ASSERT(c) ASSERTMSG(c, "GX Internal: Register field out of range")

#define GX_FLAG_SET(regOrg, newFlag, regName)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        GX_REG_ASSERT(!((newFlag) & ~((1 << (regName##_SIZE)) - 1)));                                                  \
        (regOrg) = (((u32)(regOrg) & ~(regName##_MASK)) | (((u32)(newFlag) << (regName##_SHIFT)) & (regName##_MASK))); \
    } while (0)

#define GX_GENMODE_ID 0
#define GX_GENMODE_REG_ID_SIZE 8
#define GX_GENMODE_REG_ID_SHIFT 24
#define GX_GENMODE_REG_ID_MASK 0xff000000
#define GX_GENMODE_GET_REG_ID(genMode) ((((u32)(genMode)) & GX_GENMODE_REG_ID_MASK) >> GX_GENMODE_REG_ID_SHIFT)

#define GX_BPMASK_ID 15
#define GX_BPMASK_REG_ID_SIZE 8
#define GX_BPMASK_REG_ID_SHIFT 24
#define GX_BPMASK_REG_ID_MASK 0xff000000

#define GX_LPSIZE_ID 34
#define GX_LPSIZE_REG_ID_SIZE 8
#define GX_LPSIZE_REG_ID_SHIFT 24
#define GX_LPSIZE_REG_ID_MASK 0xff000000

#define TEV_COLOR_ENV_REG_ID_SIZE 8
#define TEV_COLOR_ENV_REG_ID_SHIFT 24
#define TEV_COLOR_ENV_REG_ID_MASK 0xff000000

#define TEV_ALPHA_ENV_REG_ID_SIZE 8
#define TEV_ALPHA_ENV_REG_ID_SHIFT 24
#define TEV_ALPHA_ENV_REG_ID_MASK 0xff000000

#define TEV_COLOR_ENV_0_ID 0x000000c0
#define TEV_ALPHA_ENV_0_ID 0x000000c1
#define TEV_COLOR_ENV_1_ID 0x000000c2
#define TEV_ALPHA_ENV_1_ID 0x000000c3
#define TEV_COLOR_ENV_2_ID 0x000000c4
#define TEV_ALPHA_ENV_2_ID 0x000000c5
#define TEV_COLOR_ENV_3_ID 0x000000c6
#define TEV_ALPHA_ENV_3_ID 0x000000c7
#define TEV_COLOR_ENV_4_ID 0x000000c8
#define TEV_ALPHA_ENV_4_ID 0x000000c9
#define TEV_COLOR_ENV_5_ID 0x000000ca
#define TEV_ALPHA_ENV_5_ID 0x000000cb
#define TEV_COLOR_ENV_6_ID 0x000000cc
#define TEV_ALPHA_ENV_6_ID 0x000000cd
#define TEV_COLOR_ENV_7_ID 0x000000ce
#define TEV_ALPHA_ENV_7_ID 0x000000cf
#define TEV_COLOR_ENV_8_ID 0x000000d0
#define TEV_ALPHA_ENV_8_ID 0x000000d1
#define TEV_COLOR_ENV_9_ID 0x000000d2
#define TEV_ALPHA_ENV_9_ID 0x000000d3
#define TEV_COLOR_ENV_A_ID 0x000000d4
#define TEV_ALPHA_ENV_A_ID 0x000000d5
#define TEV_COLOR_ENV_B_ID 0x000000d6
#define TEV_ALPHA_ENV_B_ID 0x000000d7
#define TEV_COLOR_ENV_C_ID 0x000000d8
#define TEV_ALPHA_ENV_C_ID 0x000000d9
#define TEV_COLOR_ENV_D_ID 0x000000da
#define TEV_ALPHA_ENV_D_ID 0x000000db
#define TEV_COLOR_ENV_E_ID 0x000000dc
#define TEV_ALPHA_ENV_E_ID 0x000000dd
#define TEV_COLOR_ENV_F_ID 0x000000de
#define TEV_ALPHA_ENV_F_ID 0x000000df

#define TEV_KSEL_REG_ID_SIZE 8
#define TEV_KSEL_REG_ID_SHIFT 24
#define TEV_KSEL_REG_ID_MASK 0xff000000

#define TEV_KSEL_0_ID 0x000000f6
#define TEV_KSEL_1_ID 0x000000f7
#define TEV_KSEL_2_ID 0x000000f8
#define TEV_KSEL_3_ID 0x000000f9
#define TEV_KSEL_4_ID 0x000000fa
#define TEV_KSEL_5_ID 0x000000fb
#define TEV_KSEL_6_ID 0x000000fc
#define TEV_KSEL_7_ID 0x000000fd

#define RAS_IREF_ID 39
#define RAS_IREF_REG_ID_SIZE 8
#define RAS_IREF_REG_ID_SHIFT 24
#define RAS_IREF_REG_ID_MASK 0xff000000

#define RAS_TREF0_ID 40
#define RAS_TREF_REG_ID_SIZE 8
#define RAS_TREF_REG_ID_SHIFT 24
#define RAS_TREF_REG_ID_MASK 0xff000000

#define SU_TS0_REG_ID_SIZE 8
#define SU_TS0_REG_ID_SHIFT 24
#define SU_TS0_REG_ID_MASK 0xff000000

#define SU_TS1_REG_ID_SIZE 8
#define SU_TS1_REG_ID_SHIFT 24
#define SU_TS1_REG_ID_MASK 0xff000000

#define SU_SCIS0_ID 0x00000020
#define SU_SCIS1_ID 0x00000021

#define SU_SCIS0_REG_ID_SIZE 8
#define SU_SCIS0_REG_ID_SHIFT 24
#define SU_SCIS0_REG_ID_MASK 0xff000000

#define SU_SCIS1_REG_ID_SIZE 8
#define SU_SCIS1_REG_ID_SHIFT 24
#define SU_SCIS1_REG_ID_MASK 0xff000000

#define SU_SSIZE0_ID 0x00000030
#define SU_TSIZE0_ID 0x00000031
#define SU_SSIZE1_ID 0x00000032
#define SU_TSIZE1_ID 0x00000033
#define SU_SSIZE2_ID 0x00000034
#define SU_TSIZE2_ID 0x00000035
#define SU_SSIZE3_ID 0x00000036
#define SU_TSIZE3_ID 0x00000037
#define SU_SSIZE4_ID 0x00000038
#define SU_TSIZE4_ID 0x00000039
#define SU_SSIZE5_ID 0x0000003a
#define SU_TSIZE5_ID 0x0000003b
#define SU_SSIZE6_ID 0x0000003c
#define SU_TSIZE6_ID 0x0000003d
#define SU_SSIZE7_ID 0x0000003e
#define SU_TSIZE7_ID 0x0000003f

#define GX_ZMODE_ID 64
#define GX_ZMODE_REG_ID_SIZE 8
#define GX_ZMODE_REG_ID_SHIFT 24
#define GX_ZMODE_REG_ID_MASK 0xff000000

#define GX_CMODE0_ID 65
#define GX_CMODE0_REG_ID_SIZE 8
#define GX_CMODE0_REG_ID_SHIFT 24
#define GX_CMODE0_REG_ID_MASK 0xff000000

#define GX_CMODE1_ID 66
#define GX_CMODE1_REG_ID_SIZE 8
#define GX_CMODE1_REG_ID_SHIFT 24
#define GX_CMODE1_REG_ID_MASK 0xff000000

#define PE_CONTROL_ID 67
#define PE_CONTROL_REG_ID_SIZE 8
#define PE_CONTROL_REG_ID_SHIFT 24
#define PE_CONTROL_REG_ID_MASK 0xff000000

#define PE_COPY_CMD_GAMMA_SIZE 2
#define PE_COPY_CMD_GAMMA_SHIFT 7
#define PE_COPY_CMD_GAMMA_MASK 0x00000180

#define GEN_MODE_REG_ID_SIZE 8
#define GEN_MODE_REG_ID_SHIFT 24
#define GEN_MODE_REG_ID_MASK 0xff000000

#define GX_VAT_REG_A_UNK_SIZE 1
#define GX_VAT_REG_A_UNK_SHIFT 30
#define GX_VAT_REG_A_UNK_MASK 0x40000000

#define GX_VAT_REG_B_UNK_SIZE 1
#define GX_VAT_REG_B_UNK_SHIFT 31
#define GX_VAT_REG_B_UNK_MASK 0x80000000

#define GX_OPCODE_INDEX_SIZE 3
#define GX_OPCODE_INDEX_SHIFT 0
#define GX_OPCODE_INDEX_MASK 0x00000007

#define GX_OPCODE_CMD_SHIFT 3

#define GX_OPCODE(index, cmd) ((((u32)(index)) << GX_OPCODE_INDEX_SHIFT) | (((u32)(cmd)) << GX_OPCODE_CMD_SHIFT))

#define GX_WRITE_RA_REG(reg)                                                                                           \
    {                                                                                                                  \
        GX_WRITE_U8(GX_OPCODE(1, 12));                                                                                 \
        GX_WRITE_U32((reg));                                                                                           \
    }

#define CP_STREAM_REG_INDEX_SIZE 4
#define CP_STREAM_REG_INDEX_SHIFT 0
#define CP_STREAM_REG_INDEX_MASK 0x0000000f

#define CP_STREAM_REG_ADDR_SIZE 4
#define CP_STREAM_REG_ADDR_SHIFT 4
#define CP_STREAM_REG_ADDR_MASK 0x000000f0

#define CP_STREAM_REG(index, addr)                                                                                     \
    ((((unsigned long)(index)) << CP_STREAM_REG_INDEX_SHIFT) | (((unsigned long)(addr)) << CP_STREAM_REG_ADDR_SHIFT))

#define GX_WRITE_CP_STRM_REG(addr, vtxfmt, data)                                                                       \
    {                                                                                                                  \
        GX_WRITE_U8(GX_OPCODE(0, 1));                                                                                  \
        GX_WRITE_U8(CP_STREAM_REG((vtxfmt), (addr)));                                                                  \
        GX_WRITE_U32((data));                                                                                          \
    }

#define PE_REFRESH_REG_ID_SIZE 8
#define PE_REFRESH_REG_ID_SHIFT 24
#define PE_REFRESH_REG_ID_MASK 0xff000000

#define PE_REFRESH_INTERVAL_SHIFT 0
#define PE_REFRESH_ENABLE_SHIFT 9
#define PE_REFRESH_TOTAL_SIZE 32
#define PE_REFRESH(interval, enable, rid)                                                                              \
    ((((u32)(interval)) << PE_REFRESH_INTERVAL_SHIFT) | (((u32)(enable)) << PE_REFRESH_ENABLE_SHIFT)                   \
     | (((u32)(rid)) << PE_REFRESH_REG_ID_SHIFT))

#define TX_REFRESH_REG_ID_SIZE 8
#define TX_REFRESH_REG_ID_SHIFT 24
#define TX_REFRESH_REG_ID_MASK 0xff000000

#define TX_REFRESH_INTERVAL_SHIFT 0
#define TX_REFRESH_ENABLE_SHIFT 10
#define TX_REFRESH_TOTAL_SIZE 32
#define TX_REFRESH(interval, enable, rid)                                                                              \
    ((((u32)(interval)) << TX_REFRESH_INTERVAL_SHIFT) | (((u32)(enable)) << TX_REFRESH_ENABLE_SHIFT)                   \
     | (((u32)(rid)) << TX_REFRESH_REG_ID_SHIFT))

struct GX
{
    /*0x00*/ u16 vNum;
    u16 unk2;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u8 filler14[0x1C-0x14];
    /*0x1C*/ u32 vatA[GX_MAX_VTXFMT];
    /*0x3C*/ u32 vatB[GX_MAX_VTXFMT];
    u8 filler5C[0x7C-0x5C];
    /*0x7C*/ u32 lpSize;
    u32 unk80;
    u32 unk84;
    u8 filler88[0xA8-0x88];
    /*0xA8*/ u32 unkA8[2];
    /*0xB0*/ u32 unkB0[2];
    /*0xB8*/ u32 suTs0[GX_MAX_TEXCOORD];
    /*0xD8*/ u32 suTs1[GX_MAX_TEXCOORD];
    /*0xF8*/ u32 suScis0;
    /*0xFC*/ u32 suScis1;
    /*0x100*/ u32 tref[GX_MAX_TEVSTAGE/2];
    /*0x120*/ u32 iref;
    /*0x124*/ u32 bpMask;
    u8 filler128[0x130-0x128];
    /*0x130*/ u32 tevc[GX_MAX_TEVSTAGE];
    /*0x170*/ u32 teva[GX_MAX_TEVSTAGE];
    /*0x1B0*/ u32 tevKsel[GX_MAX_TEVSTAGE/2];
    /*0x1D0*/ u32 cmode0;
    /*0x1D4*/ u32 cmode1;
    /*0x1D8*/ u32 zmode;
    /*0x1DC*/ u32 peCtrl;
    u8 filler1E0[0x1FC-0x1E0];
    /*0x1FC*/ u32 cpTex;
    u8 filler200[4];
    /*0x204*/ u32 genMode;
    /*0x208*/ GXTexRegion TexRegions[8];
    /*0x288*/ GXTexRegion TexRegionsCI[4];
    /*0x2C8*/ u32 nextTexRgn;
    /*0x2CC*/ u32 nextTexRgnCI;
    /*0x2D0*/ GXTlutRegion TlutRegions[20];
    u8 filler410[0x10];
    u32 unk420;
    f32 unk424;
    f32 unk428;
    f32 unk42C;
    f32 unk430;
    f32 unk434;
    f32 unk438;
    f32 unk43C;
    f32 unk440;
    f32 unk444;
    f32 unk448;
    f32 unk44C;
    f32 unk450;
    u8 unk454;
    f32 unk458;
    u8 filler45C[0x49C-0x45C];
    /*0x49C*/ u32 texmapId[GX_MAX_TEVSTAGE];
    /*0x4DC*/ u32 tcsManEnab;
    u8 filler4E0[0x4EC-0x4E0];
    /*0x4EC*/ u8 inDispList;
    /*0x4ED*/ u8 dlSaveContext;
    /*0x4EE*/ u8 dirtyVAT;
    /*0x4F0*/ u32 dirtyState;
};  // size = 0x4F4

extern struct GX *gx;

#define GX_WRITE_U8(ub)	    \
    GXWGFifo.u8 = (u8)(ub)

#define GX_WRITE_U16(us)   \
   GXWGFifo.u16 = (u16)(us)

#define GX_WRITE_U32(ui)   \
   GXWGFifo.u32 = (u32)(ui)

#define GX_WRITE_F32(f)	 	\
   GXWGFifo.f32 = (f32)(f);

#define INSERT_FIELD(reg, value, nbits, shift) \
do \
{ \
    (reg) = ((u32)(reg) & ~(((1<<(nbits))-1) << (shift))) | ((u32)(value) << (shift)); \
} while (0)

// GXFifoObj private fields
typedef struct
{
    void *base;  // base
    void *end;  // end
    u32 size;  // size
    u32 hiWaterMark;  // hiWaterMark
    u32 loWaterMark;  // loWaterMark
    void *readPtr;  // readPtr
    void *writePtr;  // writePtr
    s32 unk1C;
    u8 filler20[4];
} __GXFifoObj;

void __GXSetDirtyState(void);
void __GXSaveCPUFifoAux(__GXFifoObj *);
void __GXSendFlushPrim(void);
void __GXSetSUTexSize(void);
void __GXUpdateBPMask(void);
void __GXSetVCD(void);
void __GXSetVAT(void);
void __GXSetMatrixIndex(int);
void __GXSetRange(f32, f32);
void __GXFifoInit(void);
void __GXPEInit(void);
void __GXFlushTextureState(void);
