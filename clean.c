/*
 * Notice that clean_shift is only 3.21391s vs 2.1979s for clean_add on large
 * numbers of cleans, thus, it is a comparable function (+50% slower). Assuming
 * you know what the constant needs to be. If you don't: 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef void (*func)(void);

static inline double
z2d(const uint64_t a)
{
    union {double d; uint64_t z;} mem = {.z = a};
    return mem.d;
}

static inline uint64_t
d2z(const double a)
{
    union {double d; uint64_t z;} mem = {.d = a};
    return mem.z;
}

double
clean_shift(double a)
{
    uint64_t z = d2z(a);
    // there are 52 bits of significance in a double
    // flush bottom half
    uint64_t cleaned = (z >> 26) << 26;
    return z2d(cleaned);
}

double
clean_add(double a)
{
    return (a + (a * 1e8)) - (a * 1e8);
    // return (a + 1e8) - 1e8;
}

#define NUM 1000000000

void
clean_add_test()
{
    double a = 0;
    for (int i = 0; i < NUM; i++) {
        clean_add(a + i);
    }
}

void
clean_shift_test()
{
    double a = 0;
    for (int i = 0; i < NUM; i++) {
        clean_shift(a + 1);
    }
}

double
measure_time(func f)
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    f();
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    return cpu_time_used;
}

int
main()
{
    double dirty = 3890429301.000432432;
    printf("%llx\t\t%.16g\n", d2z(dirty), dirty);
    printf("%llx\t\t%.16g\n", d2z(clean_shift(dirty)), clean_shift(dirty));
    printf("%llx\t\t%.16g\n", d2z(clean_add(dirty)), clean_add(dirty));

    // double t1 = measure_time(clean_add_test);
    // double t2 = measure_time(clean_shift_test);
    // printf("%g %g\n", t1, t2);
    return 0;
}
