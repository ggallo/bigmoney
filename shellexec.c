//#! shellexec.c

#include "shellexec.h"

int doaline(pInput toDo, ilist* pid_store, int* style, int mode, char** path) {
  
  int run_status;
  int eof = 0;
  
  pCom curCommand = getNext(toDo);
  while (curCommand) {

    char** comArgs = curCommand->argv;

    //Need to change or display the style of the shell
    if (comArgs && !strcmp(comArgs[0], "style")) {
      if (mode == 1) 
        rewriteAndPrint(comArgs);
      if (comArgs[1] && !strcmp(comArgs[1], "s"))      *style = 0;
      else if (comArgs[1] && !strcmp(comArgs[1], "p")) *style = 1;
      else printf("Current style is %s\n", getStyleName(*style));
      
      curCommand = getNext(toDo);
    }

    //Any EOF flag should be handled after all other commands 
    //on a line are executed
    else if (comArgs && !strcmp(comArgs[0], "exit")) {
      eof = 1;
      curCommand = getNext(toDo);
    }

    else if (comArgs && isPipe(curCommand)) {
      startPipe(curCommand, path);
      //sleep(1);
      curCommand = getNext(toDo);
    }   

    //Once here, we aren't changing shell parameters
    //If sequential mode
    else if (*style == 0) {
      run_status = runSequential(comArgs, path, mode);
      if (run_status)
        return -1;
      curCommand = getNext(toDo);
    }
    
    //If parallel mode
    else if (*style == 1) {

      pid_t pid;

      //A new while loop needed since structure of fork/create
      //is different
      while (curCommand) {

        char** comArgs = curCommand->argv;
        
        //We reached a pipe command or a request to change/view style
        //We can no longer be in parallel mode. Break to the top
        if (comArgs && (!strcmp(comArgs[0], "style") || isPipe(curCommand))) {
          waitOnAll(pid_store, 1);
          break;

          //Same as before; make note of any EOF that might be seen
        } else if (comArgs && !strcmp(comArgs[0], "exit")) {
          eof = 1;
          curCommand = getNext(toDo);

        } else if (comArgs && !strcmp(comArgs[0], "jobs")) {
          printJobs(pid_store);
          curCommand = getNext(toDo);

        } else if (comArgs && !strcmp(comArgs[0], "pause")) {
          pid_t tokill = atoi(comArgs[1]);
          if(kill(tokill, SIGSTOP) < 0) {
            perror("pause failed");
          } else {
            int i;
            for (i=0; i < len(pid_store); i++) {
              Data* item = get(pid_store, i);
              if (item->pid == tokill) {
                item->status = 0;
              }
            }
          }
          curCommand = getNext(toDo);

        } else if (comArgs && !strcmp(comArgs[0], "resume")) {
          pid_t tokill = atoi(comArgs[1]);
          if(kill(tokill, SIGCONT) < 0) {
            perror("resume failed");
          } else {
            int i;
            for (i=0; i < len(pid_store); i++) {
              Data* item = get(pid_store, i);
              if (item->pid == tokill) {
                item->status = 1;
              }
            }            
          }
          curCommand = getNext(toDo);
          
        } else {
          if (mode == 1)
            rewriteAndPrint(comArgs);

          pid = fork();

          if (pid < 0) {
            perror("fork failed");
            return -1;

          } else if (pid == 0) {
            makeGoodCommand(comArgs, path);
            if (execv(comArgs[0], comArgs) < 0) {
              perror("process failed");
              printf("process name is %s\n", comArgs[0]);
              exit(-1);
            }

          //Parent needs to do housekeeping
          } else {
            //Store the PID to wait on it later
            append(pid_store, pid, comArgs, 1);
            curCommand = getNext(toDo);          
          }
        }
      } //End parallel loop
      
      //Update process list
      waitOnAll(pid_store, 0);
    } //End parallel conditional

    waitOnAll(pid_store, 0);
  } //End while

  waitOnAll(pid_store, 0);

  //Flag shell for termination
  if (eof == 1) {
    if(len(pid_store)) {
      eof = 0;
      return -1;
    }
    else {
      return 1;
    }
  }

  //Exited normally
  return 0;
}

void waitOnAll(ilist* pid_store, int hang) {

  int i = 0;
  //We must force the parent to wait for all children: parallel might end
  if (hang) {
    for (i=0; i < len(pid_store); i++)
      waitpid(get(pid_store, i)->pid, NULL, 0);
    cleanList(pid_store);
  }

  //We don't need to hang. Just check on the process, and if it finished, say so
  else {
    for (i=0; i < len(pid_store); i++) {
      pid_t pid = get(pid_store, i)->pid;
      int status = 0;
      int changed = waitpid(pid, &status, WNOHANG);
      if (changed || WIFEXITED(&status)) {
        printf("Process %d completed.\n", pid);
        removeItem(pid_store, i);
      }
    }
  }
}

char* getStyleName(int style) {
  if (style == 0)
    return "sequential";
  else if (style == 1)
    return "parallel";
  else
    return "WTF";
}

char** getPaths(FILE* stream) {
  char** path = (char**)malloc(MAX_ARGS * sizeof(char*));
  
  int count = 0;
  char* new_line = (char*)malloc(MAX_LINE * sizeof(char));
  while (fgets(new_line, MAX_LINE, stream)) {
    path[count] = (char*) malloc(strlen(new_line) * sizeof(char));
    strcpy(path[count], strtok(new_line, "\n"));
    count++;
  }

  path[count] = NULL;
  
  return path;
}

void makeGoodCommand(char** command, char** path) {

  if(path) {
    struct stat* command_stat = (struct stat*) malloc(sizeof(struct stat));
    int i = 0;
    while(path[i]) {
      char* attempt = (char*) malloc((strlen(command[0])+strlen(path[i])+1) \
                                     *sizeof(char));
      strcpy(attempt, path[i]);
      strcat(attempt, command[0]);
    
      //Check if this path/file combo is valid. If not, try again!
      if(!stat(attempt, command_stat)) {
        free(command[0]);
        command[0] = attempt;
        break;
      }
      
      i++;
      free(attempt);
    }
  }
}

int runSequential(char** command, char** path, int mode) {
  if (command && (!strcmp(command[0], "jobs") || !strcmp(command[0], "pause") || !strcmp(command[0], "resume"))) {
    fprintf(stderr, "command not valid in sequential mode\n");
    return 0;
  }
  
  pid_t pid;
  pid = fork();
  if (pid < 0) {
    perror("fork failed");
    return -1;
    
  } else if (pid == 0) {
    if (mode == 1) 
      rewriteAndPrint(command);

    makeGoodCommand(command, path);
    int fail = execv(command[0], command);
    if (fail == -1) {
      perror("process failed");
      printf("process name is %s\n", command[0]);
      exit(-1);
    }
  } else {
    wait(NULL);
  }
  return 0;
}

void free2DChar(char** aList) {
  char** iter = aList;
  int i = 0;
  while (iter[i])
    free(iter[i++]);
  free(aList);
  aList = NULL;
}

// Used for reassembling commands into a single string to echo in batch mode
void rewriteAndPrint(char** commands) {
  char** iter = commands;
  char* theCommand = (char*) malloc(MAX_LINE*sizeof(char));
  
  int i = 0;
  strcpy(theCommand, iter[i++]);
  while(iter[i]) {
    strcat(theCommand, " ");
    strcat(theCommand, iter[i++]);
  }
  
  printf("RUNNING: %s\n", theCommand);
  free(theCommand);
}

void printJobs(ilist* pid_store) { 
  printf("Current jobs: \n");
  char* s = "Command";
  printf("PID\t : %-50s : Status \n", s);
  int i = 0;
  for (i=0; i < len(pid_store); i++) {
    Data* item = get(pid_store, i);
    
    char* line = NULL;
    // add the status of the process
    if(item->status == 1) {
      line = strdup("active");
    } else {
      line = strdup("paused");
    }

    // output to terminal
    printf("%d\t : %-50s : %s\n", item->pid, item->command, line);
    free(line);
  }
}
