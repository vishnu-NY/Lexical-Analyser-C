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
// printf("\nError in Line %d, Error: %s\n", linecount, buffer);
static const char* keywords[MAX_KEYWORDS] = 
{
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct", "typedef", "enum","volatile","register","extern","goto","union","signed","unsigned",
    "auto","inline","long","short"
};
static int linecount=1;

int previoustoken=-1;

static const char* singleoperators = "+-*/%=!<>.|&^~";
static const char* doubleoperators[19] = 
{
    "==","++","--","+=",">>","<<","<=",">=","!=","&&","||","&=","|=","^=","->","-=","*=","/=","%="
};
static const char* specialCharacters = ",;:{}()[]";
static const char* openbrackets = "{([";
static const char* closebrackets = "})]";
 int openbracketscount=0;
 int closebracketscount=0;
 #define MAX_BRACKET_STACK 100

char bracketStack[MAX_BRACKET_STACK];
int bracketLineStack[MAX_BRACKET_STACK];

int bracketTop = -1;
 char temp=0;

char currentChar;
int errorflag=0;
short int identifier=-1;
 int ch;
char buffer[MAX_TOKEN_SIZE];
extern FILE* fp;
int i=10;

static int isCandidateDelimiter(int c)
{
    if(c == EOF || isspace((unsigned char)c))
        return 1;

    if(strchr(specialCharacters, c))
        return 1;

    if(strchr(singleoperators, c))
        return 1;

    if(c == '"' || c == '\'')
        return 1;

    return 0;
}

static int appendToBuffer(int *index, int c)
{
    if(*index >= MAX_TOKEN_SIZE - 1)
    {
        buffer[MAX_TOKEN_SIZE - 1] = '\0';
        printf("\nError in Line %d: Token too long: %s\n", linecount, buffer);
        errorflag = 1;
        return 0;
    }

    buffer[(*index)++] = (char)c;
    return 1;
}

static int isMatchingBracket(char open, char close)
{
    if(open == '(' && close == ')')
        return 1;

    if(open == '[' && close == ']')
        return 1;

    if(open == '{' && close == '}')
        return 1;

    return 0;
}



Token getNextToken( )
{
   // printf("Getting next token\n");
    while((ch=getc(fp))!=EOF)
    {
        for(int j = 0; j < MAX_TOKEN_SIZE; j++) 
        {
            buffer[j] = '\0'; // Clear the buffer
        }
        if(ch == '\n') 
        {
            linecount++;
        }
       // printf("Current character: %c\n", ch);
        if(isspace(ch))
        {   
            continue;
        }
        else if(ch =='#')
        {
            while((ch=getc(fp))!=EOF && ch!='\n');
             if(ch == '\n')
                linecount++;
            continue;
        }
        else if(ch =='/')
        {
            if((ch=getc(fp))=='/')
            {
                while((ch=getc(fp))!=EOF && ch!='\n');
                continue;
                linecount++;
            }
            else if(ch=='*')
            {
                char commentClosed=0;   
                while((ch=getc(fp))!=EOF)
                {
                    if(ch=='\n')
                    {
                        linecount++;
                    }
                    if(ch=='*')
                    {
                        if((ch=getc(fp))=='/')
                           {
                            commentClosed=1;
                             break; 
                           }

                        else
                            ungetc(ch,fp);
                    }
                
                   
                }
                 if(!commentClosed)
                {
                    Token token;
                    strcpy(token.lexeme, "/*");
                    token.type = UNKNOWN;

                    printf("\nError in Line %d: Unterminated multi-line comment\n", linecount);
                    errorflag = 1;
                    return token;
                }
                 continue;


                
            }
            else if(ch == '=')
            {
                Token token;

                buffer[0] = '/';
                buffer[1] = '=';
                buffer[2] = '\0';

                strcpy(token.lexeme, buffer);
                categorizeToken(&token, 3);

                return token;
            }
            else
            {
                if(ch != EOF)
                    ungetc(ch, fp);

                Token token;

                buffer[0] = '/';
                buffer[1] = '\0';

                strcpy(token.lexeme, buffer);
                categorizeToken(&token, 3);

                return token;
        }

    }
        else if(isalpha(ch) || ch=='_' || ch=='$')
        {
            int i = 0;
            appendToBuffer(&i, ch);

            while((ch = getc(fp)) != EOF && !isCandidateDelimiter(ch))
            {
                if(!appendToBuffer(&i, ch))
                {
                    Token token;
                    strcpy(token.lexeme, buffer);
                    token.type = UNKNOWN;
                    return token;
                }
            }

            if(ch != EOF)
                ungetc(ch, fp);

            buffer[i] = '\0';

            Token token;
            strcpy(token.lexeme, buffer);
            categorizeToken(&token, 1);
            return token;
        }
        /*--------------------------N O T E        N O T   U S E F U L-----------------------------*/
        else if(ch=='-')
        {
            if(isdigit(ch=getc(fp)) && previoustoken!=5)
            {
                int i=0,dotflag=0;
                buffer[i++]='-';
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
            else
            {
                        ungetc(ch,fp);
                        ch='-';
               if(strchr(singleoperators,ch))
                {
                // printf("Entered singl operator part\n");
                    Token token;
                    buffer[0]=ch;
                    if(strchr(singleoperators,ch=getc(fp)))
                    {
                    // printf("In if condition");
                        buffer[1]=ch;
                        buffer[2]='\0';
                        for(int j=0;j<19;j++)
                        {
                            //printf("in for loop");
                            if(strcmp(doubleoperators[j],buffer)==0)
                            {
                                strcpy(token.lexeme,buffer);
                                categorizeToken(&token,3);
                                return token;
                            }
                        
                        }

                    }
                    //printf("Reached here");
                    ungetc(ch,fp);
                    buffer[1]='\0';
                    strcpy(token.lexeme,buffer);
                    categorizeToken(&token,3);
                    return token;
                 }
            }

        }
        else if(isdigit(ch))
        {
            int i = 0;
            appendToBuffer(&i, ch);

            while((ch = getc(fp)) != EOF)
            {
                /* '.' belongs to a possible number, so do not stop on it. */
                if((ch != '.' && isCandidateDelimiter(ch)))
                {
                    break;
                }

                if(!appendToBuffer(&i, ch))
                {
                    Token token;
                    strcpy(token.lexeme, buffer);
                    token.type = UNKNOWN;
                    return token;
                }
            }

            if(ch != EOF)
                ungetc(ch, fp);

            buffer[i] = '\0';

            Token token;
            strcpy(token.lexeme, buffer);
            categorizeToken(&token, 2);
            return token;
                        
        }
        else if(strchr(singleoperators,ch))
        {
            Token token;
            int first = ch;
            int next = getc(fp);

            buffer[0] = (char)first;
            buffer[1] = '\0';

            if(next != EOF)
            {
                char doubleBuffer[3];
                doubleBuffer[0] = (char)first;
                doubleBuffer[1] = (char)next;
                doubleBuffer[2] = '\0';

                if(isOperator(doubleBuffer) == 2)
                {
                    strcpy(token.lexeme, doubleBuffer);
                    categorizeToken(&token, 3);
                    return token;
                }

                ungetc(next, fp);
            }

            strcpy(token.lexeme, buffer);
            categorizeToken(&token, 3);
            return token;
        }
else if(strchr(specialCharacters,ch))
{
    Token token;

    /* ---------------- OPENING BRACKETS ---------------- */
    if(strchr(openbrackets, ch))
    {
        openbracketscount++;

        if(bracketTop >= MAX_BRACKET_STACK - 1)
        {
            printf("\nError in Line %d: Bracket stack overflow\n",
                   linecount);

            errorflag = 1;

            token.lexeme[0] = ch;
            token.lexeme[1] = '\0';
            token.type = UNKNOWN;

            return token;
        }

        bracketTop++;

        bracketStack[bracketTop] = ch;
        bracketLineStack[bracketTop] = linecount;
    }

    /* ---------------- CLOSING BRACKETS ---------------- */
    else if(strchr(closebrackets, ch))
    {
        closebracketscount++;

        /* No opening bracket exists */
        if(bracketTop == -1)
        {
            printf("\nError in Line %d: Unexpected closing bracket '%c'\n",
                   linecount, ch);

            errorflag = 1;

            token.lexeme[0] = ch;
            token.lexeme[1] = '\0';
            token.type = UNKNOWN;

            return token;
        }

        /*
         * Closing bracket exists, but check whether
         * it matches the most recent opening bracket.
         */
        if(!isMatchingBracket(bracketStack[bracketTop], ch))
        {
            printf("\nError in Line %d: Bracket mismatch. "
                   "Opening '%c' from Line %d cannot be closed by '%c'\n",
                   linecount,
                   bracketStack[bracketTop],
                   bracketLineStack[bracketTop],
                   ch);

            errorflag = 1;

            token.lexeme[0] = ch;
            token.lexeme[1] = '\0';
            token.type = UNKNOWN;

            return token;
        }

        /*
         * Correct matching pair.
         * Remove opening bracket from stack.
         */
        bracketTop--;
    }

    /* Return the bracket/special character normally */
    buffer[0] = ch;
    buffer[1] = '\0';

    strcpy(token.lexeme, buffer);
    categorizeToken(&token, 4);

    return token;
}

        else if(ch == '"')
        {
             int i = 0;
            int escaped = 0;
            Token token;

            appendToBuffer(&i, ch);

            while((ch = getc(fp)) != EOF)
            {
                if(ch == '\n' && !escaped)
                {
                    buffer[i] = '\0';
                    strcpy(token.lexeme, buffer);
                    token.type = UNKNOWN;

                    printf("\nError in Line %d: Unterminated string literal: %s\n",
                           linecount, buffer);
                    linecount++;
                    errorflag = 1;
                    return token;
                }

                if(!appendToBuffer(&i, ch))
                {
                    strcpy(token.lexeme, buffer);
                    token.type = UNKNOWN;
                    return token;
                }

                if(ch == '"' && !escaped)
                {
                    buffer[i] = '\0';
                    strcpy(token.lexeme, buffer);
                    categorizeToken(&token, 5);
                    return token;
                }

                if(ch == '\\' && !escaped)
                    escaped = 1;
                else
                    escaped = 0;
            }

            buffer[i] = '\0';
            strcpy(token.lexeme, buffer);
            token.type = UNKNOWN;
            printf("\nError in Line %d: Unterminated string literal: %s\n", linecount, buffer);
            errorflag = 1;
            return token;
        }

        /* ---------------- CHARACTER LITERAL ---------------- */
        else if(ch == '\'')
        {
            int i = 0;
            int escaped = 0;
            int contentCount = 0;
            Token token;

            appendToBuffer(&i, ch);

            while((ch = getc(fp)) != EOF)
            {
                if(ch == '\n' && !escaped)
                {
                    buffer[i] = '\0';
                    strcpy(token.lexeme, buffer);
                    token.type = UNKNOWN;
                    printf("\nError in Line %d: Unterminated character literal: %s\n",
                           linecount, buffer);
                    linecount++;
                    errorflag = 1;
                    return token;
                }

                if(!appendToBuffer(&i, ch))
                {
                    strcpy(token.lexeme, buffer);
                    token.type = UNKNOWN;
                    return token;
                }

                if(ch == '\'' && !escaped)
                {
                    buffer[i] = '\0';

                    if(contentCount != 1)
                    {
                        strcpy(token.lexeme, buffer);
                        token.type = UNKNOWN;
                        printf("\nError in Line %d: Invalid character literal: %s\n",
                               linecount, buffer);
                        errorflag = 1;
                        return token;
                    }

                    strcpy(token.lexeme, buffer);
                    categorizeToken(&token, 6);
                    return token;
                }

                if(ch == '\\' && !escaped)
                {
                    escaped = 1;
                    /* '\\x' represents one logical character. */
                }
                else
                {
                    contentCount++;
                    escaped = 0;
                }
            }

            buffer[i] = '\0';
            strcpy(token.lexeme, buffer);
            token.type = UNKNOWN;
            printf("\nError in Line %d: Unterminated character literal: %s\n", linecount, buffer);
            errorflag = 1;
            return token;
        } 
        /*----------------N O T   U S E D---------------------------------------------------------------*/
        else if((int)ch == 39) // single quot
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
            if(strlen(buffer)!=3)
            {
                printf("\nError in Line %d, Error: %s \n", linecount, buffer);
                errorflag=1;
                Token token;
                categorizeToken(&token,8);
                return token;
            }
        
            else
            {
                Token token;
                strcpy(token.lexeme,buffer);
                categorizeToken(&token,6);
                return token;
            }
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
         if(bracketTop != -1)
        {
            printf("\nError: Unclosed bracket '%c' opened in Line %d\n",
                bracketStack[bracketTop],
                bracketLineStack[bracketTop]);

            errorflag = 1;
        }
        //while loop ends meaning reached EOF
                 Token token;
               strcpy(token.lexeme,"");
               token.type=TOKEN_EOF;
               return token;
}
    


void categorizeToken(Token* token, short int category)
{
    token->type = UNKNOWN;
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
        for(int j=0;j<19;j++)
        {
            if(strcmp(str,doubleoperators[j])==0)
            {
                return 2; //double oiperator
            }
        }
    }
    return 0; //not operator

}
int isConstant(char *str)

{
    int i = 0;
    int dotCount = 0;

    if(str[0] == '\0')
    {
        printf("\nError in Line %d: Empty numeric constant\n", linecount);
        errorflag = 1;
        return 0;
    }

    /* '-' is now always tokenized as an operator, not part of a constant. */

    /* ---------------- HEXADECIMAL ---------------- */
    if(str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))
    {
        i += 2;

        if(str[i] == '\0')
        {
            printf("\nError in Line %d: At least one hexadecimal digit required: %s\n",
                   linecount, str);
            errorflag = 1;
            return 0;
        }

        while(str[i] != '\0')
        {
            if(!isxdigit((unsigned char)str[i]))
            {
                printf("\nError in Line %d: Invalid hexadecimal constant %s \n",
                       linecount, str);
                errorflag = 1;
                return 0;
            }
            i++;
        }

        return 1;
    }

    /* ---------------- BINARY ---------------- */
    if(str[i] == '0' && (str[i + 1] == 'b' || str[i + 1] == 'B'))
    {
        i += 2;

        if(str[i] == '\0')
        {
            printf("\nError in Line %d: At least one binary digit required: %s\n",
                   linecount, str);
            errorflag = 1;
            return 0;
        }

        while(str[i] != '\0')
        {
            if(str[i] != '0' && str[i] != '1')
            {
                printf("\nError in Line %d: Invalid binary constant %s\n",
                       linecount, str);
                errorflag = 1;
                return 0;
            }
            i++;
        }

        return 1;
    }

    /* ---------------- OCTAL ---------------- */
    if(str[i] == '0' &&
       isdigit((unsigned char)str[i + 1]) &&
       strchr(str, '.') == NULL)
    {
        i++;

        while(str[i] != '\0')
        {
            if(str[i] < '0' || str[i] > '7')
            {
                printf("\nError in Line %d: Invalid octal constant %s\n",
                       linecount, str);
                errorflag = 1;
                return 0;
            }
            i++;
        }

        return 1;
    }
/* ---------------- DECIMAL / FLOAT ---------------- */

/* Optional negative sign */
if(str[i] == '-')
{
    i++;

    /* "-" alone is not a valid constant */
    if(str[i] == '\0')
    {
        printf("\nError in Line %d: Invalid numeric constant %s\n",
               linecount, str);
        errorflag = 1;
        return 0;
    }
}
while(str[i] != '\0')
{
    if(isdigit((unsigned char)str[i]))
    {
        i++;
    }

    else if(str[i] == '.')
    {
        dotCount++;

        if(dotCount > 1)
        {
            printf("\nError in Line %d: More than one decimal point in %s\n",
                   linecount, str);
            errorflag = 1;
            return 0;
        }

        /* Require digit after decimal point */
        if(!isdigit((unsigned char)str[i + 1]))
        {
            printf("\nError in Line %d: Digit required after decimal point in %s\n",
                   linecount, str);
            errorflag = 1;
            return 0;
        }

        i++;
    }

    else
    {
        printf("\nError in Line %d: Invalid numeric constant %s\n",
               linecount, str);
        errorflag = 1;
        return 0;
    }
}

return 1;
}





int isIdentifier(const char* str)
{
     int i = 0;

    if(!(isalpha((unsigned char)str[0]) || str[0] == '_' || str[0] == '$'))
    {
        printf("\nError in Line %d, Error: %s \n", linecount, str);
        printf("Identifiers must start with a letter, underscore, or dollar sign.\n");
        errorflag=1;
        return 0;
    }

    i++;

    while(str[i] != '\0')
    {
        if(!(isalnum((unsigned char)str[i]) || str[i] == '_'))
        {
            printf("\nError in Line %d, Error: %s \n", linecount, str);
            printf("Identifiers can only contain letters, digits, underscores, or dollar signs.\n");
            errorflag=1;
            return 0;
        }

        i++;
    }

    return 1;
  //  printf("Checking if '%s' is an identifier\n", str);
}

int isSpecialCharacter(char ch)
{
    if (strchr(specialCharacters, ch)) 
    {
        return 1; // It's a special character
    }
    return 0; // Not a special character
}
