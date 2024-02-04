#ifndef _PPCINTRINSIC_H_
#define _PPCINTRINSIC_H_

// Implement MetroWerks PowerPC intrinsics for other compilers

#ifndef __MWERKS__
static inline unsigned int __cntlzw(unsigned int n)
{
#ifdef __PPC__
    unsigned int ret;
    asm("cntlzw %0, %1" : "=r"(ret) : "r"(n));
    return ret;
#else
    int i;
    for (i = 0; i < 31; i++)
    {
        if ((n >> (31 - i)) & 1)
            break;
    }
    return i;
#endif
}

static inline unsigned int __lwbrx(void *ptr, unsigned int offset)
{
#ifdef __PPC__
    unsigned int ret;
    asm("lwbrx %0, %1, %2" : "=r"(ret) : "r"(ptr), "r"(offset));
    return ret;
#else
    unsigned int val = *(unsigned int *)((unsigned char *)ptr + offset);
    //return ((val & 0xFF) << 24) | (((val >> 8) & 0xFF) << 16) | (((val >> 16) & 0xFF) << 8) | ((val >> 24) & 0xFF);
    return val;
#endif
}

static inline float __frsqrte(float n)
{
#ifdef __PPC__
    float ret;
    asm("frsqrte %0, %1" : "=f"(ret) : "f"(n));
    return ret;
#else
    extern float sqrtf(float);
    return 1.0f / sqrtf(n);
#endif
}
#endif

#endif
