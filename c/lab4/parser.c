#include "lexer.c"

// create data structure
typedef struct {
    int lineno;
    char* line;
} Line;
Line storedProgram[10]; // preallocate enough room for 10 lines

// data structures needed for the interpreter
int lineno = 0;   // if this is equal to 0 then we should execute immediately
char* lines[1000];  // preallocate enough room for 10 lines
int linenos[1000];  
int lineindex = 0; //keeps track of how many lines wee have and where the next line should be stored in the lines data structure

//Dirty symbol table 
int symboltable[26]; // preallocate enough room for 26 variables (A-Z) (position 0 = A, position 1 = B, etc.)
int symboldefined[26]; // 0 if the variable is not defined, 1 if it is defined

void line();
void statement();
void expr_list();
void var_list();
int expression();
void relop();
int term();
int factor();

//hard coded swap of the positions applied to two different data structures
void swap(int j, int k) {
    //char* first
    char* temp = lines[j];
    lines[j] = lines[k];
    lines[k] = temp;

    //swapping the int line numbers in the linenos array
    int tmp = linenos[j];
    linenos[j] = linenos[k];
    linenos[k] = tmp;
}

void sort() { //helper function to sort lines by line number
    int i = 1;
    while (i < lineindex) {
        int j = i;
        while (j > 0 && linenos[j-1] > linenos[j]) {
            swap(j, j-1);
            j = j - 1;
        }
        i = i + 1;
    }
}  

//search through linenos and return index if found
//otherwise return -1
int search(int lineno) {
    for (int i = 0; i < lineindex; i++){
        if(linenos[i] == lineno) {
            return i;
        }
    }
    return -1;
}

/******************************************************/
/* main driver */
int main()
{

    //prep symbol table
    for(int i = 0; i < 26; i++) {
        symboldefined[i] = 0; // initialize all variables to be undefined
    }

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
        int previndex = search(lineno);
        if (previndex < 0) {
            linenos[lineindex] = lineno;
        }
        // take whatever is left in the rest of the line and store it for processing later!

        // consume the token by looking at the line number
        // and storing the statement that follows in the right place in our stored program
        // BUT NOT FOR THIS ASSIGNMENT
        // Call special function lex_endl() to get the rest of the line
        lex_endl(); //sets global variable named rest_of_line that we must store in the right place in lines data structure
        //allocate memory for the new line we read via lex_endl()
        // copy line we read into the new memory location
        if (previndex < 0) {
            lines[lineindex] = malloc(1000);
            strcpy(lines[lineindex], rest_of_line);
            printf("Stored this line: %s at line number %d, which is index %d\n", rest_of_line, lineno, lineindex);
            lineindex++;
        } else {
            strcpy(lines[previndex], rest_of_line);
            printf("Overwrote old line with this line: %s at line number %d, which is index %d\n", rest_of_line, lineno, previndex);
        }
    } else {
        statement(); // note that statement MUST have an extra call to lex()
    }
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
            int pos = lexeme[0] - 'A';
            symboldefined[pos] = 1; // mark this variable as defined in the symbol table
            lex();
            if(nextToken != EQUALS_OP) {
                printf("Expecting EQUALS_OP but found: %d\n", nextToken);
                exit(1);
            }
            lex();
            symboltable[pos] =expression();
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
            lineindex = 0; // just reset the lineindex to 0 to "clear" the program
            lex();
            break;

        case LIST:
            sort();
            for(int i = 0; i < lineindex; i++) {
                printf("%d: %s\n", linenos[i], lines[i]);
            }
            lex();
            break;

        case RUN:
            sort(); 
            //TODO: update lexer to take in a string 
            //instead of always reading from file
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
        printf("%s", lexeme);
    } else {
        printf("%d\t", expression());
    }
    while (nextToken == COMMA) {
        lex(); // extra call to look for the comma
        // next assignment: printf("\t");
        if (nextToken == STRING) {
            // do nothing for this assignment
            // but in the next assignment you will need to print something
            lex(); //extra call to look for comma or carriage return
            printf("%s", lexeme);
        } else {
            printf("%d\t", expression());
        }
        // there are only two valid tokens AT THIS SPOT
        if (nextToken != COMMA && nextToken != CR) {
            printf("Expecting COMMA or CR but found: %d\n", nextToken);
            exit(1);
        }
    }
    printf("\n");
}
// lex() MUST have already been called before here
int expression() {
    if(nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        //next assignment: if next token is sub op, negate the statement
    }
    int result = term();
    while (nextToken == ADD_OP || nextToken == SUB_OP) {
        lex();
        if (nextToken == ADD_OP) {
            result += term();
        } else {
            result -= term();
        }
    }
    return result;
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
    else if(nextToken == EQUALS_OP){
        lex();
    }
    else {
        printf("Expecting a relop (>, <, >=, <=) but found %d instead\n", nextToken);
        exit(1);
    }
}
// lex() MUST have already been called before here
int term() {
    int result = factor();
    lex();
    while (nextToken == MULT_OP || nextToken == DIV_OP) {
        lex();
        if (nextToken == MULT_OP) {
            result *= factor();
        } else {
            result /= factor();
        }
        lex();
    }
    return result;
}
// lex() MUST have already been called before here
int factor() {
    if (nextToken == IDENT) {
        // do nothing for this assignment
        // but in the next assignment you will need to print something!
        int pos = lexeme[0] - 'A';
        if(symboldefined[pos] == 0) {
            printf("HOW? Undefined symbol %c\n", lexeme[0]);
            exit(1);
        }
        return symboltable[pos];
    } else if (nextToken == NUMBER) {
        return atoi(lexeme);
    } else if (nextToken == LEFT_PAREN) {
        lex();
        int result = expression();
        if(nextToken != RIGHT_PAREN) {
            printf("Expecting RIGHT_PAREN but found: %d\n", nextToken);
            exit(1);
        }
        return result;
    } else {
        printf("Expecting IDENT, NUMBER, or LEFT_PAREN but found: %d\n", nextToken);
        exit(1);
    }
    printf("WHAT? Syntax error, expecting IDENT, NUMBER, or LEFT_PAREN but found something else\n");
    exit(1);
    return -999; //should never happen
}