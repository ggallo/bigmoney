#include "input.h"

int main()
{
  pInput input = createInput(256);
  int done = 0;
  while(!done)
    {
      update(stdin, input);
      while(1)
	{
	  char* string = getNext(input);
	  if(string)
	    printf("%s\n", string);
	  else
	    break;
	  if(strstr(string, "exit()"))
	    done = 1;
	}
    }
  
  return 0;
}
      
