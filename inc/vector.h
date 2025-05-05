/**
 * @file vector.c
 * @brief API for a dynamic array (vector) in C.
 *
 * @author Abdulla Almosalami (memphis242)
 * @date April 12, 2025
 * @copyright MIT License
 */

/* File Inclusions */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
// TODO: Exceptions
// TODO: Support custom allocators passed in

/* Public Macro Definitions */

/* Public Datatypes */

// Opaque type to act as a handle for the user to pass into the API
struct Vector_S;

/* Public API */

/**
 * @brief Constructor for the vector object
 *
 * Allocates memory for a dynamic array (vector) and initializes its properties.
 *
 * @param element_size The size of each element in the vector (in bytes).
 * @param initial_capacity The initial number of elements the vector can hold.
 * @param max_capacity The maximum number of elements the vector can hold ever.
 *                     Note that if max_capacity is larger than an internal limit
 *                     set within the implementation, that internal limit will be
 *                     used instead.
 * @param initial_len  Initial length of the vector that allows the user to get
 *                     some zero elements in to begin with. 0 if user will handle
 *                     manual push/insertion operations to initial fill the vec.
 * 
 * @todo Exceptions
 *
 * @return A pointer to the initialized vector, or NULL if allocation fails.
 */
struct Vector_S * VectorInit( size_t element_size,
                              size_t initial_capacity,
                              size_t max_capacity,
                              size_t initial_len );

/**
 * @brief Frees the memory allocated for the vector.
 *
 * Releases all resources associated with the vector, including its internal data.
 *
 * @param self Vector handle
 */
void VectorFree( struct Vector_S * self );

/**
 * @brief Retrieves the number of elements in the vector.
 *
 * Returns the current number of elements stored in the vector.
 *
 * @param self Vector handle
 * 
 * @return The number of elements in the vector.
 */
size_t VectorLength( struct Vector_S * self );

/**
 * @brief Retrieves the current capacity of the vector.
 *
 * Returns the total number of elements the vector can hold without resizing.
 *
 * @param self Vector handle
 * 
 * @return The current capacity of the vector.
 */
size_t VectorCapacity( struct Vector_S * self );

/**
 * @brief Retrieves the maximum capacity of the vector.
 *
 * Returns the maximum number of elements the vector can ever hold.
 *
 * @param self Vector handle
 * 
 * @return The maximum capacity of the vector.
 */
size_t VectorMaxCapacity( struct Vector_S * self );

/**
 * @brief Retrieves the size of each element in the vector.
 *
 * Returns the size (in bytes) of each element stored in the vector.
 *
 * @param self Vector handle
 * 
 * @return The size of each element in the vector.
 */
size_t VectorElementSize( struct Vector_S * self );

/**
 * @brief Checks if the vector is empty.
 * 
 * @param self Vector handle
 * 
 * @return true if empty, false otherwise.
 */
bool VectorIsEmpty( struct Vector_S * self );

/**
 * @brief Checks if the vector is full - i.e., at max capacity.
 * 
 * @param self Vector handle
 * 
 * @return true if full, false otherwise.
 */
bool VectorIsFull( struct Vector_S * self );

/**
 * @brief Inserts an element at the _end_ of the vector.
 *
 * @param self Vector handle
 * @param element A pointer to the element to be inserted.
 *
 * @return `true` if the insertion is successful, `false` otherwise.
 *         (e.g., memory allocation fails).
 */
bool VectorPush( struct Vector_S * self, const void * element );

/**
 * @brief Inserts an element into the vector at the specified index.
 * 
 * This function inserts the given element into the vector at the specified
 * index, shifting all subsequent elements one position to the right.
 *
 * @note You are not allowed to insert past the length of the vector.
 *  
 * @param self Vector handle
 * @param idx The index at which the element should be inserted.
 * @param element Pointer to the element to be inserted.
 * 
 * @return true if the element was successfully inserted, false otherwise
 *         (e.g., memory allocation fails).
 */
bool VectorInsertAt( struct Vector_S * self,
                     size_t idx,
                     const void * element );

/**
 * @brief Retrieves a pointer to the element at the specified index in the vector.
 *
 * Provides direct access to the element at the given index _without_ removing it.
 * The returned pointer points to the internal memory of the vector, and the user
 * needs to be aware of the risks in working with pointers to opaque data. For
 * example, reallocations that may occur due to vector insertions or deletions
 * could render the pointers stale.
 *
 * @param self Vector handle
 * @param idx The index of the element to retrieve, 0-indexed.
 * 
 * @return A pointer to the element if the retrieval was successful, or NULL if
 *         something is off.
 */
void * VectorGetElementAt( struct Vector_S * self, size_t idx );

/**
* @brief Copies the element at the specified index in the vector to a provided buffer.
*
* Copies the element at the given index into the provided data buffer. This ensures
* that any internal reallocations of the vector do not lead to unintended stale pointers.
*
* @param self Vector handle
* @param idx The index of the element to retrieve, 0-indexed.
* @param data Pointer to the buffer where the element will be copied.
*             The buffer must be large enough to hold the element.
* 
* @return true if the retrieval and copy were successful, false otherwise
*         (e.g., if the index is out of bounds or data is NULL).
*/
bool VectorCpyElementAt( struct Vector_S * self, size_t idx, void * data );

/**
 * @brief Sets the value of an element at the specified index in the vector.
 *
 * Updates the element at the given index with the provided value.
 *
 * @param self Vector handle
 * @param idx The index of the element to update, 0-indexed.
 * @param element A pointer to the new value to set at the specified index.
 *                The memory pointed to must not overlap with the vector's memory.
 * 
 * @return true if the update was successful, false otherwise
 */
bool VectorSetElementAt( struct Vector_S * self,
                           size_t idx,
                           const void * element );

/**
 * @brief Removes an element from the vector at the specified index.
 *
 * Deletes the element at the given index and shifts subsequent elements to fill
 * the gap. If the `data` pointer is not NULL, the data of the removed element
 * is copied to the memory location pointed to by `data`.
 * 
 * @note No realloc is done to release memory.
 * @todo TODO: Determine when it makes sense to realloc to release memory...
 *
 * @param self Vector handle
 * @param idx The index of the element to be removed, 0-indexed.
 * @param data Pointer to a memory location where the data of the removed element
 *             will be copied. If NULL, the data is discarded.
 * 
 * @return true if the element was removed, false otherwise
 */
bool VectorRemoveElementAt( struct Vector_S * self, size_t idx, void * data );

/**
 * @brief Removes the last element from the vector and optionally retrieves its data.
 *
 * This function removes the last element from the vector. If the `data` pointer
 * is not NULL, the data of the removed element is copied to the memory location
 * pointed to by `data`.
 *
 * @param self Vector handle
 * @param data Pointer to a memory location where the data of the removed element
 *             will be copied. If NULL, the data is discarded.
 * @return true if the last element was successfully removed, false if the vector
 *         is empty or an error occurred.
 */
bool VectorRemoveLastElement( struct Vector_S * self, void * data );

/**
 * @brief Clears the element at the specified index in the vector.
 *
 * This function sets the element at the given index to a cleared state,
 * effectively removing its value while maintaining the vector's structure.
 * Use instead of VectorRemoveElementAt if you don't wish to shift and fill-in
 * the gap.
 *
 * @param self Vector handle.
 * @param idx Index of the element to clear.
 * @return true if the element was successfully cleared, false otherwise
 *         (e.g., if the index is out of bounds).
 */
bool VectorClearElementAt( struct Vector_S * self, size_t idx );

/**
 * @brief Retrieves the last element in the vector.
 *
 * Provides access to the last element in the vector without removing it.
 * Also, performs an internal memcpy() to the provided data buffer so that
 * any internal realloc()'s don't lead to unintended stale pointers.
 *
 * @param self Vector handle
 * @param data Pointer to the data that will be returned.
 * 
 * @return true if the last element was successfully retrieved and placed in data,
 *         false otherwise
 */
void * VectorLastElement( struct Vector_S * self );

/**
 * @brief Copies the last element in the vector to a provided buffer.
 *
 * Copies the last element in the vector into the provided data buffer. This ensures
 * that any internal reallocations of the vector do not lead to unintended stale pointers.
 *
 * @param self Vector handle
 * @param data Pointer to the buffer where the last element will be copied.
 *             The buffer must be large enough to hold the element.
 * 
 * @return true if the last element was successfully copied to the buffer, false otherwise
 *         (e.g., if the vector is empty or data is NULL).
 */
bool VectorCpyLastElement( struct Vector_S * self, void * data );

/**
 * @brief Resets the vector length to zero, thereby losing track of old data.
 *
 * @note The allocated memory is not freed, allowing the vector to be reused.
 *
 * @param self Vector handle
 */
bool VectorClear( struct Vector_S * self );

/**
 * @brief Sets all elements to 0 and resets the vector length to zero.
 *
 * Similar to VectorClear except all elements that were previously added will
 * be memset to 0. This helps prevent old data from remaining in memory past
 * this point (better for security/privacy purposes).
 *
 * @param self Vector handle
 */
bool VectorHardReset( struct Vector_S * self );

/**
 * @brief Duplicates the vector passed in and returns a pointer to the result.
 *
 * @param self Vector handle
 */
struct Vector_S * VectorDuplicate( struct Vector_S * self );

/**
 * @brief Checks if two vectors are equivalent element-wise, length-wise,
 *        and capacity-wise (including max capacity).
 *
 * @param a Vector handle for the first vector
 * @param b Vector handle for the second vector
 */
bool VectorsAreEqual( struct Vector_S * a, struct Vector_S * b );


/* Sub-Range Based Vector Operations */

void * VectorGetElementsInRange( struct Vector_S * self,
                                 size_t idx_start,
                                 size_t idx_end );

bool VectorCpyElementsInRange( struct Vector_S * self,
                               size_t idx_start,
                               size_t idx_end,
                               void * buffer );

void * VectorGetElementsFromStartToIdx( struct Vector_S * self,
                                        size_t idx );

bool VectorCpyElementsFromStartToIdx( struct Vector_S * self,
                                      size_t idx,
                                      void * buffer );

void * VectorGetElementsFromIdxToEnd( struct Vector_S * self,
                                      size_t idx );

bool VectorCpyElementsFromIdxToEnd( struct Vector_S * self,
                                    size_t idx,
                                    void * buffer );

bool VectorSetElementsInRange( struct Vector_S * self,
                               size_t idx_start,
                               size_t idx_end,
                               const void * data );

bool VectorSetElementsFromStartToIdx( struct Vector_S * self,
                                      size_t idx,
                                      const void * data );

bool VectorSetElementsFromIdxToEnd( struct Vector_S * self,
                                    size_t idx,
                                    const void * data );

bool VectorRemoveElementsInRange( struct Vector_S * self,
                                  size_t idx_start,
                                  size_t idx_end );

// TODO: Support deep copies?
