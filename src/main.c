#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Different kinds of tokens that can appear in the source code.
 */
enum TokenType {
    TOKEN_RETURN,
    TOKEN_INT_LITERAL  
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
    arr->data[arr->size] = t;
    arr->size++;
}

char *get_token_text(char *string, int start, long length) {
    char *tokenText = malloc(sizeof(char) * (length + 1));
    for (int i = 0; i < length; i++)
    {
        tokenText[i] = string[start + i];
    }
    tokenText[length] = '\0';
    return tokenText;
}

TokenArray tokenize(char *string) {
    TokenArray tokens;
    init_array(&tokens);

    long length = strlen(string);
    int i = 0;

    while (i < length)
    {
        char c = string[i];

        // Skip whitespace 
        if (isspace(c)) { i++; continue; }

        // Identifiers / keywords - start with letter or underscore
        if (isalpha(c))
        {
            int tokenStart = i;

            // Letters, digits and underscores valid
            while (i < length && (isalnum(string[i]) || string[i] == '_')) { i++; }

            int tokenLength = i - tokenStart;
            char *tokenText = get_token_text(string, tokenStart, tokenLength);

            Token token;

            // TOKEN_RETURNERA
            if (strcmp(tokenText, "returnera") == 0)
            {
                token.text = tokenText;
                token.type = TOKEN_RETURN;
                push_token(&tokens, token);
            } else {
                fprintf(stderr, "Error");
                free(tokenText);
                exit(1);
            }

        // Integer literal       
        } else if (isdigit(c))
        {
            int tokenStart = i;

            // 0-9 valid
            while (i < length && isdigit(c)) { i++; }

            int tokenLength = i - tokenStart;
            char *tokenText = get_token_text(string, tokenStart, tokenLength);
            
            Token token;
            token.text = tokenText;
            token.type = TOKEN_INT_LITERAL;
            push_token(&tokens, token);
        }
        i++;
    }
    return tokens;
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
char *read_file(const char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) exit(1);

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
        printf("eerrorr");
        return 1;
    }

    // Get file content as string
    const char *input_file = argv[1];
    char *file_content = read_file(input_file);
    TokenArray tokens = tokenize(file_content);
    for (int i = 0; i < tokens.size; i++)
    {
        printf("Token: %s (type %d)\n", tokens.data[i].text, tokens.data[i].type);
        free(tokens.data[i].text);
    }
    free(tokens.data);
    free(file_content);
    return 0;
}