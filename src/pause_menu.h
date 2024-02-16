#ifndef _SRC_PAUSE_MENU_H_
#define _SRC_PAUSE_MENU_H_

struct PauseMenuState
{
    s32 unk0;
    u32 unk4;
    /*0x08*/ s32 selection;
    /*0x0C*/ s32 itemCount;
    /*0x10*/ s32 menuType;
    /*0x14*/ s8 padId;  // controller that pressed start
    /*0x15*/ s8 playerId;  // player who paused the game?
    s16 unk16;
};

extern struct PauseMenuState pauseMenuState;

void u_menu_input_game_notdebug(void);

#endif
