#ifndef _LEX_H
#define _LEX_H 1

enum lexem {
	_EOF,
    SEMILOCON,
    COLON,
    COMMA,
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
	ELSE,
    FOR,
    DO,
    SWITCH,
	RETURN,
	CONTINUE,
	BREAK,
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
    TILDA,
    TILDA_EQUAL,
	CLASS,
	UNION,
	LET,
	FUN,
    IDENTIFIER,
	TYPEDEF,
	SIZEOF,
	PUBLIC,
	PRIVATE,
	PROTECTED
};

typedef struct {
    enum lexem type;
	int line;
    char*  string_value;
    int    integer_value;
    double double_value;
} token;

typedef struct {
    token** tokens;
    int capacity;
    int size;
    int ptr;
    int flags;
	int last_line;
} token_stream;

int lex(char* const input,  token_stream* stream);

void lex_init();

token_stream* lex_stream_create();
void lex_stream_free(token_stream* stream);
void lex_stream_advance(token_stream* stream);
token* lex_stream_current(token_stream* stream);
token* lex_stream_previous(token_stream* stream);
token* lex_stream_next(token_stream* stream);
void lex_stream_rewind(token_stream* stream);
int lex_stream_is_eof(token_stream* stream);

const char* lex_lexem_to_string(enum lexem t);
#define lex_current_to_string(s) lex_lexem_to_string(lex_stream_current(s)->type)

#endif
