s32 AmcEXISelect();
s32 AmcEXIDeselect();
s32 AmcEXIImm(void *buf, s32 len, u32 type, EXICallback func);
void AmcEXISync(void);
void *AmcEXISetExiCallback(EXICallback func);
void AmcEXIEnableInterrupts(void);
void AmcEXIInit(void);
