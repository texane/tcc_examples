#include <stdio.h>
#include "../common/dext.h"


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
  dext_handle_t dext;

  if (dext_init())
  {
    printf("dext_init error\n");
    goto on_error_0;
  }

  if (dext_open_c(&dext, s, NULL))
  {
    printf("dext_open_c error\n");
    goto on_error_1;
  }

  for (i = 0; i != n; ++i) x[i] = 0.0;
  dext_exec4(&dext, x, n, w, dt);
  for (i = 0; i != n; ++i) printf("%lf\n", x[i]);

  dext_close(&dext);
 on_error_1:
  dext_fini();
 on_error_0:
  return 0;
}
