#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include "add_syscall.h"

#define INT_MAX 0x7fffffff
#define EXP_MASK 0x7f800000
#define SIG_MASK 0x80000000
#define MAN_MASK 0x007fffff


//  System call function. Should only be called by the wrapper
//  function add_floats()
/*
 * Pre-condition : (int)num1 & (int)num2 -- IEEE-754 representation of
 *                 positive, finite floats.
 *                 (float)num1 + (float)num2 is a positive finite float
 *
 * Post-condition : (int)(num1 + num2) -- IEEE-754 representation of
 *                  (float)num1 + (float)num2
 *
 * Expects 2 positive int inputs num1, num2.
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
SYSCALL_DEFINE2(add_syscall, int, num1, int, num2) {
  // Returns true if number is negative / inf / nan
  inline int invalid_float(int num) {
    return num & SIG_MASK || ((num & EXP_MASK) == EXP_MASK);
  }
  // Handle bad inputs according to above specs
  int num1_inval = invalid_float(num1), num2_inval = invalid_float(num2);
  if (num1_inval && num2_inval) {
    return INT_MAX - 3;
  } else if (num2_inval) {
    return INT_MAX - 2;
  } else if (num1_inval) {
    return INT_MAX - 1;
  }

  int num1_exp = num1 & EXP_MASK, num2_exp = num2 & EXP_MASK;
  // If num1_exp < num2_exp, swap them
  if (num1_exp < num2_exp) {
    num1_exp += num2_exp;
    num2_exp = num1_exp - num2_exp;
    num1_exp -= num2_exp;
    num1 += num2;
    num2 = num1 - num2;
    num1 -= num2;
  }
  int num1_man = num1 & MAN_MASK, num2_man = num2 & MAN_MASK;
  // Add a 1-bit to left of 23-bit mantissa
  num1 = num1_man | 0x00800000, num2 = num2_man | 0x00800000;
  // Align the numbers according to their decimal points
  num2 = num2 >> ((num1_exp - num2_exp) >> 23);
  // Add num2 to num1;
  num1 += num2;
  // If there is overflow to 25th bit, shift right
  if (num1 & 0x01000000) {
    num1_exp += 1 << 23;
    num1 = num1 >> 1;
  }
  // Remove 24th bit to retain only mantissa
  num1 &= 0xff7fffff;
  // Add exponent to num1
  num1 |= num1_exp;
  if (invalid_float(num1)) {
    return INT_MAX;
  } else {
    return num1;
  }
}