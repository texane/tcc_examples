#ifndef DEXT_H_INCLUDED
#define DEXT_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
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
  char* id;
  TCCState* tcc;
  void (*f)();
  char* code;
  size_t len;
} dext_handle_t;


int dext_init(void);
void dext_fini(void);
dext_handle_t* dext_create(const char*);
void dext_destroy(dext_handle_t*);
int dext_add(dext_handle_t*, const char*);
void dext_clear(dext_handle_t*);
int dext_compile(dext_handle_t*, const dext_sym_t*);
unsigned int dext_is_compiled(dext_handle_t*);
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


/* menu */

#define fmtDEXTID fmtLABEL

/* DEXT <DEXTID> LINE <LINE> */
/* DEXT <DEXTID> CLEAR */
/* DEXT <DEXTID> EXEC [ ARGS ] */
#define fmtDEXT			\
 fmtDEXTID			\
 "{"				\
 "   (T#LINE " fmtSTR ")"	\
 " | (T#CLEAR)"			\
 " | (T#EXEC nF)"		\
 "}"

/* ?DEXT [ <DEXTID> ] */
#define fmtqDEXT "o" fmtDEXTID

#define DEXT_MENU_LIST							\
 MENU_IT(DEXT, fmtDEXT, fmtNONE, _B, "set a dance extension conf")	\
 MENU_IT(qDEXT, fmtqDEXT, fmtSTR, _B, "get dance a extension conf")

#define DEXT_LISTS


#endif /* DEXT_H_INCLUDED */
