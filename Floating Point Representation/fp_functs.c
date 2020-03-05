#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "fp.h"

int computeFP(float val) {
// input: float value to be represented
// output: integer version in our representation

  int bias;
  int s = 0;		// assume sign bit is always positive
  int exp;
  int E;
  float frac;
  double m;
  float value;
  int e = 4;		//# bits in exp field,(always 4 in this representation)

  // calculates bias to be 7
  bias = pow(2, e-1) - 1;
 
  // Checks for the smallest possible denormalized value 
  // 0 0000 00000001 -> using the formula 2^(1-bias) * 2^-8
  float smallestValue = pow(2, 1- bias) * pow(2, -8);
  if(val < smallestValue)
  {
  	//printf("Too small\n");
	return 0;
  }

  // Divides or multiplies the value by 2 until it is in the correct range 
  // (between 1 and 2). The exponent E is the number of times this operation occured
  value = val;
  while(value < 1)
  {
  	value = value * 2;
	E--;
  }

  while(value > 2)
  {
  	value = value / 2;
	E++;
  }
  
  // calculates exp using the given formula
  m = value;
  exp = E + bias;
  
  // Check for Denormalized. A value is Denormalized if exp is <= 0.
  if(exp <= 0) 		
  {
   	 // printf("Denormalized\n");
	 E = 1 - bias;
	 exp = 0;
	 frac = val / pow(2,E);
	 m = frac;
  }
  
  // calculates frac by subtracting the int part of m
  frac = m - (int)m;
  
  // Checks for overflow. Overflow if exp is all 1's and frac is all
  // 0's. (infinity -> 0 1111 00000000) or if exp is > 14
  // returns (0 1111 00000000) or 3840
  if(exp > 14) 
  {
	//printf("Infinity\n");
	return 3840;
  }

  //8 bits is used for frac -> 2^8 = 256
  float newFrac = frac * 256.0;
 
  // for testing purposes
  /*value = m * pow(2,E);
  printf("M: %lf\n", m);
  printf("exp: %d\n", exp);
  printf("e: %d\n", E);
  printf("frac: %lf or %lf\\%d\n", frac, newFrac, 256);
  printf("value: %lf\n", value); */

  // converts our representation to decimal value
  // 4 btis for exp and 8 bits for frac
  newFrac = newFrac / 16;
  int output = (pow(16, 2) * exp) + (pow(16, 1) * newFrac);

  //printf("Output: %d\n", output); 
  return output;
}

float getFP(int val) {
// input: integer version of value to be represented
// output: float value of representation

 int bias = 7;  	        // computed earlier 
 float value = val / 256.0;    	// 2^8 = 256, converts from out representation
 int exp = (int)value;
 float frac;
 float m;
 int e;

 // Denormalized 
 if(exp <= 0)
 {
	e = 1 - bias;
	frac = value;
	m = frac;
 }
 else	// normalized
 {
 	frac = value - (int)value;
 	m = 1 + frac;
 	e = exp - bias;
 }

 // returns zero if exp and frac are all 0's
 if(exp == 0 && frac == 0.0)
 {
	return 0;      
 }

 // returns -1 (infinity) if exp is all 1's and frac all 0's
 // 0 1111 00000000 -> 15
 if(exp >= 15 && frac == 0.0)
 {
	return -1;
 }

 // computes the floating point value
 float output =  m * pow(2,e);
 return output;
}

int multVals(int source1, int source2) {
// Takes two ints in our representation and multiplies them
// input: two ints in our representation
// output: int product of the two inputs in our representation

 // computes value, exp, frac, e for both inputs
 int bias = 7;
 float valueOne = source1 / 256.0;
 float valueTwo = source2 / 256.0;
 int exp1 = (int)valueOne;
 int exp2 = (int)valueTwo;
 float fracOne;
 float fracTwo;
 float m1;
 float m2;
 int e1;
 int e2;

 // denormalized
 if(exp1 <= 0)
 {
 	fracOne = valueOne;
 	m1 = fracOne;
	e1 = 1 - bias;
 }
 else	// normalized
 {
	fracOne = valueOne - (int)valueOne; 
	m1 = 1 + fracOne;
	e1 = exp1 - bias;
 }

 if(exp2 <= 0)
 {
	fracTwo = valueTwo;
	m2 = fracTwo;
	e2 = 1- bias;
 }
 else
 {
 	fracTwo = valueTwo - (int)valueTwo;
 	m2 = 1 + fracTwo;
 	e2 = exp2 - bias;
 }
 // multiplies the fractional values 
 float m = m1 * m2;

 // adds the exponents
 int e = e1 + e2;

 // if m is too large divide by 2 and increment e
 if(m >= 2)
 {
	m = m / 2.0;
	e++;
 }

 // computes the float value
 float output = m * pow(2,e);

 // checks for overflow and if value is too small and returns 
 // integer version of out representation
 return computeFP(output);
}

int addVals(int source1, int source2) {
// takes two ints in our representation and adds them
// input: two ints in our representation
// output: int product in our representation

// If the sum results in overflow, return Infinity
// If the sum is 0, return 0

  // computes value, exp, frac, m, e for both inputs
  int bias = 7;
  float valueOne = source1 / 256.0;
  float valueTwo = source2 / 256.0;
  int exp1 = (int)valueOne;
  int exp2 = (int)valueTwo;
  float fracOne, fracTwo;
  float m1, m2;
  int e1, e2;

  // denormalized
  if(exp1 <= 0)
  {
	fracOne = valueOne;
	m1 = fracOne;
	e1 = 1 - bias;
  }
  else	// normalized
  {
	fracOne = valueOne - (int)valueOne;
	m1 = 1 + fracOne;
	e1 = exp1 - bias;
  }

  if(exp2 <= 0)
  {
	fracTwo = valueTwo;
	m2 = fracTwo;
	e2 = 1- bias;
  }
  else
  {
  	fracTwo = valueTwo - (int)valueTwo;
  	m2 = 1 + fracTwo;
  	e2 = exp2 - bias;
  }

  // adjusts one of the numbers so both have same exponent
  // if the exponent is smaller, divide value by 2 and increment exponent
  // if the exponent is larger, multiply value by 2 and decrement exponent
  while(e2 != e1) 
  { 
	if(e2 < e1)
	{	
		m2 = m2 / 2.0;
		e2++;
	}
	if(e2 > e1)
	{
		m2 = m2 * 2.0;
		e2--;
	}
  }

  // add both values and fractional parts
  float m = m1 + m2;

  // adjusts m so that it is in the correct range
  if(m >= 2.0)
  {
  	m = m / 2.0;
	e2++;
  }

  // computes the float value
  float output = m * pow(2,e2);
  
  // checks for overflow and if value is too small returns
  // integer version of our representation
  return computeFP(output);
}



