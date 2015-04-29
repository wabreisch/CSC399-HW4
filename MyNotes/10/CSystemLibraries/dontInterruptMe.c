#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

const int MAXSTRING=100;

/***
 * Return the nth Fibonacci number
 *  Very very very inefficient 
 *  but done to make program really slow
 ***/
int fib(int n) {
  if (n <= 1) return 1;
  else return fib(n-1) + fib(n-2);
}

void requestStop(int sig) {
  static int counter = 0;
  char answer[MAXSTRING];
  printf("\n\nInterrupt received!  Signal = %d.  Do you wish to (c)ontinue or (q)uit?\n", sig);
  fgets(answer, MAXSTRING-1, stdin);
  while (*answer != 'c' && *answer != 'q') {
    printf("Do you wish to (c)ontinue or (q)uit?\n");
    fgets(answer, MAXSTRING-1, stdin);
  };

  if (*answer == 'q') {
    if (++counter < 5) {
      printf("Sorry I don't believe you!\n");
    } else {
      printf("Ok, fine.  I guess you mean it!\n");
      exit(0);
    }
  } else {
    // Wanted to continue, we'll reset counter :-)
    counter = 0;
  }
}

int main(int argc, char **argv) {
  int n = 10; // Default value
  if (argc >= 2) {
    n = atoi(argv[1]);
  }

  // Register the signal handler
  struct sigaction act;
  act.sa_handler = requestStop;
  sigaction(SIGINT, &act, NULL);

  int v = fib(n);
  printf("Fib of %d is %d\n", n, v);

  return 0;
}
