#include "pipe.h"

int isPipe(pCom command)
{
  int i;
  for(i = 0; i < command->argc - 1; i++) {
    if(!strcmp(command->argv[i], "|")) return 1; 
  }
  return 0;
}

pInput convert(pCom command, char** paths)
{
  pInput newInput = createInput(50);
  int i = 0;
  char* iter = command->argv[i];  
  while(iter) {
    char** argv = (char**) malloc(command->argc*sizeof(char*));
    int j = 0;
    while(iter && iter[0] != '|') {
      int len = strlen(iter) + 1;
      argv[j] = (char*) malloc(len*sizeof(char));
      strcpy(argv[j], iter);
      i++;
      j++;
      iter = command->argv[i]; 
    }
    argv[j] = NULL;
    j++;
    argv = (char**) realloc(argv, j*sizeof(char*));
    makeGoodCommand(argv, paths);
    pCom newCom = createCommand(j, argv);
    add(newInput, newCom);
    
    if(iter && iter[0] == '|') {
      i++; 
      iter = command->argv[i]; 
    } 
  }
  return newInput;
}    

int startPipe(pCom command, char** paths)
{
  pInput input = convert(command, paths);
  pid_t pid = fork();  
  if (pid < 0) {
    perror("fork failed"); 
    return -1; 
  }
  else if (pid == 0) {
    return spawn(input); 
  }
  else {
    wait(NULL);
    return 0; 
  }
}   

int spawn(pInput pipedCommands)
{
  pCom curr = getNext(pipedCommands);
  pCom next = curr->next;
  int* link = curr->link;
  char** argv = curr->argv;

  // create pipe to child
  if (pipe(link) < 0) {
    perror("pipe failed");
    return -1; 
  } 
    
  // fork a new process
  pid_t pid = fork();  
  if (pid < 0) {
    perror("fork failed"); 
    return -1; 
  }
  else if (pid == 0) {
    dup2(link[PIPE_READ], STDIN_FILENO);
    close(link[PIPE_WRITE]);
    if(next) {         
      spawn(pipedCommands); 
    }
    else {    
      if(execv(argv[0], argv) < 0) {
        perror("process failed");
	printf("process name is %s\n", argv[0]);
        exit(-1); 
      } 
    } 
  } 
  else {  
    dup2(link[PIPE_WRITE], STDOUT_FILENO);
    close(link[PIPE_READ]);    
    if(execv(argv[0], argv) < 0) {
      perror("process failed"); 
      printf("process name is %s\n", argv[0]);
      exit(-1); 
    } 
  }

  wait(NULL);
  
  return 0;
}
