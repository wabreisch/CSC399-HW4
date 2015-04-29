#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int main() {
  printf("Here is the last time the file %s was modified:\n",
	 __FILE__);
  // % date -r FILE +%c
  execlp("./modDate",            // Command to execute
	"date",                 // arg0 (file name - just a convention)
	"-r", __FILE__, "+%c",  // arg1,arg2,arg3
	NULL);                  // NULL terminator

  // The rest of this code should be reached ONLY if there is an error!
  //   Try changing the executable to an invalid name like "dater"
  int localErrno = errno;       // Save the errno locally (else it could change!)

  printf("This line will only be executed if there is an error:\n");
  printf("The error was %d\n", localErrno);
  printf("  Message: %s\n", strerror(localErrno));

  return localErrno;  // Why not return on the error!
}
