#pragma once

#include "tokenization.h"

typedef struct {
    const Token *int_lit;
} NodeExpr;

typedef struct {
    NodeExpr *expr;
} NodeExit;

typedef struct {
    const Token *tokens;
    size_t len;
    size_t pos;
} Parser;

void parser_init(Parser *p, const TokenArray *arr);  // does not allocate
NodeExit *parse(Parser *p);  // caller frees with ast_free
void ast_free(NodeExit *node);  // frees AST allocated by parse