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
    p->arena = arena_create(1024 * 1024 * 4);  // 4 mb
}

void parser_free(Parser *p) {
    arena_destroy(p->arena);
}

static Token parser_peek(const Parser *p, size_t offset) {
    if (p->pos + offset >= p->size) {
        return (Token) { .type = TOKEN_INVALID };
    }
    return p->tokens[p->pos + offset];
}

static Token parser_consume(Parser *p) {
    return p->tokens[p->pos++];
}

static NodeExpr *parse_expr(Parser *p) {

    switch (parser_peek(p, PEEK_CURRENT).type) {

        case TOKEN_INT_LITERAL: {
            NodeExprLit *expr_int_lit = arena_alloc(p->arena, sizeof(NodeExprLit));
            expr_int_lit->int_lit = parser_consume(p);
            NodeExpr *expr = arena_alloc(p->arena, sizeof(NodeExpr));
            expr->type = EXPR_INT_LIT;
            expr->data.int_lit = expr_int_lit;
            return expr;
        }

        case TOKEN_IDENT: {
            NodeExprIdent *expr_ident = arena_alloc(p->arena, sizeof(NodeExprIdent));
            expr_ident->ident = parser_consume(p);
            NodeExpr *expr = arena_alloc(p->arena, sizeof(NodeExpr));
            expr->type = EXPR_IDENT;
            expr->data.ident = expr_ident;
            return expr;
        }

        default:
            return NULL;
    }
}

static NodeStmt *parse_stmt(Parser *p) {
    const Token t = parser_peek(p, PEEK_CURRENT);
    parser_consume(p);

    switch (t.type) {

        case TOKEN_EXIT: {

            NodeStmt *node_stmt = arena_alloc(p->arena, sizeof(NodeStmt));
            node_stmt->type = STMT_EXIT;
            node_stmt->data.exit = arena_alloc(p->arena, sizeof(NodeStmtExit));

            if (parser_peek(p, PEEK_CURRENT).type == TOKEN_OPEN_PAREN) {
                parser_consume(p);
            } else {
                fprintf(stderr, "Expected '('\n");
                exit(EXIT_FAILURE);
            }

            NodeExpr *node_expr = parse_expr(p);
            if (node_expr) { 

                node_stmt->data.exit->expr = node_expr; 

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

            NodeStmt *node_stmt = arena_alloc(p->arena, sizeof(NodeStmt));
            node_stmt->type = STMT_LET;
            node_stmt->data.let = arena_alloc(p->arena, sizeof(NodeStmtLet));

            if (parser_peek(p, PEEK_CURRENT).type == TOKEN_IDENT) {
                node_stmt->data.let->ident = parser_consume(p);
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

            NodeExpr *node_expr = parse_expr(p);
            if (node_expr) {

                node_stmt->data.let->expr = node_expr;

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
            return NULL;
        }

    }
}

NodeProg parse_prog(Parser *p) {
    if (!p) {
        return (NodeProg) {0};
    }

    NodeProg prog;
    NodeStmtListBuilder ll = {0};

    while (parser_peek(p, PEEK_CURRENT).type != TOKEN_INVALID) {

        NodeStmt *stmt = parse_stmt(p);

        if (stmt) {

            NodeStmtList *node = arena_alloc(p->arena, sizeof(NodeStmtList));
            node->stmt = stmt;
            node->next = NULL;

            if (!ll.head) ll.head = node;
            else ll.tail->next = node;
            ll.tail = node;
            ll.size++;

        } else {
            fprintf(stderr, "Invalid statement\n");
            exit(EXIT_FAILURE);
        }
    }
    prog.stmts = ll.head;
    return prog;
}
