#include <string.h>

#include <dolphin.h>

#include "global.h"
#include "event.h"
#include "mode.h"
#include "vibration.h"

struct Vibration
{
    u8 state;
    u16 timer;
};

static struct Vibration vibrationStartInfo[4];
static struct Vibration vibrationStopInfo[4];
static u8 contEnableMask = 0xF;

void ev_vibration_init(void)
{
    memset(vibrationStartInfo, 0, sizeof(vibrationStartInfo));
    memset(vibrationStopInfo, 0, sizeof(vibrationStopInfo));
}

void ev_vibration_main(void)
{
    int i;
    struct Vibration *stop;
    struct Vibration *start;

    if (gameMode == MD_ADV)
        return;

    // start vibration
    start = vibrationStartInfo;
    stop = vibrationStopInfo;
    for (i = 0; i < 4; i++, stop++, start++)
    {
        if (start->state == VIBRATION_STATE_0)
            continue;
        stop->state = start->state;
        if (start->state == VIBRATION_STATE_4 || start->state == VIBRATION_STATE_3)
            stop->timer = 0;
        else if (start->state == VIBRATION_STATE_1 || start->state == VIBRATION_STATE_2)
        {
            PADControlMotor(i, PAD_MOTOR_RUMBLE);
            stop->timer = start->timer;
        }
        start->state = VIBRATION_STATE_0;
    }

    // stop vibration
    stop = vibrationStopInfo;
    for (i = 0; i < 4; i++, stop++)
    {
        if (stop->state == VIBRATION_STATE_4)
        {
            PADControlMotor(i, PAD_MOTOR_STOP);
            stop->state = VIBRATION_STATE_0;
        }
        else if (stop->state == VIBRATION_STATE_3)
        {
            PADControlMotor(i, PAD_MOTOR_STOP_HARD);
            stop->state = VIBRATION_STATE_0;
        }
        else
        {
            if (stop->timer != 0)
                stop->timer--;
            if (stop->timer == 1)
            {
                PADControlMotor(i, (stop->state == VIBRATION_STATE_1) ? PAD_MOTOR_STOP_HARD : PAD_MOTOR_STOP);
                stop->state = VIBRATION_STATE_0;
                stop->timer = 0;
            }
        }
    }
}

void ev_vibration_dest(void)
{
    u32 commandArray[4];
    int i;

    for (i = 0; i < 4; i++)
        commandArray[i] = PAD_MOTOR_STOP_HARD;
    PADControlAllMotors(commandArray);
    memset(vibrationStartInfo, 0, sizeof(vibrationStartInfo));
    memset(vibrationStopInfo, 0, sizeof(vibrationStopInfo));
}

void vibration_control(int contId, enum VibrationState state, int duration)
{
    struct Vibration *vib;

    if (eventInfo[EVENT_VIBRATION].state != EV_STATE_RUNNING)
        return;
    if (state == VIBRATION_STATE_0)
        return;
    if (contId < 0 || contId >= 4)
        return;
    if (duration > 180)
        return;
    if (!(contEnableMask & (1 << contId)))
        return;
    if (state == VIBRATION_STATE_3 || state == VIBRATION_STATE_4)
        duration = 0;
    vib = &vibrationStartInfo[contId];
    switch (vib->state)
    {
    case VIBRATION_STATE_4:
        if (state == VIBRATION_STATE_3)
            vib->state = state;
        break;
    case VIBRATION_STATE_1:
        switch (state)
        {
        case VIBRATION_STATE_3:
        case VIBRATION_STATE_4:
            vib->state = state;
            break;
        case VIBRATION_STATE_1:
            if (vib->timer < duration)
            {
                vib->state = state;
                vib->timer = duration;
            }
            break;
        }
        break;
    case VIBRATION_STATE_2:
        switch (state)
        {
        case VIBRATION_STATE_3:
        case VIBRATION_STATE_4:
            vib->state = state;
            break;
        case VIBRATION_STATE_2:
            if (vib->timer < duration)
                break;
            // fall through
        case VIBRATION_STATE_1:
            vib->state = state;
            vib->timer = duration;
            break;
        }
        break;
    case VIBRATION_STATE_0:
        vib->state = state;
        vib->timer = duration;
        break;
    }
}

void vibration_set_cont_enable_mask(u8 contMask)
{
    contEnableMask = contMask;
}

u8 vibration_get_cont_enable_mask(void)
{
    return contEnableMask;
}
