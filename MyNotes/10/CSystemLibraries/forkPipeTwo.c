#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

// This code shows how forks can communicate by using pipes

void doChildWork(int inFd, int outFd);
void doParentProcess(int inFd, int outFd);

int main() {
  // First let us create a few pipes (we need four of them)
  int pipeA[2];
  int pipeB[2];
  
  if (pipe(pipeA) == -1) {
    // Error
    printf("Error creating pipe: %s\n", strerror(errno));
    exit(1);
  }

  if (pipe(pipeB) == -1) {
    // Error
    printf("Error creating pipe: %s\n", strerror(errno));
    exit(1);
  }

  // We are going to use the following mapping
  //    pipeA[0] = input stream for parent
  //    pipeB[0] = input stream for child
  //    pipeA[1] = output stream for child (going to input stream of parent)
  //    pipeB[1] = output stream for parent (going to input stream of child)
  printf("Pre-forking fd's pipeA=[%d,%d], pipeB=[%d,%d]\n", 
	 pipeA[0], pipeA[1], pipeB[0], pipeB[1]);

  pid_t childId = fork();
  if (childId == 0) {
    // We are the child process
    printf("Child: We are the child process\n");
    close(pipeB[1]);  // Cleaner to close unused file descriptors
    close(pipeA[0]);  // Also prevents accidentally using them!
    doChildWork(pipeB[0], pipeA[1]);
  } else {
    // We are the parent process
    printf("Parent: Child Process ID is %d\n", childId);
    close(pipeA[1]);  // Cleaner to close unused file descriptors
    close(pipeB[0]);
    doParentProcess(pipeA[0], pipeB[1]);
  }
  return 0;
}

void doChildWork(int inFd, int outFd) {
  FILE* in = fdopen(inFd, "r");
  FILE* out = fdopen(outFd, "w");

  // Let us have the child read a "message" from the parent (read in an int)
  int buf;

  fscanf(in, "%d", &buf);
  printf("Child: Read value from parent pipe: %d\n", buf);
  int val=2*buf;

  printf("Child: Writing value to parent pipe: 2*buf=%d\n", val);
  fprintf(out, "%d\n", val);
  printf("Child: I am done.\n");
}

void doParentProcess(int inFd, int outFd) {
  FILE* in = fdopen(inFd, "r");
  FILE* out = fdopen(outFd, "w");

  int buf;
  buf = 42;

  printf("Parent: Writing value to child pipe: %d\n", buf);
  fprintf(out, "%d\n", buf);
  fflush(out);

  // Get response
  int response = 0;
  fscanf(in, "%d", &buf);

  printf("Parent: Read value from parent pipe: %d\n", response);

  wait(NULL);
  printf("Parent: I am done.\n");
}
