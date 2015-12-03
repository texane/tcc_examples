#include <stdio.h>
#include <stdint.h>
#include "libtcc.h"


static void bar(int x)
{
  printf("%s %d\n", __FUNCTION__, x);
}

static const char* s = "void fu(int x) { bar(x); }";

/* main */

int main(int ac, char** av)
{
  void (*f)(int);

  TCCState* tcc;
  int err = -1;

  tcc = tcc_new();
  if (tcc == NULL)
  {
    printf("creation error\n");
    goto on_error_0;
  }

  tcc_set_options(tcc, "-static -nostdlib -nostdinc");

  tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

  if (tcc_compile_string(tcc, s) == -1)
  {
    printf("compilation error\n");
    goto on_error_1;
  }

  tcc_add_symbol(tcc, "bar", bar);

  if (tcc_relocate(tcc, TCC_RELOCATE_AUTO) < 0)
  {
    printf("relocation error\n");
    goto on_error_1;
  }

  f = tcc_get_symbol(tcc, "fu");
  if (f == NULL)
  {
    printf("symbol not found\n");
    goto on_error_1;
  }

  f(42);
  f(43);

  err = 0;

 on_error_1:
  tcc_delete(tcc);
 on_error_0:
  if (err) printf("error\n");
  return err;
}
