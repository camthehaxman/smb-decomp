#ifndef _SRC_WINDOW_H_
#define _SRC_WINDOW_H_

extern u32 debugFlags;
extern u32 dipSwitches;
extern u32 lbl_802F1ED8;
extern u16 lbl_802F1ED4;
extern u16 lbl_802F1ED2;
extern u16 lbl_802F1ED0;
extern float lbl_802F1ECC;
extern float lbl_802F1EC8;
extern float lbl_802F1EC4;
extern float lbl_802F1EC0;
extern float lbl_802F1EBC;
extern float lbl_802F1EB8;
extern s32 lbl_802F1EB4;
extern s32 lbl_802F1EB0;
extern u32 lbl_802F1EAC;
extern u32 lbl_802F1EA8;
extern char *lbl_802F1EA4;
extern u16 lbl_802F1EA2;
extern u16 lbl_802F1EA0;
extern u32 lbl_802F1E9C;
extern u32 lbl_802F1E98;
extern u32 lbl_802F1E94;
extern u32 lbl_802F1E90;
extern u32 lbl_802F1E8C;
extern u32 lbl_802F1E88;
extern u32 lbl_802F1E84;
extern u32 lbl_802F1E80;
extern u32 lbl_802F1E7C;
extern float lbl_802F1E78;
extern float lbl_802F1E74;
extern float lbl_802F1E70;
extern float lbl_802F1E6C;
extern u16 lbl_802F1E6A;
extern u16 lbl_802F1E68;
extern u16 lbl_802F1E66;
extern u16 lbl_802F1E64;
extern float lbl_802F1E60;
extern float lbl_802F1E5C;

void window_draw(void);
void window_init(void);
void window_main(void);
void window_set_cursor_pos(int, int);
void window_move_cursor(int, int);
// ? func_8002FCAC();
int window_printf(int, char *, ...);
void window_set_text_color(int);
void u_set_window_text(int, const char *);
void u_clear_buffers_2_and_5(void);
// ? clear_buffer_region();
int window_printf_2(const char *, ...);
void u_debug_print();
int u_printf_if_debug(int, char *, ...);

#endif
