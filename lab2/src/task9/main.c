#include <stdio.h>
#include "fractions.h"


int main(void) {
    unsigned int base = 0;

    printf("Enter base (2..36): ");
    if (scanf("%u", &base) != 1 || base < 2 || base > 36) {
        fprintf(stderr, "Error: base must be an integer in range [2; 36]\n");
        return 1;
    }

    const char *f1 = "0.5";
    const char *f2 = "0.1";
    const char *f3 = "1/2";
    const char *f4 = "3/8";
    const char *f5 = "0.333333";
    const char *f6 = "1/3";

    process_fractions(base, 6, f1, f2, f3, f4, f5, f6);

    return 0;
}