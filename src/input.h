#ifndef _SRC_INPUT_H_
#define _SRC_INPUT_H_

#include <dolphin/types.h>
#include <dolphin/pad.h>

struct ControllerInfo
{
    PADStatus unk0[5];  // func_80025640 only matches (unrolled loops) if this is an array.
};

/*
 * [0] = held buttons
 * [1] = prev held buttons
 * [2] = new buttons
 * [3] = released buttons
 * [4] = button repeats
 */

#define CONTROLLER_SOMETHING(idx, btn) \
( \
    (controllerInfo[idx].unk0[4].button & (btn)) \
 || (analogInputs[idx].repeat & btn) \
 || ( \
        ((controllerInfo[idx].unk0[0].button & (btn)) || (analogInputs[idx].held & (btn))) \
     && (analogInputs[idx].held & PAD_BUTTON_B) \
    ) \
)

#define ANY_CONTROLLER_PRESSED(btn) \
( \
    (controllerInfo[0].unk0[2].button & btn) \
 || (controllerInfo[1].unk0[2].button & btn) \
 || (controllerInfo[2].unk0[2].button & btn) \
 || (controllerInfo[3].unk0[2].button & btn) \
)

extern struct ControllerInfo controllerInfo[4];
extern struct ControllerInfo lbl_801F3C60[4];

#define ANALOG_STICK_LEFT (1 << 0)
#define ANALOG_STICK_RIGHT (1 << 1)
#define ANALOG_STICK_DOWN (1 << 2)
#define ANALOG_STICK_UP (1 << 3)
#define ANALOG_CSTICK_LEFT (1 << 4)
#define ANALOG_CSTICK_RIGHT (1 << 5)
#define ANALOG_CSTICK_DOWN (1 << 6)
#define ANALOG_CSTICK_UP (1 << 7)
#define ANALOG_TRIGGER_LEFT (1 << 8)
#define ANALOG_TRIGGER_RIGHT (1 << 9)

struct AnalogButtonInfo
{
    u16 held;
    u16 prevHeld;
    u16 pressed;
    u16 released;  // hmm not actually sure about that one?
    u16 repeat;
};

extern struct AnalogButtonInfo analogInputs[4];
//extern struct Struct801F3D88 g_currPlayerButtons;
extern u16 g_currPlayerButtons[6];
//extern struct Struct801F3D94 g_currPlayerAnalogButtons;
extern u16 g_currPlayerAnalogButtons[6];

void input_init(void);
void input_main(void);
void func_80025158(PADStatus *);
void get_analog_presses(void);
void get_key_repeats(void);
void func_80025640(void);
void handle_reset_input(PADStatus *);
void func_80025B1C(struct CoordsS8 *, s8 *);
void save_input_data(struct MemcardContents *);
void load_input_data(const struct MemcardContents *);

#endif
