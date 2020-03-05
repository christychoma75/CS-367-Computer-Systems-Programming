#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "fp.h"

// output all values representable...

int main()
{
    int i, j;
    float E;
    int exp_size = (1 << EXPONENT_BITS);
    int fract_size = (1 << FRACTION_BITS);
    int bias = (1 << (EXPONENT_BITS - 1)) - 1;

    printf("All values for %d exponent bits (bias = %d) and %d fraction bits\n\n",
	 EXPONENT_BITS, bias, FRACTION_BITS);

    printf("DE-NORMALIZED\n");
    E = powf(2.0, (float) 1 - bias);
    printf("E=%d\n", 1 - bias);
    for (j = 0; j < fract_size; j++) {
	double M = (0 + (double) j / fract_size);
	printf("  M = %1f, val=%0.15lf\n", M, M * E);
    }

    printf("NORMALIZED\n");
    for (i = 1; i < exp_size - 1; i++) {
	E = powf(2.0, (float) i - bias);
	printf("E=%d\n", i - bias);
	for (j = 0; j < fract_size; j++) {
	    double M = (1 + (double) j / fract_size);
	    printf("  M = %1f, val=%0.15lf\n", M, M * E);
	}
    }

}
