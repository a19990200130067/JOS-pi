#ifndef JOS_INC_ARM_H
#define JOS_INC_ARM_H

#include <inc/types.h>

static __inline uint32_t rttbr0(void) __attribute__((always_inline));
static __inline void lttbr0(uint32_t val) __attribute__((always_inline));

static __inline uint32_t rc1(void) __attribute__((always_inline));
static __inline void lc1(uint32_t val) __attribute__((always_inline));

static __inline void invlpg(void *addr) __attribute__((always_inline));

static __inline uint32_t rcpsr(void) __attribute__((always_inline));
static __inline void lcpsr(uint32_t val) __attribute__((always_inline));

static __inline uint32_t rspsr(void) __attribute__((always_inline));
static __inline void lspsr(uint32_t val) __attribute__((always_inline));

static __inline uint32_t
rc1(void)
{
    uint32_t val;
    __asm __volatile("mrc p15, 0, %[result], c1, c0, 0" : [result] "=r" (val) :);
    return val;
}

static __inline void
lc1(uint32_t val)
{
    __asm __volatile("mcr p15, 0, %[value], c1, c0, 0" : : [value] "r" (val));
}

static __inline uint32_t
rttbr0(void)
{
    uint32_t val;
    __asm __volatile("mrc p15, 0, %[value], c2, c0, 0" : [value] "=r" (val) :);
    return val;
}

static __inline void
lttbr0(uint32_t val)
{
    __asm __volatile("mcr p15, 0, %[value], c2, c0, 0" : : [value] "r" (val));
}

static __inline void
invlpg(void *addr)
{
    __asm __volatile("mcr p15, 0, %[address], c8, c7, 1" : : [address] "r" (addr) : "memory");
}

static __inline uint32_t
rcpsr(void)
{
    uint32_t val;
    __asm __volatile("mrs %[result], cpsr" : [result] "=r" (val) :);
    return val;
}

static __inline void
lcpsr(uint32_t val)
{
    __asm __volatile("msr cpsr, %[value]" : : [value] "r" (val));
}

static __inline uint32_t
rspsr(void)
{
    uint32_t val;
    __asm __volatile("mrs %[result], spsr" : [result] "=r" (val) :);
    return val;
}

static __inline void
lspsr(uint32_t val)
{
    __asm __volatile("msr spsr, %[value]" : : [value] "r" (val));
}

static __inline uint32_t
read_pc(void)
{
    uint32_t val;
    __asm __volatile("movs %[result], pc" : [result] "=r" (val) :);
    return val;
}

#endif
