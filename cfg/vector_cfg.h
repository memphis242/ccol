/**
 * @file vector_cfg.c
 * @brief Configuration of aspects of the vector implementation.
 *
 * @author Abdulla Almosalami (memphis242)
 * @date June 10, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Public Macro Definitions */

// Custom malloc configuration macros
#define VEC_USE_STATIC_MALLOC             100000
#define VEC_MALLOC_STATIC_MAX_ARENA_SIZE  100000
