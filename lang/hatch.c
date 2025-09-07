#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lex.h>
#include <syntax.h>

#define ARG_INVALID_FLAG -1
#define ARG_OUTPUT_FLAG   1

#define MAX_INPUTS 128

void help() {
    printf("usage: hatch [flags] <files>");
}

int flag(char c) {
    switch(c) {
        case 'o':
            return ARG_OUTPUT_FLAG;
        default:
            return ARG_INVALID_FLAG;
    }
}

const char** inputs;
int inputs_amount = 0;
const char* output = NULL;

int parse_arguments(int argc, const char** argv) {
    int last_flag = 0;
    inputs = malloc(sizeof(char*) * argc);
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {
            last_flag = flag(argv[i][1]);
            if(last_flag == ARG_INVALID_FLAG) {
                return 1;
            }
        } else {
            if(last_flag != ARG_OUTPUT_FLAG) {
                inputs[inputs_amount] = argv[i];
                inputs_amount++;
            } else {
                output = argv[i];
            }
        }
    }
    return 0;
}

int read_file(const char* path, char** buffer_ptr) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(fp);
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, file_size, fp);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(fp);
        return 1;
    }

    buffer[file_size] = '\0';
    fclose(fp);

    *buffer_ptr = buffer;

    return 0;
}

int compile(char* const in) {
    int code = 0;
    
    token_stream* tokens;
    syntax_tree* ast;

    WITH_CODE_GOTO(lex(in, &tokens), "Failed to parse tokens. Code: %d\n");

	for(int i = 0; i < tokens->size; i++) {
		printf("%s ", lex_lexem_to_string(tokens->tokens[i]->type));
	}
	printf("\n\n");

    WITH_CODE_GOTO(syntax_build_tree(tokens, &ast), "Failed to build syntax tree. Code: %d\n");

	syntax_print_tree(ast);
    
error:
    lex_stream_free(tokens);

    return code;
}

int main(int argc, const char** argv) {
    int code = 0;
    
    if((code = parse_arguments(argc, argv))) {
        help();
        return 0;
    }

    lex_init();

    for(int i = 0; i < inputs_amount; i++) {
        char* buffer = NULL;

        WITH_CODE(read_file(inputs[i], &buffer), "Failed to read file. Code: %d\n");
        WITH_CODE(compile(buffer), "Failed to compile file. Code: %d\n");
        
        free(buffer);
    }

    return 0;
}
