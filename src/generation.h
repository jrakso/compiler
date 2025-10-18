#pragma once

#include "parser.h"

typedef struct {
    const NodeExit *root;
} Generator;

void generator_init(Generator *g, const NodeExit *root);  // does not allocate
char *generate(Generator *g);  // caller frees
