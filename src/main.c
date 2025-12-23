#include <stdio.h>
#include <stdlib.h>

#include "helpers/file.h"
#include "tokenization/tokenization.h"
#include "parser/parser.h"
#include "codegen/generation.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    char *src = read_file(argv[1]);  // caller frees

    TokenArray tokens = tokenize(src);  // caller frees with token_array_free

    Parser p;  // Caller frees
    parser_init(&p, &tokens);
    NodeProg prog = parse_prog(&p);

    Generator g;  // Caller frees
    generator_init(&g, &prog);
    char *output = gen_prog(&g);

    write_file("output.asm", output);

    free(src);
    token_array_free(&tokens);
    parser_free(&p);
    generator_free(&g);
    return 0;
}
