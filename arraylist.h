#ifndef ARRAYLIST_H
#define ARRAYLIST_H

typedef struct arraylist {
  int* intarray;
  int numitems;
  int size;
}list;

struct arraylist* create(void);

//Place a new integer on the end of the array
int append(struct arraylist*, int);

//Place a new integer at a given position
int insert(struct arraylist*, int, int);

//Find the item at a given position
int get(struct arraylist*, int);

//Get length of the array
int len(struct arraylist*);

//Remove item at a given position
int removeItem(struct arraylist*, int);

//Display list contents
void print(struct arraylist*);

//Handles any reallocation needed by other functions
int addMemory(struct arraylist*);

//Deletes all contents of list by reallocating
void cleanList(struct arraylist*);

//Frees the memory from thhe list and sets the pointer to NULL
void freeList(struct arraylist*);

#endif

