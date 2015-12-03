#include <stdio.h>
#include <stdint.h>
#include "libtcc.h"
#include "../common/common.h"


/* timing */

#include <sys/time.h>

static inline uint64_t get_ticks(void)
{
  struct timeval tm;
  gettimeofday(&tm, NULL);
  return tm.tv_sec * 1000000 + tm.tv_usec;
}

static inline uint64_t sub_ticks(uint64_t a, uint64_t b)
{
  if (a > b) return UINT64_MAX - a + b;
  return b - a;
}


/* computation kernel */

#define L(__s) __s "\n"

static const char* s =							\
L("typedef double real_t;")						\
									\
L("void matmul")							\
L("(real_t* y, real_t* a, real_t* x, real_t* b, unsigned int n)")	\
L("{")									\
L("  unsigned int i;")							\
L("  unsigned int j;")							\
L("  for (i = 0; i != n; ++i)")						\
L("  {")								\
L("    y[i] = 0;")							\
L("    for (j = 0; j != n; ++j) y[i] += a[i * n + j] * x[j];")		\
L("    y[i] += b[i];")							\
L("  }")								\
L("}");


typedef double real_t;

static void matmul_pre
(real_t* y, real_t* a, real_t* x, real_t* b, unsigned int n)
{
  const unsigned int nn = n * n;

  unsigned int i;

  for (i = 0; i != n; ++i) y[i] = 0.0;
  for (i = 0; i != nn; ++i) a[i] = 0.5;
  for (i = 0; i != n; ++i) x[i] = (real_t)i;
  for (i = 0; i != n; ++i) b[i] = (real_t)i + 1000.42;
}

void matmul							
(real_t* y, real_t* a, real_t* x, real_t* b, unsigned int n)	
{									
  unsigned int i;							
  unsigned int j;							
  for (i = 0; i != n; ++i)						
  {								
    y[i] = 0;							
    for (j = 0; j != n; ++j) y[i] += a[i * n + j] * x[j];		
    y[i] += b[i];							
  }								
}									

__attribute__((unused))
static void matmul_print
(const real_t* y, unsigned int n)
{
  unsigned int i;
  for (i = 0; i != n; ++i) printf(" %lf", y[i]);
  printf("\n");
}


/* main */

int main(int ac, char** av)
{
  static const size_t n = 8;

  real_t y[n];
  real_t a[n * n];
  real_t x[n];
  real_t b[n];

  void (*f)(real_t*, real_t*, real_t*, real_t*, unsigned int);

  uint64_t ticks[2];
  double us[2];
  size_t i;

  TCCState* tcc;
  int err = -1;

  tcc = tcc_new_with_opts();
  if (tcc == NULL)
  {
    printf("creation error\n");
    goto on_error_0;
  }

  if (tcc_compile_relocate(tcc, s) == -1)
  {
    printf("compilation error\n");
    goto on_error_1;
  }

  f = tcc_get_symbol(tcc, "matmul");
  if (f == NULL)
  {
    printf("symbol not found\n");
    goto on_error_1;
  }

  ticks[0] = get_ticks();
  for (i = 0; i != 100000; ++i)
  {
    matmul_pre(y, a, x, b, n);
    f(y, a, x, b, n);
  }
  ticks[1] = get_ticks();
  us[0] = (double)sub_ticks(ticks[0], ticks[1]);

  f = matmul;
  ticks[0] = get_ticks();
  for (i = 0; i != 100000; ++i)
  {
    matmul_pre(y, a, x, b, n);
    f(y, a, x, b, n);
  }
  ticks[1] = get_ticks();
  us[1] = (double)sub_ticks(ticks[0], ticks[1]);

  printf("times: %lfus, %lfus\n", us[0], us[1]);

  /* matmul_print(y, n); */

  err = 0;

 on_error_1:
  tcc_delete(tcc);
 on_error_0:
  if (err) printf("error\n");
  return err;
}
