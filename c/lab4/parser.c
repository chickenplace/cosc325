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
int linei = 0; //keeps track of which line index we are on while running a program
int reti = 0; //keeps track of index we should return to afrer a gosub

//Dirty symbol table 
int symboltable[26]; // preallocate enough room for 26 variables (A-Z) (position 0 = A, position 1 = B, etc.)
int symboldefined[26]; // 0 if the variable is not defined, 1 if it is defined

void line();
void statement();
void expr_list();
void var_list();
int expression();
int relop();
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

int findLine(int lineno) {
    int hit = search(lineno);
    if(hit < 0) {
        printf("Could not find line number target %d\n", lineno);
        exit(1);
    }
    return hit;
}

void killIf() {
    //foce the lexer to read to the end of the line
    //this is when an if statement is false so that it finishes parsing
    while(nextChar != '\n' && nextToken != 0) {
        getChar();
    }
    nextToken = CR; // set next token to CR so that the rest of the line is ignored and we can move on to the next line
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
            //printf("Stored this line: %s at line number %d, which is index %d\n", rest_of_line, lineno, lineindex);
            lineindex++;
        } else {
            strcpy(lines[previndex], rest_of_line);
            //printf("Overwrote old line with this line: %s at line number %d, which is index %d\n", rest_of_line, lineno, previndex);
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
    int targetlineno; // Only used for GOTO and GOSUB statements
    int if_a;
    int if_b;
    int op;
    switch(nextToken) {
        case PRINT:
            lex();
            expr_list();
            // unconditionally printf("\n");
            break;
        case IF:
            lex();
            if_a = expression(); 
            op = relop();
            //lex();
            if_b = expression();
            if (nextToken != THEN) {
                printf("error! expecting then found something else\n");
            }
            lex();
            switch (op) {
                case 0:
                    if (if_a < if_b) {
                        statement();
                    } else {
                        killIf();
                    }
                    break;

                case 1:
                    if (if_a > if_b) {
                        statement();
                    } else {
                        killIf();
                    }
                    break;

                case 2:
                    if (if_a == if_b) {
                        statement();
                    } else {
                        killIf();
                    }
                    break;

                case 3:
                    if (if_a <= if_b) {
                        statement();
                    } else {
                        killIf();
                    }
                    break;

                case 4:
                    if (if_a >= if_b) {
                        statement();
                    } else {
                        killIf();
                    }
                    break;

                case 5:
                    if (if_a != if_b) {
                        statement();
                    } else {
                        killIf();
                    }
                    break;
            }
            // we never need an extra call to lex() here 
            // because statement() ALWAYS has an extra call to lex()
            break;

        case GOTO:
            lex();
            targetlineno = expression();
            //find linei we are supposed to jump to
            linei = findLine(targetlineno) - 1; // from a recursive call in the middle of a for loop that executes linei++, must set to target index - 1
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
            targetlineno = expression();
            // find linei we are supposed to jump to
            reti = linei; // store the current linei in reti so we know where to return to after the subroutine is done
            linei = findLine(targetlineno) - 1; // from a recursive call in the middle of a for loop that executes linei++, must set to target index - 1
            // extra call to lex to look for the carriage return
            break;
            
        case RETURN:
            linei = reti;
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
            for(linei=0; linei<lineindex; linei++) {
                //GOTO and GOSUB alter linei
                in_str = lines[linei];
                stri = 0;
                //printf("Executing: %d %s\n", linenos[linei], in_str);
                getChar();
                lex();
                line();
            }
            lex();
            break;

        case REM:
            // ignore the line
            lex();
            killIf(); // just read to the end of the line and do nothing
            break;

        case END:
            //force loop to stop by setting linei = to lineindex
            linei = lineindex;
            stri = -1; // this puts us back into "file reading" mode
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
        printf("%s", lexeme);
        lex(); //extra call to look for comma or carriage return
        
    } else {
        printf("%d\n", expression());
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
            printf("%d\n", expression());
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
    int varpos[26];
    int varcnt = 0;
    if (nextToken == IDENT) {
        // grab position before calling lex() again
        varpos[varcnt++] = lexeme[0] - 'A'; //Switch to nextChar if not working
        lex();
    } else {
        printf("Expecting IDENT but found: %d\n", nextToken);
        exit(1);
    }
    while(nextToken == COMMA) {
        lex();
        if (nextToken == IDENT) {
            // grab position before calling lex() again
            varpos[varcnt++] = lexeme[0] - 'A'; //Switch to nextChar if not working
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
    // now we need to use scanf to read from the console and convert whatever they type into a number
    // and then store that number in the symbol table varpos[] found during parsing
    for (int i = 0; i < varcnt; i++) {
        scanf("%d", &symboltable[varpos[i]]);
        symboldefined[varpos[i]] = 1; // mark this variable as defined in the symbol table
    }
}

//0: <, 1; >, 2: =, 3: <=, 4: >=, 5: <> or ><
int relop() {
    if(nextToken == LT_OP){
        lex();
        if(nextToken == EQUALS_OP || nextToken == GT_OP) {
            lex();
            if(nextToken == GT_OP) {
                return 5; // <>
            } else {
                return 3; // <=
            }
        } else {
            return 0;
        }
    }
    else if(nextToken == GT_OP){
        lex();
        if(nextToken == EQUALS_OP || nextToken == LT_OP) {
            lex();
            if(nextToken == LT_OP) {
                return 5; // ><
            } else {
                return 4; // >=
            }
        } else {
            return 1;
        }
    }
    else if(nextToken == EQUALS_OP){
        lex();
        return 2;
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
        if (nextToken == MULT_OP) {
            lex();
            result *= factor();
        } else {
            lex();
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