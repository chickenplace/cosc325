#include <stdio.h>

/*
 * C Imperative Paradigm Lab
 * Parts 1–4 are implemented in this file.
 *
 * Do NOT change function signatures.
 * 
 * 1/15/26 - Brian Toone
 */

/* ===== Part 2 ===== */
int sum_to(int n) {
    int i;
    int s = 0;
    for(i = 1; i <= n; i++) { 
        s += i; //Reflection 2.1: Line 17 is where control happens and where the state of the sum variable (represented by s) changes.
    }
    return s;
}
//Reflection 2.2: The function in C must define the loop structure as well as the return value.

/* ===== Part 3 ===== */
double avg_1_to_n(int n) {
    int sum = sum_to(n);
    double avg = (double)sum / n;
    return avg;
}
//Reflection 3.1: C requires you to cast sum to a double to avoid integer division.

/* ===== Part 4 ===== */
void increment_bad(int x) {
    x++;
}
//Reflection 4.1: The increment_bad function does not work as intended since it uses pass-by-value
//meaning that it only modifies a copy of the variable passed in, not the original variable itself.

void increment(int *x) {
    (*x)++;
}
//Reflection 4.2: Using pointers allows the function to modify the state of the original variable
//and this allows for multiple function to access this variable through state sharing.

int main() {

    /* ===== Part 1 ===== */
    printf("--- Part 1: State and Sequence ---\n");

    int x = 2;
    x = x + 3;
    x = x * 4;
    printf("x=%d\n", x); //x = 20 for this case
    //Reflection 1.1: The value of x changed over time because it was reassigned multiple times.
    //The intermediate values of x were in the location in memory allocated to x.

    
    x = 2;
    x = x * 4;
    x = x + 3;
    printf("x=%d\n", x); 
    //Reflection 1.2: The meaning changed, and the order of operations matters because it effects the final result.


    /* ===== Part 2 ===== */
    printf("\n--- Part 2: Control Flow ---\n");
    printf("sum_to(5) = %d (expected 15)\n", sum_to(5));
    printf("sum_to(1) = %d (expected 1)\n", sum_to(1));


    /* ===== Part 3 ===== */
    printf("\n--- Part 3: Functions as Procedures ---\n");
    printf("avg_1_to_n(5) = %.1f (expected 3.0)\n", avg_1_to_n(5));


    /* ===== Part 4 ===== */
    printf("\n--- Part 4: Pass-by-Value vs Pointers ---\n");

    int a = 5;
    increment_bad(a);
    printf("after increment_bad: a=%d (expected 5)\n", a);

    increment(&a);
    printf("after increment: a=%d (expected 6)\n", a);

    return 0;
}
