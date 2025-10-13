#pragma once

#include "parser.h"

typedef struct {
    const NodeExit *root;
} Generator;

void generator_init(Generator *g, NodeExit *root);
char *generate(Generator *g);
