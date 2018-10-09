// COMP1521 18s1 Assignment 2

// myHeap test: init Heap and dump



#include <stdio.h>

#include <stdlib.h>

#include "myHeap.h"



typedef unsigned int uint;



typedef struct {             // headers for Chunks

   uint  status;             // status (ALLOC or FREE)

   uint  size;               // #bytes, including header

} Header;



#define RESET "\x1B[0m"

#define BGRN  "\033[1m\033[32m"

#define BRED  "\033[1m\033[31m"

#define BCYN  "\033[1m\033[36m"

#define BBRED "\033[1m\033[4m\033[31m"



int main(int argc, char *argv[])

{

   if (argc < 2) {

      printf("Usage: %s Size\n", argv[0]);

      exit(1);

   }



   printf(BGRN"[myTest] "RESET" Creating myHeap\n");

   if (initHeap(atoi(argv[1])) < 0) {

      printf("Can't init heap of size %d\n", atoi(argv[1]));

      exit(1);

   }



   printf("\n"BGRN"[myTest] "RESET" Dumping myHeap contents\n");

   dumpHeap();



   printf("\n"BGRN"[myTest] "RESET" Malloc 14 -> 16\n");

   void *ptr1 = myMalloc(14); // should round up to 16

   printf(BGRN"[myTest] "RESET" Ptr1: %p\n", ptr1);



   printf("\n"BGRN"[myTest] "RESET" Malloc 1000\n");

   void *ptr2 = myMalloc(1000);

   printf(BGRN"[myTest] "RESET" Ptr1: %p\n", ptr2);



   printf("\n"BGRN"[myTest] "RESET" Malloc 3960\n");

   void *ptr3 = myMalloc(3960);

   printf(BGRN"[myTest] "RESET" Ptr1: %p\n", ptr3);



   printf("\n"BGRN"[myTest] "RESET" Attempting to free ptr3: %p\n", ptr3);

   myFree(ptr3);

   printf("\n"BGRN"[myTest] "RESET" Attempting to free ptr1: %p\n", ptr1);

   myFree(ptr1);

   printf("\n"BGRN"[myTest] "RESET" Attempting to free ptr2: %p\n", ptr2);

   myFree(ptr2);







   dumpHeap();



   printf("\n"BGRN"[myTest] "RESET" Freeing myHeap\n");

   freeHeap();

   printf("\n"BGRN"[myTest] "RESET" All tests complete\n");

   return 0;

}
