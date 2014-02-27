#include <stdio.h>
#include <stdlib.h>
#include "infolist.h"
#include "shellexec.h"

ilist* create(void) {
  ilist* newlist = (ilist*)malloc(sizeof(ilist));
  newlist->dataArray = (Data**) malloc(sizeof(Data*));
  newlist->numitems = 0;
  newlist->size = 1;
  return newlist;
}

Data* createData(int pid, char** command, int status) {
  Data* newData = (Data*) malloc(sizeof(Data));
  newData->pid = pid;


  char* theCommand = (char*) malloc(MAX_LINE*sizeof(char));  
  int i = 0;
  char** iter = command;
  strcpy(theCommand, iter[i++]);
  while(iter[i]) {
    strcat(theCommand, " ");
    strcat(theCommand, iter[i++]);
  }

  newData->command = theCommand;
  newData->status = status;
  return newData;
}

int append(ilist* alist, int pid, char** command, int status) {

  if (alist->numitems == alist->size) {
    int fail = addMemory(alist);
    if (fail) {
      fprintf(stderr, "Allocation Failed");
      return -1;
    }
  }

  Data* newData = createData(pid, command, status);

  Data** iter = alist->dataArray;
  int i;
  for (i=0; i < alist->numitems; i++)
    iter++;
  
  
  *iter = newData;
  alist->numitems++;
  return 0;
}

int insert(ilist* alist, int pos, int pid, char** command, int status) {
  
  if (pos < 0 || pos >= alist->numitems) {
    fprintf(stderr, "List index out of bounds\n");
    return -1;
  }
  
  if (alist->numitems == alist->size) {
    int fail = addMemory(alist);
    if (fail) {
      fprintf(stderr, "Allocation failed\n");
      return -1;
    }
  }
  
  Data* newData = createData(pid, command, status);
  
  Data** iter = alist->dataArray;
  int i;
  for (i=0; i < pos; i++)
    iter++;
  
  Data* itemin, *itemout;  
  itemin = newData;
  int j;
  for (j=0; j < alist->numitems - pos; j++) {
    itemout = *iter;
    *iter = itemin;
    itemin = itemout;
    iter++;
  }
  
  *iter = itemin;
  
  alist->numitems++;
  return 0;
}


Data* get(ilist* alist, int pos) {
  if (pos < 0 || pos >= alist->numitems) {
    fprintf(stderr, "List index out of bounds\n");
    return NULL;
  }
  
  return *(alist->dataArray + pos);
}

int len(ilist* alist) {
  return alist->numitems;
}

int removeItem(ilist* alist, int pos) {
  if (pos < 0 || pos >= alist->numitems) {
    fprintf(stderr, "List index out of bounds\n");
    return -1;
  }
  else if (alist->numitems == 0) {
    fprintf(stderr, "List already empty\n");
    return -1;
  }

  Data** iter = alist->dataArray + alist->numitems - 1;
  Data* itemout = 0;
  Data* itemin = 0;

  while (iter != alist->dataArray + pos) {
    itemout = *iter;
    *iter = itemin;
    itemin = itemout;
    iter--;
  }

  *iter = itemin;

  alist->numitems--;

  if (alist->numitems != 0 && alist->size / alist->numitems == 2) {
    alist->dataArray = (Data**) realloc(alist->dataArray, sizeof(Data*) * (alist->size / 2));
    alist->size /= 2;
  }
  return 0;
}

void print(ilist* alist) {
  if (alist->numitems == 0) {
    printf("[]\n");
    exit(EXIT_SUCCESS);
  }

  printf("[(%d, %d), ", alist->dataArray[0]->pid, alist->dataArray[0]->status);
  Data** iter = alist->dataArray + 1;
  int i;
  for (i=1; i < alist->numitems; i++) {
    printf(", (%d, %d)", iter[i]->pid, iter[i]->status);
    iter++;
  }
  printf("]\n");
  exit(EXIT_SUCCESS);
}
  
    
  
int addMemory(ilist* alist) {
  Data** ready = (Data**) realloc(alist->dataArray, sizeof(Data*) * (2 * alist->size));
  if (!ready)
    return 1;
  else {
    alist->dataArray = ready;
    alist->size *= 2;
    return 0;
  }
}

void cleanList(ilist* alist) {
  free(alist->dataArray);
  alist->dataArray = (Data**) malloc(sizeof(Data*) * alist->size);
  alist->numitems = 0;
}

void freeList(ilist* alist) {
  int i;
  for (i=0; i < alist->numitems; i++)
    free(alist->dataArray[i]->command);
    free(alist->dataArray[i]);
  
  free(alist->dataArray);
  free(alist);
  alist = NULL;
}
