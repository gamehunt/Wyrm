#include <stdio.h>

#include "class.h"
#include "expr.h"
#include "lex.h"
#include "program.h"
#include "statement.h"
#include "syntax.h"
#include "type.h"

static void _syntax_printer_visit_unary_expr(unary_expr* e);
static void _syntax_printer_visit_bin_expr(binary_expr* e);
static void _syntax_printer_visit_group_expr(group_expr* e);
static void _syntax_printer_visit_literal_expr(literal_expr* e);
static void _syntax_printer_visit_assignment_expr(assignment_expr* e);
static void _syntax_printer_visit_program(prog* e);
static void _syntax_printer_visit_decl_stmt(decl* e);
static void _syntax_printer_visit_expr_stmt(expr* e);
static void _syntax_printer_visit_block_stmt(stmt_list* e);
static void _syntax_printer_visit_if_stmt(conditional* e);
static void _syntax_printer_visit_for_stmt(for_loop* e);
static void _syntax_printer_visit_while_stmt(while_loop* e);
static void _syntax_printer_visit_ret_stmt(expr* e);
static void _syntax_printer_visit_call_expr(call_expr* e);
static void _syntax_printer_visit_subscript_expr(subscript_expr* e);
static void _syntax_printer_visit_fun_def(fun_def* e);
static void _syntax_printer_visit_loop_ctrl_stmt(token* e);
static void _syntax_printer_visit_type(type_info* e);
static void _syntax_printer_visit_class(class_info* e);
static void _syntax_printer_visit_typedef(typedef_stmt* e);

ast_visitor _ast_printer = {
	.visit_expr = NULL,
	.visit_binary_expr     = _syntax_printer_visit_bin_expr,
	.visit_unary_expr      = _syntax_printer_visit_unary_expr,
	.visit_group_expr      = _syntax_printer_visit_group_expr,
	.visit_literal_expr    = _syntax_printer_visit_literal_expr,
	.visit_assignment_expr = _syntax_printer_visit_assignment_expr,
	.visit_program         = _syntax_printer_visit_program,
	.visit_decl_stmt       = _syntax_printer_visit_decl_stmt,
	.visit_expr_stmt       = _syntax_printer_visit_expr_stmt,
	.visit_block_stmt      = _syntax_printer_visit_block_stmt,
	.visit_for_stmt        = _syntax_printer_visit_for_stmt,
	.visit_if_stmt         = _syntax_printer_visit_if_stmt,
	.visit_while_stmt      = _syntax_printer_visit_while_stmt,
	.visit_call_expr       = _syntax_printer_visit_call_expr,
	.visit_ret_stmt        = _syntax_printer_visit_ret_stmt,
	.visit_fun_def_stmt    = _syntax_printer_visit_fun_def,
	.visit_subscript_expr  = _syntax_printer_visit_subscript_expr,
	.visit_loop_ctrl_stmt  = _syntax_printer_visit_loop_ctrl_stmt,
	.visit_type            = _syntax_printer_visit_type,
	.visit_class           = _syntax_printer_visit_class,
	.visit_typedef         = _syntax_printer_visit_typedef
};

static void _syntax_printer_visit_unary_expr(unary_expr* e) {
	printf("[");
	if(e->postfix) {
		expr_accept(e->right, _ast_printer);
		printf(" %s ", lex_lexem_to_string(e->op));
	} else {
		printf(" %s ", lex_lexem_to_string(e->op));
		expr_accept(e->right, _ast_printer);
	}
	printf("]");
}

static void _syntax_printer_visit_bin_expr(binary_expr* e) {
	printf("[");
	expr_accept(e->left, _ast_printer);
	printf(" %s ", lex_lexem_to_string(e->op));
	expr_accept(e->right, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_group_expr(group_expr* e) {
	printf("GROUP [");
	expr_accept(e->expr, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_literal_expr(literal_expr* e) {
	switch(e->value->type) {
		case NIL:
			printf("NIL");
			break;
		case TRUE:
			printf("TRUE");
			break;
		case FALSE:
			printf("FALSE");
			break;
		case NUMERIC:
			printf("%f", e->value->double_value);
			break;
		case INTEGER:
			printf("%d", e->value->integer_value);
			break;
		case STRING:
		case IDENTIFIER:
			printf("%s", e->value->string_value);
			break;
		default:
			printf("UNKNOWN");
			break;
	}
}

static void _syntax_printer_visit_assignment_expr(assignment_expr* e) {
	printf("ASSIGNMENT [");
	expr_accept(e->lvalue, _ast_printer);
	printf(" %s ", lex_lexem_to_string(e->op));
	expr_accept(e->rvalue, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_program(prog* e) {
	printf("PROG [\n");
	for(int i = 0; i < e->statements->size; i++) {
		stmt_accept(e->statements->data[i], _ast_printer);
		printf("\n");
	}
	printf("]\n");
}

static void _syntax_printer_visit_decl_stmt(decl* e) {
	printf("DECL [");
	for(int i = 0; i < e->specifiers->size; i++) {
		printf("%s ", lex_lexem_to_string(e->specifiers->data[i]));
	}
	type_accept(e->type, _ast_printer);
	printf(" %s ", e->identifier->string_value);
	if(e->initializer) {
		printf(" := ");
		expr_accept(e->initializer, _ast_printer);
	}
	printf("]");
}

static void _syntax_printer_visit_expr_stmt(expr* e) {
	printf("EXPR [");
	expr_accept(e, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_block_stmt(stmt_list* e) {
	printf("[");
	for(int i = 0; i < e->size; i++) {
		stmt_accept(e->data[i], _ast_printer);
		printf("\n");
	}
	printf("]");
}

static void _syntax_printer_visit_if_stmt(conditional* e) {
	printf("IF [{");
	expr_accept(e->condition, _ast_printer);
	printf("}\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
	if(e->branch) {
		printf("\nELSE [\n");
		stmt_accept(e->branch, _ast_printer);
		printf("]");
	}
}

static void _syntax_printer_visit_for_stmt(for_loop* e) {
	printf("FOR [{");
	if(e->initializer) {
		stmt_accept(e->initializer, _ast_printer);
	}
	printf("}\n{");
	if(e->condition) {
		expr_accept(e->condition, _ast_printer);
	}
	printf("}\n{");
	if(e->increment) {
		expr_accept(e->increment, _ast_printer);
	}
	printf("}\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_while_stmt(while_loop* e) {
	if(e->prefix) {
		printf("DO-WHILE [{");
	} else {
		printf("WHILE [{");
	}
	expr_accept(e->condition, _ast_printer);
	printf("}\n");
	stmt_accept(e->body, _ast_printer);
	printf("]");
}

static void _syntax_printer_visit_call_expr(call_expr* e) {
	printf("CALL [");
	expr_accept(e->callee, _ast_printer);
	printf(" (");
	for(int i = 0; i < e->args->size; i++) {
		expr_accept(e->args->data[i], _ast_printer);
		printf(", ");
	}
	printf(")]");
}

static void _syntax_printer_visit_subscript_expr(subscript_expr* e) {
	printf("SUBS [");
	expr_accept(e->array, _ast_printer);
	printf("[");
	expr_accept(e->index, _ast_printer);
	printf("]]");
}

static void _syntax_printer_visit_ret_stmt(expr* v) {
	printf("RET ");
	if(v) {
		expr_accept(v, _ast_printer);
	} 
}

static void _syntax_printer_visit_fun_def(fun_def* e) {
	printf("FUNC [");
	for(int i = 0; i < e->specifiers->size; i++) {
		printf("%s ", lex_lexem_to_string(e->specifiers->data[i]));
	}
	type_accept(e->ret_type, _ast_printer);
	printf(" %s ", e->identifier->string_value);
	printf("(");
	for(int i = 0; i < e->params->size; i++) {
		stmt_accept(e->params->data[i], _ast_printer);
		printf(", ");
	}
	printf(")");
	printf("\n");
	if(e->body) {
		stmt_accept(e->body, _ast_printer);
	}
	printf("]");
}

static void _syntax_printer_visit_loop_ctrl_stmt(token* e) {
	printf("[%s]", lex_lexem_to_string(e->type));
}

static void _syntax_printer_visit_type(type_info* e) {
	switch(e->type) {
		case T_TRIVIAL:
			printf("%s", lex_lexem_to_string(((token*) e->data)->type));
			break;
		case T_POINTER:
			printf("*");
			type_accept(((pointer*) e->data)->value, _ast_printer);
			break;
		case T_ARRAY:
			type_accept(((array*) e->data)->value, _ast_printer);
			printf("[%d]", ((array*) e->data)->size);
	}	
}

static void _syntax_printer_visit_class(class_info* e) {
	printf("CLASS %s [\n", e->identifier->string_value);
	if(e->body) {
		for(int i = 0; i < e->body->size; i++) {
			printf("%s ", access_qualifier_to_string(e->body->data[i]->qualifier));
			stmt_accept(e->body->data[i]->declaration, _ast_printer);	
			printf("\n");
		}
	}
	printf("]");	
}

static void _syntax_printer_visit_typedef(typedef_stmt* e) {
	printf("TYPEDEF ");
	type_accept(e->type, _ast_printer);
	printf(" -> %s", e->alias->string_value);
}
