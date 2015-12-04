/* dance extension */
/* user provided codes, typically used as computation kernels */


#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "dext.h"
#include "libtcc.h"


int dext_init(void)
{
  return 0;
}


void dext_fini(void)
{
}


static void on_tcc_error(void* err, const char* msg)
{
  /* skip this warning */
#define SOFTFP_WARN "<string>:1: warning: soft float ABI"
  if (strncmp(SOFTFP_WARN, msg, sizeof(SOFTFP_WARN) - 1) == 0) return ;
  *(int*)err = -1;
}


int dext_open_c(dext_handle_t* dext, const char* s, const dext_sym_t* app_syms)
{
  const dext_sym_t default_syms[] =
  {
    /* TODO: dance specific symbols */

    /* math symbols */
    { "sin", "double sin(double);", sin },
    { "cos", "double cos(double);", cos },
    { "sqrt", "double sqrt(double);", sqrt },
    { "fabs", "double fabs(double);", fabs },

    DEXT_INVALID_SYM
  };

  size_t nsym;
  size_t i;
  size_t j;
  size_t new_len;
  char* new_s;
  char* new_p;
  int err = -1;

  /* allocate and setup tcc state */

  dext->tcc = tcc_new();
  if (dext->tcc == NULL) goto on_error_0;

  tcc_set_options(dext->tcc, "-static -nostdlib -nostdinc");
  tcc_set_output_type(dext->tcc, TCC_OUTPUT_MEMORY);

  /* prepend symbol decls to code */

  new_len = 0;

  for (i = 0; default_syms[i].name != NULL; ++i)
    new_len += strlen(default_syms[i].decl);

  nsym = i;

  if (app_syms != NULL)
  {
    for (i = 0; app_syms[i].name != NULL; ++i)
      new_len += strlen(app_syms[i].decl);
    nsym += i;
  }

  new_len += strlen(s);

  new_s = malloc(new_len + 1);
  if (new_s == NULL) goto on_error_1;
  
  new_p = new_s;

  for (i = 0; default_syms[i].name != NULL; ++i)
  {
    const dext_sym_t* const sym = &default_syms[i];
    for (j = 0; sym->decl[j]; ++j, ++new_p) *new_p = sym->decl[j];
    tcc_add_symbol(dext->tcc, sym->name, sym->addr);
  }

  if (app_syms != NULL)
  {
    for (i = 0; app_syms[i].name != NULL; ++i)
    {
      const dext_sym_t* const sym = &app_syms[i];
      for (j = 0; sym->decl[j]; ++j, ++new_p) *new_p = sym->decl[j];
      tcc_add_symbol(dext->tcc, sym->name, sym->addr);
    }
  }

  strcpy(new_p, s);

  /* compile and relocate the code */

  err = 0;
  tcc_set_error_func(dext->tcc, &err, on_tcc_error);
  if ((tcc_compile_string(dext->tcc, new_s) == -1) || (err)) goto on_error_2;

  if (tcc_relocate(dext->tcc, TCC_RELOCATE_AUTO) < 0)
  {
    err = -1;
    goto on_error_2;
  }

  /* resolve entry point */

  dext->f = tcc_get_symbol(dext->tcc, "main");
  if (dext->f == NULL)
  {
    err = -1;
    goto on_error_2;
  }

 on_error_2:
  free(new_s);
 on_error_1:
  if (err) tcc_delete(dext->tcc);
 on_error_0:
  return err;
}


void dext_close(dext_handle_t* dext)
{
  tcc_delete(dext->tcc);
}
