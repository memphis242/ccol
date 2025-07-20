/**
 * @file ccol_shared.h
 * @brief Shared macros and datatypes across the ccol library.
 *
 * @author Abdulla Almosalami (memphis242)
 * @date Jul 16, 2025
 * @copyright MIT License
 */

#ifndef CCOL_SHARED_H
#define CCOL_SHARED_H

/* File Inclusions */
#include <stdint.h>

/* Public Macro Definitions */
#define DEFAULT_ALLOCATOR              \
(                                      \
 (struct Allocator){                   \
   .alloc = default_alloc,             \
   .realloc = default_realloc,         \
   .reclaim = default_reclaim,         \
   .arena = NULL                       \
 }                                     \
)

#define DONTCARE (0)

/* Public Datatypes */

struct Allocator
{
   void * (*alloc)(size_t req_sz, void * arena);
   void * (*realloc)(void * old_ptr, size_t new_sz, size_t old_sz);
   void   (*reclaim)(void * old_ptr, size_t old_sz);
   void * arena;
};

/* Public Functions */

// These will default to the stdlib memory fcns and not utilize all possible parameters.
// Use these if you don't want to provide your own allocator.
void * default_alloc(size_t req_sz, void *);
void * default_realloc(void * old_ptr, size_t new_sz, size_t);
void   default_reclaim(void * old_ptr, size_t);

#endif // CCOL_SHARED_H
