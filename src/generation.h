#pragma once

#include "parser.h"

typedef struct {
    const NodeExit *root;
} Generator;

void init_generator(Generator *g, NodeExit *root);
char *generate(NodeExit *root);
