#include <stdio.h>      /* Standard Library of Input and Output */
#include <complex.h>    /* Standart Library of Complex Numbers */

int main() {

    float complex z1 = 1.0 + 1.0 * I;
    float complex z2 = 1.0 - 1.0 * I;

    float complex sum = z1 + z2;

    printf("Z1=%.2f %+.2fi\n", creal(z1), cimag(z1));
    printf("Z2=%.2f %+.2fi\n", creal(z2), cimag(z2));
    printf("The sum: Z1 + Z2 = %.2f %+.2fi\n", creal(sum), cimag(sum));

    return 0;

}
