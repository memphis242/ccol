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

//! Uncomment to set a maximum vector length for any vector
#ifndef MAX_VEC_LEN // Define at compile-command time if desired
#define MAX_VEC_LEN 1000
#endif // MAX_VEC_LEN

//! Specify the size of the `struct Vector` pool which is internally used to deploy vector objects
#define VEC_STRUCT_POOL_SIZE  25

