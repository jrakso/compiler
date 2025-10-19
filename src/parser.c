#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tokenization.h"

void parser_init(Parser *p, const TokenArray *arr) {
    p->tokens = arr->tokens;
    p->size = arr->size;
    p->pos = 0;
}

static const Token parser_peek(const Parser *p, size_t offset) {
    if (p->pos + offset >= p->size) {
        return (Token) { .type = TOKEN_INVALID };
    }
    return p->tokens[p->pos + offset];
}

static Token parser_consume(Parser *p) {
    return p->tokens[p->pos++];
}

NodeExpr parse_expr(Parser *p) {
    if (parser_peek(p, PEEK_CURRENT).type == TOKEN_INT_LITERAL) {
        return (NodeExpr) { .type = EXPR_INT_LIT, .data.int_lit = (NodeExprLit) { .int_lit = parser_consume(p) } };
    }
    else if (parser_peek(p, PEEK_CURRENT).type == TOKEN_IDENT) {
        return (NodeExpr) { .type = EXPR_IDENT, .data.ident = (NodeExprIdent) { .ident = parser_consume(p) } };
    }
    else {
        return (NodeExpr) { .type = EXPR_INVALID };
    }
}

NodeStmt parse_stmt(Parser *p) {
    const Token t = parser_peek(p, PEEK_CURRENT);
    parser_consume(p);

    switch (t.type) {

        case TOKEN_EXIT: {

            NodeStmt node_stmt = { .type = STMT_EXIT };
            NodeExpr node_expr = { .type = EXPR_INVALID };  // start invalid

            if (parser_peek(p, PEEK_CURRENT).type == TOKEN_OPEN_PAREN) {
                parser_consume(p);
            } else {
                fprintf(stderr, "Expected '('\n");
                exit(EXIT_FAILURE);
            }

            node_expr = parse_expr(p);
            if (node_expr.type != EXPR_INVALID) { 

                node_stmt.data.exit.expr = node_expr; 

                if (parser_peek(p, PEEK_CURRENT).type == TOKEN_CLOSE_PAREN) {
                    parser_consume(p);
                } else {
                    fprintf(stderr, "Expected ')'\n");
                    exit(EXIT_FAILURE);
                }

                if (parser_peek(p, PEEK_CURRENT).type == TOKEN_SEMICOLON) {
                    parser_consume(p);
                } else {
                    fprintf(stderr, "Expected ';'\n");
                    exit(EXIT_FAILURE);
                }

                return node_stmt;

            } else {
                fprintf(stderr, "Invalid expression\n");
                exit(EXIT_FAILURE);
            }
        }

        case TOKEN_LET: {

            NodeStmt node_stmt = { .type = STMT_LET };
            NodeExpr node_expr = { .type = EXPR_INVALID };  // start invalid

            if (parser_peek(p, PEEK_CURRENT).type == TOKEN_IDENT) {
                node_stmt.data.let.ident = parser_consume(p);
            } else {
                fprintf(stderr, "Expected Identifier\n");
                exit(EXIT_FAILURE);
            }

            if (parser_peek(p, PEEK_CURRENT).type == TOKEN_EQ) {
                parser_consume(p);
            } else {
                fprintf(stderr, "Expected '='\n");
                exit(EXIT_FAILURE);
            }

            node_expr = parse_expr(p);
            if (node_expr.type != EXPR_INVALID) {

                node_stmt.data.let.expr = node_expr;

                if (parser_peek(p, PEEK_CURRENT).type == TOKEN_SEMICOLON) {
                    parser_consume(p);
                } else {
                    fprintf(stderr, "Expected ';'\n");
                    exit(EXIT_FAILURE);
                }

                return node_stmt;

            } else {
                fprintf(stderr, "Invalid expression\n");
                exit(EXIT_FAILURE);
            }
        }

        default: {
            return (NodeStmt) { .type = STMT_INVALID };
        }

    }
}

static void node_stmt_array_init(NodeStmtArray *arr) {
    arr->size = 0;
    arr->capacity = 4;
    arr->stmts = malloc(sizeof(NodeStmt) * arr->capacity);  // caller frees with node_stmt_array_free
    if (!arr->stmts) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

static void node_stmt_array_push(NodeStmtArray *arr, NodeStmt stmt) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        NodeStmt *new_stmts = realloc(arr->stmts, sizeof(NodeStmt) * arr->capacity);
        if (!new_stmts) {
            perror("realloc");
            free(arr->stmts);
            exit(EXIT_FAILURE);
        }
        arr->stmts = new_stmts;
    }
    arr->stmts[arr->size++] = stmt;
}

static void node_stmt_array_free(NodeStmtArray *arr) {
    if (!arr) return;
    free(arr->stmts);
    arr->stmts = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

NodeProg parse_prog(Parser *p) {
    NodeProg prog;
    node_stmt_array_init(&prog.stmts);  // caller frees with node_stmt_array_free

    while (parser_peek(p, PEEK_CURRENT).type != TOKEN_INVALID) {
        NodeStmt stmt = parse_stmt(p);
        if (stmt.type != STMT_INVALID) {
            node_stmt_array_push(&prog.stmts, stmt);
        } else {
            fprintf(stderr, "Invalid statement\n");
            exit(EXIT_FAILURE);
        }
    }
    return prog;
}
