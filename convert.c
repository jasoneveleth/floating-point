#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <readline/readline.h>

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

int main(int argc, char *argv[])
{
    while (true) {
        // note EOF with text is treated as newline if line is nonempty
        char *line = readline("float> ");
        if (line == NULL) break; // recieved EOF on empty line
        if (*line == '\0') { // recieved empty line
            free(line);
            continue; 
        }

        double input;
        int num_matches = sscanf(line, "%lg", &input);
        assert(num_matches != EOF); // we know line isn't empty
        if (num_matches == 0) {
            fprintf(stderr, "repl: '%s' didn't match input\n", line);
        } else {
            printf("%llx\n", d2z(input));
        }
        free(line);
    }
    return 0;
}

