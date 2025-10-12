#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tokenization.h"

// typedef struct {
//     NodeExpr *expr;
// } NodeExit;

// typedef struct {
//     Token int_lit;
// } NodeExpr;

static const Token *peek(const Parser *p, size_t ahead) {
    if (p->index + ahead >= p->length) {
        return NULL;
    }
    return &p->tokens[p->index + ahead];
}

static Token consume(Parser *p) {
    return p->tokens[p->index++];
}

static void init_parser(Parser *p, const TokenArray *tokens) {
    p->tokens = tokens->tokens;
    p->length = tokens->size;
    p->index = 0;
}

NodeExpr *parse_expr(Parser *p) {
    if (peek(p, 0) != NULL && peek(p, 0)->type == TOKEN_INT_LITERAL) {
        NodeExpr *expr_node = malloc(sizeof(NodeExpr));
        expr_node->int_lit = consume(p);
        return expr_node;
    }
    else {
        return NULL;
    }
}

NodeExit *parse(const TokenArray *tokens) {
    Parser p;
    init_parser(&p, tokens);
    NodeExit *exit_node = malloc(sizeof(NodeExit));
    while (peek(&p, 0) != NULL) {
        if (peek(&p, 0)->type == TOKEN_EXIT) {
            consume(&p);
            NodeExpr *expr_node = parse_expr(&p);
            if (expr_node != NULL) {
                exit_node->expr = expr_node;
            }
            else {
                fprintf(stderr, "Invalid expression");
                exit(1);
            }
            if (peek(&p, 0) != NULL && peek(&p, 0)->type == TOKEN_SEMICOLON) {
                consume(&p);
            }
            else {
                fprintf(stderr, "Invalid expression");
                exit(1);
            }
            p.index = 0;
            return exit_node;
        }
    }
}