//#! shellmain.c

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "shellexec.h"
#include "input.h"
#include "globals.h"

int main(int argc, char** argv) {

  //interactive = 0, batch = 1
  ilist* pid_store = create();
  int mode = 0;
  FILE *filestream, *pathstream;
  filestream = pathstream = NULL;
  char** path = NULL;
  pInput commands = NULL;
  struct rusage *resources = (struct rusage*) malloc(sizeof(struct rusage));

  //Read in the desired PATH variable from .shell-config, if file exists
  pathstream = fopen(".shell-config","r");
  if (!pathstream)
    fprintf(stderr, "Shell could not locate '.shell-config'. Full path is now required to open files.");
  else
    path = getPaths(pathstream);
  
  //Too many args
  if (argc > 2) {
    fprintf(stderr, "error: shell starts with at most one argument (a batch file), %d given.\n", argc-1);
    return 1;

    //Time for batch mode!
  } else if (argc == 2) {
    mode = 1;
    filestream = fopen(argv[1], "r");
    if (!filestream) {
      fprintf(stderr, "error: batch file cannot be opened\n");
      return 1;     
    }
  
    //The user wants interactive mode.
  } else if (argc == 1) { 
    filestream = stdin;
    mode = 0;
  }

  commands = createInput(MAX_LINE);
  
  //styles: sequential = 0, parallel = 1
  int style = 0;
  int* pstyle = &style;
  int status = -1;
  time_t begin = time(NULL);

  while(42 > 0) {

    if (mode == 0) {
      fflush(stdin);
      printf("BIG$ "); 
    }
   
    update(filestream, commands); //Will wait here
    status = doaline(commands, pid_store, pstyle, mode, path);
    
    //Read an EOF; terminate the shell
    if (status == 1) {
      printf("\n");
      break;
    } else if (status == -1) {
      printf("error: background processes have not completed\n");
    }
  }

  deleteInput(commands);
  time_t end = time(NULL);
  
  getrusage(RUSAGE_SELF, resources);
  printf("Shell spent %ld.%lds in user mode.\n", resources->ru_utime.tv_sec, resources->ru_utime.tv_usec);
  printf("Shell spent %ld.%lds in kernel mode.\n", resources->ru_stime.tv_sec, resources->ru_stime.tv_usec);
  printf("Shell spent %lds total.\n", end - begin);
  freeList(pid_store);
  free(resources);
  if(path) free2DChar(path);
  return 0;
}
