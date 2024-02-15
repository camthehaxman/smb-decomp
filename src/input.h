#ifndef _SRC_INPUT_H_
#define _SRC_INPUT_H_

#include <dolphin/types.h>
#include <dolphin/pad.h>

struct ControllerInfo
{
    PADStatus held;      // buttons that are down this frame
    PADStatus prevHeld;  // buttons that were down on the previous frame
    PADStatus pressed;   // buttons that went down on this frame
    PADStatus released;  // buttons that went up on this frame
    PADStatus repeat;    // buttons that repeat due to being held
};

struct AnalogButtonInfo
{
    u16 held;      // buttons that are down this frame
    u16 prevHeld;  // buttons that were down on the previous frame
    u16 pressed;   // buttons that went down on this frame. However, due to an oversight, this field
                   // is not correct and is the same as pressed.
    u16 released;  // buttons that went up on this frame
    u16 repeat;    // buttons that repeat due to being held
};

#define CONTROLLER_SOMETHING(idx, btn) \
( \
    (controllerInfo[idx].repeat.button & (btn)) \
 || (analogInputs[idx].repeat & btn) \
 || ( \
        ((controllerInfo[idx].held.button & (btn)) || (analogInputs[idx].held & (btn))) \
     && (analogInputs[idx].held & PAD_BUTTON_B) \
    ) \
)

#define ANY_CONTROLLER_PRESSED(btn) \
( \
    (controllerInfo[0].pressed.button & btn) \
 || (controllerInfo[1].pressed.button & btn) \
 || (controllerInfo[2].pressed.button & btn) \
 || (controllerInfo[3].pressed.button & btn) \
)

extern struct ControllerInfo controllerInfo[4];
extern struct ControllerInfo lbl_801F3C60[4];

#define ANALOG_STICK_LEFT    (1 << 0)
#define ANALOG_STICK_RIGHT   (1 << 1)
#define ANALOG_STICK_DOWN    (1 << 2)
#define ANALOG_STICK_UP      (1 << 3)
#define ANALOG_CSTICK_LEFT   (1 << 4)
#define ANALOG_CSTICK_RIGHT  (1 << 5)
#define ANALOG_CSTICK_DOWN   (1 << 6)
#define ANALOG_CSTICK_UP     (1 << 7)
#define ANALOG_TRIGGER_LEFT  (1 << 8)
#define ANALOG_TRIGGER_RIGHT (1 << 9)

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
