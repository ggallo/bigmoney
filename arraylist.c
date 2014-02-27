#include <stdio.h>
#include <stdlib.h>
#include "arraylist.h"

list* create(void) {
  list* newlist = (list*)malloc(sizeof(list));
  newlist->intarray = (int*) malloc(sizeof(int));
  newlist->numitems = 0;
  newlist->size = 1;
  return newlist;
}

int append(list* alist, int item) {
  if (alist->numitems == alist->size) {
    int fail = addMemory(alist);
    if (fail) {
      fprintf(stderr, "Allocation Failed");
      return -1;
    }
  }

  int* iter = alist->intarray;
  int i;
  for (i=0; i < alist->numitems; i++)
    iter++;
  
  *iter = item;
  alist->numitems++;
  return 0;
}

int insert(list* alist, int pos, int item) {
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

  int* iter = alist->intarray;
  int i;
  for (i=0; i < pos; i++) {
    iter++;
  }

  int itemin, itemout;
  itemin = item;
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

int get(list* alist, int pos) {
  if (pos < 0 || pos >= alist->numitems) {
    fprintf(stderr, "List index out of bounds\n");
    exit(EXIT_FAILURE);
  }

  return *(alist->intarray + pos);
}

int len(list* alist) {
  return alist->numitems;
}

int removeItem(list* alist, int pos) {
  if (pos < 0 || pos >= alist->numitems) {
    fprintf(stderr, "List index out of bounds\n");
    return -1;
  }
  else if (alist->numitems == 0) {
    fprintf(stderr, "List already empty\n");
    return -1;
  }

  int* iter = alist->intarray + alist->numitems - 1;
  int itemout = 0;
  int itemin = 0;

  while (iter != alist->intarray + pos) {
    itemout = *iter;
    *iter = itemin;
    itemin = itemout;
    iter--;
  }
  *iter = itemin;

  alist->numitems--;

  if (alist->numitems != 0 && alist->size / alist->numitems == 2) {
    alist->intarray = (int*)realloc(alist->intarray, sizeof(int) * (alist->size / 2));
    alist->size /= 2;
  }
  return 0;
}

void print(list* alist) {
  if (alist->numitems == 0) {
    printf("[]\n");
    exit(EXIT_SUCCESS);
  }

  printf("[%d", *(alist->intarray));
  int* iter = alist->intarray + 1;
  int i;
  for (i=1; i < alist->numitems; i++) {
    printf(", %d",*iter);
    iter++;
  }
  printf("]\n");
}
  
    
  
int addMemory(list* alist) {
  int* ready = (int*)realloc(alist->intarray, sizeof(int) * (2 * alist->size));
  if (!ready)
    return 1;
  else {
    alist->intarray = ready;
    alist->size *= 2;
    return 0;
  }
}

void cleanList(list* alist) {
  free(alist->intarray);
  alist->intarray = (int*)malloc(sizeof(int) * alist->size);
  alist->numitems = 0;
}

void freeList(list* alist) {
  free(alist->intarray);
  free(alist);
  alist = NULL;
}
