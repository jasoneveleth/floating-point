/*
 * The purpose of this file is to check if dividing a float by 1 will change
 * the value. 
 *
 * VERDICT: f/1. is a nop
 */

#include <stdio.h>
#include <stdint.h>

static inline uint32_t
f2u(float a)
{
    union {float f; uint32_t u;} mem = {.f = a};
    return mem.u;
}

static inline float
u2f(uint32_t a)
{
    union {float f; uint32_t u;} mem = {.u = a};
    return mem.f;
}

// static inline uint64_t
// d2z(double a)
// {
//     union {double d; uint64_t u;} mem = {.d = a};
//     return mem.u;
// }

// static inline double
// z2d(uint64_t a)
// {
//     union {double d; uint64_t u;} mem = {.u = a};
//     return mem.d;
// }

int main(){
    uint32_t u = 0;
    uint32_t end = 0xFFFFFFFF;
    float one = 1;

    do {
        if (f2u(u2f(u) / one) != u)
            if ((u & 0x7F800000) != 0x7F800000) // don't check NaNs and Infs
                printf("%x\n", u);
        u += 1;
    } while (u != end);

    return 0;
}

