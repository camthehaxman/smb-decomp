#ifndef _DOLPHIN_OSRESET_H_
#define _DOLPHIN_OSRESET_H_

#define OS_RESET_RESTART  0
#define OS_RESET_HOTRESET 1
#define OS_RESET_SHUTDOWN 2

#include <dolphin/types.h>

typedef BOOL (*OSResetFunction)(BOOL);

typedef struct OSResetFunctionInfo OSResetFunctionInfo;
struct OSResetFunctionInfo
{
    OSResetFunction func;
    u32 priority;
    OSResetFunctionInfo *next;
    OSResetFunctionInfo *prev;
};

void OSRegisterResetFunction(OSResetFunctionInfo *info);
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu);

#endif
