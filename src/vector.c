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
// Macro constants
#define EXPANSION_FACTOR                    (2)   //TODO: Make the expansion factor user-configurable
#define DEFAULT_INITIAL_CAPACITY            (10)  //! Not 1 because there would likely be a resize shortly after
#define DEFAULT_MAX_CAPACITY_FACTOR         (10)  //! How many multiples of initial capacity do we set max capacity by default
#define DEFAULT_LEN_TO_CAPACITY_FACTOR      (2)   //! How many multiples of length should capacity be set to by default

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

// Function-like macros

#define IS_EMPTY(self)     ( 0 == (self)->len )

/**
 * PTR_TO_IDX - Calculates the pointer to the element at a given index in a vector.
 * @vec: Pointer to the vector structure containing the array and element size.
 * @idx: Index of the element to access.
 */
#define PTR_TO_IDX(vec, idx) \
      ( (uint8_t *)((vec)->arr) + ((vec)->element_size * (idx)) )


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
static bool LocalVectorExpandBy( struct Vector_S * self, size_t add_len );
static void ShiftNOver( struct Vector_S * self, size_t idx,
                        bool move_right, size_t n );

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
         ShiftNOver(self, idx, true, 1);
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
   if ( idx < (self->len - 1) )
   {
      ShiftNOver(self, idx + 1, false, 1);
   }
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
bool VectorReset( struct Vector_S * self )
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

   memset( self->arr, 0, self->len * self->element_size );
   free(self->arr);
   self->arr = NULL; // After freeing memory, clear out stale pointers!
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
/******************************************************************************/

struct Vector_S * VectorSplitAt( struct Vector_S * self, size_t idx )
{
   if ( (NULL == self) || (self->len == 0) || (self->capacity == 0) ||
        (idx >= self->len) || (idx == 0) )
   {
      // TODO: Throw exception
      return NULL;
   }

   assert(self->len > 0);
   assert(self->arr != NULL);
   assert(self->element_size > 0);

   size_t new_vec_len = self->len - idx;
   struct Vector_S * new_vec = VectorInit( self->element_size,
                                           new_vec_len * 2,
                                           new_vec_len * 4,
                                           new_vec_len );
   if ( (NULL == new_vec) || (NULL == new_vec->arr) )
   {
      return NULL;
   }

   // NOTE: Don't mutate the original vector until after we've successfully
   //       initialized the new vector!
   self->len = idx;    // Does not include original element at idx
   memcpy( new_vec->arr,
           PTR_TO_IDX(self, idx),
           new_vec_len * self->element_size );
   
   #ifdef NO_DATA_LEFT_BEHIND
   memset( PTR_TO_IDX(self, idx), 0, new_vec_len * self->element_size );
   #endif

   return new_vec;
}

/******************************************************************************/

struct Vector_S * VectorSlice( const struct Vector_S * self,
                               size_t idx_start,
                               size_t idx_end )
{
   if ( (NULL == self) || (self->len == 0) || (self->capacity == 0) ||
        (idx_start >= self->len) || (idx_end >= self->len) ||
        (idx_start > idx_end) || (idx_end == 0) )
   {
      // TODO: Throw exception
      return NULL;
   }

   // Slicing the whole vector is the same as duplication
   if ( (0 == idx_start) && ((self->len - 1) == idx_end) )
   {
      return VectorDuplicate(self);
   }

   assert(self->len > 0);
   assert(self->arr != NULL);
   assert(self->element_size > 0);

   size_t new_vec_len = idx_end - idx_start + 1;   // inclusive of both indices
   struct Vector_S * new_vec = VectorInit( self->element_size,
                                           new_vec_len * 2,
                                           new_vec_len * 4,
                                           new_vec_len );
   if ( (NULL == new_vec) || (NULL == new_vec->arr) )
   {
      return NULL;
   }

   memcpy( new_vec->arr,
           PTR_TO_IDX(self, idx_start),
           new_vec_len * self->element_size );
   
   return new_vec;
}

/******************************************************************************/

struct Vector_S * VectorConcatenate( const struct Vector_S * v1,
                                     const struct Vector_S * v2 )
{
   if ( (NULL == v1) || (NULL == v2) ||
        (v1->element_size != v2->element_size) ||
        (v2->len > (MAX_VECTOR_LENGTH - v1->len)) // Unsupported length
      )
   {
      return NULL;
   }

   assert(
      ( (v1->len > 0) && (v1->arr != NULL) ) ||
        (0 == v1->len)
   );
   assert(
      ( (v2->len > 0) && (v2->arr != NULL) ) ||
        (0 == v2->len)
   );
   assert(v1->element_size > 0);
   assert(v2->element_size > 0);
   assert(v1->element_size == v2->element_size);

   struct Vector_S * NewVec = NULL;
   // If one of the vectors is empty, simply create a duplicate of the non-empty
   // vector. If both vectors are empty, create an empty vector.
   if ( (0 == v1->len) && (0 == v2->len) )
   {
      NewVec = VectorInit( v1->element_size,
                           DEFAULT_INITIAL_CAPACITY,
                           DEFAULT_INITIAL_CAPACITY * DEFAULT_MAX_CAPACITY_FACTOR,
                           0 );
   }

   else if ( (v1->len > 0)  && (v2->len == 0) )
   {
      NewVec = VectorDuplicate(v1);
   }
   else if ( (v1->len == 0) && (v2->len > 0) )
   {
      NewVec = VectorDuplicate(v2);
   }

   else  // Both must be non-empty
   {
      size_t new_vec_len = v1->len + v2->len;
      size_t new_vec_cap = MAX_VECTOR_LENGTH;
      if ( v2->capacity < (MAX_VECTOR_LENGTH - v1->capacity) )
      {
         new_vec_cap = v1->capacity + v2->capacity;
      }
      size_t new_vec_max_cap = MAX_VECTOR_LENGTH;
      if ( v2->max_capacity < (MAX_VECTOR_LENGTH - v1->max_capacity) )
      {
         new_vec_max_cap = v1->max_capacity + v2->max_capacity;
      }

      NewVec = VectorInit( v1->element_size,
                           new_vec_cap,
                           new_vec_max_cap,
                           new_vec_len );
      if ( (NewVec != NULL) && (NewVec->arr != NULL) )
      {
         memcpy( NewVec->arr,                 v1->arr, (v1->len * v1->element_size) );
         memcpy( PTR_TO_IDX(NewVec, v1->len), v2->arr, (v2->len * v2->element_size) );
      }
      else
      {
         // Something failed in creating the vector...
         VectorFree(NewVec);
         NewVec = NULL; // We'll return NULL
      }
   }

   return NewVec;
}

/******************************************************************************/
/******************************************************************************/

bool VectorSubRange_PushElements( struct Vector_S * self,
                                  const void * data,
                                  size_t dlen )
{
   if ( (NULL == self) || (NULL == data) ||
        ( (self->len + dlen) > self->max_capacity ) || (dlen == 0) )
   {
      // TODO: Throw exception
      return false;
   }

   assert( self->len <= self->capacity );
   assert( self->capacity <= self->max_capacity );
   assert( (self->len == 0) || ( (self->len > 0) && (self->arr != NULL) ) );

   // Ensure there's space
   bool successfully_expanded = true;
   if ( (self->len + dlen) > self->capacity )
   {
      successfully_expanded = LocalVectorExpandBy(
                                       self,
                                       /* Expand relative to capacity, not len*/
                                       dlen - (self->capacity - self->len) );
   }

   if ( successfully_expanded )
   {
      void * insertion_spot = (void *)PTR_TO_IDX(self, self->len);
      memcpy( insertion_spot, data, (self->element_size * dlen) );
      self->len += dlen;
   }
   else
   {
      return false;
   }

   return true;
}

/******************************************************************************/

bool VectorSubRange_InsertElementsAt( struct Vector_S * self,
                                      size_t idx, 
                                      const void * data,
                                      size_t dlen )
{
   if ( (NULL == self) || (NULL == data) ||
        ( (self->len + dlen) > self->max_capacity ) || (dlen == 0) ||
        (idx > self->len) )
   {
      // TODO: Throw exception
      return false;
   }

   assert( self->len <= self->capacity );
   assert( self->capacity <= self->max_capacity );
   assert( (self->len == 0) || ( (self->len > 0) && (self->arr != NULL) ) );

   // Ensure there's space
   bool successfully_expanded = true;
   if ( (self->len + dlen) > self->capacity )
   {
      successfully_expanded = LocalVectorExpandBy(
                                       self,
                                       /* Expand relative to capacity, not len*/
                                       dlen - (self->capacity - self->len) );
   }

   if ( successfully_expanded )
   {
      if ( idx < self->len )
      {
         ShiftNOver(self, idx, true, dlen);
      }
      void * insertion_spot = (void *)PTR_TO_IDX(self, idx);
      memcpy( insertion_spot, data, (self->element_size * dlen) );
      self->len += dlen;
   }
   else
   {
      return false;
   }

   return true;
}

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

/******************************************************************************/
/******************************************************************************/

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

/******************************************************************************/

bool VectorSubRange_CpyElementsFromStartToIdx( const struct Vector_S * self,
                                               size_t idx,
                                               void * buffer )
{
   return VectorSubRange_CpyElementsInRange(self, 0, idx, buffer);
}

/******************************************************************************/

bool VectorSubRange_CpyElementsFromIdxToEnd( const struct Vector_S * self,
                                             size_t idx,
                                             void * buffer )
{
   if ( (NULL == self) || (NULL == buffer) )
   {
      return false;
   }
   return VectorSubRange_CpyElementsInRange(self, idx, self->len - 1, buffer);
}

/******************************************************************************/
/******************************************************************************/

bool VectorSubRange_SetElementsInRange( struct Vector_S * self,
                                        size_t idx_start,
                                        size_t idx_end,
                                        const void * data )
{
   if ( (NULL == self) || (NULL == data) ||
        (idx_start >= self->len) || (idx_end >= self->len) ||
        (idx_start > idx_end) ) 
   {
      return false;
   }

   assert(self->len > 0);
   assert(self->arr != NULL);
   assert(self->element_size > 0);

   uint8_t * ptr_to_start = PTR_TO_IDX(self, idx_start);
   size_t idx_diff = (idx_end - idx_start) + 1; // inclusive copy!
   memcpy( ptr_to_start, data, (idx_diff * self->element_size) );

   return true;
}

/******************************************************************************/

bool VectorSubRange_SetElementsFromStartToIdx( struct Vector_S * self,
                                               size_t idx,
                                               const void * data )
{
   return VectorSubRange_SetElementsInRange( self, 0, idx, data );
}

/******************************************************************************/

bool VectorSubRange_SetElementsFromIdxToEnd( struct Vector_S * self,
                                             size_t idx,
                                             const void * data )
{
   if ( (NULL == self) || (NULL == data) )
   {
      return false;
   }

   return VectorSubRange_SetElementsInRange( self, idx, self->len - 1, data );
}

/******************************************************************************/

bool VectorSubRange_RemoveElementsInRange( struct Vector_S * self,
                                           size_t idx_start,
                                           size_t idx_end,
                                           void * buf )
{
   if ( (NULL == self) || (self->len == 0) ||
        (idx_start > idx_end) ||
        (idx_start > (self->len - 1)) || (idx_end > (self->len - 1)) )
   {
      return false;
   }

   assert(self->len > 0);
   assert(self->arr != NULL);
   assert(self->element_size > 0);

   if ( idx_start == idx_end )
   {
      return VectorRemoveElementAt(self, idx_start, buf);
   }

   return true;
}

/* Private Function Implementations */

/**
 * @brief Expands the capacity of the vector to accommodate additional elements.
 * 
 * @param self Vector handle.
 * @return true if the expansion was successful, false otherwise.
 */
static bool LocalVectorExpand( struct Vector_S * self )
{
   // Since this is a purely internal function, I will destructively assert at any invalid inputs
   assert(self != NULL);
   assert(self->element_size != 0);
   assert(self->arr != NULL);
   assert(self->len <= self->capacity);
   assert(self->len <= self->max_capacity); 

   // If we're already at max capacity, can't expand further.
   if ( self->capacity == self->max_capacity )
   {
      return false;
   }

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
      return true;
   }

   return false;
}

/**
 * @brief Expands the capacity of the vector by a specified number of elements.
 *
 * @param self Vector handle.
 * @param add_len The number of additional elements to expand the capacity by.
 * @return true if the expansion was successful; false otherwise.
 */
static bool LocalVectorExpandBy( struct Vector_S * self, size_t add_len )
{
   // Since this is a purely internal function, I will destructively assert at any invalid inputs
   assert(self != NULL);
   assert(self->element_size != 0);
   assert(self->arr != NULL);
   assert(self->len <= self->capacity);
   assert(self->len <= self->max_capacity); 

   // If there's no space in the vector, we can't expand
   if ( (self->capacity + add_len) > self->max_capacity )
   {
      return false;
   }

   size_t new_capacity = self->capacity + add_len;
   void * new_ptr = realloc( self->arr, (self->element_size * new_capacity) );
   if ( new_ptr != NULL )
   {
      self->arr = new_ptr;
      self->capacity = new_capacity;
      return true;
   }

   return false;
}

/**
 * @brief Shifts elements in the vector either to the left or right by one position.
 * 
 * This function moves all elements in the vector starting from the given index
 * n positions to the right or left.
 * 
 * @note This function won't automatically expand capacity. Make sure there's
 *       room before calling this function to shift right, or it will fail.
 * 
 * @note This function will also fail if you try to shift left by more than the
 *       length of the vector.
 *
 * @param self Pointer to the Vector_S structure.
 * @param idx The index at which the shift operation begins.
 * @param n Number of indices to shift by.
 * @param move_right A boolean flag indicating the direction of the shift.
 *        - If true, elements are shifted to the right.
 *        - If false, elements are shifted to the left.
 */
static void ShiftNOver( struct Vector_S * self, size_t idx,
                        bool move_right, size_t n )
{
   assert(self != NULL);
   assert(self->arr != NULL);
   assert(self->len < self->capacity);
   // Don't try to shift data past the data range of the array
   assert(idx < self->len);
   assert( (move_right && ((n + self->len) <= self->capacity)) || (!move_right && (n <= self->len)) );
   // Extra checks to trap paradox states
   assert(idx < MAX_VECTOR_LENGTH);
   assert(self->element_size > 0);

   if ( move_right )
   {
      // Start at the end and shift over to the right until we hit idx
      for ( size_t i = self->len; i > idx; i-- )
      {
         uint8_t * old_spot = PTR_TO_IDX(self, i - 1);
         uint8_t * new_spot = PTR_TO_IDX(self, (i + n) - 1);
         memcpy( new_spot, old_spot, self->element_size );
      }
   }
   else // shift left
   {
      // Start at the one to the right of the idx and shift over to left by one
      // until we hit the end
      for ( size_t i = idx; i < self->len; i++ )
      {
         uint8_t * old_spot = PTR_TO_IDX(self, i);
         uint8_t * new_spot = PTR_TO_IDX(self, i - (n - 1) - 1);
         memcpy( new_spot, old_spot, self->element_size );
      }
   }
}
