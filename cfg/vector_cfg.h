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

//! Uncomment to enable the ability to initialize a vector /w a custom allocator
//#define VEC_USE_CUSTOM_ALLOC

#ifndef VEC_USE_CUSTOM_ALLOC
//! Uncomment to enable the built-in static arena for the built-in arena allocator
//#define VEC_USE_BUILT_IN_STATIC_ALLOC

//! Specify the static arena allocator's arena size in bytes
#define VEC_BUILT_IN_STATIC_ALLOC_ARENA_SIZE  1000
#endif
