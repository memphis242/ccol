/**
 * @file heap.h
 * @brief Binary heap
 *
 * This file defines the API of this collection's binary heap, which supports
 * the basic heap API one might expect elsewhere.
 * Heaps are generally useful as priority queues or for heap sorting, from which
 * more child functionalities are born.
 *
 * @author Abdulla Almosalami (@memphis242)
 * @date April 9, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Public Datatypes */

// Opaque type for a user to hold a handle to a heap to pass to the API
struct Heap_S;

/* Public API */

/**
 * @brief Constructor for the heap object.
 *
 * Allocates memory for a binary heap and initializes its properties.
 *
 * @param cmp A comparison function to maintain the heap property.
 *            Should return a negative value if `a` has higher priority than `b`,
 *            zero if they are equal, and a positive value otherwise.
 * @param element_size The size of each element in the heap (in bytes).
 * 
 * @return A pointer to the initialized heap, or NULL if allocation fails.
 */
struct Heap_S * HeapInit( int (* cmp)(const void * a, const void * b),
                          size_t element_size );

// bool HeapifyArray( const void * array, size_t length, size_t size_of_member, int (* comp)(const void * a, const void * b) );
// bool HeapInsert( const void * element );