#pragma once

#include "tokenization.h"

typedef struct {
    Token int_lit;
} NodeExpr;

typedef struct {
    NodeExpr *expr;
} NodeExit;

typedef struct {
    const Token *tokens;
    size_t length;
    size_t index;
} Parser;

NodeExit *parse(const TokenArray *tokens);