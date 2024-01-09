#include <stdio.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

/*
 * The purpose of this file is to find the machine precision of IEEE doubles
 */

typedef uint64_t int64;
typedef double float64;

#define AQUA printf("\033[36m")
#define RED printf("\033[31m")
#define GREEN printf("\033[32m")
#define RESET printf("\033[0m")
#define YELLOW printf("\033[33m")

void
dp(const float64 d)
{
    printf("%.*e\n", DECIMAL_DIG, d);
}

void
underline()
{
    printf("^^"); // for the {x.}xxxxxxxe-xx part
    for (int i = 0; i < DECIMAL_DIG; i++)
        printf("^"); // for the x.{xxxxxxx}e-xx part
    printf("^^^^\n"); // for the x.xxxxxx{e-xx} part
}

void
emph_dp(const float64 d, const char *msg)
{
    printf("%s", msg);
    YELLOW;
    dp(d);
    for (size_t i = 0; i < strlen(msg); i++)
        printf(" ");
    underline();
    RESET;
}

int 
main() 
{
    const float64 one = 1.0;

    // next double (f64) in IEEE 754
    const int64 bytes = 0x3FF0000000000001;
    const float64 next_after_one = *(float64 *)&bytes;
    emph_dp(next_after_one, "one plus small: ");
    printf("\tnext floating point after 1.0\n\n");

    // diff from next double and one
    emph_dp(next_after_one - one, "machine eps: ");
    printf("\tdiff btwn next floating point and 1.0\n\n");

    // calc alt eps
    float64 eps = next_after_one - one;
    printf("calculating alt eps...");
    fflush(stdout);
    while (one != (one + eps)) {
        eps *= 0.9999999995;
    }
    printf("\r");
    emph_dp(eps, "alt machine eps: ");
    printf("\twhat to add to 1.0 to round it up\n\n");

    return 0;
}
