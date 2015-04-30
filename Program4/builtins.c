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
void pwd();

char *builtinNames[] = { "SET", "LIST", "EXIT", "CD", "STATUS", "PWD", NULL };
void (*builtinFn[])(Command*) = { processSet, processList, exitShell, cd, status, pwd, NULL };
int currStatus = 0;

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
  if (*(cmd->tail->arg) == '$') {
    char* tailCopy = malloc(sizeof(cmd->tail->arg)+1); // copy cmd->tail->arg into tailCopy because it's less awkward to deal with
    char tempStr[100];
    int i = 0;
    strcpy(tailCopy, cmd->tail->arg);
    tailCopy++; // Skip over the first $
    while (*tailCopy != '$') { // parse the name of the variable we're looking for WITHOUT $
      tempStr[i] = *tailCopy;
      tailCopy++;
      i++;
    }
    tempStr[i] = '\0'; // fixes a strange error in which garbage characters were being included with tempStr
    if (findInSet(varList, tempStr) == NULL) {
      printf("Unknown variable: %s\n", tempStr);
      return;
    }
    char* tempValue = findInSet(varList, tempStr)->value;
    addToSet(varList, cmd->head->arg, tempValue);
  } else {
    addToSet(varList, cmd->head->arg, cmd->head->next == NULL ? "" : cmd->head->next->arg);
  }
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
* cd: Changes the working directory to the directory specified by the user
* as the first argument to the cd command
* If no argument is given, the working directory is changed to $HOME
***/
void cd(Command* cmd) {
  if (cmd->head == NULL) {
    if (getenv("HOME") != NULL) {
      chdir(getenv("HOME"));
    } else {
      chdir("/");
    }
  } else {
    chdir(cmd->head->arg); // Attempt to change directory...
    if (errno != 0) {
      fprintf(stderr, "Error: directory %s does not exist\n", cmd->head->arg); // print to stderr
      errno = 0; // reset errno so we don't incorrectly report an error if the next directory is valid
    }
  }
}

/***
* status: turns on (1) or off (0) the report of exit status of any statement
***/
void status() {
  if (currStatus == 0) { currStatus = 1; }
  else if (currStatus == 1) { currStatus = 0; }
  printf("status: %d\n", currStatus);
}

/***
* pwd is not required for the assignment, but I thought it would be a useful function to have
***/
void pwd() {
  char* cwd;
  char buff[100];
  cwd = getcwd(buff, 100);
  printf("%s\n", cwd);
}
