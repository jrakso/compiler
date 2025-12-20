#pragma once

#include "parser.h"
#include "strbuilder.h"

typedef struct {
    char *name;
    size_t stack_loc;
} Variable;

typedef struct {
    Variable *vars;
    size_t size;
    size_t capacity;
} VariableTable;

typedef struct {
    NodeProg *prog;
    StringBuilder *sb;
    size_t stack_size;
    VariableTable vars;
} Generator;

Generator *generator_create(NodeProg *prog);
char *gen_prog(Generator *g);
void generator_destroy(Generator *g);
