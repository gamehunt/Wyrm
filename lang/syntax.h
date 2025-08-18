#ifndef _SYNTAX_H
#define _SYNTAX_H 1

#include <lex.h>

typedef struct {

} syntax_tree;

int syntax_build_tree(lexem_stream* stream, syntax_tree** result);

syntax_tree* syntax_tree_create();
void syntax_tree_free(syntax_tree* tree);

#endif
