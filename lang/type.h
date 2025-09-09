#ifndef _TYPE_H
#define _TYPE_H

#include "lex.h"
#include "syntax.h"

#define trivial_type_list \
			VOID, \
			I8, \
			I16, \
			I32,\
			I64,\
			U8,\
			U16,\
			U32,\
			U64,\
			STR,\
			FLOAT,\
			DOUBLE,\
			IDENTIFIER

#define TRIVIAL_TYPE_AMOUNT 13

#define check_trivial_type(s) \
	(syntax_check_tokens(s, TRIVIAL_TYPE_AMOUNT, \
		trivial_type_list \
	)) 

#define check_token_trivial_type(t) \
	(syntax_check_specific_token(t, TRIVIAL_TYPE_AMOUNT, \
		trivial_type_list \
	)) 

#define match_trivial_type(s) \
	(syntax_match_tokens(s, TRIVIAL_TYPE_AMOUNT, \
		trivial_type_list \
	)) 

#define SPEC_AMOUNT 1

#define specificator_list \
		CONST

#define check_spec(s) \
	(syntax_check_tokens(s, SPEC_AMOUNT, \
		specificator_list \
	))

#define check_token_spec(t) \
	(syntax_check_specific_token(t, SPEC_AMOUNT, \
		specificator_list \
	))

#define match_spec(s) \
	(syntax_match_tokens(s, SPEC_AMOUNT, \
		specificator_list \
	)) 

enum type_type {
	T_TRIVIAL,
	T_POINTER,
	T_ARRAY
};

typedef struct _type_info {
	enum type_type type;
	void* data;
} type_info;

typedef struct _pointer {
	type_info* value;
} pointer;

typedef struct _array {
	type_info* value;
	int size;
} array;

void type_accept(type_info* t, ast_visitor v);

type_info* type(token_stream* s);

#endif
