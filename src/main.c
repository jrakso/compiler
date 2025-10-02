#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Different kinds of tokens that can appear in the source code.
 */
enum TokenType {
    TOKEN_SKRIV,     // Like print
    TOKEN_RETURNERA  // Like return
};


/**
 * Represents a single token from the input file.
 * Stores the token type and the exact text from the source.
 */
typedef struct Token {
    enum TokenType type;
    char *text;
} Token;

/**
 * A dynamic array of tokens that grows as needed.
 * Keeps track of how many tokens are stored and the allocated capacity.
 */
typedef struct {
    Token *data;   // pointer to array of tokens
    int size;      // how many tokens currently stored
    int capacity;  // allocated slots
} TokenArray;

// Initialize a TokenArray
void init_array(TokenArray *arr) {
    arr->size = 0;
    arr->capacity = 4;
    arr->data = malloc(sizeof(Token) * arr->capacity);
}

// Push a token into the array, resize if needed
void push_token(TokenArray *arr, Token t) {
    if (arr->size == arr->capacity)
    {
        arr->capacity *= 2;  // Double capacity
        arr->data = realloc(arr->data, sizeof(Token) * arr->capacity);  // Reallocate memory for new capacity
    }
    arr->data[arr->size++] = t;  // Add token
}

TokenArray tokenize(char *string) {
    // TODO
}

/**
 * Reads the entire contents of a text file into a dynamically allocated string.
 *
 * The function opens the given file, determines its size, allocates a buffer,
 * and reads all characters until EOF. A null terminator is appended at the end.
 *
 * @param filename path to the file to be read
 * @return a pointer to a newly allocated null-terminated string containing
 *         the file contents, or NULL if the file could not be opened
 *
 * @note The caller is responsible for freeing the returned string with free().
 */
char *read_file(char *filename) {
    // Open file in reading mode
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) return NULL;

    // Get file size
    fseek(file, 0, SEEK_END);  // Place pointer at end of file
    long file_size = ftell(file);  // Get value of position indicator (file size)

    // Reset file pointer to beginning of file
    fseek(file, 0, SEEK_SET);

    // Allocate memory for file content (+1 for null terminator)
    char *string = malloc(sizeof(char) * (file_size + 1));

    // Read file to string, character by character, until we reach end of file
    int c;
    int i = 0;
    while ((c = fgetc(file)) != EOF)
    {
        string[i] = (char)c;
        i++;
    }
    string[i] = '\0';  // Null terminator

    // Close file
    fclose(file);

    return string;
}

int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        return 1;
    }

    // Get file content as string
    char *input_file = argv[1];
    char *file_content = read_file(input_file);
    printf("%s", file_content);
}