#ifndef _PROGRAM_H
#define _PROGRAM_H

#include "lex.h"
#include "syntax.h"

struct _stmt_list;

typedef struct _prog {
	struct _stmt_list* statements;
} prog;

prog* program(token_stream* s);

void program_accept(prog* p, ast_visitor visitor);

#endif
