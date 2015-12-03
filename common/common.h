#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


#include "libtcc.h"

TCCState* tcc_new_with_opts(void);
int tcc_compile_relocate(TCCState*, const char*);


#endif /* COMMON_H_INCLUDED */
