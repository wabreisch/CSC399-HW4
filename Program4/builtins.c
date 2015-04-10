/*******
 * Christian Duncan
 *
 * Builtins
 *    A set of functions to process various built-in
 *    commands.
 *    See builtins.h for any details.
 *******/

#include "builtins.h"
#include "global.h"
#include "varSet.h"
#include "command.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

void processSet(Command* cmd);
void processList(Command* cmd);
void exitShell();
void cd(Command* cmd);
void status();

char *builtinNames[] = { "SET", "LIST", "EXIT", "CD", "STATUS", NULL };
void (*builtinFn[])(Command*) = { processSet, processList, exitShell, cd, status, NULL };

/***
 * processBuiltin:
 *    Determines if the given command is a builtin and executes
 *    it if so.
 *
 *    cmd: A BORROWED reference to the command to process
 *    Returns 
 ***/
int processBuiltin(Command* cmd) {
  assert(cmd->command != NULL);

  int i;
  for (i = 0; builtinNames[i] != NULL; i++) {
    // Does the given command match the builtin string name
    if (strcasecmp(cmd->command, builtinNames[i]) == 0) {
      // If so, execute the processing function for that command
      (builtinFn[i])(cmd);
      return 1;    // And return  1 (found builtin)
    }
  }
  
  return 0; // Did not find any builtin... execute normally
}

/***
 * processSet:
 *   Assign a variable a given value.
 *   Arg1: is the variable name
 *   Arg2: is the value.
 *   If Arg1 is empty - the command does nothing
 *   If Arg2 is empty - the command sets the variable to an empty string ""
 ***/
void processSet(Command* cmd) {
  assert(cmd != NULL);
  if (cmd->head == NULL) {
    // No argument... do nothing
    return;
  }
  
  addToSet(varList, cmd->head->arg, cmd->head->next == NULL ? "" : cmd->head->next->arg);
}

/***
 * processList:
 *    List the variables and their values in the current shell
 ***/
void processList(Command* cmd) {
  printSet(varList, stdout);
  fflush(stdout);
}

/***
* exitShell:
*   Exits the shell on the "EXIT" command
***/
void exitShell() {
  exit(0);
}

/***
* cd:
***/
void cd(Command* cmd) {
  if (cmd->head == NULL || cmd->tail == NULL) {
    chdir(getenv("HOME"));
  } else {
    chdir(cmd->head->arg);
  }

  char* cwd;
  char buff[100];
  cwd = getcwd(buff, 100);

  printf("%s\n", cwd);
}

/***
* status:
***/
void status() {

}