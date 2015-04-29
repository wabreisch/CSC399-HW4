#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void foo();

int main() {
  foo();
  fork();
  printf("Bye\n");
}

void foo() {
  fork();
  printf("Hello\n");
}
