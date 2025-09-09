#include "type.h"
#include "lex.h"
#include "syntax.h"

static type_info* _make_type(enum type_type type, void* data) {
	type_info* t = malloc(sizeof(type_info));
	t->type = type;
	t->data = data;
	return t;
}

static type_info* _make_trivial(token* t) {
	return _make_type(T_TRIVIAL, t);
}

static type_info* _make_pointer(type_info* to) {
	pointer* t = malloc(sizeof(pointer));
	t->value = to;
	return _make_type(T_POINTER, t);
}

static type_info* _make_array(type_info* t, int sz) {
	array* a = malloc(sizeof(array));
	a->value = t;
	a->size  = sz;
	return _make_type(T_ARRAY, a);
}

type_info* type(token_stream* s) {
	type_info* t = NULL;

	while(syntax_match_token(s, ASTERISK)) {
		t = _make_pointer(type(s));
	} 

	if(t == NULL) {
		token* _t = match_trivial_type(s);
		if(_t == NULL) {
			syntax_error_on_current(s, "trivial type required");
		}
		t = _make_trivial(_t);
	}

	while(syntax_match_token(s, LSQBRACE)) {
		int sz = syntax_consume_token(s, INTEGER, "array size required after type specification")->integer_value;
		syntax_consume_token(s, RSQBRACE, "']' required after array size specification");
		t = _make_array(t, sz);
	} 

	return t;
}

void type_accept(type_info* t, ast_visitor v) {
	v.visit_type(t);
}

