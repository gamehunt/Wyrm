#ifndef _CLASS_H
#define _CLASS_H

#include "lex.h"
#include "list.h"
#include "statement.h"
#include "syntax.h"

enum access_qualifiers {
	A_PUBLIC,
	A_PRIVATE,
	A_PROTECTED
};

typedef struct _q_stmt {
	int is_static;
	enum access_qualifiers qualifier;	
	stmt* declaration;
} qualified_statement;

DEFINE_LIST_TYPE(q_stmt, qualified_statement*)

typedef struct _class_info {
	token* identifier;
	q_stmt_list* body;
} class_info;

q_stmt_list* class_body(token_stream* s);
class_info*  class(token_stream* s); 

const char* access_qualifier_to_string(enum access_qualifiers ac);

#define match_access_qualifier(s) \
	(syntax_match_tokens(s, 3, PUBLIC, PRIVATE, PROTECTED))

#endif
