/*
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#define INT_MAX 0x7fffffff
#define EXP_MASK 0x7f800000
#define SIG_MASK 0x80000000

/*	For testing purpose only.  */

//  System call function. Should only be called by the wrapper
//  function add_floats()
/*
 * Pre-condition : float num1 >= 0, float num2 >= 0
 * Post-condition : num1 + num2 != nan
 *
 * Expects 2 positive float inputs num1, num2.
 *
 * Input Validation and Error Codes:
 * 		INT_MAX	 	==> sum of inputs invalid
 *		INT_MAX-1	==> num1 invalid
 *		INT_MAX-2	==> num2 invalid
 *		INT_MAX-3 	==> both invalid
 *
 * Adds the inputs and returns the IEEE-754 float
 * stored in a 32-bit int.
 *
 */
int syscall_add_floats(int num1, int num2) {
  // Handle bad inputs according to above specs
  int num1_invalid = num1 & SIG_MASK || ((num1 & EXP_MASK) == EXP_MASK);
  int num2_invalid = num2 & SIG_MASK || ((num2 & EXP_MASK) == EXP_MASK);
  if (num1_invalid && num2_invalid) {
    return INT_MAX - 3;
  } else if (num2_invalid) {
    return INT_MAX - 2;
  } else if (num1_invalid) {
    return INT_MAX - 1;
  }
  // Convert arguments to floats
  float *num1_f = (float *)&num1, *num2_f = (float *)&num2;
  // Calculate the sum;
  *num1_f += *num2_f;
  // Store it as int
  int sum = *(int *)num1_f;
  // If the sum is invalid return error code
  if (sum & SIG_MASK || ((sum & EXP_MASK) == EXP_MASK)) {
    return INT_MAX;
  } else {
    return sum;
  }
}

//  Wrapper function for the system call syscall_add_floats()
/*
 * Error Codes:
 * 		INT_MAX	 	==> sum of inputs negative
 *		INT_MAX-1	==> num1 negative
 *		INT_MAX-2	==> num2 negative
 *		INT_MAX-3 	==> both inputs negative
 */
float add_floats(float num1, float num2) {
  // Store the 32-bit IEE-754 float arguments into 32-bit int
  int *temp1 = (int *)&num1, *temp2 = (int *)&num2;
  // Call syscall_add_floats()
  int ret = syscall_add_floats(*temp1, *temp2);
  // Make the error codes readable / convert result to float
  // before returning
  switch (ret) {
    case INT_MAX - 1:
      return -1.0;
    case INT_MAX - 2:
      return -2.0;
    case INT_MAX - 3:
      return -3.0;
    case INT_MAX:
      return -4.0;
    // At this point the sum is guaranteed to be valid
    default: {
      float *sum = (float *)&(ret);
      return *sum;
    }
  }
}

int main() {
  // Testing add_floats() wrapper that calls custom syscall_add_floats()
  float *num1 = (float *)malloc(sizeof(float)),
        *num2 = (float *)malloc(sizeof(float)), num3;
  int temp1, temp2;

  // Case 0: Both parameters valid.
  *num1 = 54.35, *num2 = 34.6;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 1: Both parameters valid.
  *num1 = 681.46, *num2 = 9238.972;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 2: Both parameters valid.
  *num1 = 523.549, *num2 = 658.743;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 3: Both parameters valid.
  *num1 = 2378.235, *num2 = 65.874;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 4: num1 negative.
  *num1 = -237.878, *num2 = 6587.43;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 5: num1 and num2 negative.
  *num1 = -237.8781, *num2 = -6587.43;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 5: num1 nan.
  temp1 = 0x7ff00000;
  num1 = (float *)&temp1, *num2 = 658.456;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 6: num2 inf.
  temp2 = 0x7f800000;
  *num1 = 723.1549, num2 = (float *)&temp2;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 7: num1 inf, num2 nan.
  temp1 = 0x7f800000, temp2 = 0x7ff00000;
  num1 = (float *)&temp1, num2 = (float *)&temp2;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 8: num1 valid, num2 valid, sum invalid.
  temp1 = 0x7f400000, temp2 = 0x7f600000;
  num1 = (float *)&temp1, num2 = (float *)&temp2;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  // Case 9: num1 inf, num2 nan.
  temp1 = 0x7f800000, temp2 = 0x7ff00000;
  num1 = (float *)&temp1, num2 = (float *)&temp2;
  num3 = add_floats(*num1, *num2);
  printf("\n\n\t%f\n+\t%f\n=\t%f\n", *num1, *num2, num3);
  printf("\nUsing '+' : %f\n\n\n", *num1 + *num2);

  return 0;
}
