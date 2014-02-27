#include "input.h"

pInput createInput(int maxLen)
{
  pInput newInput = (pInput) malloc(sizeof(Input));

  newInput->count = 0;
  newInput->maxLen = maxLen;
  newInput->head = NULL;
  newInput->tail = NULL;
  newInput->curr = NULL;

  return newInput;
}

pCom createCommand(int argc, char** argv)
{
  pCom newCom = (pCom) malloc(sizeof(Com));

  newCom->argv = argv;  
  newCom->argc = argc;
  newCom->next = NULL;

  return newCom;
}

void reset(pInput input)
{
  input->count = 0;
  
  pCom iter = input->head;
  pCom del;
  while(iter) {
      del = iter;
      iter = iter->next;
      deleteCommand(del); }
  
  input->head = NULL;
  input->tail = NULL;
  input->curr = NULL;
}

void deleteInput(pInput input)
{
  reset(input);
  free(input);
}

void deleteCommand(pCom command)
{
  int i;
  for(i = 0; i < command->argc; i++)
    free(command->argv[i]);    
  free(command->argv);
  free(command);
}

void add(pInput input, pCom command)
{  
  if(input->count == 0) {
    input->head = command;
    input->tail = command;
    input->curr = command; }
  else {
    input->tail->next = command;
    input->tail = command; }
  input->count++;
}

pCom getNext(pInput input)
{
  if(!input->curr)
    return NULL;
  else {
    pCom value = input->curr;
    input->curr = input->curr->next;
    return value; }
}

void update(FILE* stream, pInput input)
{
  reset(input);
  char* newLine = (char*) malloc(input->maxLen*sizeof(char));
  
  if(fgets(newLine, input->maxLen, stream) != NULL) {
    char* temp = preformat(newLine);
    free(newLine);
    newLine = temp;   

    char* text;
    int count = 0;
    char** coms = (char**) malloc(MAX_ARGS*sizeof(char*));    
    text = strtok(newLine, ";\n");
    while(text != NULL) {
      coms[count] = (char*) malloc((strlen(text) + 1)*sizeof(char));
      strcpy(coms[count], text);
      count++;              
      text = strtok(NULL, ";\n"); 
    }
    coms = (char**) realloc(coms, count);
            
    int argc, i = 0;
    for(i = 0; i < count; i++) {
      char** argv = (char**) malloc(MAX_LINE*sizeof(char*));
      cleanup(coms[i], &argc, argv);
      if(argv[0]) {
        pCom command = createCommand(argc, argv);
        add(input, command); 
      } 
    } 
  }
  
  if(feof(stream) || ferror(stream)) {
    clearerr(stream);
    char** argv = (char**) malloc(2*sizeof(char*));
    argv[0] = (char*) malloc(5*sizeof(char));
    strcpy(argv[0], "exit\0");
    argv[1] = NULL;
    pCom command = createCommand(2, argv);
    add(input, command); 
  }
  
  free(newLine);
}
      
void cleanup(char* text, int* argc, char** argv)
{   
  int i = 0;
  char* argument;
  
  argument = strtok(text, " ");
  while(argument != NULL) {
      int len = strlen(argument) + 1;
      argv[i] = (char*) malloc(len*sizeof(char));
      strcpy(argv[i], argument);
      argument = strtok(NULL, " ");
      i++; 
  }
  argv[i] = NULL;
  i++;
  
  argv = (char**) realloc(argv, i*sizeof(char*));  
  *argc = i;
}

char* preformat(char* text)
{  
  text = strtok(text, "#");
  char* newText = (char*) malloc(sizeof(char));
  newText[0] = '\0';
  
  char* part = strtok(text, "|");
  while(part != NULL) {
    int len = strlen(newText) + strlen(part) + 4;
    newText = (char*) realloc(newText, len*sizeof(char));
    strcat(newText, part);
    strcat(newText, " | ");
    part = strtok(NULL, "|"); 
  }
    
  newText[strlen(newText) - 3] = '\0';
  int len = strlen(newText) + 1;  
  newText = (char*) realloc(newText, len*sizeof(char));
  
  return newText;
}
