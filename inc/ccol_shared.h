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

/* Public Macro Definitions */

/* Public Datatypes */

struct Allocator
{
   void * (*alloc)(size_t);
   void * (*realloc)(void *, size_t);
   void   (*reclaim)(void *);
};

#endif // CCOL_SHARED_H
