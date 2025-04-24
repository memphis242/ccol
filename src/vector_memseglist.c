/**
 * @file vector.c
 * @brief Implementation of a dynamic array (vector) in C.
 *
 * This file contains the implementation of a dynamic array, also known as a vector,
 * which provides functionalities for dynamic resizing, element access, and manipulation.
 * This implementation utilizes an array of pointers to arrays of even sizes to
 * mitigate the risk of stale pointers due to reallocations. So, instead of realloc,
 * this will simply alloc a new segment and a pointer to it to grow the vector.
 *
 * @author Abdulla Almosalami (memphis242)
 * @date April 23, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <limits.h>
#include "vector.h"

/* Local Macro Definitions */
// Constants
#define EXPANSION_FACTOR   (2)

// Function-like
#define IS_EMPTY(self)     ( 0 == (self)->len )
#define PTR_TO_IDX(vec, idx) \
      ( (uint8_t *)((vec)->arr) + ((vec)->element_size * (idx)) )

// Enforce a maximum length to help prevent extreme memory requests
#define TENTATIVE_MAX_VEC_LEN UINT32_MAX
#if ( SIZE_MAX < PTRDIFF_MAX )
   #define SYSTEM_LIMIT SIZE_MAX
#else
   #define SYSTEM_LIMIT PTRDIFF_MAX
#endif
#if (SYSTEM_LIMIT < TENTATIVE_MAX_VEC_LEN)
   #define MAX_VECTOR_LENGTH  SIZE_MAX
#else
   #define MAX_VECTOR_LENGTH TENTATIVE_MAX_VEC_LEN
#endif

/* Local Datatypes */
struct Vector_S
{
   void * arr;
   size_t element_size;
   uint32_t len;
   uint32_t capacity;
   uint32_t max_capacity;
};

/* Private Function Prototypes */
static bool LocalVectorExpand( struct Vector_S * self );
static void ShiftOneOver( struct Vector_S * self, uint32_t idx );

/* Public API Implementations */

struct Vector_S * VectorInit( size_t element_size,
                              uint32_t initial_capacity,
                              uint32_t max_capacity )
{
   // TODO
}

void VectorFree( struct Vector_S * self )
{
   if ( self != NULL )
   {
      free(self->arr);
   }
   free(self);
}

uint32_t VectorLength( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->len;
}

uint32_t VectorCapacity( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->capacity;
}

uint32_t VectorMaxCapacity( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->max_capacity;
}

size_t VectorElementSize( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->element_size;
}

bool VectorIsEmpty( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return true;
   }
   return IS_EMPTY(self);
}

bool VectorPush( struct Vector_S * self, const void * restrict element )
{
   // Early return op
   // Invalid inputs
   if ( (NULL == self) || (NULL == element) )
   {
      // TODO: Throw exception
      return false;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( (self->element_size * self->len) <= PTRDIFF_MAX );

   bool ret_val = true;

   // Ensure there's space
   bool successfully_expanded = true;
   if ( self->len == self->capacity )
   {
      successfully_expanded = LocalVectorExpand(self);
   }

   if ( successfully_expanded )
   {
      void * insertion_spot = (void *)PTR_TO_IDX(self, self->len);
      memcpy( insertion_spot, element, self->element_size );
      self->len++;
   }
   else
   {
      ret_val = false;
   }

   return ret_val;
}

bool VectorInsertAt( struct Vector_S * self,
                     uint32_t idx,
                     const void * restrict element )
{
   // Early return op
   // Invalid inputs
   if ( (NULL == self) || (NULL == element) || (idx > self->len) )
   {
      // TODO: Throw exception
      return false;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( idx <= self->len );

   bool ret_val = true;

   // Ensure there's space
   bool successfully_expanded = true;
   if ( self->len == self->capacity )
   {
      successfully_expanded = LocalVectorExpand(self);
   }

   if ( successfully_expanded )
   {
      if ( self->len > 0 )
      {
         ShiftOneOver(self, idx);
      }
      void * insertion_spot = (void *)PTR_TO_IDX(self, idx);
      memcpy( insertion_spot, element, self->element_size );
      self->len++;
   }
   else
   {
      ret_val = false;
   }

   return ret_val;
}

void * VectorGetElementAt( struct Vector_S * self, uint32_t idx )
{
   if ( (NULL == self) || (idx >= self->len) || (NULL == self->arr) )
   {
      return NULL;
   }

   return (void *)PTR_TO_IDX(self, idx);
}

bool VectorCpyElementAt( struct Vector_S * self, uint32_t idx, void * data )
{
   if ( (NULL == self) || (idx >= self->len) || (NULL == self->arr) ||
        (NULL == data) || (0 == self->element_size) )
   {
      return false;
   }

   (void)memcpy( data, (void *)PTR_TO_IDX(self, idx), self->element_size );

   return true;
}

bool VectorSetElementAt( struct Vector_S * self,
                           uint32_t idx,
                           const void * restrict element )
{
   return false;
}

bool VectorRemoveElementAt( struct Vector_S * self, uint32_t idx, void * data )
{
   return false;
}

bool VectorLastElement( struct Vector_S * self, void * data )
{
   if ( (NULL == self) || (NULL == self->arr) || (0 == self->len) )
   {
      return NULL;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( (self->element_size * self->len) <= PTRDIFF_MAX );

   return (void *)( (uint8_t *)(self->arr) + (self->element_size * self->len) );
}

bool VectorClear( struct Vector_S * self )
{
   return false;
}

/* Private Function Implementations */

static bool LocalVectorExpand( struct Vector_S * self )
{
   // TODO
}

// Shift to the right all elements from the idx onwards to length of the vector
static void ShiftOneOver( struct Vector_S * self, uint32_t idx )
{
   // TODO
}
