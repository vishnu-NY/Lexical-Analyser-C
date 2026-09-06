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

static const char* singleoperators = "+-*/%=!<>.|&^~";
static const char* doubleoperators[19] = 
{
    "==","++","--","+=",">>","<<","<=",">=","!=","&&","||","&=","|=","^=","->","-=","*=","/=","%="
};
static const char* specialCharacters = ",;{}()[]";
static const char* openbrackets = "{([";
static const char* closebrackets = "})]";
 int openbracketscount=0;
 int closebracketscount=0;

char currentChar;
int errorflag=0;
short int identifier=-1;
 int ch;
char buffer[MAX_TOKEN_SIZE];
extern FILE* fp;
int i=10;
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
            continue;
        else if(ch =='#')
        {
            while((ch=getc(fp))!=EOF && ch!='\n');
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
                while((ch=getc(fp))!=EOF)
                {
                    if(ch=='\n')
                    {
                        linecount++;
                    }
                    if(ch=='*')
                    {
                        if((ch=getc(fp))=='/')
                            break;
                        else
                            ungetc(ch,fp);
                    }
                    continue;
                }
                
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
            int i=0;
            buffer[i++]=ch;
            while((ch=getc(fp))!=EOF && (isalnum(ch) || ch=='_' || ch=='$'))
                {
                    buffer[i]=ch;
                     i++;
                }
                
              if(ch != EOF)  ungetc(ch, fp);  
            buffer[i]='\0';
          // printf("Buffer = %s\n",buffer);
            
          //  printf("before token");
            Token token;
            strcpy(token.lexeme,buffer);
          //  printf("Token = %s, ",token.lexeme);
            categorizeToken(&token,1);
         
            return token;
        }
        
        else if(ch=='-')
        {
            if(isdigit(ch=getc(fp)))
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
                        if(ch=='0')
                        {
                            ch=getc(fp);
                            if(ch=='x' || ch=='X')
                            {
                                int i=0;
                                buffer[i++]='0';
                                buffer[i++]=ch;
                                while((ch=getc(fp))!=EOF && (isxdigit(ch)))
                                    {
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
                            
                            else if(ch=='b' || ch=='B')
                            {
                                int i=0;
                                buffer[i++]='0';
                                buffer[i++]=ch;
                                while((ch=getc(fp))!=EOF && (ch=='0' || ch=='1'))
                                    {
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
                            
                            else if(ch<= '7' && ch>='0')
                            {
                                int i=0;
                                buffer[i++]='0';
                                buffer[i++]=ch;
                                while(((ch=getc(fp))!=EOF) && ((ch<='7' && ch>='0'))||((ch=='.')||ch==' '))
                                    {
                                        buffer[i]=ch;
                                        i++;
                                    }
                                    if(isdigit(ch))
                                    {
                                        buffer[i]='\0';
                                        Token token;
                                        strcpy(token.lexeme,buffer);
                                        printf("\nError in Line %d, Error: %s \n", linecount, buffer);
                                        errorflag=1;    
                                        //error : invalid octal number
                                        categorizeToken(&token,8);
                                        return token;
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
                                ch='0';
                            }

                        }
                    
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
        else if(strchr(specialCharacters,ch))
        {
            if(strchr(openbrackets,ch))
            {
                openbracketscount++;
            }
            if(strchr(closebrackets,ch))
            {
                closebracketscount++;
            }
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
                if(ch==EOF || ch=='\n')
                {
                     printf("\nError in Line %d, Error: %s \n", linecount, buffer);
                     errorflag=1;
                    categorizeToken(&token,8);
                    return token;
                }
                buffer[i]=ch;
                i++;
            }
            buffer[i] = ch; i++; 
            buffer[i]='\0';
            strcpy(token.lexeme,buffer);
            categorizeToken(&token,5);
            return token;
        } 
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
int isConstant(const char *str)
{
    int i = 0;
    int dotCount = 0;

    /* Empty string */
    if (str[0] == '\0')
        return 0;

    /* Optional negative sign */
    if (str[i] == '-')
    {
        i++;

        /* Only "-" is not a constant */
        if (str[i] == '\0')
            return 0;
    }

    /* -------------------------------------------------
       HEXADECIMAL CONSTANT
       ------------------------------------- */
    if (str[i] == '0' &&
        (str[i + 1] == 'x' || str[i + 1] == 'X'))
    {
        i += 2;

        /* At least one hexadecimal digit required */
        if (str[i] == '\0')
            return 0;

        while (str[i] != '\0')
        {
            if (!isxdigit((unsigned char)str[i]))
                return 0;

            i++;
        }

        return 1;
    }

    /* -------------------------------------------------
       BINARY CONSTANT
       
       ------------------------------------------------- */
    if (str[i] == '0' &&
        (str[i + 1] == 'b' || str[i + 1] == 'B'))
    {
        i += 2;

        /* At least one binary digit required */
        if (str[i] == '\0')
            return 0;

        while (str[i] != '\0')
        {
            if (str[i] != '0' && str[i] != '1')
                return 0;

            i++;
        }

        return 1;
    }

    /* -------------------------------------------------
       OCTAL CONSTANT
       
       ------------------------------------------------- */
    if (str[i] == '0' &&
        isdigit((unsigned char)str[i + 1]))
    {
        i++;

        while (str[i] != '\0')
        {
            if (str[i] < '0' || str[i] > '7')
                return 0;

            i++;
        }

        return 1;
    }

    /* -------------------------------------------------
       DECIMAL / FLOATING POINT CONSTANT
      
       ------------------------------------------------- */

    while (str[i] != '\0')
    {
        if (isdigit((unsigned char)str[i]))
        {
            i++;
        }
        else if (str[i] == '.')
        {
            dotCount++;

            /* More than one decimal point */
            if (dotCount > 1)
                return 0;

            /*
             * Require a digit after the decimal point.
             * Therefore:
             * 12.   -> invalid
             * 12.5  -> valid
             */
            if (!isdigit((unsigned char)str[i + 1]))
                return 0;

            i++;
        }
        else
        {
            /* Any other character makes it invalid */
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
        errorflag=1;
        return 0;
    }

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

int isSpecialCharacter(char ch)
{
    if (strchr(specialCharacters, ch)) 
    {
        return 1; // It's a special character
    }
    return 0; // Not a special character
}
