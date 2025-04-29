/**
 * @file vector.c
 * @brief Implementation of a dynamic array (vector) in C.
 *
 * This file contains the implementation of a dynamic array, also known as a vector,
 * which provides functionalities for dynamic resizing, element access, and manipulation.
 * The underlying primitive data structure used is an array.
 *
 * @author Abdulla Almosalami (memphis242)
 * @date April 12, 2025
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

// TODO: Add support for custom alloactors.

/* Local Macro Definitions */
// Constants
#define EXPANSION_FACTOR            (2)   //TODO: Make the expansion factor user-configurable
#define DEFAULT_INITIAL_CAPACITY    (10)  //! Not 1 because there would likely be a resize shortly after

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
   size_t len;
   size_t capacity;
   size_t max_capacity;
};

/* Private Function Prototypes */
static bool LocalVectorExpand( struct Vector_S * self );
static void ShiftOneOver( struct Vector_S * self, size_t idx );

/* Public API Implementations */

struct Vector_S * VectorInit( size_t element_size,
                              size_t initial_capacity,
                              size_t max_capacity )
{
   // Early return op
   // Invalid inputs
   if ( (element_size == 0) ||
        (initial_capacity > MAX_VECTOR_LENGTH) ||
        (max_capacity == 0) ||
        (initial_capacity > max_capacity) )
   {
      // TODO: Vector constructor exception
      return NULL;
   }

   struct Vector_S * NewVec = (struct Vector_S *)malloc( sizeof(struct Vector_S) );
   if ( NULL == NewVec )
   {
      return NULL;
   }

   if ( 0 == initial_capacity )
   {
      NewVec->arr = NULL;
   }
   else
   {
      NewVec->arr = malloc( element_size * initial_capacity );
   }

   // If malloc failed to allocate space for the array...
   if ( (initial_capacity > 0) && (NULL == NewVec->arr) )
   {
      // TODO: Throw exception to inform user...
      NewVec->capacity = 0;
   }
   else
   {
      NewVec->capacity = initial_capacity;
   }

   NewVec->element_size = element_size;
   NewVec->len = 0;
   if ( max_capacity > MAX_VECTOR_LENGTH )
   {
      // TODO: Throw exception for max_capacity too large
      NewVec->max_capacity = MAX_VECTOR_LENGTH;
   }
   else
   {
      NewVec->max_capacity = max_capacity;
   }

   return NewVec;
}

void VectorFree( struct Vector_S * self )
{
   if ( (self != NULL) && (self->arr != NULL) )
   {
      free(self->arr);
   }
   free(self);
}

size_t VectorLength( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->len;
}

size_t VectorCapacity( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->capacity;
}

size_t VectorMaxCapacity( struct Vector_S * self )
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
                     size_t idx,
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

void * VectorGetElementAt( struct Vector_S * self, size_t idx )
{
   if ( (NULL == self) || (idx >= self->len) || (NULL == self->arr) )
   {
      return NULL;
   }

   return (void *)PTR_TO_IDX(self, idx);
}

bool VectorCpyElementAt( struct Vector_S * self, size_t idx, void * data )
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
                           size_t idx,
                           const void * restrict element )
{
   return false;
}

bool VectorRemoveElementAt( struct Vector_S * self, size_t idx, void * data )
{
   return false;
}

void * VectorLastElement( struct Vector_S * self )
{
   if ( (NULL == self) || (NULL == self->arr) || (0 == self->len) )
   {
      return NULL;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( (self->element_size * self->len) <= PTRDIFF_MAX );

   return (void *)PTR_TO_IDX(self, (self->len - 1));
}

bool VectorCpyLastElement( struct Vector_S * self, void * data )
{
   if ( (NULL == self) || (NULL == self->arr) || (0 == self->len) ||
        (NULL == data) || (0 == self->element_size) )
   {
      return false;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( (self->element_size * self->len) <= PTRDIFF_MAX );

   (void)memcpy( data,
                 VectorLastElement(self),
                 self->element_size );
   
   return true;
}

bool VectorClear( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return false;
   }

   self->len = 0;
   return true;
}

bool VectorHardReset( struct Vector_S * self )
{
   if ( (NULL == self) || (NULL == self->arr) || (0 == self->element_size) )
   {
      return false;
   }

   for ( size_t i = 0; i < self->len; i++ )
   {
      memset( PTR_TO_IDX(self, i), 0, self->element_size );
   }
   self->len = 0;
   return true;
}

/* Private Function Implementations */

static bool LocalVectorExpand( struct Vector_S * self )
{
   // Since this is a purely internal function, I will destructively assert at any invalid inputs
   assert( (self != NULL) &&
           (self->element_size != 0) &&
           (self->arr != NULL) &&
           (self->len <= self->capacity) &&
           (self->len <= self->max_capacity) ); 

   // If we're already at max capacity, can't expand further.
   if ( self->capacity == self->max_capacity )
   {
      return false;
   }

   bool ret_val = false;

   // First determine new capacity, and then realloc.
   size_t new_capacity;
   if ( 0 == self->capacity )
   {
      new_capacity = DEFAULT_INITIAL_CAPACITY;
   }
   else if ( (self->capacity * EXPANSION_FACTOR) < self->max_capacity )
   {
      new_capacity = self->capacity * EXPANSION_FACTOR;
   }
   else
   {
      new_capacity = self->max_capacity;
   }

   void * new_ptr = realloc( self->arr, (self->element_size * new_capacity) );
   if ( new_ptr != NULL )
   {
      self->arr = new_ptr;
      self->capacity = new_capacity;
      ret_val = true;
   }

   return ret_val;
}

// Shift to the right all elements from the idx onwards to length of the vector
static void ShiftOneOver( struct Vector_S * self, size_t idx )
{
   assert( (self != NULL) &&
           (self->arr != NULL) &&
            // Don't try to shift data past the data range of the array
           (idx < self->len) &&
           (idx < MAX_VECTOR_LENGTH) &&
           (self->element_size > 0) );

   // Start at the end and shift over to the right by one until we hit idx
   for ( size_t i = self->len; i > idx; i-- )
   {
      uint8_t * old_spot = PTR_TO_IDX(self, i - 1);
      uint8_t * new_spot = PTR_TO_IDX(self, i);
      memcpy( new_spot, old_spot, self->element_size );
   }
}
