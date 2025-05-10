/**
 * @file vector.c
 * @brief Implementation of a dynamic array (vector) in C.
 *
 * This file contains the implementation of a dynamic array, also known as a 
 * vector, which provides functionalities for dynamic resizing, element access, 
 * and manipulation.
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
static void ShiftOneOver( struct Vector_S * self, size_t idx, bool move_right );

/* Public API Implementations */

/******************************************************************************/
struct Vector_S * VectorInit( size_t element_size,
                              size_t initial_capacity,
                              size_t max_capacity,
                              size_t initial_len )
{
   // Early return op
   // Invalid inputs
   if ( (element_size == 0) ||
        (initial_capacity > MAX_VECTOR_LENGTH) ||
        (max_capacity == 0) ||
        (initial_capacity > max_capacity) ||
        (initial_len > initial_capacity) )
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
      NewVec->len = 0;
   }
   else
   {
      NewVec->capacity = initial_capacity;
      if ( initial_len > 0 )
      {
         memset( NewVec->arr, 0, (element_size * initial_len) );
         NewVec->len = initial_len;
      }
      else
      {
         NewVec->len = 0;
      }
   }

   NewVec->element_size = element_size;

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

/******************************************************************************/
void VectorFree( struct Vector_S * self )
{
   if ( (self != NULL) && (self->arr != NULL) )
   {
      free(self->arr);
   }
   free(self);
}

/******************************************************************************/
size_t VectorLength( const struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->len;
}

/******************************************************************************/
size_t VectorCapacity( const struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->capacity;
}

/******************************************************************************/
size_t VectorMaxCapacity( const struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->max_capacity;
}

/******************************************************************************/
size_t VectorElementSize( const struct Vector_S * self )
{
   if ( NULL == self )
   {
      return 0;
   }
   return self->element_size;
}

/******************************************************************************/
bool VectorIsEmpty( const struct Vector_S * self )
{
   if ( NULL == self )
   {
      return true;
   }
   return IS_EMPTY(self);
}

/******************************************************************************/
bool VectorIsFull( const struct Vector_S * self )
{
   if ( NULL == self )
   {
      return false;
   }
   return self->len == self->max_capacity;
}

/******************************************************************************/
bool VectorPush( struct Vector_S * self, const void * element )
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

/******************************************************************************/
bool VectorInsertAt( struct Vector_S * self,
                     size_t idx,
                     const void * element )
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
      if ( idx < self->len )
      {
         ShiftOneOver(self, idx, true);
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

/******************************************************************************/
void * VectorGetElementAt( const struct Vector_S * self, size_t idx )
{
   if ( (NULL == self) || (idx >= self->len) )
   {
      return NULL;
   }

   assert(self->arr != NULL);

   return (void *)PTR_TO_IDX(self, idx);
}

/******************************************************************************/
bool VectorCpyElementAt( const struct Vector_S * self, size_t idx, void * data )
{
   if ( (NULL == self) || (idx >= self->len) || (NULL == data))
   {
      return false;
   }

   assert(self->arr != NULL);
   assert(self->element_size > 0);

   (void)memcpy( data, (void *)PTR_TO_IDX(self, idx), self->element_size );

   return true;
}

/******************************************************************************/
bool VectorSetElementAt( struct Vector_S * self,
                           size_t idx,
                           const void * element )
{
   if ( (NULL == self) || (idx >= self->len) || (NULL == element) )
   {
      return false;
   }

   assert(self->arr != NULL);
   assert(self->element_size > 0);

   memcpy( PTR_TO_IDX(self, idx), element, self->element_size );

   return true;
}

/******************************************************************************/
bool VectorRemoveElementAt( struct Vector_S * self, size_t idx, void * data )
{
   if ( (NULL == self) || (idx >= self->len) || (self->len == 0) )
   {
      return false;
   }

   assert(self->arr != NULL);
   assert(self->element_size > 0);
   assert(self->len <= self->capacity);
   assert(self->capacity <= self->max_capacity);

   if ( data != NULL )
   {
      memcpy(data, PTR_TO_IDX(self, idx), self->element_size);
   }
   ShiftOneOver(self, idx, false);
   self->len--;

   return true;
}

/******************************************************************************/
bool VectorRemoveLastElement( struct Vector_S * self, void * data )
{
   return VectorRemoveElementAt( self, VectorLength(self) - 1, data );
}

/******************************************************************************/
bool VectorClearElementAt( struct Vector_S * self, size_t idx )
{
   if ( (NULL == self) || (0 == self->len) || (idx >= self->len) )
   {
      return false;
   }

   assert(self->element_size > 0);
   assert(self->arr != NULL);

   memset( (void *)PTR_TO_IDX(self, idx), 0, self->element_size );

   return true;
}

/******************************************************************************/
void * VectorLastElement( const struct Vector_S * self )
{
   if ( (NULL == self) || (0 == self->len) )
   {
      return NULL;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( (self->element_size * self->len) <= PTRDIFF_MAX );
   assert( self->arr != NULL );

   return (void *)PTR_TO_IDX(self, (self->len - 1));
}

/******************************************************************************/
bool VectorCpyLastElement( const struct Vector_S * self, void * data )
{
   if ( (NULL == self) || (0 == self->len) || (NULL == data) )
   {
      return false;
   }

   assert( self->len <= self->capacity );
   assert( self->len <= self->max_capacity );
   assert( self->element_size > 0 );
   assert( (self->element_size * self->len) <= PTRDIFF_MAX );
   assert( self->arr != NULL );

   (void)memcpy( data,
                 VectorLastElement(self),
                 self->element_size );
   
   return true;
}

/******************************************************************************/
bool VectorClear( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return false;
   }

   self->len = 0;
   return true;
}

/******************************************************************************/
bool VectorHardReset( struct Vector_S * self )
{
   if ( NULL == self )
   {
      return false;
   }

   assert(self->arr != NULL);
   assert(self->element_size > 0);

   for ( size_t i = 0; i < self->len; i++ )
   {
      memset( PTR_TO_IDX(self, i), 0, self->element_size );
   }
   self->len = 0;
   return true;
}

/******************************************************************************/
struct Vector_S * VectorDuplicate( const struct Vector_S * self )
{
   if ( (NULL == self) ||
        // Check for internal paradoxes within passed in vector...
        (0 == self->element_size) ||
        (self->len > self->capacity) ||
        (self->capacity > self->max_capacity) ||
        ( (self->len > 0) && (NULL == self->arr) ) )
   {
      return NULL;
   }

   struct Vector_S * Duplicate = (struct Vector_S *)malloc( sizeof(struct Vector_S) );
   if ( NULL == Duplicate )
   {
      // TODO: Throw exception that the vector handle failed to get duplicated.
      return NULL;
   }

   memcpy( Duplicate, self, sizeof(struct Vector_S) );
   
   Duplicate->arr = NULL;
   if ( Duplicate->len > 0 )
   {
      Duplicate->arr = malloc( Duplicate->len * Duplicate->element_size );
      if ( Duplicate->arr != NULL )
      {
         memcpy( Duplicate->arr,
                 self->arr,
                 Duplicate->len * Duplicate->element_size );
      }
      else
      {
         // TODO: Throw exception that underlying data failed to get duplicated.
      }
   }

   // Duplicated vector _must not_ reference original vector's data!
   assert( Duplicate->arr != self->arr );

   return Duplicate;
}

/******************************************************************************/
bool VectorsAreEqual( const struct Vector_S * a, const struct Vector_S * b )
{
   // Check for NULL pointers
   if ( (NULL == a) || (NULL == b) )   return false;

   // First check lengths
   if (a->len != b->len) return false;
   
   // Then check the element sizes
   if (a->element_size != b->element_size) return false;

   // Then check capacities
   if (a->capacity != b->capacity) return false;

   // Then check max capacities
   if (a->max_capacity != b->max_capacity) return false;

   // Then check each element in the vector
   for ( size_t i = 0; i < a->len; i++ )
   {
      if ( memcmp( PTR_TO_IDX(a,i), PTR_TO_IDX(b,i), a->element_size ) )
      {
         return false;
      }
   }

   return true;
}

/* Sub-Range Based Vector Operations */

/******************************************************************************/
void * VectorSubRange_GetElementsFromIdx( const struct Vector_S * self,
                                          size_t idx )
{
   if ( (NULL == self) ||
        (idx >= self->len) ||
        IS_EMPTY(self) )
   {
      return NULL;
   }

   assert(self->len > 0);
   assert(self->arr != NULL);
   assert(self->element_size > 0);

   return (void *)PTR_TO_IDX(self, idx);
}

bool VectorSubRange_CpyElementsInRange( const struct Vector_S * self,
                                        size_t idx_start,
                                        size_t idx_end,
                                        void * buffer )
{
   if ( (NULL == self) || (NULL == buffer) ||
        (idx_start >= self->len) || (idx_end >= self->len) ||
        (idx_start > idx_end)
      )
   {
      return false;
   }

   assert(self->len > 0);
   assert(self->arr != NULL);
   assert(self->element_size > 0);

   uint8_t * ptr_to_start = PTR_TO_IDX(self, idx_start);
   size_t idx_diff = (idx_end - idx_start) + 1; // inclusive copy!
   memcpy( buffer, ptr_to_start, (idx_diff * self->element_size) );

   return true;
}


/* Private Function Implementations */

/**
 * @brief Expands the capacity of the vector to accommodate additional elements.
 *
 * This function is used internally to increase the storage capacity of the vector
 * when the current capacity is insufficient to hold new elements. It ensures that
 * the vector can grow dynamically as needed.
 *
 * @param self A pointer to the Vector_S structure representing the vector.
 * @return true if the expansion was successful, false otherwise.
 */
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

/**
 * @brief Shifts elements in the vector either to the left or right by one position.
 * 
 * This function moves all elements in the vector starting from the given index
 * one position to the right or left, depending on the implementation, to make
 * room for new elements or to fill gaps after an element is removed.
 *
 * @param self Pointer to the Vector_S structure.
 * @param idx The index at which the shift operation begins.
 * @param move_right A boolean flag indicating the direction of the shift.
 *        - If true, elements are shifted to the right.
 *        - If false, elements are shifted to the left.
 */
static void ShiftOneOver( struct Vector_S * self, size_t idx, bool move_right )
{
   assert( (self != NULL) &&
           (self->arr != NULL) &&
           (self->len < self->capacity) &&
            // Don't try to shift data past the data range of the array
           (idx < self->len) &&
           (idx < MAX_VECTOR_LENGTH) &&
           (self->element_size > 0) );

   if ( move_right )
   {
      // Start at the end and shift over to the right by one until we hit idx
      for ( size_t i = self->len; i > idx; i-- )
      {
         uint8_t * old_spot = PTR_TO_IDX(self, i - 1);
         uint8_t * new_spot = PTR_TO_IDX(self, i);
         memcpy( new_spot, old_spot, self->element_size );
      }
   }
   else // shift left
   {
      // Start at the one to the right of the idx and shift over to left by one
      // until we hit the end
      for ( size_t i = (idx + 1); i < self->len; i++ )
      {
         uint8_t * old_spot = PTR_TO_IDX(self, i);
         uint8_t * new_spot = PTR_TO_IDX(self, i - 1);
         memcpy( new_spot, old_spot, self->element_size );
      }
   }
}
