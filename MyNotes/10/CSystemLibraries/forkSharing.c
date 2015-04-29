#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// This code shows how forks can and cannot share memory.

#define WAIT_FOR_CHILD 3

void doChildWork();
void doParentProcess();
int x;  // Global variable
int *y; // Reference to some variable

int main() {
  // First let us initialize and create the variables
  x = 100;
  y = (int*) malloc(sizeof(int));
  *y = 200;

  printf("Pre-forking: x=%d, y=%p, *y=%d\n", x, y, *y);

  pid_t childId = fork();
  if (childId == 0) {
    // We are the child process
    printf("Child: We are the child process\n");
    doChildWork();
  } else {
    // We are the parent process
    printf("Parent: Child Process ID is %d\n", childId);
    doParentProcess();
  }
  return 0;
}

void doChildWork() {
  // Sleep for a few seconds to let the parent do its job first
  sleep(10);  // Watch when it gets printed!

  // Now print out the values of x and y
  printf("Child: x=%d, y=%p, *y=%d\n", x, y, *y);
  printf("Child: I am done.\n");
  exit(10);  // We will exit with status 10 (for no real reason!)
}

void doParentProcess() {
  // Let us change the values of x and *y
  x = 500;  // Can we change global variables - in the child process?
  *y = 500; // How about if they point to the "same" memory location?
  printf("Parent: x=%d, y=%p, *y=%d\n", x, y, *y);

#if (defined(WAIT_FOR_CHILD) && (WAIT_FOR_CHILD==1))
  wait(NULL); // Wait for the child process to finish
#elif (defined(WAIT_FOR_CHILD) && (WAIT_FOR_CHILD==2))
  // Wait and capture the exit status
  int status = 0;
  wait(&status);   // Stores several status values in this int (lower 8-bits stores exit status)
  printf("Parent: Child exited with status %d\n", WEXITSTATUS(status));
#elif (defined(WAIT_FOR_CHILD) && (WAIT_FOR_CHILD==3))
  // Fall asleep first... for a very long time
  sleep(1000);
  wait(NULL);
#else
  // No waiting at all!
#endif
  printf("Parent: I am done.\n");
}
