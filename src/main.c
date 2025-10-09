#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenization.h"

char *read_file(const char *filename) {
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("eerrorr");
        return 1;
    }

    // Get file content as string
    const char *input_file = argv[1];
    char *file_content = read_file(input_file);
    TokenArray tokens = tokenize(file_content);
    // for (int i = 0; i < tokens.size; i++) {
    //     printf("Token: %d (type %s)\n", tokens.data[i].type, tokens.data[i].value);
    //     free(tokens.data[i].value);
    // }
    printf("\n");
    free(tokens.data);
    free(file_content);
    return 0;
}