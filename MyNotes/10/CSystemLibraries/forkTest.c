#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  int x = 10;
  if (fork()) x++;
  if (fork()) x++;
  printf("x = %d\n", x);
}
