#ifndef _EXPR_H
#define _EXPR_H

#include "lex.h"
#include "list.h"
#include "syntax.h"

enum expr_type {
	ET_UNARY,
	ET_BINARY,
	ET_GROUP,
	ET_LITERAL,
	ET_ASSIGNMENT,
	ET_CALL,
	ET_SUBSCRIPT
};

typedef struct _expr {
	enum expr_type type;
	void* data;
} expr;

typedef struct _binary_expr {
	expr* left;
	enum lexem op;
	expr* right;
} binary_expr;

typedef struct _unary_expr {
	enum lexem op;
	expr* right;
	int postfix;
} unary_expr;

typedef struct _group_expr {
	expr* expr;	
} group_expr;

typedef struct _literal_expr {
	token* value;
} literal_expr;

typedef struct _assignment_expr {
	expr* lvalue;
	enum lexem op;
	expr* rvalue;
} assignment_expr;

DEFINE_LIST_TYPE(args, expr*)

typedef struct _call_expr {
	expr* callee;
	args_list* args;
} call_expr;

typedef struct _subscript_expr {
	expr* array;
	expr* index;
} subscript_expr;

void expr_accept(expr* e, ast_visitor visitor);

expr* term(token_stream* s);
expr* unary_postfix(token_stream* s);
expr* unary(token_stream* s);
expr* access(token_stream* s);
expr* multiplication(token_stream* s);
expr* addition(token_stream* s);
expr* bit_and(token_stream* s);
expr* bit_xor(token_stream* s);
expr* bit_or(token_stream* s);
expr* logic_and(token_stream* s);
expr* logic_or(token_stream* s);
expr* shifts(token_stream* s);
expr* comparison(token_stream* s);
expr* equality(token_stream* s);
expr* assignment(token_stream* s);
expr* call(token_stream* s);
expr* subscript(token_stream* s);
expr* expression(token_stream* s);

#endif
