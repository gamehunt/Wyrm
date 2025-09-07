#include "statement.h"
#include "expr.h"
#include "lex.h"
#include "list.h"
#include "syntax.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

static int _check_type_or_spec(token_stream* s) {
	return syntax_check_type(s) || syntax_check_spec(s);
}

static stmt* _make_statement(enum stmt_type type, void* data) {
	stmt* st = malloc(sizeof(stmt));
	st->type = type;
	st->data = data;
	return st;
}

static stmt* _make_block_statement(stmt_list* stmts) {
	return _make_statement(ST_BLOCK, stmts);
}

static stmt* _make_expr_statement(expr* e) {
	return _make_statement(ST_EXPRESSION, e);
}

static stmt* _make_decl_statement(spec_list* specs, enum lexem type, token* identifier, expr* initializer) {
	decl* d = malloc(sizeof(decl));
	d->specifiers = specs;
	d->type = type;
	d->identifier = identifier;
	d->initializer = initializer;
	return _make_statement(ST_DECL, d);
}

static stmt* _make_if_statement(expr* cond, stmt* body, stmt* branch) {
	conditional* c = malloc(sizeof(conditional));
	c->condition = cond;
	c->body = body;
	c->branch = branch;
	return _make_statement(ST_IF, c);
}

static stmt* _make_for_statement(stmt* initializer, expr* condition, expr* increment, stmt* body) {
	for_loop* c = malloc(sizeof(for_loop));
	c->initializer = initializer;
	c->condition = condition;
	c->increment = increment;
	c->body = body;
	return _make_statement(ST_FOR, c);
}

static stmt* _make_while_statement(expr* cond, stmt* body, int prefix) {
	while_loop* c = malloc(sizeof(while_loop));
	c->condition = cond;
	c->body = body;
	c->prefix = prefix;
	return _make_statement(ST_WHILE, c);
}

stmt* statement(token_stream* s) {
	if(syntax_match_token(s, FOR)) {
		return for_stmt(s);
	} else if (syntax_match_token(s, IF)) {
		return if_stmt(s);
	} else if (syntax_match_tokens(s, DO, WHILE)) {
		return while_stmt(s);
	} else if(syntax_match_token(s, LBRACE)) {
		return block(s);
	} else {
		return expr_statement(s);
	}
}

stmt* expr_statement(token_stream* s) {
	return _make_expr_statement(expression(s));	
}

stmt* block(token_stream* s) {
	stmt_list* l = stmt_list_create();
	while(!lex_stream_is_eof(s) && lex_stream_current(s)->type != RBRACE) {
		stmt_list_append(l, declaration(s));
		syntax_match_token(s, SEMILOCON);
	}
	syntax_consume_token(s, RBRACE, "'}' expected after block statement");
	return _make_block_statement(l);
}

stmt* var_decl(token_stream* s) {
	enum lexem type = _EOF;
	token* tok = NULL;
	spec_list* l = spec_list_create();

	while((tok = syntax_match_type(s)) || (tok = syntax_match_spec(s))) {
		if(syntax_check_token_spec(tok)) {
			spec_list_append(l, tok->type);
		} else if (type == _EOF) {
			type = tok->type;	
		} else {
			syntax_error(tok, "only single type specification allowed");
		}
	}

	if(type == _EOF) {
		syntax_error(tok ? tok : lex_stream_current(s), "type specification required");
	}

	token* identifier = syntax_consume_token(s, IDENTIFIER, "identifier expected");

	expr* initializer = NULL;

	if(syntax_match_token(s, EQUAL)) {
		initializer = expression(s);	
	}

	return _make_decl_statement(l, type, identifier, initializer);
}

stmt* if_stmt(token_stream* s) {
	syntax_consume_token(s, LPAREN, "'(' expected before if expression");
	expr* condition = expression(s);
	syntax_consume_token(s, RPAREN, "')' expected after if expression");

	stmt* body = statement(s);
	stmt* branch = NULL;

	if(syntax_match_token(s, ELSE)) {
		branch = statement(s);
	}

	return _make_if_statement(condition, body, branch);
}

stmt* for_stmt(token_stream* s) {
	syntax_consume_token(s, LPAREN, "'(' exprected after for");

	stmt* initializer = NULL;
	expr* condition = NULL;
	expr* increment = NULL;

	if(!syntax_match_token(s, SEMILOCON)) {
		if(_check_type_or_spec(s)) {
			initializer = declaration(s);
		} else {
			initializer = expr_statement(s);
		}
		syntax_consume_token(s, SEMILOCON, "';' required after initializer");
	}

	if(!syntax_match_token(s, SEMILOCON)) {
		condition = expression(s);
		syntax_consume_token(s, SEMILOCON, "';' required after condition");
	}

	if(!syntax_match_token(s, RPAREN)) {
		increment = expression(s);
		syntax_consume_token(s, RPAREN, "')' exprected before for body");
	}

	stmt* body = statement(s);

	return _make_for_statement(initializer, condition, increment, body);
}

stmt* while_stmt(token_stream* s) {
	int prefix = 0;
	stmt* body = NULL;
	expr* cond = NULL;
	if(lex_stream_previous(s)->type == DO) {
		prefix = 1;
		body = statement(s);
		syntax_consume_token(s, WHILE, "'while' required after do block");
		cond = expression(s);
	} else {
		cond = expression(s);
		body = statement(s);
	}
	return _make_while_statement(cond, body, prefix);
}

stmt* declaration(token_stream* s) {
	if(_check_type_or_spec(s)) {
		return var_decl(s);
	}
	return statement(s);
}

void stmt_accept(stmt* statement, ast_visitor visitor) {
	switch(statement->type) {
		case ST_EXPRESSION:
			SAFE_CALL(visitor.visit_expr_stmt, statement->data);
			break;
		case ST_DECL:
			SAFE_CALL(visitor.visit_decl_stmt, statement->data);
			break;
		case ST_BLOCK:
			SAFE_CALL(visitor.visit_block_stmt, statement->data);
			break;
		case ST_IF:
			SAFE_CALL(visitor.visit_if_stmt, statement->data);
			break;
		case ST_FOR:
			SAFE_CALL(visitor.visit_for_stmt, statement->data);
			break;
		case ST_WHILE:
			SAFE_CALL(visitor.visit_while_stmt, statement->data);
			break;
	}
	SAFE_CALL(visitor.visit_stmt, statement);
}
