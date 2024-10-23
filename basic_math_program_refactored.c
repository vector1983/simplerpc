#include <stdio.h>

#include "is_prime.h"

int main(void) {
  // Prompt the user to enter a number.
  printf("Please enter a number: ");
  int input_number;
  scanf("%d", &input_number);

  // check if it's prime
  if (is_prime(input_number)) {
    printf("%d is prime\n", input_number);
  } else {
    printf("%d is not prime\n", input_number);
  }

  return 0;
}