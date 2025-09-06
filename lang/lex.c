#include <lex.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "util.h"

#define STREAM_EOF (1 << 0)

DEFINE_MAP_TYPE(enum lexem, const char*, token, builtin_string_hash, builtin_string_comparator)

token_map* _reserved_words;

static token _eof_token = {.type = _EOF};

static void _lex_stream_append(token_stream* stream, token* s) {
    if(!stream->capacity) {
        stream->capacity = 1;
        stream->tokens = malloc(sizeof(token*) * stream->capacity);
    } else if (stream->capacity <= stream->size) {
        stream->capacity *= 2; 
        stream->tokens = realloc(stream->tokens, sizeof(token*) * stream->capacity);
    }
    stream->tokens[stream->size] = s;
    stream->size++;
}

token* _lex_create_token(token_stream* s, enum lexem type) {
    token* l = malloc(sizeof(token));
    l->type = type;
    l->string_value = NULL;
    l->integer_value = 0;
    l->double_value = 0;
    _lex_stream_append(s, l);
    return l;
}

#define SIMPLE_MATCH(char, type) \
    case char: \
        _lex_create_token(stream, type); \
        break;

#define SIMPLE_MATCH(char, type) \
    case char: \
        _lex_create_token(stream, type); \
        break;

#define DOUBLE_MATCH(start, next, type1, type2) \
    case start: \
        SUBMATCH(next, type2) \
        FALLBACK(type1) \
        break;

#define SUBMATCH_CALL(char, code) \
    if(_match(is, char)) { \
        code \
    } else

#define SUBMATCH(char, type) \
    SUBMATCH_CALL(char, _lex_create_token(stream, type);)

#define FALLBACK(type) \
    { \
        _lex_create_token(stream, type); \
    }

#define IGNORE(char) \
    case char: \
        break;

#define NEWLINE() \
    case '\n': \
        is->line++; \
        break;

#define COMMENT() \
    case '/': \
        if (_match(is, '/')) { \
        } else { \
            _lex_create_token(stream, SLASH); \
        } \
        break;

#define STRING() \
    case '"': \
        WITH_CODE(string(is, stream), "Unterminated string. Code: %d"); \
        break;

#define NUMBER() \
    WITH_CODE(number(is, stream), "Ill-formed number. Code: %d")

#define IDENTIFIER() \
    WITH_CODE(identifier(is, stream) , "Ill-formed identifier. Code: %d")

typedef struct {
    char* data;
    int ptr;
    int size;
    int line;
} input_stream;

static char _is_eof(input_stream* s) {
    return s->ptr >= s->size;
}

static char _current(input_stream* s) {
    if(_is_eof(s)) {
        return '\0';
    }
    return s->data[s->ptr];
}

static char _next(input_stream* s) {
    if(_is_eof(s)) {
        return '\0';
    }
    return s->data[s->ptr + 1];
}

static char _advance(input_stream* s) {
    char r = _current(s);
    s->ptr++;
    return r;
}

static int _match(input_stream* s, char c) {
    if(_current(s) == c) {
        s->ptr++;
        return 1;
    } else {
        return 0;
    }
}

static char* _slice(input_stream* s, int start) {
    int size = s->ptr - start + 1;
    char* buf = calloc(size + 1, 1);
    strncpy(buf, &s->data[start - 1], size);
    return buf;
}

static int _stream_from_input(char* const input, input_stream** result) {
    input_stream* s = malloc(sizeof(input_stream));
    if(s == NULL) {
        return 1;
    }
    s->data = input; 
    s->ptr = 0;
    s->size = strlen(input);
    *result = s;
    return 0;
}

static int string(input_stream* input, token_stream* stream) {
    _advance(input);

    int start = input->ptr;

    while(_current(input) != '"' && !_is_eof(input)) {
        if(_current(input) == '\n') {
            input->line++;
        }
        _advance(input);
    }

    if(_is_eof(input)) {
        return 1;
    }

    token* l = _lex_create_token(stream, STRING);
    l->string_value = _slice(input, start);

    _advance(input);

    return 0;
}

static int number(input_stream* input, token_stream* stream) {
    int start = input->ptr;

    int d = 0;

    while(isdigit(_current(input))) {
        _advance(input);
    }

    if(_current(input) == '.') {
        d = 1;
        _advance(input);
        while(isdigit(_current(input))) {
            _advance(input);
        }
    }

    char* tmp = _slice(input, start);

    if(d == 0) {
        token* s = _lex_create_token(stream, INTEGER);
        s->integer_value = atoi(tmp);
    } else {
        token* s = _lex_create_token(stream, NUMERIC);
        s->double_value = atof(tmp);
    }

    free(tmp);

    return 0;
}

static int identifier(input_stream* input, token_stream* stream) {

    int start = input->ptr;

    while(isalnum(_current(input))) {
        _advance(input);
    } 

    char* ident = _slice(input, start);

    enum lexem* type = token_map_get(_reserved_words, ident);

    token* l = NULL;

    if(type) {
        l = _lex_create_token(stream, *type);
    } else {
        l = _lex_create_token(stream, IDENTIFIER);
    }

    l->string_value = ident;

    return 0;
}

static int comment(input_stream* input, int multiline) {
    if(multiline) {
        while((_current(input) != '*' || _next(input) != '/') && !_is_eof(input)) {
            char c = _advance(input);
            if(c == '/' && _match(input, '*')) {
                comment(input, 1);
            }
        }
        if(_is_eof(input)) {
            return 1;
        } else {
            _advance(input);
            _advance(input);
        }
    } else {
        while(_current(input) != '\n' && !_is_eof(input)) {
            _advance(input);
        }
    }

    return 0;
}

void lex_init() {
    _reserved_words = token_map_create();

    token_map_insert(_reserved_words, "while", WHILE);
    token_map_insert(_reserved_words, "for", FOR);
    token_map_insert(_reserved_words, "do", DO);
    token_map_insert(_reserved_words, "if", IF);
    token_map_insert(_reserved_words, "switch", SWITCH);
    token_map_insert(_reserved_words, "u8", U8);
    token_map_insert(_reserved_words, "u16", U16);
    token_map_insert(_reserved_words, "u32", U32);
    token_map_insert(_reserved_words, "u64", U64);
    token_map_insert(_reserved_words, "i8", I8);
    token_map_insert(_reserved_words, "i16", I16);
    token_map_insert(_reserved_words, "i32", I32);
    token_map_insert(_reserved_words, "i64", I64);
    token_map_insert(_reserved_words, "float", FLOAT);
    token_map_insert(_reserved_words, "double", DOUBLE);
    token_map_insert(_reserved_words, "str", STR);
    token_map_insert(_reserved_words, "const", CONST);
    token_map_insert(_reserved_words, "void", VOID);
    token_map_insert(_reserved_words, "null", NIL);
    token_map_insert(_reserved_words, "true", TRUE);
    token_map_insert(_reserved_words, "false", FALSE);
}

int lex(char* const input, token_stream** _stream) {
    input_stream* is;
    token_stream* stream = lex_stream_create();

    int code = 0;
    WITH_CODE(_stream_from_input(input, &is), "Failed to create input stream. Code: %d");

    while(!_is_eof(is)) {
        char c = _advance(is);
        switch(c) {
        SIMPLE_MATCH(';', SEMILOCON)
        SIMPLE_MATCH(':', COLON)
        SIMPLE_MATCH(',', PERIOD)
        SIMPLE_MATCH('.', DOT)
        SIMPLE_MATCH('{', LBRACE)
        SIMPLE_MATCH('}', RBRACE)
        SIMPLE_MATCH('#', HASH)
        case '~':
            SUBMATCH('=', TILDA_EQUAL)
            FALLBACK(TILDA)
        break;
        case '[':
            SUBMATCH('[', LSQBRACE_DOUBLE)
            FALLBACK(LSQBRACE)
        break;
        case ']':
            SUBMATCH(']', RSQBRACE_DOUBLE)
            FALLBACK(RSQBRACE)
        break;
        SIMPLE_MATCH('(', LPAREN)
        SIMPLE_MATCH(')', RPAREN)
        DOUBLE_MATCH('=', '=', EQUAL, EQUAL_EQUAL)
        DOUBLE_MATCH('!', '=', BANG, BANG_EQUAL)
		DOUBLE_MATCH('&', '&', AMPERSAND, DOUBLE_AMPERSAND)
		DOUBLE_MATCH('|', '|', OR, DOUBLE_OR)
        SIMPLE_MATCH('^', XOR)
        case '>':
            SUBMATCH('>', DOUBLE_GREATER)
            SUBMATCH('=', GREATER_EQUAL)
            FALLBACK(GREATER)
        break;
        case '<':
            SUBMATCH('<', DOUBLE_LESS)
            SUBMATCH('=', LESS_EQUAL)
            FALLBACK(GREATER)
        break;
        case '-':
            SUBMATCH('>', POINTER)
            SUBMATCH('-', DOUBLE_MINUS)
            SUBMATCH('=', MINUS_EQUAL)
            FALLBACK(MINUS)
        break;
        case '+':
            SUBMATCH('+', DOUBLE_PLUS)
            SUBMATCH('=', PLUS_EQUAL)
            FALLBACK(PLUS)
        break;
        case '*':
            SUBMATCH('=', ASTERISK_EQUAL)
            FALLBACK(ASTERISK)
        break;
        case '/':
            SUBMATCH('=', SLASH_EQUAL)
            SUBMATCH_CALL('/', comment(is, 0);)
            SUBMATCH_CALL('*', WITH_CODE(comment(is, 1), "Unterminated multiline comment. Code: %d");)
            FALLBACK(SLASH)
        break;
        IGNORE(' ')
        IGNORE('\t')
        IGNORE('\r')
        NEWLINE()
        STRING()
        default:
            if(isdigit(c)) {
                NUMBER();
            } else if (isalpha(c)) {
                IDENTIFIER();
            } else {
                printf("Unexpected token: %c at line %d", c, is->line);
                return 1;
            }
        }
    }

    *_stream = stream;

    return 0;
}

token_stream* lex_stream_create() {
    return calloc(1, sizeof(token_stream));
}

void lex_stream_advance(token_stream* stream) {
    if(stream->flags & STREAM_EOF) {
        return;
    }
    stream->ptr++;
    if(stream->ptr == stream->size) {
        stream->flags |= STREAM_EOF;
    }
}

token* lex_stream_current(token_stream* stream) {
    if(stream->flags & STREAM_EOF) {
        return &_eof_token;
    }
    return stream->tokens[stream->ptr];
}

token* lex_stream_previous(token_stream* stream) {
	if(stream->ptr == 0) {
        return &_eof_token;
	}

	return stream->tokens[stream->ptr - 1];
}

token* lex_stream_next(token_stream* stream) {
	if(stream->flags & EOF) {
		return &_eof_token;
	}

	if(stream->ptr == stream->size - 1) {
		return &_eof_token;
	}

	return stream->tokens[stream->ptr + 1];
}

void lex_stream_free(token_stream* stream) {
    for(int i = 0; i < stream->size; i++) {
        if(stream->tokens[i]->string_value) {
            free(stream->tokens[i]->string_value);
        }
        free(stream->tokens[i]);
    }
    free(stream->tokens);
    free(stream);
}

void lex_stream_rewind(token_stream* stream) {
	stream->ptr = 0;
	stream->flags = 0;
}

#define LT(x) \
	case x: \
		return #x;

const char* lex_lexem_to_string(enum lexem t) {
	switch(t) {
    LT(SEMILOCON)
    LT(COLON)
    LT(PERIOD)
    LT(DOT)
    LT(PLUS)
    LT(DOUBLE_PLUS)
    LT(MINUS)
    LT(DOUBLE_MINUS)
    LT(SLASH)
    LT(ASTERISK)
    LT(RBRACE)
    LT(LBRACE)
    LT(RSQBRACE)
    LT(LSQBRACE)
    LT(RSQBRACE_DOUBLE)
    LT(LSQBRACE_DOUBLE)
    LT(RPAREN)
    LT(LPAREN)
    LT(LESS)
    LT(GREATER)
    LT(EQUAL)
    LT(EQUAL_EQUAL)
    LT(LESS_EQUAL)
    LT(GREATER_EQUAL)
    LT(PLUS_EQUAL)
    LT(MINUS_EQUAL)
    LT(ASTERISK_EQUAL)
    LT(SLASH_EQUAL)
    LT(DOUBLE_GREATER)
    LT(DOUBLE_LESS)
    LT(BANG)
    LT(BANG_EQUAL)
	LT(AMPERSAND)
	LT(DOUBLE_AMPERSAND)
	LT(OR)
	LT(DOUBLE_OR)
	LT(XOR)
    LT(POINTER)
    LT(STRING)
    LT(INTEGER)
    LT(NUMERIC)
    LT(WHILE)
    LT(IF)
    LT(FOR)
    LT(DO)
    LT(SWITCH)
    LT(U8)
    LT(U16)
    LT(U32)
    LT(U64)
    LT(I8)
    LT(I16)
    LT(I32)
    LT(I64)
    LT(FLOAT)
    LT(DOUBLE)
    LT(STR)
    LT(VOID)
    LT(CONST)
    LT(NIL)
    LT(TRUE)
    LT(FALSE)
    LT(HASH)
    LT(TILDA)
    LT(TILDA_EQUAL)
    LT(IDENTIFIER)
	LT(_EOF)
	default:
		return "UNKNOWN";
	}
}
