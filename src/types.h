#ifndef _SRC_TYPES_H_
#define _SRC_TYPES_H_

#include <dolphin/types.h>
#include <dolphin/gx/GXStruct.h>
#include <dolphin/gx/GXEnum.h>
#include <dolphin/gx/GXFifo.h>
#include <dolphin/mtx.h>

typedef struct
{
    f32 x, y;
} Vec2d, *Vec2dPtr, Point2d, *Point2dPtr;

// DIP switches
enum
{
    DIP_DEBUG        = 1 << 0,
    DIP_DISP         = 1 << 1,
    DIP_STCOLI       = 1 << 2,
    DIP_TRIANGLE     = 1 << 3,
    DIP_BALL_TGT     = 1 << 4,
    DIP_JOINT_OFF    = 1 << 5,
    DIP_APE_COLOR    = 1 << 6,
    DIP_TIME_STOP    = 1 << 7,
    DIP_KARAKUCHI    = 1 << 8,
    DIP_NAMEENTRY    = 1 << 9,
    DIP_SHADOW_TEST  = 1 << 10,
    DIP_FALL_DISP    = 1 << 11,
    DIP_TEST_CAM     = 1 << 12,
    DIP_NO_INTR      = 1 << 13,
    DIP_CAPTURE      = 1 << 14,
    DIP_OLD_BALL     = 1 << 15,
    DIP_NO_KARI      = 1 << 16,
    DIP_PLAY_STG_ALL = 1 << 17,
    DIP_PLAY_PNT_X10 = 1 << 18,
    DIP_SARU_0       = 1 << 19,
    DIP_SWITCH20     = 1 << 20,
    DIP_SWITCH21     = 1 << 21,
    DIP_SWITCH22     = 1 << 22,
    DIP_SWITCH23     = 1 << 23,
    DIP_SWITCH24     = 1 << 24,
    DIP_SWITCH25     = 1 << 25,
    DIP_SWITCH26     = 1 << 26,
    DIP_SWITCH27     = 1 << 27,
    DIP_SWITCH28     = 1 << 28,
    DIP_SWITCH29     = 1 << 29,
    DIP_SWITCH30     = 1 << 30,
    DIP_SWITCH31     = 1 << 31,
};

struct Color3f { float r, g, b; };

// sprite alignment
// When setting the position of a sprite (x and y fields), this determines which corner or edge
// of the sprite lies at that point.
enum Alignment
{
    ALIGN_LT,
    ALIGN_LC,
    ALIGN_LB,
    ALIGN_CT,
    ALIGN_CC,
    ALIGN_CB,
    ALIGN_RT,
    ALIGN_RC,
    ALIGN_RB,
    ALIGN_PIC,
};

// avdisp.c
struct GMAShape;
struct GMATevLayer;
struct DrawShapeDeferredNode;
struct GMATevLayer;
struct TevStageInfo;

struct TPLTextureHeader
{
    /*0x00*/ u32 format;
    /*0x04*/ u32 imageOffset;
    /*0x08*/ u16 width;
    /*0x0A*/ u16 height;
    /*0x0C*/ u16 unkC;
};  // size = 0x10

struct TPL
{
    /*0x00*/ u32 numTextures;
    /*0x04*/ struct TPLTextureHeader *texHeaders;
    /*0x08*/ u8 *fileData;  // raw file data
    /*0x0C*/ GXTexObj *texObjs;  // only used by bitmap.c? avdisp.c seems to think TPL struct is only 12 bytes.
};
// maybe bitmap.c has a different struct that "contains" TPL?

struct GFXBufferInfo
{
    /*0x00*/ void *currFrameBuf;
    /*0x04*/ void *frameBufs[2];
    /*0x0C*/ u32 fbNum;
    /*0x10*/ GXFifoObj *fifos[2];
};

struct Camera;
struct Sprite;
struct FontParams;
struct GMA;
struct TPL;
struct Ape;
struct Ball;

// Represents a joint's channel (x, y, z, rotX, rotY, rotZ) whose value changes during the animation
struct MotionChannel
{
    u8 keyframeCount;
    u8 currKeyframe;
    u16 *times;
    u8 *valueCounts;
    float *values;
};  // size = 0x10

enum
{
    JOINT_FLAG_HAS_ROTATION_MTX = 8,
    JOINT_FLAG_HAS_OTHER_ROTATION_MTX = 0x40,
};

struct AnimJoint
{
    u32 flags;  // 0 here marks the end of the list
    Vec unk4;  // some optional vector (depending on flags)
    Vec unk10;  // some optional vector (depending on flags)
    /*0x1C */Mtx otherRotateMtx;
    /*0x4C*/ u32 childCount;
    /*0x50*/ const u8 *childIndexes;  // indexes of joints that are attached to this one
    /*0x54*/ struct MotionChannel channels[6];  // x, y, z, rotX, rotY, rotZ
    u8 fillerB4[0x168-0xB4];
    Mtx unk168;
    float unk198;
    float unk19C;
    /*0x1A0*/ s32 parentIdx;  // index of this joint's parent, or -1 if this is the root
    Vec unk1A4;
    Quaternion unk1B0;
    /*0x1C0*/ Point3d u_motionPos;  // value interpolated from motion channels
    Point3d unk1CC;  // possibly u_motionPos, but transformed by its parent joints?
    /*0x1D8*/ Mtx u_motRotation;  // rotation matrix interpolated from motion channels
    /*0x208*/ Mtx transformMtx;  // final transform matrix?
};  // size = 0x238

struct Struct8003699C_child_sub
{
    u32 unk0;
	u16 unk4;
    u16 unk6;
	u16 unk8;
	u16 unkA;
	u16 unkC;
	float unk10;
	float unk14;
    u32 unk18;
    float unk1C;
    float unk20;
    s32 unk24;
	u16 unk28;
    u16 unk2A;
    float unk2C;
    u8 filler30[4];
    struct AnimJoint *unk34;
    /*0x38*/ struct AnimJoint joints[29];
};  // size = 0x4090

struct Struct8003699C_child_child
{
	u16 unk0;
	u16 unk2;
	u32 unk4;
	u32 unk8;
};  // size = 0xC

struct ApeAnimationThing
{
    u32 unk0;
    u8 filler4[0x28-0x4];
    u16 unk28;
    u16 unk2A;
    u16 unk2C;
    s16 unk2E;
    u8 filler30[2];
    /*0x32*/ u16 u_animId;  // index into structs in motdat file (1-based), possibly the animation ID?
    /*0x34*/ u16 u_nextAnimId;  // next animId when current one finishes, or 0xFFFF if none?
    u16 unk36;
    /*0x38*/ u16 u_currKeyframe;  // current animation keyframe (1-based)
    /*0x3A*/ u16 u_keyframeCount;  // once u_currKeyframe reaches this value, a new anim is loaded?
    float u_someDeltaTime;   // How much the anim progresses each tick. anim speed?
    float u_timeInKeyframe;  // Time spent in current keyframe. When this reaches 1, it goes to the next keyframe. Used to interpolate between keyframes
    u8 filler44[0x54-0x44];
    Mtx unk54;
    struct Struct8003699C_child_sub unk84;
    struct Struct8003699C_child_sub unk4114;
    const struct Struct8003699C_child_child *unk81A4;
    /*0x81A8*/ struct AnimJoint joints[29];
};

struct MotRotation
{
    float rotX;
    float rotY;
    float rotZ;
};

struct SomeMotInfoStruct  // something motion related from motInfo
{
    float unk0;  // something related to time or speed?
    s32 unk4;
    u32 unk8;
    u32 unkC;
    /*0x10*/ s32 animId;
    s32 unk14;
    /*0x18*/ float u_maybeSpeed;  // possibly controls animation speed?
    u32 unk1C;  // some flags
};  // size = 0x20

enum
{
    APE_FLAG_INVISIBLE = (1 << 5),
    APE_FLAG_BLINK = (1 << 7),  // starts eye blink when this is set
    APE_FLAG_14 = (1 << 14),
    APE_FLAG_TRANSLUCENT = (1 << 20)
};

struct Ape
{
    struct ApeAnimationThing *unk0;
    struct ApeAnimationThing *unk4;
    /*0x008*/ float animTimerCurr;
    /*0x00C*/ float animTimerMax;
    /*0x010*/ s32 charaId;
    /*0x014*/ u32 flags;
    s32 unk18;
    struct SomeMotInfoStruct *unk1C;
    struct SomeMotInfoStruct *unk20;
    s32 unk24;
    s32 unk28;
    struct Skeleton *skel;  // skeleton?
    Vec pos;  // position?
    Vec unk3C;
    Vec unk48;
    s32 unk54;  // teeter edge? used for something else in the result submode
    /*0x58*/ float modelScale;  // model scale?
    u32 threadId;
    Quaternion unk60;  // orientation?
    u32 unk70;
    u32 unk74;
    u8 filler78[0x90-0x78];
    /*0x90*/ s32 lod;  // level of detail (from 0 to 3) of the character model, with 0 being the most detailed and 1 being the least detailed
    /*0x94*/ u32 u_bodyPartCount;  // number of elements in u_bodyParts
    /*0x98*/ struct BodyPart *u_bodyParts;  // some array of structs related to body parts (only used in mot_ape.c)
    u32 unk9C;
    // Sometimes treated as a Vec, sometimes treated as a Quaternion
    Vec unkA0;
    float unkAC;
    u32 unkB0;
    /*0x0B4*/ int colorId;
    void *unkB8;
    void *unkBC;
    /*0xC0*/ s8 ballId;
    u8 unkC1;
    s16 unkC2;
};  // size = 0xC4

struct AnimGroupInfo
{
    /*0x00*/ Point3d pos;
    /*0x0C*/ Point3d prevPos;
    /*0x18*/ S16Vec rot;
    /*0x1E*/ S16Vec prevRot;
    /*0x24*/ Mtx transform;     // Transform from anim group space to world space
    /*0x54*/ Mtx prevTransform; // Previous frame transform from animGroup space to world space
};

struct UnkStruct27
{
    u8 filler0[4];
    struct GMAShape *shape;
    struct GMATevLayer *modelTevs;
    u8 fillerC[0x38-0xC];
};

typedef u32 (*Func802F20F0)(struct UnkStruct27 *);
typedef void (*CameraCallback)(struct Camera *, struct Ball *);
typedef void (*BallCallback)(struct Ball *);

struct NlModel;
struct NlObj;

struct PolyShadowUnit
{
    Vec unk0;
    S16Vec unkC;
    Vec unk14;
    float unk20;
    float unk24;
    struct GMAModel *unk28;
    GXColor unk2C;
    float unk30;
    float unk34;
};

typedef void (*Struct80206DEC_Func)(void);

struct Struct80206DEC
{
    s32 unk0;
    float u_stageTimer;
    Struct80206DEC_Func unk8;
    u32 unkC;
    float unk10[3];
    u32 unk1C;
    u8 filler20[0x5C-0x20];
};

struct Stage;

// motload

struct MotDatJoint
{
    u8 jointIdx;
    u16 chanFlags;  // specifies which channels are present in the animation
};  // size = 0x4

// struct containing all transformation values for all joints in an animation
struct MotDat
{
    u16 u_keyframeCount;  // TODO: how does this differ from keyframeCounts?
    struct MotDatJoint *jointInfo; // ptr to array of structs
    u8 *keyframeCounts;  // number of keyframes per channel
    u16 *times;  // times for each keyframe
    u8 *valueCounts;  // number of values for each keyframe
    float *values;
};  // size = 0x18

struct ChildJointList
{
    u32 count;
    const u8 *children;
};

// structure in motskl.bin
// Defines (initial?) position and rotation of joints in a model?
struct Skeleton
{
    void *unused0;
    /*0x04*/ struct ChildJointList *childLists;
    /*0x08*/ struct MotRotation *rotations;
    Vec *unkC;  // optional vectors (depends on joint flags)
    Vec *unk10;  // optional vectors (depends on joint flags)
    /*0x14*/ char *name;  // skeleton name?
};  // size = 0x18

struct Struct80034B50_child2_child
{
    void *unused0;
    u8 filler4[0x18-0x4];
};

struct MotSkeletonEntry2
{
    void *unused0;
    struct Struct80034B50_child2_child *unk4[3];
    s32 unk10[3];
};

struct SkeletonFileData
{
    struct Skeleton *skeletons;
    u32 skeletonsCount;
    struct MotSkeletonEntry2 *unk8;  // not used?
    u32 unkC;  // not used?
};

struct MotInfo2_child
{
    u8 filler0[0x180];
    struct SomeMotInfoStruct unk180;
};

// motinfo.bin starts with 32 of these structs
// something else is at 0x2000 in the file
// something else is at 0x80000 in the file
struct MotInfo
{
    /*0x00*/ char skelName[24];
    /*0x10*/ char modelName[24];
    struct MotInfo2_child *unk30[16];
    u32 unk70[16];
    u8 *unkB0;
};  // size = 0xB4

struct JointPositionSomething
{
    u16 unk0;
    u16 jointIdx;
};  // size = 4

struct JointRotationSomething
{
    u16 filler0;
    u16 jointIdx;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
};  // size = 0x1C

struct CoordsS8
{
    s8 x;
    s8 y;
};

struct Struct8020A348_child
{
    u32 flags;
    struct GMAModel *model;  // GMAModel
    float unk8;
};  // size = 0xC

struct Struct8020A348
{
    struct Struct8020A348_child *unk0;
    s32 unk4;
};

struct StageSelection
{
    s32 difficulty;
    s32 levelNum;
};

struct NlVtxTypeB;
struct NlVtxTypeA;

// Part of the stage whose vertices are manipulated by functions
struct DynamicStagePart
{
    void *modelName;
    struct NlModel *origModel;  // original model
    void (*posNrmTexFunc)(struct NlVtxTypeB *);
    void (*posColorTexFunc)(struct NlVtxTypeA *);
    u32 (*raycastDownFunc)(Point3d *rayOrigin, Point3d *outHitPos, Vec *outHitNormal);
    struct NlModel *tempModel;  // modified copy of the model
};

struct Struct802F1B4C
{
    u32 unk6C;
};

struct FogInfo
{
    s8 unk0;
    u8 filler1[3];
    float unk4;
    float unk8;
    u8 r, g, b;
    s8 enabled;
};

struct Struct80209488;

struct OrdTblNode;

struct Struct802F1B3C
{
    Mtx matrices[5];
    struct OrdTblNode *entries;
    s32 maxEntries;
    float depthOffset;
    float minDepth;
    float maxDepth;
    struct OrdTblNode *lastEntry;  // the last node in 'entries'
    struct OrdTblNode *firstEntry;  // the first node in 'entries'
    float depthRange;
};

struct Sphere
{
    Point3d pos;
    float radius;
};

struct NlDispList;

struct Struct801EED88
{
    u8 unk0;
    u8 unk1[6];
    u32 unk8;
    u32 unkC;
};  // size = 0x10

struct ScoreRecord
{
    char initials[4];
    s32 score;
    u32 unk8;
    u8 floorNum;
    s8 unkD;  // 0 = normal, 1 = extra, 2 = master
    s8 unkE;
    u8 fillerF[1];
};

struct Struct802C6220
{
    u32 size;
    struct ScoreRecord records[3*5];
};

struct MemcardGameData
{
    /*0x5844*/ u8 filler0[0x4];
    /*0x5848*/ s8 padLimit[32][2];
    /*0x5888*/ u8 unk44;
    /*0x5889*/ u8 unk45;
    /*0x588A*/ u8 unk46;
    /*0x588B*/ u8 unk47;
    /*0x588C*/ u8 unk48;
    /*0x588D*/ u8 unk49;
    /*0x588E*/ u8 unk4A;
    /*0x588F*/ u8 unk4B;
    /*0x5890*/ u8 bgmVolume;
    /*0x5891*/ u8 seVolume;
    /*0x5892*/ u8 unk4E;
    /*0x5893*/ u8 unk4F;
    /*0x5894*/ u8 unk50;
    /*0x5895*/ u8 unk51;
    /*0x5896*/ u8 unk52;
    /*0x5897*/ u8 unk53;
    /*0x5898*/ u8 unk54;
    /*0x5899*/ u8 unk55;
    /*0x589A*/ u8 filler56[2];
    /*0x589C*/ s16 unk58;
    /*0x589E*/ s16 unk5A;
    /*0x58A0*/ s16 unk5C;
    /*0x58A2*/ s16 unk5E;
    /*0x58A4*/ s16 unk60;
    /*0x58A6*/ s16 unk62;
    /*0x58A8*/ s16 unk64;
    /*0x58AA*/ s16 unk66;
    /*0x58AC*/ s16 unk68;
    /*0x58AE*/ s16 unk6A;
    /*0x58B0*/ u8 filler6C[4];
    /*0x58B4*/ u32 unk70;
    /*0x58B8*/ u32 unk74;
    /*0x58BC*/ u32 unk78;
    /*0x58C0*/ u32 unk7C;
    /*0x58C4*/ u32 unk80;
    /*0x58C8*/ u32 unk84;
    /*0x58CC*/ s16 unk88;
    /*0x58CE*/ s16 unk8A;
    /*0x58D0*/ u32 unk8C;
    /*0x58D4*/ u32 visitedFloors[4];
    /*0x58E4*/ u32 unkA0;
    /*0x58E8*/ u8 unkA4[6];
    /*0x58EE*/ u8 unkAA;
    /*0x58EF*/ s8 unkAB;
    /*0x58F0*/ u8 unkAC;
    /*0x58F1*/ u8 unkAD;
    /*0x58F2*/ u8 unkAE;
    /*0x58F3*/ s8 unkAF;
    /*0x58F4*/ u8 unkB0[0x1FC];  // size = 0x1FC
    /*0x5AF0*/ u32 unk2AC;
    /*0x5AF4*/ u8 unk2B0;
    /*0x5AF5*/ u8 unk2B1;
    /*0x5AF6*/ u8 unk2B2;
    /*0x5AF7*/ u8 unk2B3;
    /*0x5AF8*/ u8 unk2B4;
    /*0x5AF9*/ u8 unk2B5;
    /*0x5AFA*/ u8 unk2B6;
    /*0x5AFB*/ s8 unk2B7;
    /*0x5AFC*/ u32 unk2B8;
    /*0x5B00*/ s8 unk2BC;
    /*0x5B01*/ u8 unk2BD;
    /*0x5B02*/ u8 filler2BE[1];
    /*0x5B03*/ u8 unk2BF;
    /*0x5B04*/ u32 totalPlayPoints;
    /*0x5B08*/ u32 recordPlayPoints;
    /*0x5B0C*/ struct Struct802C6220 unk2C8;
    /*0x5C00*/ u32 unk3BC;
};  // size = 0x3C0

struct MemcardContents
{
    /*0x0000*/ u16 crc;
    /*0x0002*/ u16 version;
    /*0x0004*/ u8 bannerAndIcon[0x5800];
    /*0x5804*/ char comment[32];
    /*0x5824*/ char title[32];
    /*0x5844*/ struct MemcardGameData gameData;
};

struct Keyframe
{
    s32 easeType;
    float timeSeconds;
    float value;
    float tangentIn;
    float tangentOut;
};

struct StageAnimGroup;
struct StageBanana;

struct Item;

struct ModelLOD
{
    s32 modelId;
    float distance;
};

struct Struct80089CBC
{
    s32 unk0;
    s32 unk4;
    u32 unk8;
    u32 unkC;
    s32 unk10;
    s32 unk14;
    u8 filler18[0x20-0x18];
};  // size = 0x20

struct BodyPartNameInfo
{
    char *unk0;
    char *names[4];
    u32 unk14;
    u32 filler18[6];  // unused padding
    s32 u_someLodRelatedIndexes[4];  // some LOD related stuff, set at runtime
};

// contains some runtime state for a body part
struct BodyPart
{
    u32 type;
    float unk4;
    s32 unk8;
    struct Struct80089CBC *unkC;  // ptr to something in motinfo
    float unk10;
    /*0x14*/ s32 u_someLodRelatedIndexes[4];
};  // size = 0x24

// Defines an ape body part (such as head, ears, hands, but not the main body) that can be animated
struct BodyPartDesc
{
    s16 modelId;  // model ID of the body part in the GMA
    s16 jointIdx;  // index of joint that this part is attached to
    Vec offset;  // position offset
    void (*draw)(struct Ape *, struct BodyPartDesc *, struct BodyPart *);
    char *name;
    u8 filler18[0x20-0x18];  // unused?
};

// Defines graphics files for body parts
struct ApeGfxFileInfo
{
    char *basename;  // base name (without suffix) of the .gma.lz or .tpl.lz file in the /test/ape directory
    struct BodyPartDesc *bodyPartInfo[4];  // body part info (the last two elements of this array are always NULL)
    /*0x14*/ s16 partCounts[4];  // lengths of each bodyPartInfo array
    s16 mainBodyModelIDs[2];  // model IDs (low poly, high poly) in GMA for the main body
    u8 unused20[4];  // unused padding?
};  // size = 0x24

struct TevStageInfo
{
    s32 tevStage;
    GXTexCoordID texCoordId;
    s32 unk8;
    /*0x0C*/ GXTexMapID texMapId;
    s32 tevIndStage;
    s32 unk14;
    u32 unk18;
    s32 unk1C;
    u32 unk20;
    GXTexCoordID u_texCoordId2;
    GXTexMapID u_someTexmapId2;
};

struct GCMMatState_Unit
{
    s32 unk0;
    void *unk4;
    void *unk8;
    struct TevStageInfo unkC;
};

typedef void (*EnvMapFunc)(struct GCMMatState_Unit *);

struct BgLightInfo
{
	float unk0;
    struct Color3f ambient;
    float unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;

    // Global directional light ("infinite" light)
    struct Color3f infLightColor;
	s16 infLightRotX;
	s16 infLightRotY;

    s8 **bgLightGroups;
};

struct Light;

struct Struct802F1BE8
{
    u32 unk0;
    u8 unk4;
};

struct Struct802F1BF4
{
    u8 unk0;
    u8 unk1;
    u8 filler2[2];
    u32 unk4;
};

struct Struct802F1BFC
{
    u8 unk0;
    u8 filler1[3];
    u32 unk4;
};

struct Struct802F1C04
{
    s16 unk0;
    s16 unk2;
    u32 unk4;
};

struct Struct802F1C10
{
    u8 unk0;
    u8 unk1;
    s8 unk2;
    u8 unk3;
    u8 unk4[4];
};

struct GBilLightGroup
{
    char *name;
    s8 u_bgLightGroupId;
};

enum
{
    PAUSEMENU_CONT_HOW_EXIT,
    PAUSEMENU_CONT_VIEW_HOW_EXIT,
    PAUSEMENU_CONT_RETRY_VIEW_HOW_SELECT_EXIT,
    PAUSEMENU_CONT_RETRY_HOW_EXIT,
    PAUSEMENU_CONT_GUIDE_HOW_EXIT,
};

struct Stobj;

struct Struct801EEDA8
{
    u8 filler0[0x100];
    float unk100;
    u8 filler104[0x118-0x104];
};

struct Struct8009DBB0
{
    int unk0;
    Vec unk4;
    Vec unk10;
};

struct Struct802C5D60
{
    u32 unk0;
    u32 unk4;
    u8 filler8[0x10-0x8];
    u32 unk10;
    u8 filler14[0x20-0x14];
    char unk20[0x400];
};

struct Struct80094870
{
    u32 unk0;
    u32 unk4;
    u16 unk8;
};

struct Struct802099E8
{
    u32 *unk0;
    void *unk4;
    s32 unk8;
};

struct Struct800993A8_sub
{
    s32 unk0;
    Vec unk4;
    Vec unk10;
    struct GMAModel *unk1C;
    float unk20;
    float unk24;
    s16 unk28;
    s16 unk2A;
    float unk2C;
};  // size = 0x30

struct Struct800993A8
{
    GXTexObj unk0;
    u32 unk20;
    void *unk24;
    u16 unk28;
    u16 unk2A;
    GXTexObj unk2C;
    void *unk4C;
    Mtx unk50;
    u8 filler80[4];
    u8 unk84;
    u8 filler85[0x88-0x85];
    float unk88;
    s32 unk8C;
    struct Struct800993A8_sub unk90[256];
};

struct Struct8008669C_sub
{
    Vec unk0;
    Vec unkC;
};

struct FacialAnimationSomething
{
    Vec unk0;
    Vec unkC;
    struct Struct8008669C_sub unk18[3];
    s16 unk60[4];  // offsets into a NlModel
};

struct Effect;
struct ScoreRecord;

#endif
