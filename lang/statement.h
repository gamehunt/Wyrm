#ifndef _STMT_H
#define _STMT_H

#include "lex.h"
#include "syntax.h"

enum stmt_type {
	ST_EXPRESSION
};

typedef struct _stmt {
	enum stmt_type type;	
	void* data;
} stmt;

void visit_statement(stmt* statement, ast_visitor visitor);

stmt* statement(token_stream* s); 
stmt* expr_statement(token_stream* s);

#endif
