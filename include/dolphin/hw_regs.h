#ifndef _DOLPHIN_HW_REGS_H_
#define _DOLPHIN_HW_REGS_H_

#ifdef __MWERKS__
volatile u16 __DSPRegs[] : 0xCC005000;
volatile u16 __VIRegs[59] : 0xCC002000;
#else
#define __DSPRegs ((volatile u16 *)0xCC005000)
#define __VIRegs ((volatile u16 *)0xCC002000)
#endif

#endif
