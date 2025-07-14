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

/* Local Macro Definitions */

#ifdef UNIT_TEST
#define STATIC
#else
#define STATIC static
#endif

// Macro constants
#define EXPANSION_FACTOR                    (2)   // TODO: Make the expansion factor user-configurable - note that a floating-point number causes a warning: conversion from 'size_t' {aka 'long long unsigned int'} to 'double' may change value [-Wconversion]
#define DEFAULT_INITIAL_CAPACITY            (10)  //! Not 1 because there would likely be a resize shortly after
#define DEFAULT_MAX_CAPACITY_FACTOR         (10)  //! How many multiples of initial capacity do we set max capacity by default
#define DEFAULT_LEN_TO_CAPACITY_FACTOR      (2)   //! How many multiples of length should capacity be set to by default

// Enforce a maximum length to help prevent extreme memory requests
#ifndef MAX_VEC_LEN
#define TENTATIVE_MAX_VEC_LEN UINT32_MAX
#if ( SIZE_MAX < PTRDIFF_MAX )
   #define SYSTEM_LIMIT SIZE_MAX
#else
   #define SYSTEM_LIMIT PTRDIFF_MAX
#endif // SIZE_MAX < PTRDIFF_MAX
#if (SYSTEM_LIMIT < TENTATIVE_MAX_VEC_LEN)
   #define MAX_VEC_LEN  SIZE_MAX
#else
   #define MAX_VEC_LEN TENTATIVE_MAX_VEC_LEN
#endif // (SYSTEM_LIMIT < TENTATIVE_MAX_VEC_LEN)
#endif // MAX_VEC_LEN

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
STATIC void StaticArrayPoolInit(void);
STATIC bool StaticArrayPoolIsInitialized(void);

STATIC struct Vector_S * StaticVectorArenaAlloc(void);
STATIC void   StaticVectorArenaFree(const struct Vector_S *);
STATIC bool   StaticVectorIsAlloc(const struct Vector_S *);

STATIC void * StaticArrayAlloc(size_t);
STATIC void * StaticArrayRealloc(void *, size_t);
STATIC void   StaticArrayFree(const void *);
STATIC bool   StaticArrayIsAlloc(const void *);
#endif

static bool LocalVectorExpand(struct Vector_S *);
static bool LocalVectorExpandBy(struct Vector_S *, size_t);
static void ShiftNOver( struct Vector_S *, size_t, bool, size_t);

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
        (initial_capacity > MAX_VEC_LEN) ||
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
   if ( !StaticArrayPoolIsInitialized() ) StaticArrayPoolInit();
   struct Vector_S * NewVec = StaticVectorArenaAlloc();
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

   if ( max_capacity > MAX_VEC_LEN )
   {
      // TODO: Throw exception for max_capacity too large
      NewVec->max_capacity = MAX_VEC_LEN;
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
#ifndef VEC_USE_BUILT_IN_STATIC_ALLOC
      if (self->vec_free != NULL)
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
        (v2->len > (MAX_VEC_LEN - v1->len)) // Unsupported length
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
      size_t new_vec_cap = MAX_VEC_LEN;
      if ( v2->capacity < (MAX_VEC_LEN - v1->capacity) )
      {
         new_vec_cap = v1->capacity + v2->capacity;
      }
      size_t new_vec_max_cap = MAX_VEC_LEN;
      if ( v2->max_capacity < (MAX_VEC_LEN - v1->max_capacity) )
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
   assert(idx < MAX_VEC_LEN);
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
   struct VectorArenaItem_S pool[VEC_ARRAY_ARENA_SIZE];
   size_t next_idx;
};

STATIC struct VectorArena_S VectorArena;

/**
 * @brief Allocates a new Vector_S structure from a static arena.
 * @return Pointer to the allocated Vector_S struct if successful, NULL otherwise.
 */
STATIC struct Vector_S * StaticVectorArenaAlloc(void)
{
   assert( VectorArena.next_idx < VEC_ARRAY_ARENA_SIZE );
   assert( VectorArena.pool != NULL );
#ifndef NDEBUG
   // If next idx is allocated, by design, that must mean we are out of vectors.
   if ( VectorArena.pool[VectorArena.next_idx].is_allocated == true )
   {
      for ( size_t i = 0; i < VEC_ARRAY_ARENA_SIZE; i++ )
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
   for ( size_t i = 1; i < VEC_ARRAY_ARENA_SIZE; i++, j++ )
   {
      if ( j >= VEC_ARRAY_ARENA_SIZE ) j = 0; // Wrap-around

      if ( !VectorArena.pool[j].is_allocated )
      {
         VectorArena.next_idx = j;
         break;
      }
   }

   return new_vec;
}

STATIC void StaticVectorArenaFree(const struct Vector_S * ptr)
{
   if ( NULL == ptr )
   {
      return;
   }

   // Find the vector address that matches this pointer
   bool found = false;
   for ( size_t i = 0; i < VEC_ARRAY_ARENA_SIZE; i++ )
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

STATIC bool StaticVectorIsAlloc(const struct Vector_S * ptr)
{
   for ( size_t i = 0; i < VEC_ARRAY_ARENA_SIZE; i++ )
   {
      if ( ptr == &VectorArena.pool[i].vec )
      {
         return VectorArena.pool[i].is_allocated;
      }
   }

   return false;
}


/********** Array Arena Material **********/

// Macro constants for the capacity of each free list.
// The lists are statically sized so that theoretically, the full static array
// arena can be owned by any single list. This is done because we have to account
// for the run-time dynamics of these lists shifting ownership of the arena in
// all sorts of strange and mysterious ways in practice. We may start off with
// the 1024 list owning the majority of the arena, and over time, each 1024
// blocks may be split off, theoretically until all the blocks are 32-byte sized.
// Of course, we cannot dynamically size the memory used by these lists because,
// these lists are the handlers of dynamic memory, so their overhead must be set
// up front!
#define BLOCKS_1024_LIST_CAPACITY (( VEC_ARRAY_ARENA_SIZE / 1024 ) + 1)
#define BLOCKS_512_LIST_CAPACITY  (( VEC_ARRAY_ARENA_SIZE / 512  ) + 1)
#define BLOCKS_256_LIST_CAPACITY  (( VEC_ARRAY_ARENA_SIZE / 256  ) + 1)
#define BLOCKS_128_LIST_CAPACITY  (( VEC_ARRAY_ARENA_SIZE / 128  ) + 1)
#define BLOCKS_64_LIST_CAPACITY   (( VEC_ARRAY_ARENA_SIZE / 64   ) + 1)
#define BLOCKS_32_LIST_CAPACITY   (( VEC_ARRAY_ARENA_SIZE / 32   ) + 1)

// Macro constants for the iniital length of each free list.
// Ideally, the distribution of the initial lengths will match the distribution
// of requests during runtime. This minimizes the amount of times splitting and
// coalescing has to happen.
// Since that's not really going to be possible up front, one can either go
// through the discretize_arena.py Python script, write their own initial lens,
// or use the default below which starts at the largest size and goes down.
#ifdef USE_EXTERNAL_INIT_LENS
#include "array_arena_cfg.h"
#else
#define BLOCKS_1024_LIST_INIT_LEN (((VEC_ARRAY_ARENA_SIZE)        / 1024) + 1)
#define BLOCKS_512_LIST_INIT_LEN  (((VEC_ARRAY_ARENA_SIZE % 1024) / 512)  + 1)
#define BLOCKS_256_LIST_INIT_LEN  (((VEC_ARRAY_ARENA_SIZE % 512)  / 256)  + 1)
#define BLOCKS_128_LIST_INIT_LEN  (((VEC_ARRAY_ARENA_SIZE % 256)  / 128)  + 1)
#define BLOCKS_64_LIST_INIT_LEN   (((VEC_ARRAY_ARENA_SIZE % 128)  / 64)   + 1)
#define BLOCKS_32_LIST_INIT_LEN   (((VEC_ARRAY_ARENA_SIZE % 64)   / 32)   + 1)
#endif // USE_EXTERNAL_INIT_LENS

enum BlockSize
{
   BLKS_LARGEST_SIZE,
   BLKS_1024 = BLKS_LARGEST_SIZE,
   BLKS_512,
   BLKS_256,
   BLKS_128,
   BLKS_64,
   BLKS_32,
   NUM_OF_BLOCK_SIZES
};

static size_t BlockSize_E_to_Int[NUM_OF_BLOCK_SIZES] = { 1024, 512, 256, 128, 64, 32 };

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
STATIC uint8_t ArrayArenaPool[VEC_ARRAY_ARENA_SIZE];

// These shall be the list of allocatable blocks (the "free lists").
struct ArrayPoolBlock_S blocks_1024[BLOCKS_1024_LIST_CAPACITY];
struct ArrayPoolBlock_S blocks_512[BLOCKS_512_LIST_CAPACITY];
struct ArrayPoolBlock_S blocks_256[BLOCKS_256_LIST_CAPACITY];
struct ArrayPoolBlock_S blocks_128[BLOCKS_128_LIST_CAPACITY];
struct ArrayPoolBlock_S blocks_64[BLOCKS_64_LIST_CAPACITY];
struct ArrayPoolBlock_S blocks_32[BLOCKS_32_LIST_CAPACITY];

STATIC struct ArrayArena_S ArrayArena =
{
   .lists =
   {
      [ BLKS_1024 ] = { .blocks = blocks_1024, .len = (BLOCKS_1024_LIST_INIT_LEN - 1), .block_size = 1024 },
      [ BLKS_512  ] = { .blocks = blocks_512,  .len = (BLOCKS_512_LIST_INIT_LEN  - 1), .block_size = 512  },
      [ BLKS_256  ] = { .blocks = blocks_256,  .len = (BLOCKS_256_LIST_INIT_LEN  - 1), .block_size = 256  },
      [ BLKS_128  ] = { .blocks = blocks_128,  .len = (BLOCKS_128_LIST_INIT_LEN  - 1), .block_size = 128  },
      [ BLKS_64   ] = { .blocks = blocks_64,   .len = (BLOCKS_64_LIST_INIT_LEN   - 1), .block_size = 64   },
      [ BLKS_32   ] = { .blocks = blocks_32,   .len = (BLOCKS_32_LIST_INIT_LEN   - 1), .block_size = 32   }
   },
   .arena_initialized = false,
   .space_available = VEC_ARRAY_ARENA_SIZE - (VEC_ARRAY_ARENA_SIZE % 32)
};

/**
 * @brief Local helper function to find the block that corresponds to the pointer passed in.
 * 
 * @note The enum BlockSize * and size_t * parameters are optional and may be
 *       set to NULL if all the user cares about is if there exists a block that
 *       lives at the address passed in.
 * @param[in]  ptr     Address to look for among the allocatable blocks
 * @param[out] blk_sz  (Ptr) Enum for which block list the ptr belongs to (optional)
 * @param[out] blk_idx (Ptr) Idx within the block list that the ptr belongs to (optional)
 * @return true if successful in finding a block; false otherwise
 */
static bool Helper_FindBlock( const void *,
     /* Return Parameters */  enum BlockSize *, size_t * );

/**
 * @brief Initializes the static array pool arena structures.
 */
STATIC void StaticArrayPoolInit(void)
{
   // Initialize the pointers within the arena free lists, calculating an offset
   // into the ArrayArenaPool for each.
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
         assert( accumulating_offset < VEC_ARRAY_ARENA_SIZE );
         list->blocks[j].ptr = &ArrayArenaPool[ accumulating_offset ];
         list->blocks[j].is_free = true;
         accumulating_offset += list->block_size;
         assert( accumulating_offset <= VEC_ARRAY_ARENA_SIZE );
      }
   }

   ArrayArena.arena_initialized = true;
}

/**
 * Check that the static array pool is initialized.
 */
STATIC bool StaticArrayPoolIsInitialized(void)
{
   return ArrayArena.arena_initialized;
}

/**
 * @brief Allocates a contiguous block that can accomodate req_bytes from
 *        a static arena. Block size guaranteed to be ≥req_bytes.
 * @note Presently uses the "Buddy System" as described in:
 *          memorymanagement.org/mmref/alloc.html
 * @return Pointer to the allocated block if successful, NULL otherwise.
 */
STATIC void * StaticArrayAlloc(size_t req_bytes)
{
   assert( ArrayArena.arena_initialized );

   if ( req_bytes > ArrayArena.space_available )
   {
      // TODO: Return result type that specifies requested bytes was greater than space available
      return NULL;
   }

   #ifndef NDEBUG
   // Confirm assumption that the lists are sorted in order of descending block size
   uint16_t prev_sz = ArrayArena.lists[0].block_size;
   for ( uint8_t sz = 1; sz < (uint8_t)NUM_OF_BLOCK_SIZES; sz++ )
   {
      assert( ArrayArena.lists[sz].block_size > prev_sz );
      prev_sz = ArrayArena.lists[sz].block_size;
   }
   #endif

   void * block_ptr = NULL;
   size_t space_allocated = 0;

   // TODO: Accomodate block requests larger than 1024
   assert( req_bytes <= 1024 );
   // TODO: Reduce internal fragmentation /w block requests that are in between powers of 2
   // Start checking from the largest block sizes down
   for ( uint8_t sz = 0; sz < (uint8_t)NUM_OF_BLOCK_SIZES; sz++ )
   {
      // Skip until we find the nearest block size that accomodates the request
      if ( req_bytes < (ArrayArena.lists[sz].block_size / 2) ) continue;

      struct ArrayPoolBlockList_S * list = &ArrayArena.lists[sz];
      bool found_block = false;
      // Allocate from the beginning of the block list. This helps maintain
      // (but does not guarantee) a convenient descending order of block sizes,
      // which will make for more efficient allocating, freeing, splitting, and
      // coalescing.
      for ( size_t i = 0; i < list->len; i++ )
      {
         if ( list->blocks[i].is_free )
         {
            found_block = true;
            space_allocated = list->block_size;
            block_ptr = list->blocks[i].ptr;
            list->blocks[i].is_free = false;
         }
      }

      if ( (!found_block) && (sz != (uint8_t)BLKS_LARGEST_SIZE) )
      {
         // Look in the free lists of the larger block sizes
         // TODO: Loop through the block sizes up to the largest block size, not just the next size up.

         // Initially, the lists are of adjacent blocks, and the lists are organized
         // in descending order. So, the end of one list's large blocks is the start of
         // the next list's smaller blocks. If we split the block at the end, this
         // convenient descending order is maintained. This is also why I allocate
         // from the beginning of a list.
         struct ArrayPoolBlockList_S * larger_size_list = &ArrayArena.lists[sz + 1];
         for ( int i = (int)(larger_size_list->len - 1); i >= 0; i-- )
         {
            if ( !larger_size_list->blocks[i].is_free )  continue;

            // Split the larger block and assign its info to the smaller size free list
            larger_size_list->len--;
            list->blocks[list->len].ptr = larger_size_list->blocks[i].ptr;
            list->blocks[list->len + 1].ptr = (uint8_t *)larger_size_list->blocks[i].ptr + larger_size_list->block_size;
            list->blocks[list->len].is_free = false;
            list->blocks[list->len + 1].is_free = true;
            list->len += 2;
            block_ptr = list->blocks[list->len - 2].ptr;
            // Assign 
            found_block = true;
            space_allocated = list->block_size;
         }
      }

      break;
   }

   ArrayArena.space_available -= space_allocated;
   return block_ptr;
}

STATIC void * StaticArrayRealloc(void * ptr, size_t req_bytes)
{
   enum BlockSize old_blk_sz;
   size_t old_blk_idx;
   bool old_blk_found = Helper_FindBlock( ptr, &old_blk_sz, &old_blk_idx );

   if ( !old_blk_found )
   {
      // TODO: Raise exception that user tried to realloc an unallocated block
      return NULL;
   }
   else if ( ArrayArena.lists[old_blk_sz].blocks[old_blk_idx].is_free )
   {
      // TODO: Raise exception that user tried to realloc a block that was free
      return NULL;
   }
   else if ( req_bytes >  (ArrayArena.lists[old_blk_sz].block_size / 2) &&
             req_bytes <= ArrayArena.lists[old_blk_sz].block_size )
   {
      // Not much point in reallocating if the size is the best fit.
      // TODO: Accomodate realloc size requests in between powers of 2 using combo of block sizes
      return ptr;
   }
   else if ( 0 == req_bytes )
   {
      StaticArrayFree( ptr );
      return NULL;
   }

   void * tmp = StaticArrayAlloc( req_bytes );
   if ( tmp != NULL )
   {
      size_t old_blk_size = BlockSize_E_to_Int[old_blk_sz];
      size_t num_of_bytes = (req_bytes > old_blk_size) ? old_blk_size : req_bytes;
      memcpy( tmp, ptr, num_of_bytes );
      StaticArrayFree( ptr );
      return tmp;
   }

   // Must not have been able to allocate new block...
   // TODO: Need a better way to express that we failed to realloc. Probably add void ** new_ptr param and return bool/exception/result type.
   return ptr;
}

/**
 * @brief Free the block at the address passed in, if applicable.
 * @note If the address passed in is not one that a block lives at, the fcn simply returns.
 * @param[in] Address of block to free
 */
STATIC void StaticArrayFree(const void * ptr)
{
   enum BlockSize blk_sz;
   size_t blk_idx;
   bool blk_found = Helper_FindBlock( ptr, &blk_sz, &blk_idx );

   if ( !blk_found ) return;  // TODO: Raise exception that user tried to free an unallocated block?

   ArrayArena.lists[blk_sz].blocks[blk_idx].is_free = true;
   ArrayArena.space_available += BlockSize_E_to_Int[blk_sz];
}

/**
 * @brief Determine whether an address is associated with a block that is allocated.
 */
STATIC bool StaticArrayIsAlloc(const void * ptr)
{
   enum BlockSize blk_sz;
   size_t blk_idx;
   bool blk_found = Helper_FindBlock( ptr, &blk_sz, &blk_idx );

   if ( !blk_found ) return false;

   return !ArrayArena.lists[blk_sz].blocks[blk_idx].is_free;
}

/* Static Array Allocator Helper Implementations */

static bool Helper_FindBlock( const void * ptr,
                              enum BlockSize * blk_sz, size_t * blk_idx )
{
   assert( ArrayArena.arena_initialized );

   if ( NULL == ptr )   return false;

#ifndef NDEBUG
   bool blk_found = false;
#endif
   for ( uint8_t sz = 0; sz < (uint8_t)NUM_OF_BLOCK_SIZES; sz++ )
   {
      const struct ArrayPoolBlockList_S * list = &ArrayArena.lists[sz];
      for ( size_t i = 0; i < list->len; i++ )
      {
         // 🗒: Should I allow for an address _inside_ a block?
         //     (ptr >= list->blocks[i].ptr) && (ptr < list->blocks[i+1].ptr)
         if ( list->blocks[i].ptr == ptr )
         {
            if ( blk_sz != NULL ) *blk_sz = (enum BlockSize)sz;
            if ( blk_idx != NULL ) *blk_idx = i;

#ifndef NDEBUG
            // Assert that this is the only block that has this address. For
            // the debug build, keep searching to make sure this assertion holds.
            // We should only enter this if statement once, so blk_found should
            // still be false.
            assert(!blk_found);
            blk_found = true;
#else
            return true;
#endif

         }
      }
   }

#ifndef NDEBUG
   return blk_found;
#else
   return false;
#endif
}

#ifdef ARRAY_ARENA_VIZ

// TODO: Implement arena viz API

#endif // ARRAY_ARENA_VIZ

#endif // VEC_USE_BUILT_IN_STATIC_ALLOC

