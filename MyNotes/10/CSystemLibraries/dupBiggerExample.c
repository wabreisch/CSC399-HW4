/***
 * Christian Duncan
 *
 * dupExample
 *    Runs the command ls | tr [a-z] [A-Z]
 *    It does this by creating a pipe for communicating between them.
 *    Then forking and execing the process while also using dup2 to 
 *    make one output stream the pipe output and one input the pipe input.
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int main() {
  // Executing 'ls' piping the output to 'tr [a-z] [A-Z]' (making letters big)

  // Create a pipe
  int comm[2];

  if (pipe(comm) == -1) {
    fprintf(stderr, "Error occurred opening pipe: %s\n", strerror(errno));
    exit(1);
  }

  // Step One: Executing '/bin/ls' and redirecting output to the created pipe
  if (fork() == 0) {         // Child process
    dup2(comm[1], 1);        // Make Outstream of this process be pipe out.
    close(comm[1]);          // Comm[1] is now 1
    close(comm[0]);          // Important: close streams you don't need
    execl("/bin/ls", "ls", "-FC", NULL); 
    fprintf(stderr,          // This should only be reached if exec failed
	    "Error: %s\n", strerror(errno));
    exit(1);
  }

  // Step Two: Executing 'tr [a-z] [A-Z]' (from path) and redirecting input from pipe.
  if (fork() == 0) {         // Child process
    dup2(comm[0], 0);        // Make Inputstream (of this proc.) be pipe in.
    close(comm[0]);          // Comm[0] is now 0.
    close(comm[1]);          // Don't need comm[1] (close - or prog hangs)
    execlp("tr", "tr", "[a-z]", "[A-Z]", NULL);
    fprintf(stderr,          // This should only be reached if exec failed
	    "Error: %s\n", strerror(errno));
    exit(1);
  }

  // Parent (main) process
  close(comm[0]); // Parent is done with both ends of the pipe
  close(comm[1]); // ... so close it on this end.
  
  int status;
  wait(&status);  // Wait for one child to finish (don't care which)
  wait(&status);  // Wait for other child to finish (don't care which)
  exit(0);
}
