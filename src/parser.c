#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tokenization.h"

void parser_init(Parser *p, const TokenArray *arr) {
    p->tokens = arr->tokens;
    p->len = arr->size;
    p->pos = 0;
}

static const Token *parser_peek(const Parser *p, size_t offset) {
    if (p->pos + offset >= p->len) return NULL;
    return &p->tokens[p->pos + offset];
}

static const Token *parser_consume(Parser *p) {
    return &p->tokens[p->pos++];
}

NodeExpr *parse_expr(Parser *p) {
    const Token *tok = parser_peek(p, 0);
    if (tok && tok->type == TOKEN_INT_LITERAL) {
        NodeExpr *expr_node = malloc(sizeof(NodeExpr));  // caller frees with ast_free
        if (!expr_node) {
            perror("malloc");
            exit(1);
        }
        expr_node->int_lit = parser_consume(p);
        return expr_node;
    }
    return NULL;
}

NodeExit *parse(Parser *p) {
    NodeExit *exit_node = malloc(sizeof(NodeExit));  // caller frees with ast_free
    if (!exit_node) {
        perror("malloc");
        exit(1);
    }

    while (parser_peek(p, 0)) {
        const Token *tok = parser_peek(p, 0);
        if (tok->type == TOKEN_EXIT) {
            parser_consume(p);

            NodeExpr *expr_node = parse_expr(p);
            if (!expr_node) {
                fprintf(stderr, "Expected integer literal after 'exit'\n");
                free(exit_node);
                exit(1);
            }
            exit_node->expr = expr_node;

            const Token *semi = parser_peek(p, 0);
            if (!semi || semi->type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Missing semicolon after expression\n");
                ast_free(exit_node);
                exit(1);
            }
            parser_consume(p);
            return exit_node;
        }
        p->pos++;
    }

    free(exit_node);
    return NULL;
}

void ast_free(NodeExit *node) {
    if (!node) return;

    if (node->expr) {
        free(node->expr);  // frees NodeExpr allocated by parse_expr
    }
    free(node);  // frees NodeExit allocated by parse
}