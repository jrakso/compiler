#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generation.h"

static void init_generator(Generator *g, NodeExit *root) {
    g->root = root;
}

char *generate(NodeExit *root) {
    Generator g;
    init_generator(&g, root);
    const char *template =
        "global _start\n"
        "_start:\n"
        "    mov rax, 60\n"
        "    mov rdi, %s\n"
        "    syscall\n";

    const char *value = g.root->expr->int_lit.value;

    size_t needed = snprintf(NULL, 0, template, value) + 1;

    char *result = malloc(needed);
    if (!result) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    snprintf(result, needed, template, value);

    return result;
}