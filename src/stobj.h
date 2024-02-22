#ifndef _SRC_STOBJ_H_
#define _SRC_STOBJ_H_

struct PhysicsBall;

// Stage object (stobj) type
enum
{
    SOT_BUMPER,
    SOT_JAMABAR,
    SOT_GOALTAPE,
    SOT_GOALBAG,
    SOT_GOALBAG_EXMASTER,
    SOT_MF_PNL_BUMPER,
    SOT_MF_PNL_ELECTRAP,
    SOT_MF_BULLET_TEST,
    SOT_MF_BOX,
    SOT_BUMPER_BGSPECIAL,
    SOT_NAMEENT_BTN,
};

enum
{
    STOBJ_FLAG_TANGIBLE     = (1 << 1),  // Ball can collide with the object
    STOBJ_FLAG_LOOKPOINT    = (1 << 3),  // The character may look at the object
    STOBJ_FLAG_ROTATION_UNK = (1 << 4),  // Something related to rotation
};

struct Stobj
{
    /*0x00*/ s16 index;  // index into g_stobjInfo array
    /*0x02*/ s16 uid;  // unique ID
    /*0x04*/ s16 type;
    u8 filler6[2];
    /*0x08*/ u32 flags;
    /*0x0C*/ s16 state;
    /*0x0E*/ s16 counter;
    /*0x10*/ Vec u_model_origin;
    /*0x1C*/ Vec pos;  // position (in world space)
    /*0x28*/ Vec prevPos;  // previous position (in world space)
    /*0x34*/ float boundSphereRadius;
    /*0x38*/ void (*coliFunc)(struct Stobj *, struct PhysicsBall *);
    Vec unk3C;  // scale?
    float unk48;
    float unk4C;
    float unk50;
    /*0x54*/ struct GMAModel *model;  // sometimes also a NlModel
    /*0x58*/ Vec localPos;  // position in anim group space
    Vec unk64;
    /*0x70*/ s16 rotX;
    /*0x72*/ s16 rotY;
    /*0x74*/ s16 rotZ;
    s16 unk76;
    s16 unk78;
    u8 filler7A[0x7C-0x7A];
    Vec unk7C;
    s16 unk88;
    s16 unk8A;
    s16 unk8C;
    u8 filler8E[2];
    /*0x90*/ Vec lookPoint;
    /*0x9C*/ float lookPointPrio;
    /*0xA0*/ s8 animGroupId;
    /*0xA4*/ void *extraData;  // pointer to a GoalTape or GoalBag struct
    Vec unkA8;
    /*0xB4*/ Vec offsetPos;  // offset from its regular position (for jamabars and goalbags)
    /*0xC0*/ Vec offsetVel;  // offset velocity (for jamabars and goalbags)
};  // size = 0xCC

struct Struct8028C0B0
{
    float unk0[4];
    u32 unk10;
    struct GMAModel *unk14[4];
};

extern struct Stobj g_stobjInfo[MAX_STOBJS];
extern struct Struct8028C0B0 lbl_8028C0B0;

void ev_stobj_init(void);
void ev_stobj_main(void);
void ev_stobj_dest(void);
void stobj_draw(void);
s16 spawn_stobj(struct Stobj *);

void u_spawn_goal_stobjs(struct StageAnimGroup *, int);
void stobj_goaltape_init(struct Stobj *);
void stobj_goaltape_main(struct Stobj *);
void stobj_goaltape_draw(struct Stobj *);
void stobj_goaltape_coli(struct Stobj *, struct PhysicsBall *);
void stobj_goaltape_destroy(struct Stobj *);
void stobj_goaltape_debug(struct Stobj *);
void stobj_goalbag_init(struct Stobj *);
void stobj_goalbag_main(struct Stobj *);
void stobj_goalbag_draw(struct Stobj *);
void stobj_goalbag_coli(struct Stobj *, struct PhysicsBall *);
void stobj_goalbag_destroy(struct Stobj *);
void stobj_goalbag_debug(struct Stobj *);
void stobj_goalbag_exmaster_init(struct Stobj *);
void stobj_goalbag_exmaster_main(struct Stobj *);
void stobj_goalbag_exmaster_draw(struct Stobj *);
void stobj_goalbag_exmaster_coli(struct Stobj *, struct PhysicsBall *);
void stobj_goalbag_exmaster_destroy(struct Stobj *);
void stobj_goalbag_exmaster_debug(struct Stobj *);
void break_goaltape(int, struct PhysicsBall *);
void relink_goaltape(int);

#endif
