/***
 * Christian Duncan
 *
 * A very simple duplicate that 
 * duplicates the output stream to a new stream
 * and sends it to a tr program (forked) to make
 * all print statements SHOUTS!
 ***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/***
 * setupShouting:
 *    Establishes a new process to take input
 *    and shout via the tr command.
 *    Then redirect all output from main process
 *    to this process for translation.
 ***/
void setupShouting() {
  int comm[2];
  pipe(comm);  // Create a pipe - for communicating with tr program

  if (fork() == 0) {   // Child process
    dup2(comm[0], 0);  // Input stream now comes from pipe!
    close(comm[0]);    // Don't need the pipe - it is now 0.
    close(comm[1]);    // (This process) doesn't need output of pipe at all.
    
    execlp("tr", "tr", "[a-z]", "[A-Z]", NULL);
    fprintf(stderr, "Error: %s\n", strerror(errno));
    exit(1);
  }

  // Parent process
  dup2(comm[1], 1);  // Output (of parent process) now goes to the pipe!
  close(comm[1]);    // Don't need comm[1] - it is now 1
  close(comm[0]);    // This process doesn't need input of pipe.
}

int main() {
  setupShouting();

  // Now just do regular stuff...
  printf("Hi there.\n");
  printf("How are you?\n");
  printf("What?  I'm not shouting.  I am just whispering.\n");

  // Done
  //  Closing stdout here will close the input stream for tr 
  //    (and end that program)
  fclose(stdout);

  // You could also try the following two lines instead:
  //    fflush(stdout); // Flush the buffer so all information is sent
  //    close(1);       // Closing output stream ends the tr program...

  int status;
  wait(&status);  // Wait for that program to finish.

  return 0;
}
