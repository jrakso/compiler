#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokenization.h"
#include "parser.h"
#include "generation.h"

static char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    long pos = ftell(file);
    if (pos < 0) {
        perror("ftell");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    size_t file_size = (size_t)pos;

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    char *buffer = malloc(file_size + 2);  // caller frees
    if (!buffer) {
        perror("malloc");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read = fread(buffer, 1, file_size, file);
    buffer[read] = '\n';
    buffer[read + 1] = '\0';
    fclose(file);

    return buffer;
}

static void write_file(const char *filename, const char *output) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if (fputs(output, f) == EOF) {
        perror("fputs");
        fclose(f);
        exit(EXIT_FAILURE);
    }

    if (fclose(f) == EOF) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    char *src = read_file(argv[1]);  // caller frees

    TokenArray tokens = tokenize(src);  // caller frees with token_array_free

    Parser *p = parser_create(&tokens);
    NodeProg prog = parse_prog(p);

    Generator *g = generator_create(&prog);
    char *output = gen_prog(g);

    write_file("output.asm", output);

    free(src);
    token_array_free(&tokens);
    parser_destroy(p);
    generator_destroy(g);
    return 0;
}
