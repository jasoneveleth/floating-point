#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>


#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"
#define WARNING(fmt, ...) fprintf(stderr, "%s:%d: " RED "WARNING: " RESET fmt, __FILE__, __LINE__, __VA_ARGS__)

struct vec
{
    size_t length;
    size_t stride;
    double *data;
    int is_owner;
};

struct matrix
{
    size_t len1;
    size_t len2;
    size_t physlen;
    double *data;
    int is_owner;
};

uint64_t
d2z(const double a)
{
    union {double d; uint64_t z;} mem = {.d = a};
    return mem.z;
}

double
z2d(const uint64_t a)
{
    union {double d; uint64_t z;} mem = {.z = a};
    return mem.d;
}

void *
safe_calloc(size_t num, size_t size)
{
    void *ret = calloc(num, size);
    if (ret == NULL) {
        perror("calloc");
        abort();
    }
    return ret;
}

void *
safe_realloc(void *ptr, size_t size)
{
    void *ret = realloc(ptr, size);
    if (ret == NULL) {
        perror("realloc");
        abort();
    }
    return ret;
}

FILE *
safe_fopen(const char * restrict path, const char * restrict mode)
{
    FILE *ret = fopen(path, mode);
    if (ret == NULL) {
        fprintf(stderr, "%s: ", path);
        perror("");
        abort();
    }
    return ret;
}

char *
read_line(FILE *fp)
{
    size_t allocd = 2;
    char *line = safe_calloc(allocd, sizeof(char));
    size_t len = 0;
    int c = getc(fp);
    while (c != EOF && c != '\n') {
        if (len >= allocd) {
            allocd *= 2;
            line = safe_realloc(line, allocd);
        }
        line[len++] = (char)c;
        c = getc(fp);
    }

    // handle possible error
    if (ferror(fp)) {
        perror("getc");
        abort();
    }

    // handle ending
    if (len > 0 && line[len - 1] == '\r') { // handle crlf delimiter
        line[len - 1] = '\0';
    }
    line[len] = '\0';
    return line;
}

static double
safe_strtod(const char *const token)
{
    if (token == NULL) {
        WARNING("no token recieved, setting ele to %g\n", 0.);
        return 0;
    }
    char *endptr;
    double ele = strtod(token, &endptr);
    if (token == endptr) {
        WARNING("unknown token\n\t\"%s\"\n\tsetting ele to 0.0\n", token);
    }
    return ele;
}

static char *
find_token(char **resumer, const char *const sep)
{
    if ((*resumer) == NULL) {
        return NULL;
    }

    char *ret = &((*resumer)[strspn(*resumer, sep)]);
    char *next = &ret[strcspn(ret, sep)];
    if (next[0] == '\0') {
        *resumer = NULL;
    } else {
        next[0] = '\0';
        *resumer = &next[1];
    }
    return ret;
}

struct vec
vec_read(FILE *file, const char *const argformat)
{
    const char *const format = (argformat == NULL) ? "%lg" : argformat;
    struct vec v;
    size_t len_of_vec = 0;
    size_t allocd = 1;
    double *data = safe_calloc(allocd, sizeof(double));

    while (!feof(file)) {
        char *line = read_line(file);

        double ele = 0;
        int num_matches = sscanf(line, format, &ele);
        if (num_matches == EOF) { // empty line
            free(line);
            continue;
        }
        if (num_matches == 0) { // invalid input
            WARNING("vec_read: line didn't match\n\t`%s`\n\tsetting ele to 0.0\n", line);
        }
        if (len_of_vec == allocd) {
            allocd = len_of_vec * 2;
            data = safe_realloc(data, allocd * sizeof(double));
        }
        data[len_of_vec++] = ele;
        free(line);
    }
    v.data = data;
    v.is_owner = true;
    v.length = len_of_vec;
    v.stride = 1;
    return v;
}

void
vec_set(struct vec v, size_t i, double a)
{
    assert(i < v.length && "trying to set an index outside bounds in vector");
    v.data[v.stride * i] = a;
}

double
vec_get(const struct vec v, size_t i)
{
    assert(i < v.length && "trying to get an index outside bounds in vector");
    return v.data[v.stride * i];
}


double
vec_bad_sum(struct vec v)
{
    double sum = 0;
    for (size_t i = 0; i < v.length; i++) {
        sum = sum + vec_get(v, i);
    }
    return sum;
}

/* kahan summation */
double
vec_sum(const struct vec v)
{
    double sum = 0;
    double old_low_bits = 0;
    for (size_t i = 0; i < v.length; i++) {
        double y = vec_get(v, i) - old_low_bits; // = high - low - oldlow = newhigh - newlow
        double t = sum + y;                      // = sum + newhigh
        old_low_bits = (t - sum) - y;            // = (newhigh) - (newhigh - newlow)
        sum = t;
    }
    return sum;
}

void
vec_square(struct vec v) 
{
    for (size_t i = 0; i < v.length; i++) {
        const double ele = vec_get(v, i);
        vec_set(v, i, ele * ele);
    }
}

double
vec_dot(struct vec v, struct vec u)
{
    double sum = 0.;
    for (size_t i = 0; i < v.length; i++) {
        sum = fma(vec_get(v, i), vec_get(u, i), sum);
    }
    return sum;
}

int
main()
{
    {
        FILE *file = fopen("kahan.txt", "r");
        struct vec vec1 = vec_read(file, NULL);
        fclose(file);

        double vec1_fma = vec_dot(vec1, vec1);

        vec_square(vec1);
        double vec1_bad_sum = vec_bad_sum(vec1);
        double vec1_kahan = vec_sum(vec1);

        printf("bad sum %llx\n", d2z(vec1_bad_sum));
        printf("kahan   %llx\n", d2z(vec1_kahan));
        printf("fma     %llx\n", d2z(vec1_fma));

        printf("\n");
    }

    // ------------------------------------

    {
        FILE *file = fopen("fma.txt", "r");
        struct vec vec2 = vec_read(file, NULL);
        fclose(file);

        double vec2_fma = vec_dot(vec2, vec2);

        vec_square(vec2);
        double vec2_bad_sum = vec_bad_sum(vec2);
        double vec2_kahan = vec_sum(vec2);

        printf("bad sum %llx\n", d2z(vec2_bad_sum));
        printf("kahan   %llx\n", d2z(vec2_kahan));
        printf("fma     %llx\n", d2z(vec2_fma));
    }
}

