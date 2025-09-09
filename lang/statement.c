#include "statement.h"
#include "class.h"
#include "expr.h"
#include "lex.h"
#include "list.h"
#include "syntax.h"
#include "type.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

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

static stmt* _make_ret_statement(expr* e) {
	return _make_statement(ST_RETURN, e);
}

static stmt* _make_decl_statement(spec_list* specs, type_info* type, token* ident, expr* initializer) {
	decl* d = malloc(sizeof(decl));
	d->specifiers = specs;
	d->type = type;
	d->identifier = ident;
	d->initializer = initializer;
	return _make_statement(ST_DECL, d);
}
static stmt* _make_fun_def_statement(spec_list* specs, type_info* type, token* ident, stmt_list* args, stmt* body) {
	fun_def* d = malloc(sizeof(fun_def));
	d->specifiers = specs;
	d->ret_type = type;
	d->identifier = ident;
	d->params = args;
	d->body = body;
	return _make_statement(ST_FUN_DEF, d);
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

static stmt* _make_loop_ctrl_statement(token* t) {
	return _make_statement(ST_LOOP_CTRL, t);
}

static stmt* _make_class_statement(class_info* ci) {
	return _make_statement(ST_CLASS, ci);
}

stmt* statement(token_stream* s) {
	if(syntax_match_token(s, FOR)) {
		return for_stmt(s);
	} else if (syntax_match_token(s, IF)) {
		return if_stmt(s);
	} else if (syntax_match_tokens(s, 2, DO, WHILE)) {
		return while_stmt(s);
	} else if (syntax_match_token(s, RETURN)) {
		return return_stmt(s);
	} else if (syntax_match_tokens(s, 2, CONTINUE, BREAK)) {
		return loop_flow_stmt(s);
	} else if (syntax_match_token(s, LBRACE)) {
		return block(s);
	} else if (syntax_match_token(s, TYPEDEF)) {
		return type_def(s);
	} else {
		return expr_statement(s);
	}
}

stmt* expr_statement(token_stream* s) {
	expr* e = expression(s);
	syntax_consume_token(s, SEMILOCON, "';' required after expression statement");
	return _make_expr_statement(e);	
}

stmt* block(token_stream* s) {
	stmt_list* l = stmt_list_create();
	while(!syntax_match_token(s, RBRACE)) {
		stmt_list_append(l, declaration(s));
	}
	return _make_block_statement(l);
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
		if(syntax_check_token(s, LET)) {
			initializer = declaration(s);
		} else {
			initializer = expr_statement(s);
		}
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
		syntax_consume_token(s, LPAREN, "'(' required before while condition");
		cond = expression(s);
		syntax_consume_token(s, RPAREN, "')' required after while condition");
		syntax_consume_token(s, SEMILOCON, "';' required after do-while");
	} else {
		cond = expression(s);
		body = statement(s);
	}
	return _make_while_statement(cond, body, prefix);
}

stmt* func_arg_decl(token_stream* s) {
	token* tok = NULL;
	spec_list* l = spec_list_create();

	while((tok = match_spec(s))) {
		spec_list_append(l, tok->type);
	}

	type_info* t = type(s);
	token* ident = syntax_match_token(s, IDENTIFIER);

	expr* initializer = NULL;
	if(syntax_match_token(s, EQUAL)) {
		initializer = expression(s);
	}

	return _make_decl_statement(l, t, ident, initializer);
}

stmt* var_decl(token_stream* s) {
	spec_list* l = spec_list_create();
	token* tok = NULL;

	while((tok = match_spec(s))) {
		spec_list_append(l, tok->type);
	}

	type_info* t = type(s);
	token* identifier = syntax_consume_token(s, IDENTIFIER, "identifier required");

	expr* initializer = NULL;
	if(syntax_match_token(s, EQUAL)) {
		initializer = expression(s);
	}
	syntax_consume_token(s, SEMILOCON, "';' required after declaration statement");
	return _make_decl_statement(l, t, identifier, initializer);
}

stmt* fun_decl(token_stream* s) {
	spec_list* l = spec_list_create();
	token* tok = NULL;

	while((tok = match_spec(s))) {
		spec_list_append(l, tok->type);
	}

	type_info* t = type(s);
	token* identifier = syntax_consume_token(s, IDENTIFIER, "identifier required");

	syntax_consume_token(s, LPAREN, "'(' required before arg list");

	stmt_list* args = stmt_list_create();
	if(!syntax_match_token(s, RPAREN)) {
		do {
			stmt_list_append(args, func_arg_decl(s));
		} while(syntax_match_token(s, COMMA));
		syntax_consume_token(s, RPAREN, "')' required after arg list");
	}

	stmt* body = NULL;
	if(syntax_match_token(s, LBRACE)) {
		body = block(s);
	} else {
		syntax_consume_token(s, SEMILOCON, "';' required after declaration statement");
	}

	return _make_fun_def_statement(l, t, identifier, args, body);
}

stmt* declaration(token_stream* s) {
	if(syntax_match_token(s, CLASS)) {
		return class_decl(s);
	} else if (syntax_match_token(s, LET)){
		return var_decl(s);
	} else if (syntax_match_token(s, FUN)) {
		return fun_decl(s);
	} else {
		return statement(s);
	}
}

stmt* return_stmt(token_stream* s) {
	expr* val = NULL;
	if(!syntax_match_token(s, SEMILOCON)) {
		val = expression(s);
	} 
	syntax_consume_token(s, SEMILOCON, "';' required after return statement");
	return _make_ret_statement(val);
}

stmt* class_decl(token_stream* s) {
	return _make_class_statement(class(s));
}

stmt* loop_flow_stmt(token_stream* s) {
	stmt* st =  _make_loop_ctrl_statement(lex_stream_previous(s));
	syntax_consume_token(s, SEMILOCON, "';' required after loop control statement");
	return st;
}

stmt* type_def(token_stream* s) {
	typedef_stmt* st = malloc(sizeof(typedef_stmt));
	st->type = type(s);
	st->alias = syntax_consume_token(s, IDENTIFIER, "type alias required");
	syntax_consume_token(s, SEMILOCON, "';' required after typedef statement");
	return _make_statement(ST_TYPEDEF, st);
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
		case ST_RETURN:
			SAFE_CALL(visitor.visit_ret_stmt, statement->data);
			break;
		case ST_FUN_DEF:
			SAFE_CALL(visitor.visit_fun_def_stmt, statement->data);
			break;
		case ST_LOOP_CTRL:
			SAFE_CALL(visitor.visit_loop_ctrl_stmt, statement->data);
			break;
		case ST_TYPEDEF:
			SAFE_CALL(visitor.visit_typedef, statement->data);
			break;
		case ST_CLASS:
			SAFE_CALL(visitor.visit_class, statement->data);
			break;
	}
	SAFE_CALL(visitor.visit_stmt, statement);
}
