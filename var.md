% standard deviation / variance

# R

source: 
r-source/src/library/stats/src/cov.c
r-source/src/library/stats/R/sd.R
r-source/src/library/stats/R/cor.R
r-source/src/include/Defn.h

macros expanded, and the paths we don't take deleted. We
disregard `NA_REAL` stuff. The values below determined which if
statements we went into.

```
if matrix:
    n = nrows
    ncx = ncols
else // vector
    n = length
    ncx = 1
everything = TRUE;
empty_err = FALSE;
sd_0 = FALSE;

cor = FALSE
kendall = FALSE

typedef enum { FALSE = 0, TRUE } Rboolean;
```

Note that xm and ind are recently allocated vectors of size ncx,
in our case, ncx = 1 (since we're doing a vector). Since we're
calling it with a vector, I deleted outer loops, to get it down
to this:

```c
int R_finite(double); // unknown source

typedef int R_xlen_t;

static void cov_na_1(int n, int ncx, double *x, double *xm, double *ans) {
    long double sum, tmp, xxm;
    R_xlen_t i, j, k, n1 = -1;

    sum = 0.;
    for (k = 0; k < n; k++)
        sum += x[k];
    tmp = sum / n;
    if (R_finite((double)tmp)) {
        sum = 0.;
        for (k = 0; k < n; k++)
            sum += (x[k] - tmp);
        tmp = tmp + sum / n;
    }
    xm[0] = (double)tmp;

    n1 = n - 1;
    xxm = xm[0];
    sum = 0.;
    for (k = 0; k < n; k++)
        sum += (long double)(x[k] - xxm) * (x[k] - xxm);
    ans[0] = (double)(sum / n1);
}
```

# GSL

gsl/statistics/variance_source.c

macros expanded

```c
static double
variance(const double data[], const size_t stride, const size_t n, const double mean)
{
  /* takes a dataset and finds the variance */

  long double variance = 0 ;

  size_t i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++) {
      const long double delta = (data[i * stride] - mean);
      variance += (delta * delta - variance) / (i + 1);
  }

  return variance;
}
```

