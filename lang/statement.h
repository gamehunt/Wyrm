#ifndef _STMT_H
#define _STMT_H

#include "expr.h"
#include "lex.h"
#include "list.h"
#include "syntax.h"

enum stmt_type {
	ST_EXPRESSION,
	ST_BLOCK,
	ST_DECL,
	ST_IF,
	ST_FOR,
	ST_WHILE
};

typedef struct _stmt {
	enum stmt_type type;	
	void* data;
} stmt;

typedef struct _decl {
	spec_list* specifiers;
	enum lexem type;
	token* identifier;
	expr* initializer;
} decl;

typedef struct _if_stmt {
	expr* condition;
	stmt* body;
	stmt* branch;
} conditional;

typedef struct _for_stmt {
	stmt* initializer;
	expr* condition;
	expr* increment;
	stmt* body;
} for_loop;

typedef struct _while_stmt {
	expr* condition;
	stmt* body;
	int prefix;
} while_loop;

void stmt_accept(stmt* statement, ast_visitor visitor);

stmt* statement(token_stream* s); 
stmt* expr_statement(token_stream* s);
stmt* block(token_stream* s);
stmt* declaration(token_stream* s);
stmt* var_decl(token_stream* s);
stmt* if_stmt(token_stream* s);
stmt* for_stmt(token_stream* s);
stmt* while_stmt(token_stream* s);

#endif
