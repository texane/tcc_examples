#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libtcc.h"
#include "../common/common.h"


#define L(__s) __s "\n"

static const char* s =						\
L("void main(double* x, unsigned int n, double w, double dt)")	\
L("{")								\
L("  unsigned int i;")						\
L("  double t = 0.0;")						\
L("  for (i = 0; i != n; ++i, t += dt) x[i] = sin(t * w);")	\
L("}");

int main(int ac, char** av)
{
  static const unsigned int n = 1000;
  double x[n];
  static const double w = 100 * 2 * 3.1415;
  static const double dt = 0.0001;
  unsigned int i;

  TCCState* tcc;
  void (*f)(double*, unsigned int, double, double);

  tcc = tcc_new_with_opts();
  if (tcc == NULL)
  {
    printf("creation error\n");
    goto on_error_0;
  }

  if (tcc_compile_relocate(tcc, s))
  {
    printf("full compile error\n");
    goto on_error_1;
  }

  f = tcc_get_symbol(tcc, "main");
  if (f == NULL)
  {
    printf("symbol not found\n");
    goto on_error_1;
  }

  for (i = 0; i != n; ++i) x[i] = 0.0;
  f(x, n, w, dt);
  for (i = 0; i != n; ++i) printf("%lf\n", x[i]);

 on_error_1:
  tcc_delete(tcc);
 on_error_0:
  return 0;
}
