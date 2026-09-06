#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS 33
#define MAX_TOKEN_SIZE 100

typedef enum {
    KEYWORD,
    SINGLE_OPERATOR,
    DOUBLE_OPERATOR,
    SPECIAL_CHARACTER,
    CONSTANT,
    IDENTIFIER,
    STRING_LITERAL,
    CHAR_LITERALS,
    UNKNOWN,
    TOKEN_EOF
} TokenType;

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];
    TokenType type;
} Token;

void initializeLexer(const char* filename);
Token getNextToken();
void categorizeToken(Token* token, short int category);
int isKeyword(const char* str);
int isOperator(const char* str);
int isSpecialCharacter(char ch);
int isConstant(const char* str);
int isIdentifier(const char* str);

#endif
