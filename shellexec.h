#ifndef SHELLEXEC_H
#define SHELLEXEC_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include "globals.h"
#include "input.h"
#include "pipe.h"
//#include "arraylist.h"
#include "infolist.h"

/* Reads in a single line of code from either a batch file or the 
   interactive mode and executes it in the current style, if possible.
   Semicolons on each line indicate separate commands. Returns an integer
   indicating if an EOF was thrown during execution.*/
int doaline(pInput, ilist*, int*, int, char**);

/* Takes a list of PIDs and waits on all of them, therby collecting
   the child's leftover resources. Cleans the list afterwards.*/
void waitOnAll(ilist*, int);

//Return name of the shell's current style
char* getStyleName(int);

//Makes a char** holding the members of the path environment
char** getPaths(FILE*);

/*Tests all paths for the file requested by a command. If it is located,
  modify the command array to include the path. If the file is not found,
  nothing is modified.*/
void makeGoodCommand(char**, char**);

//Run a process in sequential mode. Returns a -1 error code if process
//cannot be forked
int runSequential(char**, char**, int);

void free2DChar(char**);

void rewriteAndPrint(char**);

void printJobs(ilist*);

#endif
