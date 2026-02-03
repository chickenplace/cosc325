#include "lexer.c" //def not the right way to do this but ok for now

void line(); //forward declaration
void statement(); //forward declaration

int main() {
   /* Open the input data file and process its contents */
   if  ((in_fp = fopen("front.in", "r")) == NULL)
     printf("ERROR - cannot open front.in \n");
   else  {
     getChar();
     do   {
       lex(); //lex calls getnonblank which calls getchar
       line();
    }  while (nextToken != EOF);
  }
}

void line() {
    if (nextToken == NUMBER) { //If line number then handle
        //consume token by looking at line numver and storing the statement that follows in the right place in our new program
        //but not for this assignment
        //Call lex() to get next token;
        lex();
    } else { //Else its just a statement

    }
}