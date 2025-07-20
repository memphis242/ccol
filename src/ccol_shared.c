/**
 * @file ccol_shared.c
 * @brief Definition of common objects.
 *
 * @author Abdulla Almosalami (memphis242)
 * @date Jul 19, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stdlib.h>
#include "ccol_shared.h"

/* Public Function Definitions */

void * default_alloc(size_t req_sz, void * arena)
{
   (void)arena;
   return malloc(req_sz);
}

void * default_realloc(void * old_ptr, size_t new_sz, size_t old_sz)
{
   (void)old_sz;
   return realloc(old_ptr, new_sz);
}

void default_reclaim(void * old_ptr, size_t old_sz)
{
   (void)old_sz;
   free(old_ptr);
}
