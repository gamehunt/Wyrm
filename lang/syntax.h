#ifndef _SYNTAX_H
#define _SYNTAX_H 1

#include <lex.h>

struct _expr;
struct _binary_expr;
struct _unary_expr;
struct _group_expr;
struct _literal_expr;
struct _assignment_expr;

typedef struct {
	void (*visit_expr)(struct _expr* e);
	void (*visit_unary_expr)(struct _unary_expr* e);
	void (*visit_binary_expr)(struct _binary_expr* e);
	void (*visit_group_expr)(struct _group_expr* e);
	void (*visit_literal_expr)(struct _literal_expr* e);
	void (*visit_assignment_expr)(struct _assignment_expr* e);
} ast_visitor;

typedef struct {
} syntax_tree;

int syntax_build_tree(lexem_stream* stream, syntax_tree** result);
syntax_tree* syntax_tree_create();
void syntax_tree_free(syntax_tree* tree);

void syntax_print_tree(syntax_tree* tree);
void syntax_walk_tree(syntax_tree* tree, ast_visitor visitor);

int syntax_match_tokens(lexem_stream* stream, int count, ...);
#define syntax_match_token(s, t) syntax_match_tokens(s, 1, t)

void syntax_consume_token(lexem_stream* stream, enum lexem_type token);

#endif
