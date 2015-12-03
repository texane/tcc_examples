#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "libtcc.h"



TCCState* tcc_new_with_opts(void)
{
  TCCState* tcc;

  tcc = tcc_new();
  if (tcc == NULL) return NULL;

  tcc_set_options(tcc, "-static -nostdlib -nostdinc");
  tcc_set_output_type(tcc, TCC_OUTPUT_MEMORY);

  return tcc;
}


static void on_tcc_error(void* err, const char* msg)
{
  printf("%s\n", msg);
  *(int*)err = -1;
}

int tcc_compile_relocate(TCCState* tcc, const char* s)
{
  const struct
  {
    const char* name;
    const char* decl;
    void* addr;
  } syms[] =
  {
    /* math symbols */
    { "sin", "double sin(double);", sin },
    { "cos", "double cos(double);", cos },
    { "sqrt", "double sqrt(double);", sqrt },
    { "fabs", "double fabs(double);", fabs }
  };

  const size_t nsym = sizeof(syms) / sizeof(syms[0]);

  size_t i;
  size_t new_len;
  char* new_s;
  char* new_p;
  int err = 0;

  new_len = 0;
  for (i = 0; i != nsym; ++i) new_len += strlen(syms[i].decl);
  new_len += strlen(s);

  new_s = malloc(new_len + 1);
  if (new_s == NULL)
  {
    err = -1;
    goto on_error_0;
  }
  
  new_p = new_s;
  for (i = 0; i != nsym; ++i)
  {
    strcpy(new_p, syms[i].decl);
    new_p += strlen(syms[i].decl);
    tcc_add_symbol(tcc, syms[i].name, syms[i].addr);
  }
  strcpy(new_p, s);

  tcc_set_error_func(tcc, &err, on_tcc_error);
  if ((tcc_compile_string(tcc, new_s) == -1) || (err))
  {
    printf("compilation error\n");
    err = -1;
    goto on_error_1;
  }

  if (tcc_relocate(tcc, TCC_RELOCATE_AUTO) < 0)
  {
    printf("relocation error\n");
    err = -1;
    goto on_error_1;
  }

 on_error_1:
  free(new_s);
 on_error_0:
  return err;
}
