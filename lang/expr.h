#ifndef _EXPR_H
#define _EXPR_H

#include "lex.h"
#include "syntax.h"

enum expr_type {
	ET_UNARY,
	ET_BINARY,
	ET_GROUP,
	ET_LITERAL,
	ET_ASSIGNMENT
};

typedef struct _expr {
	enum expr_type type;
	void* data;
} expr;

typedef struct _binary_expr {
	expr* left;
	enum lexem_type op;
	expr* right;
} binary_expr;

typedef struct _unary_expr {
	enum lexem_type op;
	expr* right;
	int postfix;
} unary_expr;

typedef struct _group_expr {
	expr* expr;	
} group_expr;

typedef struct _literal_expr {
	lexem* value;
} literal_expr;

typedef struct _assignment_expr {
	expr* lvalue;
	enum lexem_type op;
	expr* rvalue;
} assignment_expr;

void expr_accept(expr* e, ast_visitor visitor);

expr* term(lexem_stream* s);
expr* unary_postfix(lexem_stream* s);
expr* unary(lexem_stream* s);
expr* multiplication(lexem_stream* s);
expr* addition(lexem_stream* s);
expr* bit_and(lexem_stream* s);
expr* bit_xor(lexem_stream* s);
expr* bit_or(lexem_stream* s);
expr* logic_and(lexem_stream* s);
expr* logic_or(lexem_stream* s);
expr* shifts(lexem_stream* s);
expr* comparison(lexem_stream* s);
expr* equality(lexem_stream* s);
expr* assignment(lexem_stream* s);
expr* expression(lexem_stream* s);

#endif
