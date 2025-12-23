#pragma once

#include "tokenization.h"
#include "arena.h"

// Forward declarations for pointer references
typedef struct NodeExpr NodeExpr;
typedef struct BinExpr BinExpr;
typedef struct BinExprMulti BinExprMulti;
typedef struct BinExprAdd BinExprAdd;
typedef struct NodeStmt NodeStmt;
typedef struct NodeStmtList NodeStmtList;

// ─── Enums ─────────────────────────────────────────────
typedef enum {
    BIN_INVALID,
    BIN_MULTI,
    BIN_ADD
} BinExprType;

typedef enum {
    EXPR_INVALID,
    EXPR_INT_LIT,
    EXPR_IDENT,
    EXPR_BIN
} NodeExprType;

typedef enum {
    STMT_INVALID,
    STMT_EXIT,
    STMT_LET
} NodeStmtType;

// ─── Expression Nodes ──────────────────────────────────
typedef struct {
    Token int_lit;
} NodeExprLit;

typedef struct {
    Token ident;
} NodeExprIdent;

struct BinExpr {
    BinExprType type;
    union {
        BinExprMulti *multi;
        BinExprAdd *add;
    } data;
};

struct NodeExpr {
    NodeExprType type;
    union {
        NodeExprLit *int_lit;
        NodeExprIdent *ident;
        BinExpr *bin;
    } data;
};

struct BinExprMulti {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

struct BinExprAdd {
    NodeExpr *lhs;
    NodeExpr *rhs;
};

// ─── Statement Nodes ───────────────────────────────────
typedef struct {
    NodeExpr *expr;
} NodeStmtExit;

typedef struct {
    Token ident;
    NodeExpr *expr;
} NodeStmtLet;

struct NodeStmt {
    NodeStmtType type;
    union {
        NodeStmtExit *exit;
        NodeStmtLet *let;
    } data;
};

// ─── Statement List ────────────────────────────────────
struct NodeStmtList {
    NodeStmt *stmt;
    NodeStmtList *next;
};

typedef struct {
    NodeStmtList *head;
    NodeStmtList *tail;
    size_t size;
} NodeStmtListBuilder;

// ─── Program Node ──────────────────────────────────────
typedef struct {
    NodeStmtList *stmts;
} NodeProg;

// ─── Parser ────────────────────────────────────────────
typedef struct {
    const Token *tokens;
    size_t size;
    size_t pos;
    Arena *arena;
} Parser;

// ─── Function Declarations ─────────────────────────────
void parser_init(Parser *p, const TokenArray *arr);
NodeProg parse_prog(Parser *p);
void parser_free(Parser *p);
