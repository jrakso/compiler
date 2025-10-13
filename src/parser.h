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
    size_t len;
    size_t pos;
} Parser;

void parser_init(Parser *p, const TokenArray *arr);
NodeExit *parse(Parser *p);
void ast_free(NodeExit *node);