/*******
 * Christian Duncan
 *
 * Command
 *    A specific command with a list of arguments
 *******/

#ifndef __COMMAND_H
#define __COMMAND_H

#include <stdio.h>

typedef struct argList {
  char* arg;             // The argument string (REFERENCE is OWNED)
  struct argList* next;  // The next in the list (REFERENCE is OWNED)
} ArgList;

typedef struct {
  char* command;  // The command name itself (REFERENCE is OWNED)
  ArgList* head;  // The head of the argument list (REFERENCE is OWNED)
  ArgList* tail;  // The tail of the argument list (REFERENCE is BORROWED - part of head's list)
  enum { STDIN, PIPE_IN } input;  // Identifies whether command gets input from stdin or a pipe
  enum { STDOUT, PIPE_OUT } output;  // Identifies whether command sends output to stdout or a pipe
} Command;

Command* newCommand(const char* cmd);
void freeCommand(Command* cmd);
void printCommand(Command* cmd, FILE* stream);
void processCommand(Command* cmd);
void executeCommand(Command* cmd);
void addArg(Command* cmd, const char* arg);

#endif
