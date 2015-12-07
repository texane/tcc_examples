#include <stdio.h>
#include "dext.h"


#define L(__s) __s "\n"

static const char* s[6] =
{
  L("void main(double* x, unsigned int n, double w, double dt)"),
  L("{"),
  L("  unsigned int i;"),
  L("  double t = 0.0;"),
  L("  for (i = 0; i != n; ++i, t += dt) x[i] = sin(t * w);"),
  L("}")
};

int main(int ac, char** av)
{
  static const unsigned int n = 1000;
  double x[n];
  static const double w = 100 * 2 * 3.1415;
  static const double dt = 0.0001;
  unsigned int i;
  dext_handle_t* dext;

  if (dext_init())
  {
    printf("dext_init error\n");
    goto on_error_0;
  }

  dext = dext_create("sin");
  if (dext == NULL)
  {
    printf("dext_create error\n");
    goto on_error_1;
  }

  dext_clear(dext);

  for (i = 0; i != 6; ++i)
  {
    if (dext_add(dext, s[i]))
    {
      printf("dext_add error\n");
      goto on_error_2;
    }
  }

  if (dext_compile(dext, NULL))
  {
    printf("dext_compile error\n");
    goto on_error_2;
  }

  for (i = 0; i != n; ++i) x[i] = 0.0;
  dext_exec4(dext, x, n, w, dt);
  for (i = 0; i != n; ++i) printf("%lf\n", x[i]);

 on_error_2:
  dext_destroy(dext);
 on_error_1:
  dext_fini();
 on_error_0:
  return 0;
}
