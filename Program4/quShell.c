/*******
 * Christian Duncan
 *
 * quShell
 *   This program is a simple shell script
 *   that is not partially functional.
 *
 *   It supports recognizing several built-in commands:
 *     SET [var] [value]: set the variable "var" to the given "value" argument.
 *               default value is ""
 *     LIST: prints a list of all current known variables and their values.
 *
 *   It ignores COMMENTS
 *     A COMMENT is started by the token # and continues to end of the line.
 *
 *   It also executes STATEMENTS
 *     A STATEMENT is a sequence of (zero or more) piped commands that ends with either
 *     a new line or a semicolon.
 *     The exit status of a statement is the exit status of the last
 *     command in the sequence.
 *
 *   Variable substitution:
 *      Variables are repeatedly substituted using the following sequence:
 *        $var$  - which are not done in single quotes '$var$'
 *      ...
 ********/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "global.h"
#include "tokenizer.h"
#include "varSet.h"
#include "command.h"
#include "builtins.h"

#define MAX_LINE_LENGTH 500
#define MAX_SUBSTITUTION_LEVEL 10

// The set of variables in this shell.
VarSet* varList = NULL;

// Very simple method to define the shell's prompt -- will allow for easier prompt changes
void shellPrompt() { printf(">> "); }

/***
 * processLine:
 *    line: string to process (REFERENCE is BORROWED)
 ***/
void processLine(char* line) {
  enum { CMD, PIPED_CMD, ARGS } processMode ;
  processMode = CMD;
  Command* cmd = NULL;
  int doneFlag = 0;

  startToken(line);
  aToken answer;

  answer = getNextToken();
  while (!doneFlag) {
    switch (answer.type) {
    case ERROR:
      // Error (for some reason)
      fprintf(stderr, "Error parsing line.\n");
      if (cmd != NULL) {
	freeCommand(cmd);
	cmd = NULL;
      }
      return;

    case BASIC:
    case DOUBLE_QUOTE:
    case SINGLE_QUOTE:
      if (processMode == CMD) {
	// This is a new command
	assert (cmd == NULL);
	cmd = newCommand(answer.start);
	processMode = ARGS;  // Switch modes
      } else if (processMode == PIPED_CMD) {
	// This is a new command after a pipe
	cmd = newCommand(answer.start);
	cmd->input = PIPE_IN;
	processMode = ARGS;        // Switch modes
      } else if (processMode == ARGS) {
	// This is a new argument
	assert(cmd != NULL);
	addArg(cmd, answer.start);
      }
      break;

    case PIPE:
      // We have a pipe, so command is now completed and ready to be executed

      if (processMode == CMD || processMode == PIPED_CMD) {
	// A pipe while waiting for a command!
	// Empty (blank) statements for pipes are not allowed
	fprintf(stderr, "Error: Missing command\n");
	assert (cmd == NULL);  // Otherwise some programming error occurred! (Mem leak maybe?)
	return;
      } else {
	assert(cmd != NULL);       // Otherwise some prog. error - entered ARGS mode w/o a Command!
	cmd->output = PIPE_OUT;    // Set its output stream to that of a PIPE
	processCommand(cmd);
	freeCommand(cmd);
	cmd = NULL;
	processMode = PIPED_CMD;  // Next command uses a piped command
      }
      break;

    case EOL:
      // EOL is nearly same as SEMICOLON - just flag done as well
      doneFlag = 1;

    case COMMENT:
      doneFlag = 1; // Comment - we don't need to pay any attention to it
      
    case SEMICOLON:
      // We have a statement terminator
      if (processMode == PIPED_CMD) {
	// We are in a piped command mode (without having gotten any new command)
	// An empty statement - not allowed after a pipe
	fprintf(stderr, "Error: Broken pipe\n");
	assert (cmd == NULL);
	return;
      } else if (processMode == CMD) {
	assert (cmd == NULL);
	// An empty statement - is allowed but ignored
      } else {
	assert (cmd != NULL);
	processCommand(cmd);
	freeCommand(cmd);
	cmd = NULL;
      }
      processMode = CMD;  // Switch back to processing mode
      break;

    default:
      fprintf(stderr, "Programming Error: Unrecognized type returned!!!\n");
      if (cmd != NULL ) {
	freeCommand(cmd);
	cmd = NULL;
      }
      return;
    }
    answer = getNextToken();
  }

  // Should only happen once doneFlag is set and SEMICOLON process is executed
  assert(cmd == NULL);
}

int main(int argc, char* argv[]) {
  varList = createVarSet();

  char line[MAX_LINE_LENGTH+1];

  shellPrompt();

  while (fgets(line, MAX_LINE_LENGTH+1, stdin) != NULL) {
    // We have our current line
    processLine(line);
    shellPrompt();
  }

  // Everything ran smoothly
  return 0;
}
