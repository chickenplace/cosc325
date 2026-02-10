#include "lexer.c"

// data structures needed for the interpreter
int lineno = 0;   // if this is equal to 0 then we should execute immediately
char* lines[10];  // preallocate enough room for 10 lines
int linenos[10];  

void line();
void statement();
void expr_list();
void var_list();
void expression();
void relop();
void term();
void factor();

/******************************************************/
/* main driver */
int main()
{
  /* Open the input data file and process its contents */
  if ((in_fp = fopen("front.in", "r")) == NULL)
    printf("ERROR - cannot open front.in \n");
  else
  {
    getChar();
    do
    {
      lex();
      line();
    } while (nextToken != EOF);
  }
}

// lex() MUST be called before this function
void line() {
    printf("in Line\n");
    if (nextToken == NUMBER) {
        lineno = atoi(lexeme);
        // take whatever is left in the rest of the line and store it for processing later!

        // consume the token by looking at the line number
        // and storing the statement that follows in the right place in our stored program
        // BUT NOT FOR THIS ASSIGNMENT
        // Call lex() to get the next token
        lex();
    }
    statement(); // note that statement MUST have an extra call to lex()
    if (nextToken != CR && nextToken != EOF) {
        printf("Expecting CR, but found: %d instead!\n", nextToken);
    } 

}

// lex() MUST have already been called before here
void statement() {
    printf("in statement\n");
    switch(nextToken) {
        case PRINT:
            lex();
            expr_list();
            // unconditionally printf("\n");
            break;
        case IF:
            lex();
            expression(); 
            lex();
            relop();
            lex();
            expression();
            lex();
            if (nextToken != THEN) {
                printf("error! expecting then found something else");
            }
            lex();
            statement();
            // we never need an extra call to lex() here 
            // because statement() ALWAYS has an extra call to lex()
            break;
        case GOTO:
            lex();
            expression();
            // extra call to lex to look for the carriage return
            lex();
            break;

        // keep going with more cases INPUT DOES NOT NEED THE EXTRA CALL TO LEX ... NEITHER DO THE ONES THAT ARE JUST KEYWORDS
        case INPUT:
            lex();
            var_list(); 
            break;

        case LET:
            lex();
            if(nextToken != IDENT) {
                printf("Expecting IDENT but found: %d\n", nextToken);
                exit(1);
            }
            lex();
            if(nextToken != EQUALS_OP) {
                printf("Expecting EQUALS_OP but found: %d\n", nextToken);
                exit(1);
            }
            lex();
            expression();
            // don't need extra call to lex() b/c expression already did it looking for + or - or * or /
            break;
        
        case GOSUB:
            lex();
            expression();

            // extra call to lex to look for the carriage return
            lex();
            break;
            
        case RETURN:
            lex();
            break;

        case CLEAR:
            lex();
            break;

        case LIST:
            lex();
            break;

        case RUN:
            lex();
            break;

        case END:
            lex(); // this IS the extra call to lex() since nothing comes after these keywords
                  //. you probably need another call to lex() right here!!!!
            break;
    }
}

// makes an extra call to lex() to look for the comma
// lex has ALREADY been called before expr_list
void expr_list() {
    printf("in expr_list\n");
    if (nextToken == STRING) {
        // do nothing for this assignment
        // but in the next assignment you will need to print something!
    } else {
        expression();
    }
    lex(); // extra call to look for the comma
    while (nextToken == COMMA) {
        // next assignment: printf("\t");
        if (nextToken == STRING) {
            // do nothing for this assignment
            // but in the next assignment you will need to print something
        } else {
            expression();
        }
        lex(); // extra call to look for the comma
        // there are only two valid tokens AT THIS SPOT
        if (nextToken != COMMA && nextToken != CR) {
            printf("Expecting COMMA or CR but found: %d\n", nextToken);
            exit(1);
        }
    }
}

void expression() {
    printf("in expression, nextToken is: %d\n", nextToken);
    lex(); // you gotta do more than this!
    if(nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
    }
    term();
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        term();
    }
}

void var_list() {
    if (nextToken == IDENT) {
        // do nothing for this assignment
        // but in the next assignment you will need to print something!
    } else {
        printf("Expecting IDENT but found: %d\n", nextToken);
        exit(1);
    }
}

void relop() {

}

void term() {
    printf("in term, nextToken is: %d\n", nextToken);
    lex();
    if (nextToken == IDENT || nextToken == NUMBER) {
        factor();
        lex();
        while(nextToken == MULT_OP || nextToken == DIV_OP) {
            lex();
            factor();
            lex();
        }
    } else {
        printf("Expecting IDENT or NUMBER but found: %d\n", nextToken);
        exit(1);
    }
}

void factor() {
    printf("in factor, nextToken is: %d\n", nextToken);
    if (nextToken == IDENT || nextToken == NUMBER) {
        // do nothing for this assignment
        // but in the next assignment you will need to print something!
    } else if (nextToken == LEFT_PAREN) {
        lex();
        expression();
        if(nextToken != RIGHT_PAREN) {
            printf("Expecting RIGHT_PAREN but found: %d\n", nextToken);
            exit(1);
        }
        lex();
    } else {
        printf("Expecting IDENT, NUMBER, or LEFT_PAREN but found: %d\n", nextToken);
        exit(1);
    }
}