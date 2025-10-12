#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenization.h"

typedef struct {
    const char *input;  // pointer to input string
    size_t length;    // Number of characters in input string
    size_t index;       // current index in string
} Tokenizer;

static void init_array(TokenArray *arr) {
    arr->size = 0;
    arr->capacity = 4;
    arr->tokens = malloc(sizeof(Token) * arr->capacity);
}

static void push_token(TokenArray *arr, Token token) {
    if (arr->size == arr->capacity)
    {
        arr->capacity *= 2;  // Double capacity
        arr->tokens = realloc(arr->tokens, sizeof(Token) * arr->capacity);  // Reallocate memory for new capacity
    }
    arr->tokens[arr->size] = token;
    arr->size++;
}

static char *get_token_value(const char *string, size_t start, size_t end) {
    size_t length = end - start;
    char *value = malloc(sizeof(char) * (length + 1));
    memcpy(value, string + start, length);
    value[length] = '\0';
    return value;
}

static void tokenizer_init(Tokenizer *t, const char *input) {
    t->input = input;
    t->length = strlen(input);
    t->index = 0;
}

static char peek(const Tokenizer *t, size_t ahead) {
    if (t->index + ahead >= t->length || t->input[t->index + ahead] == '\0') {
        return '\0';
    }
    return t->input[t->index + ahead];
}

static char consume(Tokenizer *t) {
    return t->input[t->index++];
}


TokenArray tokenize(const char *string) {
    Tokenizer t;
    tokenizer_init(&t, string);
    TokenArray tokens;
    init_array(&tokens);
    size_t ahead = 0;

    while (peek(&t, ahead) != '\0') {

        if (isalpha(peek(&t, ahead))) {
            size_t start = t.index;
            consume(&t);
            while (isalnum(peek(&t, ahead))) {
                consume(&t);
            }
            size_t end = t.index;
            char *value = get_token_value(string, start, end);
            if (strcmp(value, "exit") == 0) {
                push_token(&tokens, (Token){.type = TOKEN_EXIT, .value = value});
                continue;
            } else {
                fprintf(stderr, "Error");
                exit(1);
            }
        }
        
        else if (isdigit(peek(&t, ahead))) {
            size_t start = t.index;
            consume(&t);
            while (isdigit(peek(&t, ahead))) {
                consume(&t);
            }
            size_t end = t.index;
            char *value = get_token_value(string, start, end);
            push_token(&tokens, (Token){.type = TOKEN_INT_LITERAL, .value = value});
            continue;
        }
        
        else if (peek(&t, ahead) == ';') {
            consume(&t);
            push_token(&tokens, (Token){.type = TOKEN_SEMICOLON, .value = NULL});
            continue;
        }

        else if (isspace(peek(&t, ahead))) {
            consume(&t);
            continue;
        }

        else {
            fprintf(stderr, "Error");
            exit(1);
        }

    }
    return tokens;
}