#include "program.h"
#include "lex.h"
#include "statement.h"
#include "syntax.h"
#include "list.h"

#include <stdlib.h>

DEFINE_LIST_TYPE(stmt, stmt*)

static prog* _create_program() {
	prog* p = malloc(sizeof(prog));
	p->statements = stmt_list_create();
	return p;
}

prog* program(token_stream* s) {
	prog* p = _create_program();
	while(!lex_stream_is_eof(s)) {
		stmt* st = statement(s);
		stmt_list_append(p->statements, st);
		syntax_consume_token(s, SEMILOCON);
	}
	return p;
}

void program_accept(prog* p, ast_visitor visitor) {
	visitor.visit_program(p);
	for(int i = 0; i < p->statements->size; i++) {
		visitor.visit_statement(p->statements->data[i]);
	}
}
