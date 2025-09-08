#include "statement.h"
#ifndef _SYNTAX_H
#define _SYNTAX_H 1

#include <lex.h>

#include "list.h"

struct _expr;
struct _binary_expr;
struct _unary_expr;
struct _group_expr;
struct _literal_expr;
struct _assignment_expr;
struct _prog;
struct _stmt;
struct _decl;
struct _if_stmt;
struct _for_stmt;
struct _while_stmt;
struct _call_expr;
struct _subscript_expr;
struct _fun_def;

typedef struct {
	void (*visit_expr)(struct _expr* e);
	void (*visit_unary_expr)(struct _unary_expr* e);
	void (*visit_binary_expr)(struct _binary_expr* e);
	void (*visit_group_expr)(struct _group_expr* e);
	void (*visit_literal_expr)(struct _literal_expr* e);
	void (*visit_assignment_expr)(struct _assignment_expr* e);
	void (*visit_program)(struct _prog* p);
	void (*visit_stmt)(struct _stmt* s);
	void (*visit_expr_stmt)(struct _expr* s);
	void (*visit_block_stmt)(stmt_list* s);
	void (*visit_decl_stmt)(struct _decl* d);
	void (*visit_if_stmt)(struct _if_stmt* s);
	void (*visit_for_stmt)(struct _for_stmt* s);
	void (*visit_while_stmt)(struct _while_stmt* s);
	void (*visit_ret_stmt)(struct _expr* v);
	void (*visit_call_expr)(struct _call_expr* s);
	void (*visit_subscript_expr)(struct _subscript_expr* s);
	void (*visit_fun_def_stmt)(struct _fun_def* s);
	void (*visit_loop_ctrl_stmt)(token* s);
} ast_visitor;

typedef struct {
	struct _prog* program;
} syntax_tree;

int syntax_build_tree(token_stream* stream, syntax_tree* result);
syntax_tree* syntax_tree_create();
void syntax_tree_free(syntax_tree* tree);

void syntax_print_tree(syntax_tree* tree);
void syntax_walk_tree(syntax_tree* tree, ast_visitor visitor);

token* syntax_match_tokens(token_stream* stream, int count, ...);
#define syntax_match_token(s, t) syntax_match_tokens(s, 1, t)

token* syntax_check_tokens(token_stream* stream, int count, ...);
#define syntax_check_token(s, t) syntax_check_tokens(s, 1, t)

int syntax_check_specific_token(token* tok, int count, ...);

token* syntax_consume_token(token_stream* stream, enum lexem token, const char* message);
void syntax_error(token* l, const char* message) __attribute__((noreturn));

#define syntax_type_list \
			VOID, \
			I8, \
			I16, \
			I32,\
			I64,\
			U8,\
			U16,\
			U32,\
			U64,\
			STR,\
			FLOAT,\
			DOUBLE,

#define TYPE_AMOUNT 12
#define SPEC_AMOUNT 1

#define syntax_spec_list \
			CONST

#define syntax_check_type(s) \
	(syntax_check_tokens(s, TYPE_AMOUNT, \
		syntax_type_list \
	)) \

#define syntax_check_token_type(t) \
	(syntax_check_specific_token(t, TYPE_AMOUNT, \
		syntax_type_list \
	)) \

#define syntax_match_type(s) \
	(syntax_match_tokens(s, TYPE_AMOUNT, \
		syntax_type_list \
	)) \

#define syntax_check_spec(s) \
	(syntax_check_tokens(s, SPEC_AMOUNT, \
		syntax_spec_list \
	)) \

#define syntax_check_token_spec(t) \
	(syntax_check_specific_token(t, SPEC_AMOUNT, \
		syntax_spec_list \
	)) \

#define syntax_match_spec(s) \
	(syntax_match_tokens(s, SPEC_AMOUNT, \
		syntax_spec_list \
	)) \

#endif
