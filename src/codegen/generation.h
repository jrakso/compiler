#pragma once

#include "parser.h"
#include "strbuilder.h"
#include "vartable.h"

typedef struct {
    NodeProg *prog;
    StringBuilder sb;
    size_t stack_size;
    VariableTable vars;
} Generator;

void generator_init(Generator *g, NodeProg *prog);
char *gen_prog(Generator *g);
void generator_free(Generator *g);
