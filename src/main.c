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
        exit(1);
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(file);
        exit(1);
    }

    long pos = ftell(file);
    if (pos < 0) {
        perror("ftell");
        fclose(file);
        exit(1);
    }
    size_t file_size = (size_t)pos;

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek");
        fclose(file);
        exit(1);
    }

    char *buffer = malloc(file_size + 2);
    if (!buffer) {
        perror("malloc");
        fclose(file);
        exit(1);
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
        exit(1);
    }

    if (fputs(output, f) == EOF) {
        perror("fputs");
        fclose(f);
        exit(1);
    }

    if (fclose(f) == EOF) {
        perror("fclose");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    char *source = read_file(argv[1]);

    Tokenizer tokenizer;
    tokenizer_init(&tokenizer, source);
    TokenArray tokens = tokenize(&tokenizer);

    Parser parser;
    parser_init(&parser, &tokens);
    NodeExit *tree = parse(&parser);
    if (!tree) {
        fprintf(stderr, "No 'exit' statement found\n");
        token_array_free(&tokens);
        free(source);
        return 1;
    }

    Generator generator;
    generator_init(&generator, tree);
    char *output = generate(&generator);

    write_file("output.asm", output);

    free(output);
    ast_free(tree);
    token_array_free(&tokens);
    free(source);

    return 0;
}
