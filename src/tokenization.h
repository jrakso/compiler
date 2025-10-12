#pragma once

enum TokenType {
    TOKEN_RETURN,
    TOKEN_EXIT,
    TOKEN_INT_LITERAL,
    TOKEN_SEMICOLON,
    TOKEN_NULL
};

typedef struct {
    enum TokenType type;
    char *value;
} Token;

typedef struct {
    Token *tokens;
    size_t size;
    size_t capacity;
} TokenArray;

typedef struct {
    const char *input;
    size_t length;
    size_t index;
} Tokenizer;

void tokenizer_init(Tokenizer *t, const char *input);
TokenArray tokenize(Tokenizer *t);
void token_array_free(TokenArray *arr);