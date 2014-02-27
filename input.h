#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

typedef struct Input Input;
typedef struct Input* pInput;
typedef struct Command Com;
typedef struct Command* pCom;

struct Input                                            //struct Input represents one input line (could contain multiple commands separated by  semicolons).
                                                        //Each command is a struct Command, stored in a linked list
{
  int count;                                            //how many separate commands are in the linked list
  int maxLen;                                           //maximum length of the input line
  pCom head;
  pCom tail;
  pCom curr;                                            //pointer to the position of the active command
};

struct Command                                          //struct Command represents one single command
{
  char** argv;                                          //contains the arguments of the command, starting with the path
  int argc;                                             //length of the argv array, including the trailing NULL pointer 
                                                        //e.g. for argv = ["/usr/bin/ls", "-l", NULL], argc = 3
  int link[2];                                          //array to facilitate pipes
  pCom next;                                            //pointer to the next node
};

pInput createInput(int maxLen);                         //constructor for struct Input
void reset(pInput input);                               //clear the content of a struct Input object
void deleteInput(pInput input);                         //destructor for struct Input
pCom createCommand(int argc, char** argv);              //constructor for struct Command
void deleteCommand(pCom command);                       //destructor for struct Command 
void add(pInput input, pCom command);                   //adds a struct Command object to the linked list in struct Input object
pCom getNext(pInput input);                             //returns a pointer to the argv array of the active command in a struct Input object, 
                                                        //and increase its curr pointer by one
void update(FILE* stream, pInput input);                //update the content of a struct Input object with a new input line
void cleanup(char* string, int* argc, char** argv);     //format a plain text command into a struct Command object
char* preformat(char* text);                            //preformat a raw string to handle pipe and hash (comment) symbols

#endif
