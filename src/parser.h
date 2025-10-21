#pragma once

#include "tokenization.h"

typedef enum {
    EXPR_INVALID,
    EXPR_INT_LIT,
    EXPR_IDENT
} NodeExprType;

typedef struct {
    Token int_lit;
} NodeExprLit;

typedef struct {
    Token ident;
} NodeExprIdent;

typedef struct {
    NodeExprType type;
    union {
        NodeExprLit int_lit;
        NodeExprIdent ident;
    } data;
} NodeExpr;

typedef enum {
    STMT_INVALID,
    STMT_EXIT,
    STMT_LET
} NodeStmtType;

typedef struct {
    NodeExpr expr;
} NodeStmtExit;

typedef struct {
    Token ident;
    NodeExpr expr;
} NodeStmtLet;

typedef struct {
    NodeStmtType type;
    union {
        NodeStmtExit exit;
        NodeStmtLet let;
    } data;
} NodeStmt;

typedef struct {
    NodeStmt *stmts;
    size_t size;
    size_t capacity;
} NodeStmtArray;

typedef struct {
    NodeStmtArray stmts;
} NodeProg;

typedef struct {
    const Token *tokens;
    size_t size;
    size_t pos;
} Parser;

void parser_init(Parser *p, const TokenArray *arr);
NodeProg parse_prog(Parser *p);