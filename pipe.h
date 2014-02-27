#ifndef PIPE_H
#define PIPE_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "input.h"
#include "shellexec.h"

static const int PIPE_READ = 0;
static const int PIPE_WRITE = 1;

int isPipe(pCom command);
pInput convert(pCom command, char** paths);
int startPipe(pCom command, char** paths);
int spawn(pInput pipedCommands);

#endif
