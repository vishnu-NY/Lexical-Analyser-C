#include <stdio.h>
#include "lexer.h"
FILE* fp;
extern int errorflag;
extern int linecount;
extern int openbracketscount;
extern int closebracketscount;
extern int previoustoken;
int main(int argc, char *argv[]) 
{
    extern int errorflag;
extern int linecount;
extern int openbracketscount;
extern int closebracketscount;
   //initializeLexer(argv[1]);

   fp = fopen(argv[1], "r");
    if (!fp) 
    {
        printf("Failed to open file");
        return 1;
    }
    Token token;
    while(((token = getNextToken()).type != TOKEN_EOF) && (token.type != UNKNOWN) && (errorflag==0))
    {
        char* typename;
        switch(token.type)
        {
            case 0 :
            typename = "Keyword";
            break;
            case 1 :
            typename = "Operator";
            break;
            case 2 :
            typename = "Operator";
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
        previoustoken=token.type;
        
    }
    if(openbracketscount != closebracketscount && errorflag==0 &&(token.type != UNKNOWN))
    {
        printf("\nError: Mismatched brackets detected. Open brackets count: %d, Close brackets count: %d\n", openbracketscount, closebracketscount);
    }
    fclose(fp);

    return 0;
}
