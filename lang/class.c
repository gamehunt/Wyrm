#include "class.h"
#include "lex.h"
#include "list.h"
#include "syntax.h"

LIST_IMPL(q_stmt, qualified_statement*);

static enum access_qualifiers _tok_to_qualifier(token* t) {
	switch(t->type) {
		case PUBLIC:
			return A_PUBLIC;
		case PROTECTED:
			return A_PROTECTED;
		case PRIVATE:
		default:
			return A_PRIVATE;
	}	
}

q_stmt_list* class_body(token_stream* s) {
	q_stmt_list* l = q_stmt_list_create();
	while(!syntax_match_token(s, RBRACE)) {
		qualified_statement* qs = malloc(sizeof(qualified_statement));
		qs->qualifier = A_PRIVATE;
		token* t = match_access_qualifier(s);
		if(t) {
			qs->qualifier = _tok_to_qualifier(t);
		}
		if(syntax_match_token(s, LET)) {
			qs->declaration = var_decl(s);
		} else if(syntax_match_token(s, FUN)) {
			qs->declaration = fun_decl(s);
		} else {
			free(qs);
			q_stmt_list_free(l);
			syntax_error_on_current(s, "unexpected token");
		}
		q_stmt_list_append(l, qs);
	}
	return l;
}

class_info* class(token_stream* s) {
	class_info* ci = malloc(sizeof(class_info));
	ci->identifier = syntax_consume_token(s, IDENTIFIER, "identifier required after 'class'");
	if(syntax_match_token(s, LBRACE)) {
		ci->body = class_body(s);
	} else {
		ci->body = NULL;
	}
	return ci;
}

const char* access_qualifier_to_string(enum access_qualifiers ac) {
	switch(ac) {
		case A_PUBLIC:
			return "PUBLIC";
		case A_PROTECTED:
			return "PROTECTED";
		case A_PRIVATE:
			return "PRIVATE";
	}
}
