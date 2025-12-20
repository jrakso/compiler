#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "generation.h"

static void var_append(Generator *g, const char *name) {
    VariableTable *t = &g->vars;

    if (t->size == t->capacity) {
        t->capacity *= 2;
        t->vars = realloc(t->vars, t->capacity * sizeof(Variable));
        if (!t->vars) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
    }

    t->vars[t->size].name = strdup(name);
    t->vars[t->size].stack_loc = g->stack_size;
    t->size++;
}

static Variable *var_find(Generator *g, const char *name) {
    for (size_t i = 0; g->vars.size; i++) {
        if (strcmp(g->vars.vars[i].name, name) == 0) {
            return &g->vars.vars[i];
        }
    }
    return NULL;
}

static bool var_contains(Generator *g, const char *name) {
    for (size_t i = 0; i < g->vars.size; i++) {
        if (strcmp(g->vars.vars[i].name, name) == 0) {
            return true;
        }
    }
    return false;
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

Generator *generator_create(NodeProg *prog) {
    Generator *g = malloc(sizeof(Generator));
    if (!g) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    g->prog = prog;
    g->sb = malloc(sizeof(StringBuilder));
    if (!g->sb) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    sb_init(g->sb);
    g->stack_size = 0;
    g->vars = (VariableTable ){ .capacity = 0, .size = 0, .vars = NULL };
}

void generator_destroy(Generator *g) {
    sb_free(g->sb);
    free(g->sb);
    free(g);
}

static void gen_expr(Generator *g, const NodeExpr *expr) {
    switch (expr->type) {

        case EXPR_INT_LIT:
            sb_append_fmt(g->sb, "\tmov rax, %s\n", expr->data.int_lit->int_lit.value);
            push(g, "rax");
            break;

        case EXPR_IDENT:
            if (!var_contains(g, expr->data.ident->ident.value)) {
                fprintf(stderr, "Undeclared identifier: %s\n", expr->data.ident->ident.value);
                exit(EXIT_FAILURE);
            }
            const Variable *var = var_find(g, expr->data.ident->ident.value);
            push(g, "QWORD [rsp + %zu]", (g->stack_size - var->stack_loc - 1)*8);
            break;

        default:
            break;
    }
}

static void gen_stmt(Generator *g, const NodeStmt *stmt) {
    switch (stmt->type) {

        case STMT_EXIT:
            gen_expr(g, stmt->data.exit->expr);
            sb_append(g->sb, "\tmov rax, 60\n");
            pop(g, "rdi");
            sb_append(g->sb, "\tsyscall\n");
            break;

        case STMT_LET:
            if (var_contains(g, stmt->data.let->ident.value)) {
                fprintf(stderr, "Identifier already used: %s\n", stmt->data.let->ident.value);
                exit(EXIT_FAILURE);
            }
            var_append(g, stmt->data.let->ident.value);
            gen_expr(g, stmt->data.let->expr);
            break;

        default:
            break;
    }
}

char *gen_prog(Generator *g) {
    sb_append(g->sb, "global _start\n_start:\n");

    NodeStmtList *stmt = g->prog->stmts;
    while (stmt) {
        gen_stmt(g, stmt->stmt);
        stmt = stmt->next;
    }

    sb_append(g->sb,
        "\tmov rax, 60\n"
        "\tmov rdi, 0\n"
        "\tsyscall\n");

    return g->sb->data;
}