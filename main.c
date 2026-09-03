#include <stdio.h>
#include "lexer.h"
FILE* fp;

int main(int argc, char *argv[]) {
   //initializeLexer(argv[1]);

   fp = fopen("source.txt", "r");
    if (!fp) {
        printf("Failed to open file");
        return 1;
    }
    Token token;
    while((token = getNextToken()).type != TOKEN_EOF)
    {
        char* typename;
        switch(token.type)
        {
            case 0 :
            typename = "Keyword";
            break;
            case 1 :
            typename = "Single Oprator";
            break;
            case 2 :
            typename = "Double Operator";
            break;
            case 3 :
            typename = "Special Character";
            break;
            case 4 :
            typename = "Constant";
            break;
            case 5 :
            typename = "Identifier";
            break;
            case 6:
            typename = "String Literal";
            break;
            case 7:
            typename = "Char Literal";
            break;
            case 8:
            typename = "Unknown";
            break;

        }
        printf("\nToken: %s\t|\t Type: %s\n", token.lexeme, typename);
        
    }

    return 0;
}
