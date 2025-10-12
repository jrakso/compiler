#pragma once

// Different Tokens
enum TokenType {
    TOKEN_RETURN,
    TOKEN_EXIT,
    TOKEN_INT_LITERAL,
    TOKEN_SEMICOLON,
    TOKEN_NULL
};

// Represents a single Token
typedef struct {
    enum TokenType type;
    char *value;
} Token;

// Dynamic Token array
typedef struct {
    Token *tokens;
    size_t size;
    size_t capacity;
} TokenArray;


// typedef struct {
//     const char *input;  // pointer to input string
//     size_t length;    // Number of characters in input string
//     size_t index;       // current index in string
// } Tokenizer;

TokenArray tokenize(const char *string);