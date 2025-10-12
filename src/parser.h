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

void init_parser(Parser *p, const TokenArray *tokens);
NodeExit *parse(Parser *p);
void ast_free(NodeExit *node);