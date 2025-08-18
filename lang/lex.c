#include "util.h"
#include <ctype.h>
#include <lex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STREAM_EOF (1 << 0)

static void _lex_stream_append(lexem_stream* stream, lexem* s) {
    if(!stream->capacity) {
        stream->capacity = 1;
        stream->lexems = malloc(sizeof(lexem*) * stream->capacity);
    } else if (stream->capacity <= stream->size) {
        stream->capacity *= 2; 
        stream->lexems = realloc(stream->lexems, sizeof(lexem*) * stream->capacity);
    }
    stream->lexems[stream->size] = s;
    stream->size++;
}

lexem* _lex_create_token(lexem_stream* s, enum lexem_type type) {
    lexem* l = malloc(sizeof(lexem));
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
        _lex_create_token(stream, _match(is, next) ? type2 : type1); \
        break;

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
            while(_current(is) != '\n') { \
                _advance(is); \
            } \
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

static int string(input_stream* input, lexem_stream* stream) {
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

    lexem* l = _lex_create_token(stream, STRING);
    l->string_value = _slice(input, start);

    _advance(input);

    return 0;
}

static int number(input_stream* input, lexem_stream* stream) {
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
        lexem* s = _lex_create_token(stream, INTEGER);
        s->integer_value = atoi(tmp);
    } else {
        lexem* s = _lex_create_token(stream, NUMERIC);
        s->double_value = atof(tmp);
    }

    free(tmp);

    return 0;
}

static int identifier(input_stream* input, lexem_stream* stream) {
    int start = input->ptr;

    while(isalnum(_current(input))) {
        _advance(input);
    } 

    lexem* l = _lex_create_token(stream, IDENTIFIER);
    l->string_value = _slice(input, start);

    return 0;
}

int lex(char* const input, lexem_stream* stream) {
    input_stream* is;

    int code = 0;
    WITH_CODE(_stream_from_input(input, &is), "Failed to create input stream. Code: %d");

    while(!_is_eof(is)) {
        char c = _advance(is);
        switch(c) {
        SIMPLE_MATCH(';', SEMILOCON)
        SIMPLE_MATCH(',', PERIOD)
        SIMPLE_MATCH('.', DOT)
        SIMPLE_MATCH('+', PLUS)
        SIMPLE_MATCH('*', ASTERISK)
        SIMPLE_MATCH('{', LBRACE)
        SIMPLE_MATCH('}', RBRACE)
        SIMPLE_MATCH('[', LSQBRACE)
        SIMPLE_MATCH(']', RSQBRACE)
        SIMPLE_MATCH('(', LPAREN)
        SIMPLE_MATCH(')', RPAREN)
        DOUBLE_MATCH('=', '=', EQUAL, EQUAL_EQUAL)
        DOUBLE_MATCH('!', '=', BANG, BANG_EQUAL)
        DOUBLE_MATCH('>', '=', GREATER, GREATER_EQUAL)
        DOUBLE_MATCH('<', '=', LESS, LESS_EQUAL)
        DOUBLE_MATCH('-', '>', MINUS, POINTER)
        IGNORE(' ')
        IGNORE('\t')
        IGNORE('\r')
        NEWLINE()
        COMMENT()
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

    return 0;
}

lexem_stream* lex_stream_create() {
    return calloc(1, sizeof(lexem_stream));
}

void lex_stream_advance(lexem_stream* stream) {
    if(stream->flags & STREAM_EOF) {
        return;
    }
    stream->ptr++;
    if(stream->ptr == stream->size) {
        stream->flags |= STREAM_EOF;
    }
}

lexem* lex_stream_current(lexem_stream* stream) {
    if(stream->flags & STREAM_EOF) {
        return NULL;
    }
    return stream->lexems[stream->ptr];
}
