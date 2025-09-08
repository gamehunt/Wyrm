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
	ST_WHILE,
	ST_RETURN,
	ST_FUN_DEF
};

enum decl_type {
	D_VAR,
	D_POINTER,
	D_ARRAY,
	D_FUNC
};

typedef struct _stmt {
	enum stmt_type type;	
	void* data;
} stmt;

typedef struct _declarator {
	enum decl_type dtype;
	void* data;
} declarator;

typedef struct _decl {
	spec_list* specifiers;
	enum lexem type;
	declarator* declarator;
	expr* initializer;
} decl;

DEFINE_LIST_TYPE(decl, decl*)

typedef struct _func_declarator {
	token* identifier;
	decl_list* args;	
} func_declarator;

typedef struct _array_declarator {
	token* identifier;
	int size;
} array_declarator;

typedef struct _var_declarator {
	token* identifier;
} var_declarator;

typedef struct _if_stmt {
	expr* condition;
	stmt* body;
	stmt* branch;
} conditional;

typedef struct _fun_def {
	spec_list* specifiers;
	enum lexem type;
	declarator* declarator;
	stmt* body;
} fun_def;

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
stmt* if_stmt(token_stream* s);
stmt* for_stmt(token_stream* s);
stmt* while_stmt(token_stream* s);
stmt* return_stmt(token_stream* s);
declarator* stmt_declarator(token_stream* s);
stmt* func_arg_decl(token_stream* s);

#endif
