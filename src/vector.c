/**
 * @file vector.c
 * @brief Implementation of a dynamic array (vector) in C.
 *
 * This file contains the implementation of a dynamic array, also known as a vector,
 * which provides functionalities for dynamic resizing, element access, and manipulation.
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

// TODO: Realloc usage is a security vulnerability because data left behind from
//       realloc moving the data rather than growing/shrinking could be sensitive
//       and somehow exploited elsewhere. Given that, add support for custom
//       allocators and provide your own, more secure realloc.

/* Local Macro Definitions */
// Constants
#define EXPANSION_FACTOR   (2)

// Function-like
#define IS_EMPTY(self)     ( 0 == (self)->len )

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
      NewVec->arr = malloc( (size_t)(element_size * initial_capacity) );
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
      void * insertion_spot = (unsigned char *)self->arr + (self->element_size * self->len);
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
      void * insertion_spot = (unsigned char *)self->arr + (self->element_size * self->len);
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
   return NULL;
}

bool VectorSetElementAt( struct Vector_S * self,
                           uint32_t idx,
                           const void * restrict element )
{
   return false;
}

void * VectorRemoveElementAt( struct Vector_S * self, uint32_t idx )
{
   return NULL;
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

   return (void *)( (uint8_t *)(self->arr) + (self->element_size * self->len) );
}

bool VectorClear( struct Vector_S * self )
{
   return false;
}

/* Private Function Implementations */

static bool LocalVectorExpand( struct Vector_S * self )
{
   // Since this is a purely internal function, I will destructively assert at any invalid inputs
   assert( (self != NULL) && (self->element_size != 0) );

   bool ret_val = true;

   // If empty, create a single-element array
   if ( IS_EMPTY(self) )
   {
      self->arr = malloc( self->element_size );
      if ( NULL == self->arr )
      {
         ret_val = false;
         self->capacity = 0;
      }
      else
      {
         self->capacity = 1;
      }
      self->len = 0;
   }
   else if ( self->capacity == self->max_capacity )
   {
      // TODO: Throw exception: Already at capacity. Cannot expand further.
      ret_val = false;
   }
   else
   {
      // Assert if we somehow have a NULL arr when the vector was determined
      // to be non-empty...
      assert( self->arr != NULL );
      uint32_t new_capacity;

      if ( (uint32_t)(self->capacity * EXPANSION_FACTOR) <= self->max_capacity )
      {
         new_capacity = (uint32_t)(self->capacity * EXPANSION_FACTOR);
      }
      else
      {
         new_capacity = self->max_capacity;
      }

      void * new_ptr = realloc( self->arr, (self->element_size * new_capacity) );
      if ( NULL == new_ptr )
      {
         // TODO: Throw exception
         // TODO: Is the original pointer still usable?
         ret_val = false;
      }
      else
      {
         self->arr = new_ptr;
         self->capacity = new_capacity;
      }
   }

   return ret_val;
}

// Shift to the right all elements from the idx onwards to length of the vector
static void ShiftOneOver( struct Vector_S * self, uint32_t idx )
{
   assert( (self != NULL) &&
           (self->arr != NULL) &&
            // Don't try to shift data past the data range of the array
           (idx < self->len) &&
           (idx < MAX_VECTOR_LENGTH) &&
           (self->element_size > 0) );

   // Start at the end and shift over to the right by one until we hit idx
   for ( uint32_t i = self->len; i > idx; i-- )
   {
      uint8_t * old_spot = (uint8_t *)self->arr + ((i - 1) * self->element_size);
      uint8_t * new_spot = (uint8_t *)self->arr + (i * self->element_size);
      memcpy( new_spot, old_spot, self->element_size );
   }
}
