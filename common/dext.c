/* dance extension */
/* user provided codes, typically used as computation kernels */
/* C compilation based on http://bellard.org/tcc/ */
/* see libdance/doc/HOWTOs/dext-HOWTO.txt */


#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include "dext.h"
#include "libtcc.h"

#ifndef DEXT_UNIT
#include "libdance.h"
#endif /* DEXT_UNIT */


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


dext_handle_t* dext_create(const char* id)
{
  dext_handle_t* dext;
  size_t i;

  /* find free dext */
  dext = NULL;
  for (i = 0; (i != DEXT_MAX_COUNT) && (g_dext[i].id != NULL); ++i) ;
  if (i == DEXT_MAX_COUNT) return NULL;
  dext = &g_dext[i];

  dext->id = malloc(strlen(id) + 1);
  if (dext->id == NULL) return NULL;
  strcpy(dext->id, id);

  dext->tcc = NULL;
  dext->code = NULL;
  dext->len = 0;

  return dext;
}


void dext_destroy(dext_handle_t* dext)
{
  free(dext->id);
  dext->id = NULL;
  if (dext->tcc != NULL) tcc_delete(dext->tcc);
  if (dext->code != NULL) free(dext->code);
}


int dext_add(dext_handle_t* dext, const char* s)
{
  /* add code to extension */

  const size_t new_len = dext->len + strlen(s);

  dext->code = realloc(dext->code, new_len + 1);
  if (dext->code == NULL) return -1;

  strcpy(dext->code + dext->len, s);
  dext->len = new_len;

  return 0;
}


void dext_clear(dext_handle_t* dext)
{
  /* clear extension code */

  if (dext->code != NULL) free(dext->code);
  dext->code = NULL;
}


static void on_tcc_error(void* err, const char* msg)
{
  /* skip this warning */
#define SOFTFP_WARN "<string>:1: warning: soft float ABI"
  if (strncmp(SOFTFP_WARN, msg, sizeof(SOFTFP_WARN) - 1) == 0) return ;
  *(int*)err = -1;
}

int dext_compile(dext_handle_t* dext, const dext_sym_t* app_syms)
{
  /* compile the extension code */

  const dext_sym_t default_syms[] =
  {
    /* dance specific symbols */

    /* math symbols */
    { "sin", "double sin(double);", sin },
    { "cos", "double cos(double);", cos },
    { "tan", "double tan(double);", tan },
    { "pow", "double pow(double, double);", pow },
    { "sqrt", "double sqrt(double);", sqrt },
    { "fabs", "double fabs(double);", fabs },

    DEXT_INVALID_SYM
  };

  size_t nsym;
  size_t i;
  size_t j;
  size_t new_len;
  char* new_code;
  char* new_p;
  int err = -1;

  /* allocate and setup tcc state */

  dext->tcc = tcc_new();
  if (dext->tcc == NULL) goto on_error_0;

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

  new_len += strlen(dext->code);
  new_code = malloc(new_len + 1);
  if (new_code == NULL) goto on_error_1;
  
  new_p = new_code;

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

  strcpy(new_p, dext->code);

  /* compile and relocate the code */

  err = 0;
  tcc_set_error_func(dext->tcc, &err, on_tcc_error);
  if ((tcc_compile_string(dext->tcc, new_code) == -1) || (err))
  {
    err = -1;
    goto on_error_2;
  }

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
  free(new_code);

  if (err)
  {
  on_error_1:
    tcc_delete(dext->tcc);
    dext->tcc = NULL;
  }

 on_error_0:
  return err;
}


unsigned int dext_is_compiled(dext_handle_t* dext)
{
  return dext->tcc != NULL;
}


dext_handle_t* dext_find(const char* id)
{
  /* find an extension by id */

  size_t i;

  for (i = 0; i != DEXT_MAX_COUNT; ++i)
  {
    dext_handle_t* const dext = &g_dext[i];
    if (dext->id == NULL) continue ;
    if (strcmp(dext->id, id) == 0) return dext;
  }

  return NULL;
}


#ifndef DEXT_UNIT

/* commands */

void commexec_DEXT(void)
{
  /* parse */

  const char* const id = I_LABEL();
  dext_handle_t* dext;

  dext = dext_find(id);

  while (1)
  {
    const int fmt_num = I_FMT_NUM();

    if (fmt_num < 0) break ;

    if (fmt_num == 0)
    {
      I_NEXT_FMT();
      continue ;
    }

    switch (I_FMT_TYPE())
    {
    case FMT_TAG:
    {
      if (I_FMT_NAME("LINE"))
      {
	unsigned int was_created = 0;

	if (dext == NULL)
	{
	  dext = dext_create(id);
	  if (dext == NULL)
	  {
	    errorf("could not create extension");
	    return ;
	  }
	  was_created = 1;
	}

	I_NEXT_FMT();

	if (dext_add(dext, I_STRING()))
	{
	  errorf("could not add line");
	  if (was_created) dext_destroy(dext);
	  return ;
	}
      }
      else if (I_FMT_NAME("CLEAR"))
      {
	if (dext == NULL)
	{
	  errorf("extension not found");
	  return ;
	}

	dext_clear(dext);
      }
      else if (I_FMT_NAME("EXEC"))
      {
	static const size_t n = 5 + 1;
	size_t i;
	double x[n];

	if (dext == NULL)
	{
	  errorf("extension not found");
	  return ;
	}

	if (dext_is_compiled(dext) == 0)
	{
	  if (dext_compile(dext, NULL))
	  {
	    errorf("compilation error");
	    return ;
	  }
	}

	I_NEXT_FMT();

	i = 0;
	while ((I_FMT_NUM() > 0) && (I_FMT_TYPE() == FMT_FLOAT))
	{
	  x[i++] = I_FLOAT();
	  if (i == n) break ;
	}

	switch (i)
	{
	case 0: dext_exec(dext); break ;
	case 1: dext_exec1(dext, x[0]); break ;
	case 2: dext_exec2(dext, x[0], x[1]); break ;
	case 3: dext_exec3(dext, x[0], x[1], x[2]); break ;
	case 4: dext_exec4(dext, x[0], x[1], x[2], x[3]); break ;
	case 5: dext_exec5(dext, x[0], x[1], x[2], x[3], x[4]); break ;
	default: errorf("too many arguments"); break ;
	}
      }

      break ;
    }

    default:
    {
      errorf("invalid command");
      return ;
    }
    }
  }
  
}

void commexec_qDEXT(void)
{
  const char* const id = I_LABEL();

  if (id == DEFAULT_LABEL)
  {
    size_t i;
    for (i = 0; i != DEXT_MAX_COUNT; ++i)
    {
      dext_handle_t* const dext = &g_dext[i];
      if (dext->id == NULL) continue ;
      answerf("%s\n", dext->id);
    }
  }
  else
  {
    dext_handle_t* dext = dext_find(id);
    if (dext == NULL)
    {
      errorf("invalid id");
      return ;
    }

    if (dext->code != NULL) answerf("%s", dext->code);
    answerf("\n");
  }
}

#endif /* DEXT_UNIT */
