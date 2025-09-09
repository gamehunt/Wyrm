#ifndef _HATCH_H
#define _HATCH_H

#include "lex.h"
#include "syntax.h"

typedef struct {
	const char*   path;
	token_stream* tokens;
	syntax_tree*  ast;
} compilation_context;

#endif
