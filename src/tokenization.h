// Different Tokens
enum TokenType {
    TOKEN_RETURN,
    TOKEN_EXIT,
    TOKEN_INT_LITERAL,
    TOKEN_NEWLINE
};

// Represents a single Token
typedef struct {
    enum TokenType type;
    char *value;
} Token;

char *get_token_value(const char *string, size_t start, size_t end);

// Dynamic Token array
typedef struct {
    Token *data;   // pointer to array of tokens
    size_t size;      // how many tokens currently stored
    size_t capacity;  // allocated slots
} TokenArray;

void init_array(TokenArray *arr);
void push_token(TokenArray *arr, Token token);

typedef struct {
    const char *input;  // pointer to input string
    size_t length;    // Number of characters in input string
    size_t index;       // current index in string
} Tokenizer;

void tokenizer_init(Tokenizer *t, const char *input);
char peek(Tokenizer *t, int ahead);
char consume(Tokenizer *t);
TokenArray tokenize(const char *string);