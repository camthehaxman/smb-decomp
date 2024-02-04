#ifndef _DOLPHIN_VI_H_
#define _DOLPHIN_VI_H_

#include <dolphin/gx/GXStruct.h>
#include <dolphin/vifuncs.h>
#include <dolphin/vitypes.h>

// These are specific to the PC version
#ifdef TARGET_PC
void VISetWindowTitle(const char *title);
void VIShowErrorMessage(const char *message);
#endif

#endif
