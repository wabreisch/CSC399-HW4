/*******
 * Christian Duncan
 *
 * Command
 *    See command.h for details. :-)
 *******/

#include "command.h"
#include "global.h"
#include "builtins.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/*** 
 * newCommand:
 *   Create a new command using given string
 *   Creates a null list of arguments
 *   REFERENCE returned is GIVEN
 ***/
Command* newCommand(const char* cmd) {
  Command* ans = malloc(sizeof(Command));
  ans->command = strdup(cmd);
  ans->head = NULL;
  ans->tail = NULL;
  ans->input = STDIN;    // By default
  ans->output = STDOUT;  // By default
  return ans;
}

/***
 * freeCommand:
 *   Frees up the given command - and its argument list
 *   REFERENCE given is STOLEN (and freed)
 ***/
void freeCommand(Command* cmd) {
  free(cmd->command);
  
  ArgList* head = cmd->head;
  while (head != NULL) {
    ArgList* next = head->next;  // Just in case ref. is lost
    free(head->arg);
    free(head);
    head = next;
  }
  
  free(cmd);
}

/***
 * printCommand:
 *    Print out the details of the given command
 *    REFERENCEs are BORROWED
 ***/
void printCommand(Command* cmd, FILE* stream) {
  if (cmd == NULL) {
    // Empty command, nothing to execute or print
    return;
  }
  
  fprintf(stream, "Executing Command: %s\n", cmd->command);
  fprintf(stream, "...Input: %s\n", (cmd->input == STDIN ? "STDIN" : "PIPE"));
  fprintf(stream, "...Output: %s\n", (cmd->output == STDOUT ? "STDOUT" : "PIPE"));

  if (cmd->head != NULL) {
    // Print out the argument list
    int a;
    ArgList* curr = cmd->head;
    for (a = 1; curr != NULL; a++, curr=curr->next) {
      fprintf(stream, "...Arg %d: %s\n", a, curr->arg);
    }
  } else {
    fprintf(stream, "...No arguments\n");
  }
}

/***
 * processCommand:
 *    Process the command.
 *    Execute the commands
 *       Some are via exec
 *       Otherwise process certain builtin commands.
 *    REFERENCEs are BORROWED
 ***/
void processCommand(Command* cmd) {
  assert(cmd != NULL);

  if (!processBuiltin(cmd)) {
    // It was not a built-in so execute normally (ok, for now we just print it)
    printCommand(cmd, stdout);
  }
}

/***
 * addArg:
 *    Add a new argument to the command
 *    REFERENCEs are BORROWED
 ***/
void addArg(Command* cmd, const char* arg) {
  // Allocate memory (be sure to check for Out-of-mem)
  ArgList* newArg = malloc(sizeof(ArgList));

  if (newArg == NULL) {
    fprintf(stderr, ">> Error: Out of memory.  Token not added.\n");
    return;
  }
    
  // Store the contents (the new argument)
  newArg->arg = strdup(arg);
  newArg->next = NULL;

  // Insert into the arglist - at the tail (if not empty)
  if (cmd->head == NULL) {
    // First argument
    cmd->head = cmd->tail = newArg;
  } else {
    cmd->tail = cmd->tail->next = newArg;
  }
}
