#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "tokenization/tokenization.h"

static NodeExpr *parse_expr(Parser *p);

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

static NodeTerm *parse_term(Parser *p) {
    switch (parser_peek(p, PEEK_CURRENT).type) {

        case TOKEN_INT_LITERAL: {
            NodeTermIntLit *term_int_lit = arena_alloc(p->arena, sizeof(NodeTermIntLit));
            term_int_lit->int_lit = parser_consume(p);
            NodeTerm *term = arena_alloc(p->arena, sizeof(NodeTerm));
            term->type = TERM_INT_LIT;
            term->data.int_lit = term_int_lit;
            return term;
        }

        case TOKEN_IDENT: {
            NodeTermIdent *term_ident = arena_alloc(p->arena, sizeof(NodeTermIdent));
            term_ident->ident = parser_consume(p);
            NodeTerm *term = arena_alloc(p->arena, sizeof(NodeTerm));
            term->type = TERM_IDENT;
            term->data.ident = term_ident;
            return term;
        }

        default: {
            return NULL;
        }

    }
}

static NodeExpr *parse_expr(Parser *p) {
    NodeTerm *term = parse_term(p);
    if (term) {
        switch (parser_peek(p, PEEK_CURRENT).type) {

            case TOKEN_PLUS: {
                parser_consume(p);
                NodeExpr *lhs_expr = arena_alloc(p->arena, sizeof(NodeExpr));
                lhs_expr->type = EXPR_TERM;
                lhs_expr->data.term = term;
                NodeExpr *rhs_expr = parse_expr(p);
                if (!rhs_expr) {
                    fprintf(stderr, "Expected expression\n");
                    exit(EXIT_FAILURE);
                }
                NodeBinExprAdd *bin_expr_add = arena_alloc(p->arena, sizeof(NodeBinExprAdd));
                bin_expr_add->lhs = lhs_expr;
                bin_expr_add->rhs = rhs_expr;
                NodeBinExpr *bin_expr = arena_alloc(p->arena, sizeof(NodeBinExpr));
                bin_expr->type = BIN_ADD;
                bin_expr->data.add = bin_expr_add;
                NodeExpr *expr = arena_alloc(p->arena, sizeof(NodeExpr));
                expr->type = EXPR_BIN;
                expr->data.bin = bin_expr;
                return expr;
            }

            case TOKEN_MULTI: {
                fprintf(stderr, "Unsupported binary operator\n");
                exit(EXIT_FAILURE);
            }
            
            default: {
                NodeExpr *expr = arena_alloc(p->arena, sizeof(NodeExpr));
                expr->type = EXPR_TERM;
                expr->data.term = term;
                return expr;
            }

        }
    } else {
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

            if (!ll.head) {
                ll.head = node;
            } else {
                ll.tail->next = node;
            }
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
