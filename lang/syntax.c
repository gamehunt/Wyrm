#include <syntax.h>
#include <stdlib.h>

syntax_tree* syntax_tree_create() {
    syntax_tree* r = malloc(sizeof(syntax_tree));
    return r;
}

void syntax_tree_free(syntax_tree* tree) {
    free(tree);
}

int syntax_build_tree(lexem_stream* stream, syntax_tree** result) {
    syntax_tree* tree = syntax_tree_create();
    *result = tree;
    return 0;
}
