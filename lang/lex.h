#ifndef _LEX_H
#define _LEX_H 1

enum lexem_type {
    SEMILOCON,
    PERIOD,
    DOT,
    PLUS,
    MINUS,
    SLASH,
    ASTERISK,
    RBRACE,
    LBRACE,
    RSQBRACE,
    LSQBRACE,
    RPAREN,
    LPAREN,
    LESS,
    GREATER,
    EQUAL,
    EQUAL_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,
    BANG,
    BANG_EQUAL,
    POINTER,
    STRING,
    INTEGER,
    NUMERIC,
    WHILE,
    IF,
    FOR,
    DO,
    SWITCH,
    IDENTIFIER
};

typedef struct {
    enum lexem_type type;
    char* string_value;
    int integer_value;
    double double_value;
} lexem;

typedef struct {
    lexem** lexems;
    int capacity;
    int size;
    int ptr;
    int flags;
} lexem_stream;

int lex(char* const input,  lexem_stream* stream);

lexem_stream* lex_stream_create();
void lex_stream_advance(lexem_stream* stream);
lexem* lex_stream_current(lexem_stream* stream);

#endif
