#include "program.h"
#include "lex.h"
#include "statement.h"
#include "syntax.h"
#include "list.h"
#include "util.h"

#include <stdlib.h>

static prog* _create_program() {
	prog* p = malloc(sizeof(prog));
	p->statements = stmt_list_create();
	return p;
}

prog* program(token_stream* s) {
	prog* p = _create_program();
	while(!lex_stream_is_eof(s)) {
		stmt* st = declaration(s);
		stmt_list_append(p->statements, st);
		syntax_match_token(s, SEMILOCON);
	}
	return p;
}

void program_accept(prog* p, ast_visitor visitor) {
	SAFE_CALL(visitor.visit_program, p)
} 
