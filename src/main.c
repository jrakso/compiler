#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenization.h"
#include "parser.h"
#include "generation.h"

static char *read_file(const char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) exit(1);

    // Get file size
    fseek(file, 0, SEEK_END);  // Place pointer at end of file
    size_t file_size = ftell(file);  // Get value of position indicator (file size)

    // Reset file pointer to beginning of file
    fseek(file, 0, SEEK_SET);

    // Allocate memory for file content (+1 for newline and null terminator)
    char *string = malloc(sizeof(char) * (file_size + 2));

    // Read file to string, character by character, until we reach end of file
    int c;
    int i = 0;
    while ((c = fgetc(file)) != EOF) {
        string[i] = (char)c;
        i++;
    }
    string[i] = '\n';  // Null terminator
    string[i + 1] = '\0';  // Null terminator

    // Close file
    fclose(file);

    return string;
}

static void write_file(const char *filename, const char *string) {
    FILE *f = fopen(filename, "w");   // open for writing (overwrite existing)
    if (!f) {
        perror("fopen");
        exit(1);
    }

    if (fputs(string, f) == EOF) {
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
        printf("eerrorr");
        return 1;
    }

    // Get file content as string
    const char *input_file = argv[1];
    char *file_content = read_file(input_file);

    Tokenizer tokenizer;
    tokenizer_init(&tokenizer, file_content);
    TokenArray arr = tokenize(&tokenizer);

    Parser parser;
    init_parser(&parser, &arr);
    
    NodeExit *tree = parse(&parser);

    if (tree == NULL) {
        fprintf(stderr, "No exit statement found");
        exit(1);
    }

    char *output = generate(tree);

    write_file("output.asm", output);

    free(output);
    token_array_free(&arr);
    ast_free(tree);
    free(file_content);

    return 0;
}