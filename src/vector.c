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
#include "vector_cfg.h"
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
   #define MAX_VECTOR_LIST_INITIAL_LEN  SIZE_MAX
#else
   #define MAX_VECTOR_LIST_INITIAL_LEN TENTATIVE_MAX_VEC_LEN
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
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
   void * (*vec_malloc)(size_t);
   void * (*vec_realloc)(void *, size_t);
   void   (*vec_free)(void *);
   bool   (*vec_isalloc)(void *);
#endif
};

/* Private Function Prototypes */

#ifdef VEC_USE_BUILT_IN_STATIC_ALLOC
static void StaticArrayPoolInit(void);

static struct Vector_S * StaticVectorArenaAlloc(void);
static void * StaticArrayAlloc(size_t num_of_bytes);
static void * StaticArrayRealloc(void * ptr, size_t num_of_bytes);
static void   StaticVectorArenaFree(struct Vector_S * ptr);
static void   StaticArrayFree(void * ptr);
static bool   StaticVectorIsAlloc(struct Vector_S * ptr);
static bool   StaticArrayIsAlloc(void * ptr);
#endif

static bool LocalVectorExpand( struct Vector_S * self );
static bool LocalVectorExpandBy( struct Vector_S * self, size_t add_len );
static void ShiftNOver( struct Vector_S * self, size_t idx,
                        bool move_right, size_t n );

/* Public API Implementations */

/******************************************************************************/
struct Vector_S * VectorInit( size_t element_size,
                              size_t initial_capacity,
                              size_t max_capacity,
                              size_t initial_len
#ifdef VEC_USE_CUSTOM_ALLOC
                              , void * (*custom_malloc)(size_t),
                              void * (*custom_realloc)(void *, size_t),
                              void   (*custom_free)(void *),
                              bool   (*is_allocated)(void *)
#endif
                            )
{
   // Invalid inputs
   if ( (element_size == 0) ||
        (initial_capacity > MAX_VECTOR_LIST_INITIAL_LEN) ||
        (max_capacity == 0) ||
        (initial_capacity > max_capacity) ||
        (initial_len > initial_capacity)
#ifdef VEC_USE_CUSTOM_ALLOC
        || (NULL == custom_malloc)
        || (NULL == custom_realloc)
        || (NULL == custom_free)
#endif
      )
   {
      // TODO: Vector constructor exception
      return NULL;
   }

#if defined(VEC_USE_CUSTOM_ALLOC)
   struct Vector_S * NewVec = custom_malloc( sizeof(struct Vector_S) );
#elif defined(VEC_USE_BUILT_IN_STATIC_ALLOC)
   if ( !ArrayArena.arena_initialized ) StaticArrayPoolInit();
   struct Vector_S * NewVec = StaticVectorArenaAlloc( sizeof(struct Vector_S) );
#else
   struct Vector_S * NewVec = malloc( sizeof(struct Vector_S) );
#endif
   if ( NULL == NewVec )
   {
      return NULL;
   }

#if defined(VEC_USE_CUSTOM_ALLOC)
   NewVec->vec_malloc = custom_malloc;
   NewVec->vec_realloc = custom_realloc;
   NewVec->vec_free = custom_free;
   NewVec->vec_isalloc = is_allocated;
#elif !defined(VEC_USE_BUILT_IN_STATIC_ALLOC)
   NewVec->vec_malloc = malloc;
   NewVec->vec_realloc = realloc;
   NewVec->vec_free = free;
   NewVec->vec_isalloc = NULL;
#endif

   if ( 0 == initial_capacity )
   {
      NewVec->arr = NULL;
   }
   else
   {
#ifdef VEC_USE_BUILT_IN_STATIC_ALLOC
      NewVec->arr = StaticArrayAlloc( element_size * initial_capacity );
#else
      NewVec->arr = NewVec->vec_malloc( element_size * initial_capacity );
#endif
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

   if ( max_capacity > MAX_VECTOR_LIST_INITIAL_LEN )
   {
      // TODO: Throw exception for max_capacity too large
      NewVec->max_capacity = MAX_VECTOR_LIST_INITIAL_LEN;
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
   if ( (self != NULL) && (self->arr != NULL)
   {
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
      if (self->vec_free != NULL) )
      {
         self->vec_free(self->arr);
         self->vec_free(self);
      }
#else
      StaticArrayFree(self->arr);
      StaticVectorArenaFree(self);
#endif
   }
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
   if ( (NULL == self) || (NULL == self->arr) ||
        (0 == self->len) || (idx >= self->len) )
   {
      return false;
   }

   assert(self->element_size > 0);
   assert(self->arr != NULL);

   memset( PTR_TO_IDX(self, idx), 0, self->element_size );

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
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
   assert(self->vec_free != NULL);
#endif

   memset( self->arr, 0, self->len * self->element_size );
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
   self->vec_free(self->arr);
#else
   StaticArrayFree(self->arr);
#endif
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
        ( (self->len > 0) && (NULL == self->arr) )
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
        || (NULL == self->vec_malloc)
#endif
      )
   {
      return NULL;
   }

#ifdef VEC_USE_BUILT_IN_STATIC_ALLOC
   struct Vector_S * Duplicate = StaticVectorArenaAlloc();
#else
   struct Vector_S * Duplicate = self->vec_malloc( sizeof(struct Vector_S) );
#endif
   if ( NULL == Duplicate )
   {
      // TODO: Throw exception that the vector handle failed to get duplicated.
      return NULL;
   }

   memcpy( Duplicate, self, sizeof(struct Vector_S) );
   
   Duplicate->arr = NULL;
   if ( Duplicate->len > 0 )
   {
#ifdef VEC_USE_BUILT_IN_STATIC_ALLOC
      Duplicate->arr = StaticArrayAlloc( Duplicate->len * Duplicate->element_size );
#else
      Duplicate->arr = self->vec_malloc( Duplicate->len * Duplicate->element_size );
#endif
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
        (v2->len > (MAX_VECTOR_LIST_INITIAL_LEN - v1->len)) // Unsupported length
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
      size_t new_vec_cap = MAX_VECTOR_LIST_INITIAL_LEN;
      if ( v2->capacity < (MAX_VECTOR_LIST_INITIAL_LEN - v1->capacity) )
      {
         new_vec_cap = v1->capacity + v2->capacity;
      }
      size_t new_vec_max_cap = MAX_VECTOR_LIST_INITIAL_LEN;
      if ( v2->max_capacity < (MAX_VECTOR_LIST_INITIAL_LEN - v1->max_capacity) )
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

   if ( dlen == 1 )
   {
      return VectorInsertAt(self, idx, data);
   }

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
   if ( NULL == self )  return false;
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
   if ( NULL == self )  return false;
   return VectorSubRange_SetElementsInRange( self, idx, self->len - 1, data );
}

/******************************************************************************/

bool VectorSubRange_RemoveElementsInRange( struct Vector_S * self,
                                           size_t idx_start,
                                           size_t idx_end,
                                           void * buf )
{
   if ( (NULL == self) || (NULL == self->arr) ||
        (idx_start >= self->len) || (idx_end >= self->len) ||
        (idx_start > idx_end) || (self->len == 0) ) 
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

   size_t num_of_removed = idx_end - idx_start + 1;
   if ( NULL != buf )
   {
      VectorSubRange_CpyElementsInRange(self, idx_start, idx_end, buf);
   }
   // Only need to shift over if the removal does not include the end
   if ( idx_end < (self->len - 1) )
   {
      ShiftNOver(self, idx_end + 1, false, num_of_removed);
   }
#ifdef SECURE_REMOVAL
   else
   {
      // Zero out that leftover data
      VectorSubRange_ClearElementsInRange(self, idx_start, idx_end);
   }
#endif
   self->len -= num_of_removed;

   return true;
}

/******************************************************************************/

bool VectorSubRange_RemoveElementsFromStartToIdx( struct Vector_S * self,
                                                  size_t idx,
                                                  void * buf )
{
   return VectorSubRange_RemoveElementsInRange( self, 0, idx, buf );
}

/******************************************************************************/

bool VectorSubRange_RemoveElementsFromIdxToEnd( struct Vector_S * self,
                                                size_t idx,
                                                void * buf )
{
   if ( NULL == self )  return false;
   return VectorSubRange_RemoveElementsInRange(self, idx, self->len-1, buf);
}

/******************************************************************************/
/******************************************************************************/

bool VectorSubRange_ClearElementsInRange( struct Vector_S * self,
                                          size_t idx_start,
                                          size_t idx_end )
{
   if ( (NULL == self) || (NULL == self->arr) ||
        (idx_start >= self->len) || (idx_end >= self->len) ||
        (idx_start > idx_end) ) 
   {
      return false;
   }

   memset(
      PTR_TO_IDX(self, idx_start),
      0,
      self->element_size * (idx_end - idx_start + 1) );

   return true;
}

/******************************************************************************/

bool VectorSubRange_ClearElementsFromStartToIdx( struct Vector_S * self,
                                                 size_t idx )
{
   return VectorSubRange_ClearElementsInRange(self, 0, idx);
}

/******************************************************************************/

bool VectorSubRange_ClearElementsFromIdxToEnd( struct Vector_S * self,
                                               size_t idx )
{
   if ( (NULL == self) || (0 == self->len) ) return false;
   return VectorSubRange_ClearElementsInRange(self, idx, self->len - 1);
}

/******************************************************************************/

bool VectorClearAll( struct Vector_S * self )
{
   if ( (NULL == self) || (0 == self->len) ) return false;
   return VectorSubRange_ClearElementsInRange(self, 0, self->len - 1);
}

/******************************************************************************/

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
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
   assert(self->vec_realloc != NULL);
#endif

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

   void * new_ptr = 
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
      self->vec_realloc(
#else
      StaticArrayRealloc(
#endif
         self->arr, (self->element_size * new_capacity) );
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
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
   assert(self->vec_realloc != NULL);
#endif

   // If there's no space in the vector, we can't expand
   if ( (self->capacity + add_len) > self->max_capacity )
   {
      return false;
   }

   size_t new_capacity = self->capacity + add_len;
   void * new_ptr = 
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
      self->vec_realloc(
#else
      StaticArrayRealloc(
#endif
         self->arr, (self->element_size * new_capacity) );
   if ( new_ptr != NULL )
   {
      self->arr = new_ptr;
      self->capacity = new_capacity;
      return true;
   }

   return false;
}

/**
 * @brief Shifts elements in the vector either to the left or right from a given idx.
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
   assert(idx < MAX_VECTOR_LIST_INITIAL_LEN);
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
      // Start at the one to the right of the idx and shift over to left by n
      for ( size_t i = idx; i < self->len; i++ )
      {
         uint8_t * old_spot = PTR_TO_IDX(self, i);
         uint8_t * new_spot = PTR_TO_IDX(self, i - (n - 1) - 1);
         memcpy( new_spot, old_spot, self->element_size );
      }
   }
}

/******************************************************************************/

#ifdef VEC_USE_BUILT_IN_STATIC_ALLOC

/********** Vector Arena Material **********/
struct VectorArenaItem_S
{
   struct Vector_S vec;
   bool is_allocated;
};

struct VectorArena_S
{
   struct VectorArenaItem_S pool[VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE];
   size_t next_idx;
};

static struct VectorArena_S VectorArena;

/**
 * @brief Allocates a new Vector_S structure from a static arena.
 * @return Pointer to the allocated Vector_S struct if successful, NULL otherwise.
 */
static struct Vector_S * StaticVectorArenaAlloc(void)
{
   assert( VectorArena.next_idx < VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE );
   assert( VectorArena.pool != NULL );
#ifndef NDEBUG
   // If next idx is allocated, by design, that must mean we are out of vectors.
   if ( VectorArena.pool[VectorArena.next_idx].is_allocated == true )
   {
      for ( size_t i = 0; i < VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE; i++ )
      {
         assert( VectorArena.pool[i].is_allocated == true );
      }
   }
#endif

   if ( VectorArena.pool[VectorArena.next_idx].is_allocated )
   {
      return NULL;
   }

   struct Vector_S * new_vec = &VectorArena.pool[VectorArena.next_idx].vec;
   VectorArena.pool[VectorArena.next_idx].is_allocated = true;

   // 🗒️: Potential to place this in a separate asynchronous thread?
   // Find the next available spot
   size_t j = VectorArena.next_idx;
   for ( size_t i = 1; i < VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE; i++, j++ )
   {
      if ( j >= VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE ) j = 0; // Wrap-around

      if ( !VectorArena.pool[j].is_allocated )
      {
         VectorArena.next_idx = j;
         break;
      }
   }

   return new_vec;
}

static void StaticVectorArenaFree(struct Vector_S * ptr)
{
   if ( NULL == ptr )
   {
      return;
   }

   // Find the vector address that matches this pointer
   bool found = false;
   for ( size_t i = 0; i < VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE; i++ )
   {
      if ( ptr == &VectorArena.pool[i].vec )
      {
         if ( !VectorArena.pool[i].is_allocated )
         {
            // TODO: Raise exception for attempting to free an unallocated vec
         }
         VectorArena.pool[i].is_allocated = false;
      }
   }

   if ( !found )
   {
      // TODO: Raise an exception for attempting to free a random address
   }
}

static bool StaticVectorIsAlloc(struct Vector_S * ptr)
{
   for ( size_t i = 0; i < VEC_BUILT_IN_STATIC_VECTOR_ARENA_SIZE; i++ )
   {
      if ( ptr == &VectorArena.pool[i].vec )
      {
         return VectorArena.pool[i].is_allocated;
      }
   }

   return false;
}


/********** Array Arena Material **********/

// Split the configured arena size into blocks of 1024, 512, 256, ..., 2.
// Add 1 to ensure no arrays are 0-sized. This also helps ensure we fully cover
// the configured arena size, with a little extra on top.
#define BLOCKS_1024_LIST_INITIAL_LEN ( ((VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE)        / 1024) + 1)
#define BLOCKS_512_LIST_INITIAL_LEN  ( ((VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE % 1024) / 512)  + 1)
#define BLOCKS_256_LIST_INITIAL_LEN  ( ((VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE % 512)  / 256)  + 1)
#define BLOCKS_128_LIST_INITIAL_LEN  ( ((VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE % 256)  / 128)  + 1)
#define BLOCKS_64_LIST_INITIAL_LEN   ( ((VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE % 128)  / 64)   + 1)
#define BLOCKS_32_LIST_INITIAL_LEN   ( ((VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE % 64)   / 32)   + 1)

enum BlockSize_E
{
   B1024,
   B512,
   B256,
   B128,
   B64,
   B32,
   NUM_OF_BLOCK_SIZES
};

struct ArrayPoolBlock_S
{
   void * ptr; // Pointer to block
   bool is_free;  // Flag to clear when allocating this block
};
struct ArrayPoolBlockList_S
{
   struct ArrayPoolBlock_S * blocks; // Array of blocks of this list
   uint16_t block_size; // Size of blocks in this list in bytes
   size_t len; // How many blocks are in this list
};
struct ArrayArena_S
{
   struct ArrayPoolBlockList_S lists[NUM_OF_BLOCK_SIZES];
   bool arena_initialized;
   size_t space_available;
};

//! The arena of contiguous bytes from which we allocate from.
static uint8_t ArrayArenaPool[VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE];
// These shall be the list of allocatable blocks (the "free lists").
struct ArrayPoolBlock_S blocks_1024 [ VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE / 1024 ];
struct ArrayPoolBlock_S blocks_512  [ VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE / 512  ];
struct ArrayPoolBlock_S blocks_256  [ VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE / 256  ];
struct ArrayPoolBlock_S blocks_128  [ VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE / 128  ];
struct ArrayPoolBlock_S blocks_64   [ VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE / 64   ];
struct ArrayPoolBlock_S blocks_32   [ VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE / 32   ];

static struct ArrayArena_S ArrayArena =
{
   .lists =
   {
      [ B1024 ] = { .blocks = blocks_1024, .len = (BLOCKS_1024_LIST_INITIAL_LEN - 1), .block_size = 1024 },
      [ B512 ]  = { .blocks = blocks_512,  .len = (BLOCKS_512_LIST_INITIAL_LEN - 1),  .block_size = 512  },
      [ B256 ]  = { .blocks = blocks_256,  .len = (BLOCKS_256_LIST_INITIAL_LEN - 1),  .block_size = 256  },
      [ B128 ]  = { .blocks = blocks_128,  .len = (BLOCKS_128_LIST_INITIAL_LEN - 1),  .block_size = 128  },
      [ B64 ]   = { .blocks = blocks_64,   .len = (BLOCKS_64_LIST_INITIAL_LEN - 1),   .block_size = 64   },
      [ B32 ]   = { .blocks = blocks_32,   .len = (BLOCKS_32_LIST_INITIAL_LEN - 1),   .block_size = 32   }
   },
   .arena_initialized = false,
   .space_available = VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE - (VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE % 32)
};

/**
 * @brief Initializes the static array pool arena structures.
 */
static void StaticArrayPoolInit(void)
{
   // Initialize the array arena's pointers, calculating an offset into the
   // ArrayArenaPool for each of the block list pointers.
   // TODO: This can be done at compile-time. If the macro magic isn't too crazy, let's try that.
   assert( ArrayArena.lists != NULL );
   assert( !ArrayArena.arena_initialized );

   size_t accumulating_offset = 0;
   for ( uint8_t i = 0; i < (uint8_t)NUM_OF_BLOCK_SIZES; i++ )
   {
      struct ArrayPoolBlockList_S * list = &ArrayArena.lists[i];
      assert( list != NULL );
      for ( size_t j = 0; j < list->len; j++ )
      {
         assert( accumulating_offset < VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE );
         list->blocks[j].ptr = &ArrayArenaPool[ accumulating_offset ];
         list->blocks[j].is_free = true;
         accumulating_offset += list->block_size;
         assert( accumulating_offset <= VEC_BUILT_IN_STATIC_ARRAY_ARENA_SIZE );
      }
   }

   ArrayArena.arena_initialized = true;
}

/**
 * @brief Allocates a contiguous block that can accomodate num_of_bytes from
 *        a static arena.
 * @note Presently uses the "Buddy System" as described in:
 *          memorymanagement.org/mmref/alloc.html
 * @return Pointer to the allocated block if successful, NULL otherwise.
 */
static void * StaticArrayAlloc(size_t num_of_bytes)
{
   if ( num_of_bytes > ArrayArena.space_available )
   {
      return NULL;
   }

   void * block_ptr = NULL;
   size_t space_allocated = 0;
   if ( (num_of_bytes <= 1024) && (num_of_bytes > (1024 - 32)) )
   {
      bool found_block = false;
      struct ArrayPoolBlockList_S * list = &ArrayArena.list_1024;
      for ( size_t i = (list->len - 1); i >= 0; i-- )
      {
         if ( list->blocks[i].is_free )
         {
            found_block = true;
            space_allocated = 1024;
            block_ptr = list->blocks[i].ptr;
         }
      }

      if ( (!found_block) && (num_of_bytes != 1024) )
      {
         // TODO: Going to need to use multiple smaller blocks.
         //       Keep in mind that you're going to need to know how to free
         //       these "merged" blocks!

      }
   }


   ArrayArena.space_available -= space_allocated;
   return block_ptr;
}

static void * StaticArrayRealloc(void * ptr, size_t num_of_bytes)
{

}

static void StaticArrayFree(void * ptr)
{

}

static bool StaticArrayIsAlloc(void * ptr)
{

}

#endif
