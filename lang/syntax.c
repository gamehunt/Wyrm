#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <syntax.h>
#include <stdlib.h>
#include <stdarg.h>

#include "expr.h"
#include "lex.h"
#include "program.h"
#include "statement.h"

static void _syntax_printer_visit_unary_expr(unary_expr* e);
static void _syntax_printer_visit_bin_expr(binary_expr* e);
static void _syntax_printer_visit_group_expr(group_expr* e);
static void _syntax_printer_visit_literal_expr(literal_expr* e);
static void _syntax_printer_visit_assignment_expr(assignment_expr* e);
static void _syntax_printer_visit_program(prog* e);
static void _syntax_printer_visit_decl_stmt(decl* e);
static void _syntax_printer_visit_expr_stmt(expr* e);
static void _syntax_printer_visit_block_stmt(stmt_list* e);
static void _syntax_printer_visit_if_stmt(conditional* e);
static void _syntax_printer_visit_for_stmt(for_loop* e);
static void _syntax_printer_visit_while_stmt(while_loop* e);
static void _syntax_printer_visit_ret_stmt(expr* e);
static void _syntax_printer_visit_call_expr(call_expr* e);
static void _syntax_printer_visit_fun_def(fun_def* e);


static ast_visitor _ast_printer = {
	.visit_expr = NULL,
	.visit_binary_expr     = _syntax_printer_visit_bin_expr,
	.visit_unary_expr      = _syntax_printer_visit_unary_expr,
	.visit_group_expr      = _syntax_printer_visit_group_expr,
	.visit_literal_expr    = _syntax_printer_visit_literal_expr,
	.visit_assignment_expr = _syntax_printer_visit_assignment_expr,
	.visit_program         = _syntax_printer_visit_program,
	.visit_decl_stmt       = _syntax_printer_visit_decl_stmt,
	.visit_expr_stmt       = _syntax_printer_visit_expr_stmt,
	.visit_block_stmt      = _syntax_printer_visit_block_stmt,
	.visit_for_stmt        = _syntax_printer_visit_for_stmt,
	.visit_if_stmt         = _syntax_printer_visit_if_stmt,
	.visit_while_stmt      = _syntax_printer_visit_while_stmt,
	.visit_call_expr       = _syntax_printer_visit_call_expr,
	.visit_ret_stmt        = _syntax_printer_visit_ret_stmt,
	.visit_fun_def_stmt    = _syntax_printer_visit_fun_def
};

syntax_tree* syntax_tree_create() {
    syntax_tree* r = malloc(sizeof(syntax_tree));
    return r;
}

void syntax_tree_free(syntax_tree* tree) {
    free(tree);
}

static jmp_buf _error_restore_context;

int syntax_build_tree(token_stream* stream, syntax_tree* tree) {
	if(setjmp(_error_restore_context) == 0) {
		tree->program = program(stream);
    	return 0;
	} else {
		return 1;
	}
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

static void _syntax_printer_visit_program(prog* e) {
	printf("PROG [\n");
	for(int i = 0; i < e->statements->size; i++) {
		stmt_accept(e->statements->data[i], _ast_printer);
		printf("\n");
	}
	printf("]\n");
}

static void _print_declarator(declarator* d) {
	switch(d->dtype) {
		case D_ARRAY:
			printf("%s[%d] ", ((array_declarator*)d->data)->identifier->string_value, 
					((array_declarator*)d->data)->size);
			break;
		case D_POINTER:
			printf("*");
			_print_declarator((declarator*) d->data);
			break;
		case D_FUNC:
			printf("%s( ", ((func_declarator*)d->data)->identifier->string_value);
			for(int i = 0; i < ((func_declarator*) d->data)->args->size; i++) {
				_ast_printer.visit_decl_stmt(((func_declarator*) d->data)->args->data[i]);
				printf(", ");
			}
			printf(") ");
			break;
		case D_VAR:
			printf("%s ", ((var_declarator*) d->data)->identifier->string_value);
			break;
	}
}

static void _syntax_printer_visit_decl_stmt(decl* e) {
	printf("DECL [");
	for(int i = 0; i < e->specifiers->size; i++) {
		printf("%s ", lex_lexem_to_string(e->specifiers->data[i]));
	}
	printf("%s ", lex_lexem_to_string(e->type));
	_print_declarator(e->declarator);
	if(e->initializer) {
		printf(" := ");
		expr_accept(e->initializer, _ast_printer);
	}
	printf("]");
}

static void _syntax_printer_visit_expr_stmt(expr* e) {
	printf("EXPR [");
	expr_accept(e, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_block_stmt(stmt_list* e) {
	printf("[");
	for(int i = 0; i < e->size; i++) {
		stmt_accept(e->data[i], _ast_printer);
		printf("\n");
	}
	printf("]");
}

static void _syntax_printer_visit_if_stmt(conditional* e) {
	printf("IF [{");
	expr_accept(e->condition, _ast_printer);
	printf("}\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
	if(e->branch) {
		printf("\nELSE [\n");
		stmt_accept(e->branch, _ast_printer);
		printf("]");
	}
}

static void _syntax_printer_visit_for_stmt(for_loop* e) {
	printf("FOR [{");
	if(e->initializer) {
		stmt_accept(e->initializer, _ast_printer);
	}
	printf("}\n{");
	if(e->condition) {
		expr_accept(e->condition, _ast_printer);
	}
	printf("}\n{");
	if(e->increment) {
		expr_accept(e->increment, _ast_printer);
	}
	printf("}\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_while_stmt(while_loop* e) {
	if(e->prefix) {
		printf("DO-WHILE [{");
	} else {
		printf("WHILE [{");
	}
	expr_accept(e->condition, _ast_printer);
	printf("}\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_call_expr(call_expr* e) {
	printf("CALL [");
	expr_accept(e->callee, _ast_printer);
	printf(" (");
	for(int i = 0; i < e->args->size; i++) {
		expr_accept(e->args[i].data[i], _ast_printer);
	}
	printf(")]");
}

static void _syntax_printer_visit_ret_stmt(expr* v) {
	printf("RET ");
	if(v) {
		expr_accept(v, _ast_printer);
	} 
}

static void _syntax_printer_visit_fun_def(fun_def* e) {
	printf("FUNC [");
	for(int i = 0; i < e->specifiers->size; i++) {
		printf("%s ", lex_lexem_to_string(e->specifiers->data[i]));
	}
	printf("%s ", lex_lexem_to_string(e->type));
	_print_declarator(e->declarator);
	printf("\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
}

void syntax_print_tree(syntax_tree* tree) {
	syntax_walk_tree(tree, _ast_printer);
}

void syntax_walk_tree(syntax_tree* tree, ast_visitor visitor) {
	program_accept(tree->program, visitor);
}

static int _va_check_token(token* tok, int count, va_list args) {
	int r = 0;

	for(int i = 0; i < count; i++) {
		enum lexem t = va_arg(args, enum lexem);
		if(t == tok->type) {
			r = 1;
			break;
		}
	}

	return r;
}

static token* _va_check_tokens(token_stream* s, int count, va_list args) {
	token* current = lex_stream_current(s);
	return _va_check_token(current, count, args) ? current : NULL;
}

token* syntax_check_tokens(token_stream* stream, int count, ...) {
	va_list args;
	va_start(args, count);

	token* current = _va_check_tokens(stream, count, args);

	va_end(args);

	return current ? current : NULL;
}


int syntax_check_specific_token(token* tok, int count, ...) {
	va_list args;
	va_start(args, count);

	int r = _va_check_token(tok, count, args);

	va_end(args);

	return r;
}

token* syntax_match_tokens(token_stream* stream, int count, ...) {
	va_list args;
	va_start(args, count);

	token* c = _va_check_tokens(stream, count, args);

	va_end(args);

	if(c) {
		lex_stream_advance(stream);
		return c;
	} else {
		return NULL;
	}

}

token* syntax_consume_token(token_stream* stream, enum lexem required, const char* message) {
	token* current = lex_stream_current(stream);

	if(current->type == required) {
		lex_stream_advance(stream);
		return current;
	} else {
		syntax_error(current, message);
	}
}

void syntax_error(token* l, const char* message) {
	printf("Syntax error: unexpected %s at line %d: %s\n", lex_lexem_to_string(l->type), l->line + 1, message);
	longjmp(_error_restore_context, 1);
}
