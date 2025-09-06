#include <assert.h>
#include <stdio.h>
#include <syntax.h>
#include <stdlib.h>
#include <stdarg.h>

#include "expr.h"
#include "lex.h"

static void _syntax_printer_visit_unary_expr(unary_expr* e);
static void _syntax_printer_visit_bin_expr(binary_expr* e);
static void _syntax_printer_visit_group_expr(group_expr* e);
static void _syntax_printer_visit_literal_expr(literal_expr* e);
static void _syntax_printer_visit_assignment_expr(assignment_expr* e);

static ast_visitor _ast_printer = {
	.visit_expr = NULL,
	.visit_binary_expr     = _syntax_printer_visit_bin_expr,
	.visit_unary_expr      = _syntax_printer_visit_unary_expr,
	.visit_group_expr      = _syntax_printer_visit_group_expr,
	.visit_literal_expr    = _syntax_printer_visit_literal_expr,
	.visit_assignment_expr = _syntax_printer_visit_assignment_expr
};

syntax_tree* syntax_tree_create() {
    syntax_tree* r = malloc(sizeof(syntax_tree));
    return r;
}

void syntax_tree_free(syntax_tree* tree) {
    free(tree);
}

int syntax_build_tree(token_stream* stream, syntax_tree** result) {
    syntax_tree* tree = syntax_tree_create();

	for(int i = 0; i < stream->size; i++) {
		printf("%s ", lex_lexem_to_string(stream->tokens[i]->type));
	}
	printf("\n");

	expr* exp = expression(stream);

	assert(exp != NULL);

	expr_accept(exp, _ast_printer);

    *result = tree;
    return 0;
}


static void _syntax_printer_visit_unary_expr(unary_expr* e) {
	printf("[");
	if(e->postfix) {
		expr_accept(e->right, _ast_printer);
		printf(" %s ", lex_lexem_to_string(e->op));
	} else {
		printf(" %s ", lex_lexem_to_string(e->op));
		expr_accept(e->right, _ast_printer);
	}
	printf("]");
}

static void _syntax_printer_visit_bin_expr(binary_expr* e) {
	printf("[");
	expr_accept(e->left, _ast_printer);
	printf(" %s ", lex_lexem_to_string(e->op));
	expr_accept(e->right, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_group_expr(group_expr* e) {
	printf("GROUP [");
	expr_accept(e->expr, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_literal_expr(literal_expr* e) {
	switch(e->value->type) {
		case NIL:
			printf("NIL");
			break;
		case TRUE:
			printf("TRUE");
			break;
		case FALSE:
			printf("FALSE");
			break;
		case NUMERIC:
			printf("%f", e->value->double_value);
			break;
		case INTEGER:
			printf("%d", e->value->integer_value);
			break;
		case STRING:
		case IDENTIFIER:
			printf("%s", e->value->string_value);
			break;
		default:
			printf("UNKNOWN");
			break;
	}
}

static void _syntax_printer_visit_assignment_expr(assignment_expr* e) {
	printf("ASSIGNMENT [");
	expr_accept(e->lvalue, _ast_printer);
	printf(" %s ", lex_lexem_to_string(e->op));
	expr_accept(e->rvalue, _ast_printer);
	printf("]");
}

void syntax_print_tree(syntax_tree* tree) {
	syntax_walk_tree(tree, _ast_printer);
}

void syntax_walk_tree(syntax_tree* tree, ast_visitor visitor) {

}

int syntax_match_tokens(token_stream* stream, int count, ...) {
	token* current = lex_stream_current(stream);

	va_list args;
	va_start(args, count);

	int r = 0;

	for(int i = 0; i < count; i++) {
		enum lexem t = va_arg(args, enum lexem);
		if(t == current->type) {
			r = 1;
			break;
		}
	}

	va_end(args);

	if(r) {
		lex_stream_advance(stream);
	}

	return r;
}

void syntax_consume_token(token_stream* stream, enum lexem required) {
	token* current = lex_stream_current(stream);

	if(current->type == required) {
		lex_stream_advance(stream);
	} else {
		// PANIC
		printf("Invalid token: %s (needed LPAREN)\n", lex_lexem_to_string(current->type));
	}
}

void syntax_error(token* l, const char* message) {

}
