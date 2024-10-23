#include "is_prime.h"

bool is_prime(int number) {

  // check first for 2 or 3
  if (number == 2 || number == 3) {
    return true;
  }

  // check for 1 or easy modulos
  if (number == 1 || number % 2 == 0 || number % 3 == 0) {
    return false;
  } 

  // Now check all the numbers up to sqrt(number)
  int i = 5;
  while (i * i <= number) {
    // if we've found something that divides it evenly, it's not prime.
    if (number % i == 0 || number % (i+2) == 0) {
      return false;
    }
    i += 6;
  }
  return true;
}
