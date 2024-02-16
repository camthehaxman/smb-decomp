#ifndef _SRC_MEMCARD_H_
#define _SRC_MEMCARD_H_

enum
{
    /*0*/ MC_MODE_LOAD_GAMEDATA_0,
    /*1*/ MC_MODE_SAVE_GAMEDATA_1,
    /*2*/ MC_MODE_LOAD_GAMEDATA_2,
    /*3*/ MC_MODE_SAVE_GAMEDATA_3,
    /*4*/ MC_MODE_LOAD_REPLAY,
    /*5*/ MC_MODE_SAVE_REPLAY,
    /*6*/ MC_MODE_LIST_REPLAY,
    /*7*/ MC_MODE_DELETE_REPLAY,
};

void memcard_set_mode(int);
int memcard_is_write_in_progress(void);
u8 func_8009F4C4(void);
void memcard_init(void);
void ev_memcard_init(void);
void ev_memcard_main(void);
void ev_memcard_dest(void);
void memcard_draw_ui(void);
void func_800A4CEC(void);

#endif
