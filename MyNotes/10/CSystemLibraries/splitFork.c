#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// To add sleeping, change to 1
#define ENABLE_SLEEPING 1

void doChildWork(int start, int end);
void doParentProcess(int start, int end);

int main(int argc, char* argv[]) {
  int count = 10;  // DEFAULT Value
  if (argc >= 2) {
    // A count argument was passed
    count = atoi(argv[1]);
  }

  pid_t childId = fork();
  if (childId == 0) {
    // We are the child process
    printf("Child: We are the child process\n");
    doChildWork(1, count);
  } else {
    // We are the parent process
    printf("Parent: Child Process ID is %d\n", childId);
    doParentProcess(count+1, 2*count);
  }
  return 0;
}

void doChildWork(int start, int end) {
  int i;
  for (i = start; i <= end; i++) {
    printf("Child: %d\n", i);
#if (defined(ENABLE_SLEEPING) && (ENABLE_SLEEPING==1))
    sleep(1); // Sleep for one second
#endif
  }
}

void doParentProcess(int start, int end) {
  int i;
  for (i = start; i <= end; i++) {
    printf("Parent: %d\n", i);
#if (defined(ENABLE_SLEEPING) && (ENABLE_SLEEPING==1))
    if (i % 2 == 0) sleep(2);  // Sleep for two seconds (every other count)
#endif
  }
}
