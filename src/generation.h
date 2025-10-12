#pragma once

#include "parser.h"

typedef struct {
    const NodeExit *root;
} Generator;

char *generate(NodeExit *root);
