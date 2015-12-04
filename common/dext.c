/* dance extension */
/* user provided codes, typically used as computation kernels */
/* C compilation based on http://bellard.org/tcc/ */


#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "dext.h"
#include "libtcc.h"


#define DEXT_MAX_COUNT 32
static dext_handle_t g_dext[DEXT_MAX_COUNT];


int dext_init(void)
{
  size_t i;
  for (i = 0; i != DEXT_MAX_COUNT; ++i) g_dext[i].id = NULL;
  return 0;
}


void dext_fini(void)
{
  size_t i;

  for (i = 0; i != DEXT_MAX_COUNT; ++i)
  {
    if (g_dext[i].id == NULL) continue ;
    dext_destroy(&g_dext[i]);
    g_dext[i].id = NULL;
  }
}


static void on_tcc_error(void* err, const char* msg)
{
  /* skip this warning */
#define SOFTFP_WARN "<string>:1: warning: soft float ABI"
  if (strncmp(SOFTFP_WARN, msg, sizeof(SOFTFP_WARN) - 1) == 0) return ;
  *(int*)err = -1;
}


dext_handle_t* dext_create_c
(const char* id, const char* s, const dext_sym_t* app_syms)
{
  const dext_sym_t default_syms[] =
  {
    /* dance specific symbols */

    /* math symbols */
    { "sin", "double sin(double);", sin },
    { "cos", "double cos(double);", cos },
    { "sqrt", "double sqrt(double);", sqrt },
    { "fabs", "double fabs(double);", fabs },

    DEXT_INVALID_SYM
  };

  dext_handle_t* dext;
  size_t nsym;
  size_t i;
  size_t j;
  size_t new_len;
  char* new_s;
  char* new_p;
  int err = -1;

  /* find free dext */
  dext = NULL;
  for (i = 0; (i != DEXT_MAX_COUNT) && (g_dext[i].id != NULL); ++i) ;
  if (i == DEXT_MAX_COUNT) goto on_error_0;
  dext = &g_dext[i];

  dext->id = malloc(strlen(id) + 1);
  if (dext->id == NULL) goto on_error_0;
  strcpy(dext->id, id);

  /* allocate and setup tcc state */

  dext->tcc = tcc_new();
  if (dext->tcc == NULL) goto on_error_1;

  tcc_set_options(dext->tcc, "-static -nostdlib -nostdinc");
  tcc_set_output_type(dext->tcc, TCC_OUTPUT_MEMORY);

  /* prepend symbol decls to code */

  new_len = 0;

  for (i = 0; default_syms[i].id != NULL; ++i)
    new_len += strlen(default_syms[i].decl);

  nsym = i;

  if (app_syms != NULL)
  {
    for (i = 0; app_syms[i].id != NULL; ++i)
      new_len += strlen(app_syms[i].decl);
    nsym += i;
  }

  new_len += strlen(s);

  new_s = malloc(new_len + 1);
  if (new_s == NULL) goto on_error_2;
  
  new_p = new_s;

  for (i = 0; default_syms[i].id != NULL; ++i)
  {
    const dext_sym_t* const sym = &default_syms[i];
    for (j = 0; sym->decl[j]; ++j, ++new_p) *new_p = sym->decl[j];
    tcc_add_symbol(dext->tcc, sym->id, sym->addr);
  }

  if (app_syms != NULL)
  {
    for (i = 0; app_syms[i].id != NULL; ++i)
    {
      const dext_sym_t* const sym = &app_syms[i];
      for (j = 0; sym->decl[j]; ++j, ++new_p) *new_p = sym->decl[j];
      tcc_add_symbol(dext->tcc, sym->id, sym->addr);
    }
  }

  strcpy(new_p, s);

  /* compile and relocate the code */

  err = 0;
  tcc_set_error_func(dext->tcc, &err, on_tcc_error);
  if ((tcc_compile_string(dext->tcc, new_s) == -1) || (err)) goto on_error_3;

  if (tcc_relocate(dext->tcc, TCC_RELOCATE_AUTO) < 0)
  {
    err = -1;
    goto on_error_3;
  }

  /* resolve entry point */

  dext->f = tcc_get_symbol(dext->tcc, "main");
  if (dext->f == NULL)
  {
    err = -1;
    goto on_error_3;
  }

 on_error_3:
  free(new_s);

  if (err)
  {
  on_error_2:
    tcc_delete(dext->tcc);
  on_error_1:
    free(dext->id);
    dext->id = NULL;
    dext = NULL;
  }

 on_error_0:
  return dext;
}


void dext_destroy(dext_handle_t* dext)
{
  free(dext->id);
  dext->id = NULL;
  tcc_delete(dext->tcc);
}


dext_handle_t* dext_find(const char* id)
{
  size_t i;

  for (i = 0; i != DEXT_MAX_COUNT; ++i)
  {
    dext_handle_t* const dext = &g_dext[i];
    if (dext->id == NULL) continue ;
    if (strcmp(dext->id, id) == 0) return dext;
  }

  return NULL;
}
