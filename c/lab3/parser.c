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
    switch(nextToken) {
        case PRINT:
            lex();
            expr_list();
            // unconditionally printf("\n");
            break;
        case IF:
            lex();
            expression(); 
            relop();
            //lex();
            expression();
            if (nextToken != THEN) {
                printf("error! expecting then found something else\n");
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
    if (nextToken == STRING) {
        // do nothing for this assignment
        // but in the next assignment you will need to print something!
        lex(); //extra call to look for comma or carriage return
    } else {
        expression();
    }
    while (nextToken == COMMA) {
        lex(); // extra call to look for the comma
        // next assignment: printf("\t");
        if (nextToken == STRING) {
            // do nothing for this assignment
            // but in the next assignment you will need to print something
            lex(); //extra call to look for comma or carriage return
        } else {
            expression();
        }
        // there are only two valid tokens AT THIS SPOT
        if (nextToken != COMMA && nextToken != CR) {
            printf("Expecting COMMA or CR but found: %d\n", nextToken);
            exit(1);
        }
    }
}
// lex() MUST have already been called before here
void expression() {
    if(nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        //next assignment: if next token is sub op, negate the statement
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
        lex();
    } else {
        printf("Expecting IDENT but found: %d\n", nextToken);
        exit(1);
    }
    while(nextToken == COMMA) {
        lex();
        if (nextToken == IDENT) {
            // do nothing for this assignment
            // but in the next assignment you will need to print something!
            lex();
        } else {
            printf("Expecting IDENT but found: %d\n", nextToken);
            exit(1);
        }
        if(nextToken != COMMA && nextToken != CR) {
            printf("Expecting COMMA or CR but found: %d\n", nextToken);
            exit(1);
        }
    }
}

void relop() {
    if(nextToken == LT_OP){
        lex();
        if(nextToken == EQUALS_OP){
            printf("LTE op found\n");
            lex();
        }
    }
    else if(nextToken == GT_OP){
        lex();
        if(nextToken == EQUALS_OP){
            printf("GTE op found\n");
            lex();
        }
    }
    else {
        printf("Expecting a relop (>, <, >=, <=) but found %d instead\n", nextToken);
        exit(1);
    }
}
// lex() MUST have already been called before here
void term() {
    factor();
    lex();
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        factor();
        lex();
    }
}
// lex() MUST have already been called before here
void factor() {
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
    } else {
        printf("Expecting IDENT, NUMBER, or LEFT_PAREN but found: %d\n", nextToken);
        exit(1);
    }
}