#ifndef DEXT_H_INCLUDED
#define DEXT_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>
#include "libtcc.h"


typedef struct dext_sym
{
  const char* id;
  const char* decl;
  void* addr;
} dext_sym_t;

#define DEXT_INVALID_SYM { .id = NULL, }


typedef struct dext_handle
{
  TCCState* tcc;
  void (*f)();
  char* id;
} dext_handle_t;


int dext_init(void);
void dext_fini(void);
dext_handle_t* dext_create_c(const char*, const char*, const dext_sym_t*);
void dext_destroy(dext_handle_t*);
dext_handle_t* dext_find(const char*);

#define dext_exec(__h) \
((__h)->f())

#define dext_exec1(__h, __a) \
((__h)->f(__a))

#define dext_exec2(__h, __a, __b) \
((__h)->f(__a, __b))

#define dext_exec3(__h, __a, __b, __c) \
((__h)->f(__a, __b, __c))

#define dext_exec4(__h, __a, __b, __c, __d) \
((__h)->f(__a, __b, __c, __d))

#define dext_exec5(__h, __a, __b, __c, __d, __e) \
((__h)->f(__a, __b, __c, __d, __e))


#endif /* DEXT_H_INCLUDED */
