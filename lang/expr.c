#include "expr.h"
#include "lex.h"
#include "syntax.h"
#include "util.h"
#include <stdlib.h>

LIST_IMPL(args, expr*)

void expr_accept(expr* e, ast_visitor visitor) {
	switch(e->type) {
		case ET_BINARY:
			SAFE_CALL(visitor.visit_binary_expr, e->data)
			break;
		case ET_UNARY:
			SAFE_CALL(visitor.visit_unary_expr, e->data)
			break;
		case ET_GROUP:
			SAFE_CALL(visitor.visit_group_expr, e->data)
			break;
		case ET_LITERAL:
			SAFE_CALL(visitor.visit_literal_expr, e->data)
			break;
		case ET_ASSIGNMENT:
			SAFE_CALL(visitor.visit_assignment_expr, e->data)
			break;
		case ET_CALL:
			SAFE_CALL(visitor.visit_call_expr, e->data)
			break;
		case ET_SUBSCRIPT:
			SAFE_CALL(visitor.visit_subscript_expr, e->data)
			break;
	}
	SAFE_CALL(visitor.visit_expr, e)
}

static expr* _make_expr(enum expr_type type, void* data) {
	expr* e = malloc(sizeof(expr));
	e->type = type;
	e->data = data;
	return e;
}

static expr* _make_binary_expr(expr* a, enum lexem op, expr* b) {
	binary_expr* e = malloc(sizeof(binary_expr));
	e->left = a;
	e->op = op;
	e->right = b;
	return _make_expr(ET_BINARY, e);
}

static expr* _make_unary_expr(enum lexem op, expr* b, int postfix) {
	unary_expr* e = malloc(sizeof(unary_expr));
	e->op = op;
	e->right = b;
	e->postfix = postfix;
	return _make_expr(ET_UNARY, e);
}

static expr* _make_literal_expr(token* l) {
	literal_expr* e = malloc(sizeof(literal_expr));
	e->value = l;
	return _make_expr(ET_LITERAL, e);
}

static expr* _make_group_expr(expr* inner) {
	group_expr* e = malloc(sizeof(group_expr));
	e->expr = inner;
	return _make_expr(ET_GROUP, e);
}

static expr* _make_assignment_expr(expr* a, enum lexem op, expr* b) {
	assignment_expr* e = malloc(sizeof(assignment_expr));
	e->lvalue = a;
	e->op = op;
	e->rvalue = b;
	return _make_expr(ET_ASSIGNMENT, e);
}

static expr* _make_call_expr(expr* callee, args_list* args) {
	call_expr* e = malloc(sizeof(call_expr));
	e->callee = callee;
	e->args = args;
	return _make_expr(ET_CALL, e);
}

static expr* _make_subscript_expr(expr* array, expr* subs) {
	subscript_expr* e = malloc(sizeof(subscript_expr));
	e->array = array;
	e->index = subs;
	return _make_expr(ET_SUBSCRIPT, e);
}

static void _free_expr(expr*);

static void _free_binary_expr(binary_expr* e) {
	_free_expr(e->right);
	_free_expr(e->left);
	free(e);
}

static void _free_unary_expr(unary_expr* e) {
	_free_expr(e->right);
	free(e);
}

static void _free_group_expr(group_expr* e) {
	_free_expr(e->expr);
	free(e);
}

static void _free_literal_expr(literal_expr* e) {
	free(e);
}

static void _free_assignment_expr(assignment_expr* e) {
	_free_expr(e->rvalue);
	_free_expr(e->lvalue);
	free(e);
}

static void _free_call_expr(call_expr* e) {
	_free_expr(e->callee);
	for(int i = 0; i < e->args->size; i++) {
		_free_expr(e->args->data[i]);
	}
	args_list_free(e->args);
	free(e);
}

static void _free_subscript_expr(subscript_expr* e) {
	_free_expr(e->array);
	_free_expr(e->index);
	free(e);
}

static void _free_expr(expr* e) {
	if(e->data) {
		switch(e->type) {
			case ET_BINARY:
				_free_binary_expr(e->data);
				break;
			case ET_UNARY:
				_free_unary_expr(e->data);
				break;
			case ET_GROUP:
				_free_group_expr(e->data);
				break;
			case ET_LITERAL:
				_free_literal_expr(e->data);
				break;
			case ET_ASSIGNMENT:
				_free_assignment_expr(e->data);
				break;
			case ET_CALL:
				_free_call_expr(e->data);
				break;
			case ET_SUBSCRIPT:
				_free_subscript_expr(e->data);
				break;
		}
	}
	free(e);
}

expr* term(token_stream* s) {
	if(syntax_match_tokens(s, 7, 
				STRING, INTEGER, NUMERIC, 
				NIL, FALSE, TRUE, IDENTIFIER)) {
		return _make_literal_expr(lex_stream_previous(s));
	} else if(syntax_match_token(s, LPAREN)) {
		expr* e = expression(s);
		syntax_consume_token(s, RPAREN, "expected ')' after group expression");
		return _make_group_expr(e);
	}

	syntax_error(lex_stream_current(s), "expression expected");
}

expr* unary_postfix(token_stream* s) {
	token* next = lex_stream_next(s);
	expr* t = subscript(s);

	if(next && (next->type == DOUBLE_PLUS || next->type == DOUBLE_MINUS)) {
		syntax_consume_token(s, next->type, "expected operator after postfix");
		return _make_unary_expr(next->type, t, 1);
	}

	return t;
}

expr* unary(token_stream* s) {
	if(syntax_match_tokens(s, 8, 
				BANG, MINUS, PLUS, 
				TILDA, DOUBLE_PLUS, DOUBLE_MINUS,
				ASTERISK, AMPERSAND)) {
		token* op = lex_stream_previous(s);
		expr* b = unary(s);
		return _make_unary_expr(op->type, b, 0);
	}

	return unary_postfix(s);
}

expr* access(token_stream* s) {
	expr* r = unary(s);

	while(syntax_match_tokens(s, 2, DOT, POINTER)) {
		token* op = lex_stream_previous(s);
		expr* b = unary(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* multiplication(token_stream* s) {
	expr* r = access(s);

	while(syntax_match_tokens(s, 2, SLASH, ASTERISK)) {
		token* op = lex_stream_previous(s);
		expr* b = access(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* addition(token_stream* s) {
	expr* r = multiplication(s);

	while(syntax_match_tokens(s, 2, PLUS, MINUS)) {
		token* op = lex_stream_previous(s);
		expr* b = multiplication(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* shifts(token_stream* s) {
	expr* r = addition(s);

	while(syntax_match_tokens(s, 2, 
				DOUBLE_LESS, DOUBLE_GREATER)) {
		token* op = lex_stream_previous(s);
		expr* b = addition(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* comparison(token_stream* s) {
	expr* r = shifts(s);

	while(syntax_match_tokens(s, 4, 
				LESS, LESS_EQUAL, GREATER, GREATER_EQUAL)) {
		token* op = lex_stream_previous(s);
		expr* b = shifts(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* equality(token_stream* s) {
	expr* r = logic_or(s);

	while(syntax_match_tokens(s, 2, BANG_EQUAL, EQUAL_EQUAL)) {
		token* op = lex_stream_previous(s);
		expr* b = logic_or(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* bit_and(token_stream* s) {
	expr* r = comparison(s);

	while(syntax_match_tokens(s, 1, AMPERSAND)) {
		token* op = lex_stream_previous(s);
		expr* b = comparison(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* bit_xor(token_stream* s) {
	expr* r = bit_and(s);

	while(syntax_match_tokens(s, 1, XOR)) {
		token* op = lex_stream_previous(s);
		expr* b = bit_and(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* bit_or(token_stream* s) {
	expr* r = bit_xor(s);

	while(syntax_match_tokens(s, 1, OR)) {
		token* op = lex_stream_previous(s);
		expr* b = bit_xor(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* logic_and(token_stream* s) {
	expr* r = bit_or(s);

	while(syntax_match_tokens(s, 1, DOUBLE_AMPERSAND)) {
		token* op = lex_stream_previous(s);
		expr* b = bit_or(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* logic_or(token_stream* s) {
	expr* r = logic_and(s);

	while(syntax_match_tokens(s, 1, DOUBLE_OR)) {
		token* op = lex_stream_previous(s);
		expr* b = logic_and(s);
		r = _make_binary_expr(r, op->type, b);
	}

	return r;
}

expr* assignment(token_stream* s) {
	expr* l = equality(s);

	while(syntax_match_tokens(s, 5, 
				EQUAL, PLUS_EQUAL, MINUS_EQUAL, 
				SLASH_EQUAL, ASTERISK_EQUAL)) {
		token* op = lex_stream_previous(s);
		expr* r = assignment(s);
		l = _make_assignment_expr(l, op->type, r);
	}

	return l;
}

static expr* _finalize_call(token_stream* s, expr* callee) {
	args_list* args = args_list_create();

	if(!syntax_check_token(s, RPAREN)) {
		do {
			args_list_append(args, expression(s));
		} while(syntax_match_token(s, COMMA));
	}

	syntax_consume_token(s, RPAREN, "')' expected after function arg list");

	return _make_call_expr(callee, args);
}

expr* subscript(token_stream* s) {
	expr* array = call(s);

	while(syntax_match_token(s, LSQBRACE)) {
		array = _make_subscript_expr(array, expression(s));
		syntax_consume_token(s, RSQBRACE, "']' required after array subscription");
	}

	return array;
}

expr* call(token_stream* s) {
	expr* t = term(s);
	while(syntax_match_token(s, LPAREN)) {
		t = _finalize_call(s, t);
	}
	return t;
}

expr* expression(token_stream* s) {
	return assignment(s);
}
