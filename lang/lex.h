#ifndef _LEX_H
#define _LEX_H 1

enum lexem_type {
    SEMILOCON,
    COLON,
    PERIOD,
    DOT,
    PLUS,
    DOUBLE_PLUS,
    MINUS,
    DOUBLE_MINUS,
    SLASH,
    ASTERISK,
    RBRACE,
    LBRACE,
    RSQBRACE,
    LSQBRACE,
    RSQBRACE_DOUBLE,
    LSQBRACE_DOUBLE,
    RPAREN,
    LPAREN,
    LESS,
    GREATER,
    EQUAL,
    EQUAL_EQUAL,
    LESS_EQUAL,
    GREATER_EQUAL,
    PLUS_EQUAL,
    MINUS_EQUAL,
    ASTERISK_EQUAL,
    SLASH_EQUAL,
    DOUBLE_GREATER,
    DOUBLE_LESS,
    BANG,
    BANG_EQUAL,
	AMPERSAND,
	DOUBLE_AMPERSAND,
	OR,
	DOUBLE_OR,
	XOR,
    POINTER,
    STRING,
    INTEGER,
    NUMERIC,
    WHILE,
    IF,
    FOR,
    DO,
    SWITCH,
    U8,
    U16,
    U32,
    U64,
    I8,
    I16,
    I32,
    I64,
    FLOAT,
    DOUBLE,
    STR,
    VOID,
    CONST,
    NIL,
    TRUE,
    FALSE,
    HASH,
    TILDA,
    TILDA_EQUAL,
    IDENTIFIER
};

typedef struct {
    enum lexem_type type;
    char*  string_value;
    int    integer_value;
    double double_value;
} lexem;

typedef struct {
    lexem** lexems;
    int capacity;
    int size;
    int ptr;
    int flags;
} lexem_stream;

int lex(char* const input,  lexem_stream** stream);

void lex_init();

lexem_stream* lex_stream_create();
void lex_stream_free(lexem_stream* stream);
void lex_stream_advance(lexem_stream* stream);
lexem* lex_stream_current(lexem_stream* stream);
lexem* lex_stream_previous(lexem_stream* stream);
lexem* lex_stream_next(lexem_stream* stream);
void lex_stream_rewind(lexem_stream* stream);

const char* lex_type_to_string(enum lexem_type t);
#define lex_current_to_string(s) lex_type_to_string(lex_stream_current(s)->type)

#endif
