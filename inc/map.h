/**
 * @file map.h
 * @brief A datastructure to simply store mapping between two sets.
 *
 * @author Abdulla Almosalami [@memphis242]
 * @date April 13, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Public Datatypes */

// Opaque type for a user to hold a handle to a map to pass to the API
struct Map_S;

/* Public API */

/**
 * @brief Initializes the map object.
 *
 * Prepares the map for use by allocating necessary resources and setting up
 * internal structures.
 *
 * @param self A pointer to the map object to initialize.
 * @param a A pointer to the first parameter (e.g., key type or configuration).
 * @param b A pointer to the second parameter (e.g., value type or configuration).
 * 
 * @return `true` if initialization is successful, `false` otherwise.
 */
bool MapInit( struct Map_S * self,
                    const void * a, const void * b );

/**
 * @brief Inserts a key-value pair into the map.
 *
 * Adds a new key-value pair to the map. If the key already exists, its value
 * is updated.
 *
 * @param self A pointer to the map object.
 * @param a A pointer to the key.
 * @param b A pointer to the value.
 * 
 * @return `true` if the insertion is successful, `false` otherwise.
 */
bool MapInsert( struct Map_S * self,
                const void * a, const void * b );
