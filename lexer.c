#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

/*
Token character categories - identifier=-1(Default value)
1 - Characters (alpha) - keywords, identifiers
2 - Digits (0-9) - constants
3 - Operators (+, -, *, /, %, =, <, >, |, &) - single or double operators
4 - Special Characters (,, ;, {, }, (, ), [, ])
*/

static const char* keywords[MAX_KEYWORDS] = 
{
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct", "main",
};

static const char* singleoperators = "+-*/%=!<>|&^";
static const char* doubleoperators[15] = 
{
    "==","++","--","+=",">>","<<","<=",">=","!="
};
static const char* specialCharacters = ",;{}()[]";

char currentChar;
short int identifier=-1;
 int ch;
char buffer[MAX_TOKEN_SIZE];
extern FILE* fp;
int i=10;
Token getNextToken( )
{
    printf("Getting next token\n");
    while((ch=getc(fp))!=EOF)
    {
        for(int j = 0; j < MAX_TOKEN_SIZE; j++) 
        {
            buffer[j] = '\0'; // Clear the buffer
        }

       // printf("Current character: %c\n", ch);
        if(isspace(ch))
            continue;
        else if(isalpha(ch) || ch=='_')
        {
            int i=0;
            buffer[i++]=ch;
            while((ch=getc(fp))!=EOF && (isalnum(ch)))
                {
                    buffer[i]=ch;
                     i++;
                }
              if(ch != EOF)  ungetc(ch, fp);  
            buffer[i]='\0';
           printf("Buffer = %s\n",buffer);
            
            printf("before token");
            Token token;
            strcpy(token.lexeme,buffer);
             printf("Token = %s, ",token.lexeme);
            categorizeToken(&token,1);
         
            return token;
        }
        else if(isdigit(ch))
        {
            int i=0,dotflag=0;
            buffer[i++]=ch;
            while((ch=getc(fp))!=EOF &&( isdigit(ch) ||(ch=='.' && dotflag==0)))
                {
                    if(ch=='.') dotflag=1;
                    buffer[i]=ch;
                     i++;
                }
            buffer[i]='\0';
            ungetc(ch,fp);
            Token token;
            strcpy(token.lexeme,buffer);
            categorizeToken(&token,2);
            return token;
        }
        else if(strchr(singleoperators,ch))
        {
            printf("Entered singl operator part\n");
            Token token;
            buffer[0]=ch;
            if(strchr(singleoperators,ch=getc(fp)))
            {
                printf("In if condition");
                buffer[1]=ch;
                buffer[2]='\0';
                for(int j=0;j<9;j++)
                {
                     printf("in for loop");
                    if(strcmp(doubleoperators[j],buffer)==0)
                    {
                        strcpy(token.lexeme,buffer);
                         categorizeToken(&token,3);
                         return token;
                    }
                   
                }

            }
            printf("Reached here");
            ungetc(ch,fp);
            buffer[1]='\0';
            strcpy(token.lexeme,buffer);
            categorizeToken(&token,3);
            return token;
        }
        else if(strchr(specialCharacters,ch))
        {
            Token token;
            buffer[0]=ch;
            buffer[1]='\0';
            strcpy(token.lexeme,buffer);
            categorizeToken(&token,4);
            return token;
        }
        else if(ch == '"')
        {
            int i=1;
            Token token;
            buffer[0]=ch;
            while((ch=getc(fp)) != '"' )
            {
                buffer[i]=ch;
                i++;
            }
            buffer[i] = ch; i++; 
            buffer[i]='\0';
            strcpy(token.lexeme,buffer);
            categorizeToken(&token,5);
            return token;
        } 
        else if((int)ch == 39)
        {
            int i=0;
            buffer[i]=ch; i++;
            while((ch=getc(fp))!=39)


            {

                buffer[i]=ch;
                i++;

            }
            buffer[i]=ch; i++;
            buffer[i]='\0';
            Token token;
            strcpy(token.lexeme,buffer);
            categorizeToken(&token,6);
            return token;

        }
        else
        {
           
           Token token;

            token.lexeme[0] = ch;
            token.lexeme[1] = '\0';
            token.type = UNKNOWN;

            return token;
        }
    }
        //while loop ends meaning reached EOF
                 Token token;
               strcpy(token.lexeme,"");
               token.type=TOKEN_EOF;
               return token;
   
    
}

void categorizeToken(Token* token, short int category)
{
    switch(category)
    {
        int res;
        case 1:
            if(isKeyword(token->lexeme))
                token->type=KEYWORD;
            else if(isIdentifier(token->lexeme))
                token->type=IDENTIFIER;
            break;
        case 2:
            if(isConstant(token->lexeme))
            token->type=CONSTANT;
            break;
        case 3:
        if(res = isOperator(token->lexeme))
            {
                if(res==1)
            token->type=SINGLE_OPERATOR;
            if(res==2)
            token->type=DOUBLE_OPERATOR;
            }
            break;
        case 4:
        if(isSpecialCharacter(token->lexeme[0]))
            token->type=SPECIAL_CHARACTER;
            break;
        case 5:
        token->type=STRING_LITERAL;
        break;
        case 6:
        token->type=CHAR_LITERALS;
        break;
        default:
            token->type=UNKNOWN;
    }

}
int isKeyword(const char* str)
{
    
    for (int i = 0; i < MAX_KEYWORDS; i++) 
    {
        if (strcmp(str, keywords[i]) == 0) 
        {
            return 1; // It's a keyword
        }
    }
   // printf("Checking if '%s' is a keyword\n", str);
    return 0; // Not a keyword
}
int isOperator(const char* str)
{
    int j=0,flag=0;
   // printf("Checking if '%s' is an operator\n", str);
    if(str[1]=='\0')
    {
       
        if (strchr(singleoperators,str[0])) 
        {
            return 1; // It's a single operator
        }
        

    }
    else
    {
        for(int j=0;j<9;j++)
        {
            if(strcmp(str,doubleoperators[j])==0)
            {
                return 2; //double oiperator
            }
        }
    }
    return 0; //not operator

}
int isSpecialCharacter(char ch)
{
  //  printf("Checking if '%c' is a special character\n", ch);
}
int isConstant(const char* str)
{
    int isConstant(const char *str)
{
    int i = 0;

    if(str[0] == '\0')
        return 0;

    while(str[i] != '\0')
    {
        if(!isdigit((unsigned char)str[i]))
            return 0;

        i++;
    }

    return 1;
}

}

int isIdentifier(const char* str)
{
     int i = 0;

    if(!(isalpha((unsigned char)str[0]) || str[0] == '_'))
        return 0;

    i++;

    while(str[i] != '\0')
    {
        if(!(isalnum((unsigned char)str[i]) || str[i] == '_'))
            return 0;

        i++;
    }

    return 1;
  //  printf("Checking if '%s' is an identifier\n", str);
}