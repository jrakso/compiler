#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generation.h"

void generator_init(Generator *g, NodeExit *root) {
    g->root = root;
}

char *generate(Generator *g) {
    const char *template =
        "global _start\n"
        "_start:\n"
        "    mov rax, 60\n"
        "    mov rdi, %s\n"
        "    syscall\n";

    const char *value = g->root->expr->int_lit.value;

    size_t needed = snprintf(NULL, 0, template, value) + 1;

    char *result = malloc(needed);
    if (!result) {
        perror("malloc");
        exit(1);
    }

    snprintf(result, needed, template, value);
    return result;
}