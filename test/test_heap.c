#include <stdio.h>
#include "heap.h"

struct Example_S
{
   char name[20];
   int x;
   int y;
   int z;
};

struct KeyValuePair_S
{
   long long int key;
   struct Example_S value;
};

int TestCmp( const void * a, const void * b );

int main(void)
{
   struct Heap_S * heap = NULL;
   heap = HeapInit( TestCmp, sizeof(struct KeyValuePair_S) );
   printf("Address of heap: %p\n", (void *)heap);
}

int TestCmp( const void * a, const void * b )
{
   // Early return opp
   if ( (NULL == a) || (NULL == b) )
   {
      return 0;
   }

   struct KeyValuePair_S * item1 = (struct KeyValuePair_S *)a;
   struct KeyValuePair_S * item2 = (struct KeyValuePair_S *)b;

   // Classic comparison pattern:
   return (item1->key - item2->key);
}