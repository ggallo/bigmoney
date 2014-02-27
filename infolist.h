#ifndef INFOLIST_H
#define INFOLIST_H

typedef struct infolist {
  struct Data** dataArray;
  int numitems;
  int size;
} ilist;

typedef struct Data {
  pid_t pid;
  char* command;
  int status;
} Data;

struct infolist* create(void);

//Make a new Data structure and return a pointer to it
struct Data* createData(int, char**, int);

//Place a new integer on the end of the array
int append(struct infolist*, int, char**, int);

//Place a new integer at a given position
int insert(struct infolist*, int, int, char**, int);

//Find the item at a given position
struct Data* get(struct infolist*, int);

//Get length of the array
int len(struct infolist*);

//Remove item at a given position
int removeItem(struct infolist*, int);

//Display list contents
void print(struct infolist*);

//Handles any reallocation needed by other functions
int addMemory(struct infolist*);

//Deletes all contents of list by reallocating
void cleanList(struct infolist*);

//Frees the memory from thhe list and sets the pointer to NULL
void freeList(struct infolist*);

#endif

