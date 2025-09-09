#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <syntax.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lex.h"
#include "program.h"

extern ast_visitor _ast_printer;

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

void syntax_error_on_current(token_stream* s, const char* message) {
	syntax_error(lex_stream_current(s), message);
}
