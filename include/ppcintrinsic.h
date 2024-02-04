#ifndef _PPCINTRINSIC_H_
#define _PPCINTRINSIC_H_

// Implement MetroWerks PowerPC intrinsics for other compilers

#ifndef __MWERKS__
static inline unsigned int __cntlzw(unsigned int n)
{
    unsigned int ret;
    asm("cntlzw %0, %1" : "=r"(ret) : "r"(n));
    return ret;
}

static inline unsigned int __lwbrx(void *ptr, unsigned int offset)
{
    unsigned int ret;
    asm("lwbrx %0, %1, %2" : "=r"(ret) : "r"(ptr), "r"(offset));
    return ret;
}

static inline float __frsqrte(float n)
{
    float ret;
    asm("frsqrte %0, %1" : "=f"(ret) : "f"(n));
    return ret;
}
#endif

#endif
