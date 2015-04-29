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
    doChildWork(pipeB[0], pipeA[1]);
  } else {
    // We are the parent process
    printf("Parent: Child Process ID is %d\n", childId);
    doParentProcess(pipeA[0], pipeB[1]);
  }
  return 0;
}

void doChildWork(int inFd, int outFd) {
  // Let us have the child read a "message" from the parent (read in an int)
  int buf;

  read(inFd, &buf, sizeof(int));
  printf("Child: Read value from parent pipe: %d\n", buf);
  int val=2*buf;

  printf("Child: Writing value to parent pipe: 2*buf=%d\n", val);
  write(outFd, &val, sizeof(int));
  printf("Child: I am done.\n");
}

void doParentProcess(int inFd, int outFd) {
  int buf;
  buf = 42;

  printf("Parent: Writing value to child pipe: %d\n", buf);
  write(outFd, &buf, sizeof(int));

  // Get response
  int response = 0;
  read(inFd, &response, sizeof(int));
  printf("Parent: Read value from parent pipe: %d\n", response);

  wait(NULL);
  printf("Parent: I am done.\n");
}
