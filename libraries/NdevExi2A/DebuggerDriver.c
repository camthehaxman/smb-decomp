#include <dolphin.h>

#ifdef __MWERKS__
volatile u32 __EXIRegs[0x40]  : 0xCC006800;
#else
#define __EXIRegs ((volatile u32 *)0xCC006800)
#endif

/*
static inline u32 DBGEXISync(void)
{
	while (__EXIRegs[13] & 1)
		;
	return 1;
}*/

static inline u32 DBGEXISync(void)
{
	u32 signal;
	do {
		signal = __EXIRegs[13];
	} while (signal & 1);
	
	return 1;
}

static BOOL DBGEXIImm(void* buffer, s32 byteCount, u32 write)
{
	u32 writeOutValue;
	int i;
	
	if (write) {
		writeOutValue = 0;
		for (i = 0; i < byteCount; i++)
		{
            u8* write_pos = &((u8*)buffer)[i];
			writeOutValue |= *write_pos << ((3 - i) << 3);
		}
		__EXIRegs[14] = writeOutValue;
	}
	
	__EXIRegs[13] = (1 | write << 2 | (byteCount - 1) << 4);
	DBGEXISync();

	if (!write) {
		u8* tempPointer = (u8*)buffer;
		writeOutValue    = __EXIRegs[14];
		for (i = 0; i < byteCount; i++)
		{
			*tempPointer++ = writeOutValue >> ((3 - i) << 3);
		}
	}

	return 1;
}

static inline BOOL DBGEXISelect(u32 v)
{
    u32 regs = __EXIRegs[10];
    regs &= 0x405;
    regs |= 0x80 | (v << 4);
	__EXIRegs[10] = regs;
	return 1;
}

static inline BOOL DBGEXIDeselect(void)
{
	__EXIRegs[10] &= 0x405;
	return 1;
}

static inline BOOL DBGEXIIsSet(u32 val) {
    return !(val != 0);
}

#define TO_BOOL(x) (((u32)__cntlzw((x)) >> 5))
static BOOL DBGReadMailbox(u32* p1)
{
	u32 v;
	u32 total = 0;
    u32 stack0, stack1, stack2;

    DBGEXISelect(4);

	v = 0x60000000;
	total |= !(DBGEXIImm(&v, 2, 1) != 0);
	DBGEXISync();

	total |= !(DBGEXIImm(p1, 4, 0) != 0);
	DBGEXISync();

    DBGEXIDeselect();
    total = TO_BOOL(total);
	return total;
}
