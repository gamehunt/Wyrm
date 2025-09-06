#include "statement.h"
#include "expr.h"
#include <stdlib.h>

static stmt* _make_statement(enum stmt_type type, void* data) {
	stmt* st = malloc(sizeof(stmt));
	st->type = type;
	st->data = data;
	return st;
}

static stmt* _make_expr_statement(expr* e) {
	return _make_statement(ST_EXPRESSION, e);
}

stmt* statement(token_stream* s) {
	return expr_statement(s);
}

stmt* expr_statement(token_stream* s) {
	return _make_expr_statement(expression(s));	
}

void visit_statement(stmt* statement, ast_visitor visitor) {
	visitor.visit_statement(statement);
}
