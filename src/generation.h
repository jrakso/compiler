#pragma once

#include "parser.h"
#include "strbuilder.h"
#include "uthash.h"

typedef struct {
    size_t stack_loc;
} Var;

typedef struct {
    char *name;
    Var var;
    UT_hash_handle hh;
} VarEntry;

typedef struct {
    NodeProg *prog;
    StringBuilder *sb;
    size_t stack_size;
    VarEntry *vars;
} Generator;

void generator_init(Generator *g, NodeProg *prog);
char *gen_prog(Generator *g);
