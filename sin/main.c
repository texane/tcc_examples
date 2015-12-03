#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "libtcc.h"


#define L(__s) __s "\n"

static const char* s =							\
L("double sin(double);")						\
L("void gen_wave(double* x, unsigned int n, double w, double dt)")	\
L("{")									\
L("  unsigned int i;")							\
L("  double t = 0.0;")							\
L("  for (i = 0; i != n; ++i, t += dt) x[i] = sin(t * w);")		\
L("}");


/* main */

static void on_tcc_error(void* err, const char* msg)
{
  *(int*)err = -1;
}

int main(int ac, char** av)
{
  static const unsigned int n = 1000;
  double x[n];
  static const double w = 100 * 2 * 3.1415;
  static const double dt = 0.0001;
  unsigned int i;

  TCCState* tcc;
  void (*f)(double*, unsigned int, double, double);
  int err;

  tcc = tcc_new();
  if (tcc == NULL)
  {
    printf("creation error\n");
    goto on_error_0;
  }

  tcc_set_options(tcc, "-static -nostdlib -nostdinc");
  tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

  err = 0;
  tcc_set_error_func(tcc, &err, on_tcc_error);
  if ((tcc_compile_string(tcc, s) == -1) || (err))
  {
    printf("compilation error\n");
    goto on_error_1;
  }

  tcc_add_symbol(tcc, "sin", sin);

  if (tcc_relocate(tcc, TCC_RELOCATE_AUTO) < 0)
  {
    printf("relocation error\n");
    goto on_error_1;
  }

  f = tcc_get_symbol(tcc, "gen_wave");
  if (f == NULL)
  {
    printf("symbol not found\n");
    goto on_error_1;
  }

  for (i = 0; i != n; ++i) x[i] = 0.0;
  f(x, n, w, dt);
  for (i = 0; i != n; ++i) printf("%lf\n", x[i]);

  err = 0;

 on_error_1:
  tcc_delete(tcc);
 on_error_0:
  if (err) printf("error\n");
  return err;
}
