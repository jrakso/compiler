#pragma once

#include <stddef.h>

typedef enum {
    TOKEN_RETURN,
    TOKEN_EXIT,
    TOKEN_INT_LITERAL,
    TOKEN_SEMICOLON,
    TOKEN_NULL
} TokenType;

typedef struct {
    TokenType type;
    char *value;
} Token;

typedef struct {
    Token *tokens;
    size_t size;
    size_t capacity;
} TokenArray;

typedef struct {
    const char *src;
    size_t len;
    size_t pos;
} Tokenizer;

void tokenizer_init(Tokenizer *t, const char *src);
TokenArray tokenize(Tokenizer *t);
void token_array_free(TokenArray *arr);
