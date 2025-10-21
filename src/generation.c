#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "generation.h"

static void add_var(Generator *g, const char *name, Var var) {
    VarEntry *entry = malloc(sizeof(VarEntry));
    entry->name = strdup(name);
    entry->var = var;
    HASH_ADD_KEYPTR(hh, g->vars, entry->name, strlen(entry->name), entry);
}

static Var *get_var(Generator *g, const char *name) {
    VarEntry *entry;
    HASH_FIND_STR(g->vars, name, entry);
    return entry ? &entry->var : NULL;
}

static bool contains_var(Generator *g, const char *name) {
    VarEntry *entry;
    HASH_FIND_STR(g->vars, name, entry);
    return entry != NULL;
}

void generator_init(Generator *g, NodeProg *prog) {
    g->prog = prog;
    g->sb = malloc(sizeof(StringBuilder));
    if (!g->sb) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sb_init(g->sb);
    g->stack_size = 0;
    g->vars = NULL;
}

static void push(Generator *g, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    sb_append_fmt(g->sb, "\tpush ");
    sb_append_fmtv(g->sb, fmt, args);
    sb_append(g->sb, "\n");
    va_end(args);
    g->stack_size++;
}

static void pop(Generator *g, const char *reg) {
    sb_append_fmt(g->sb, "\tpop %s\n", reg);
    g->stack_size--;
}

static void gen_expr(Generator *g, const NodeExpr *expr) {
    switch (expr->type) {

        case EXPR_INT_LIT:
            sb_append_fmt(g->sb, "\tmov rax, %s\n", expr->data.int_lit.int_lit.value);
            push(g, "rax");
            break;

        case EXPR_IDENT:
            if (!contains_var(g, expr->data.ident.ident.value)) {
                fprintf(stderr, "Undeclared identifier: %s\n", expr->data.ident.ident.value);
                exit(EXIT_FAILURE);
            }
            const Var *var = get_var(g, expr->data.ident.ident.value);
            push(g, "QWORD [rsp + %zu]", (g->stack_size - var->stack_loc - 1)*8);
            break;

        default:
            break;
    }
}

static void gen_stmt(Generator *g, const NodeStmt *stmt) {
    switch (stmt->type) {

        case STMT_EXIT:
            gen_expr(g, &stmt->data.exit.expr);
            sb_append(g->sb, "\tmov rax, 60\n");
            pop(g, "rdi");
            sb_append(g->sb, "\tsyscall\n");
            break;

        case STMT_LET:
            if (contains_var(g, stmt->data.let.ident.value)) {
                fprintf(stderr, "Identifier already used: %s\n", stmt->data.let.ident.value);
                exit(EXIT_FAILURE);
            }
            add_var(g, stmt->data.let.ident.value, (Var) { .stack_loc = g->stack_size });
            gen_expr(g, &stmt->data.let.expr);
            break;

        default:
            break;
    }
}

char *gen_prog(Generator *g) {
    sb_append(g->sb, "global _start\n_start:\n");

    for (size_t i = 0; i < g->prog->stmts.size; i++) {
        gen_stmt(g, &g->prog->stmts.stmts[i]);
    }

    sb_append(g->sb,
        "\tmov rax, 60\n"
        "\tmov rdi, 0\n"
        "\tsyscall\n");

    return g->sb->data;
}