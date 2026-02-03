#include "lexer.c" //def not the right way to do this but ok for now

int main() {
   /* Open the input data file and process its contents */
   if  ((in_fp = fopen("front.in", "r")) == NULL)
     printf("ERROR - cannot open front.in \n");
   else  {
     getChar();
     do   {
       lex(); //lex calls getnonblank which calls getchar
    }  while (nextToken != EOF);
  }
}