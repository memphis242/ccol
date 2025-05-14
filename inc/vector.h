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
size_t VectorLength( const struct Vector_S * self );

/**
 * @brief Retrieves the current capacity of the vector.
 *
 * Returns the total number of elements the vector can hold without resizing.
 *
 * @param self Vector handle
 * 
 * @return The current capacity of the vector.
 */
size_t VectorCapacity( const struct Vector_S * self );

/**
 * @brief Retrieves the maximum capacity of the vector.
 *
 * Returns the maximum number of elements the vector can ever hold.
 *
 * @param self Vector handle
 * 
 * @return The maximum capacity of the vector.
 */
size_t VectorMaxCapacity( const struct Vector_S * self );

/**
 * @brief Retrieves the size of each element in the vector.
 *
 * Returns the size (in bytes) of each element stored in the vector.
 *
 * @param self Vector handle
 * 
 * @return The size of each element in the vector.
 */
size_t VectorElementSize( const struct Vector_S * self );

/**
 * @brief Checks if the vector is empty.
 * 
 * @param self Vector handle
 * 
 * @return true if empty, false otherwise.
 */
bool VectorIsEmpty( const struct Vector_S * self );

/**
 * @brief Checks if the vector is full - i.e., at max capacity.
 * 
 * @param self Vector handle
 * 
 * @return true if full, false otherwise.
 */
bool VectorIsFull( const struct Vector_S * self );

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
void * VectorGetElementAt( const struct Vector_S * self, size_t idx );

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
void * VectorLastElement( const struct Vector_S * self );

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
bool VectorCpyElementAt( const struct Vector_S * self, size_t idx, void * data );

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
bool VectorCpyLastElement( const struct Vector_S * self, void * data );

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
 * @brief Resets the vector length to zero but leaves the previously allocated
 *        memory for the vector's data untouched.
 *
 * @param self Vector handle
 */
bool VectorReset( struct Vector_S * self );

/**
 * @brief Sets all elements to 0, resets the vector length to zero, and frees
 *        the memory allocated for the vector's data.
 *
 * This might be more appropriate for privacy/security purposes (e.g., not
 * leaving behind session data).
 *
 * @param self Vector handle
 */
bool VectorHardReset( struct Vector_S * self );

/**
 * @brief Duplicates the vector passed in and returns a pointer to the result.
 * 
 * @note Performs a deep copy - i.e., the underlying data is also copied over
 *       to a new location, so the returned object has no association to the
 *       original.
 *
 * @note Memory is dynamically allocated for this duplicatation. The user is
 *       responsible for free'ing this later (e.g., by calling VectorFree).
 *
 * @param self Vector handle
 */
struct Vector_S * VectorDuplicate( const struct Vector_S * self );

/**
 * @brief Checks if two vectors are equivalent element-wise, length-wise,
 *        and capacity-wise (including max capacity).
 *
 * @param a Vector handle for the first vector
 * @param b Vector handle for the second vector
 */
bool VectorsAreEqual( const struct Vector_S * a, const struct Vector_S * b );


/* Sub-Range Based Vector Operations */

/**
 * @brief Splits a vector at the specified index into two separate vectors.
 *
 * This function creates a new vector containing the elements from the specified
 * index to the end of the original vector. The original vector's length
 * is truncated accordingly, and thus _is_ mutated.
 * 
 * @note Passing in an idx of 0 results no action being taken and the returned
 *       pointer to be NULL.
 * @note This will mutate the original vector.
 *
 * @param self Pointer to the original vector to be split.
 * @param idx The index at which to split the vector. Elements from this index
 *            onward will be moved to the new vector.
 * @return Pointer to the newly created vector containing the split elements,
 *         or NULL if the operation fails (e.g., invalid index or memory
 *         allocation error).
 */
struct Vector_S * VectorSplitAt( struct Vector_S * self, size_t idx );

/**
 * @brief Creates a slice (subvector) from the given vector.
 *
 * Returns a new vector that contains elements from the original vector
 * starting at index `idx_start` (inclusive) and ending at `idx_end` (inclusive).
 * VectorSplit would behave somewhat similarly if idx_end was the end of the
 * original vector, however VectorSplit does _mutate_ the original vector,
 * whereas this function does not mutate the original vector. The original vector
 * remains an invariant through this function.
 *
 * @param self Pointer to the original Vector_S structure.
 * @param idx_start The starting index of the slice (inclusive).
 * @param idx_end The ending index of the slice (inclusive).
 * @return Pointer to a new Vector_S containing the specified slice,
 *         or NULL if the indices are invalid or memory allocation fails.
 */
struct Vector_S * VectorSlice( const struct Vector_S * self,
                               size_t idx_start,
                               size_t idx_end );

/**
 * @brief Concatenates the contents of one vector onto another.
 *
 * This function appends all elements from the vector pointed to by
 * `vec_to_append` to the end of the vector pointed to by `vec_to_append_onto`.
 * 
 * @note The vec_to_append is not mutated or free'd.
 * @note The vec_to_append_onto is expanded if the length of the other vector
 *       is too long.
 *
 * @param vec_to_append_onto Destination vector to which elements will be appended
 * @param vec_to_append Source vector whose elements will be appended.
 * @return true if the concatenation was successful,
 *         false otherwise (e.g., on allocation failure).
 */
bool VectorConcatenate( struct Vector_S * vec_to_append_onto,
                        const struct Vector_S * vec_to_append );

bool VectorSubRange_PushElements( struct Vector_S * self,
                                  size_t len,
                                  void * data );

bool VectorSubRange_InsertElementsAt( struct Vector_S * self,
                                      size_t idx,
                                      size_t len,
                                      void * data );

/**
 * @brief Retrieves a range of elements from the vector.
 *
 * Returns a pointer to an array of elements in the specified range within the
 * vector, starting at the idx provided.
 * 
 * @note The returned array directly references the underlying data structure!
 *       The caller must be aware that modifications done on the returned array
 *       affect the vector directly.
 * @note The caller must also handle the bounds of the array based on knowledge
 *       of the vector's length.
 * @note The memory underlying the vector is assumed to be contiguous, so getting
 *       a pointer to any index basically gives you access to all indices.
 *
 * @param self Pointer to the vector structure.
 * @param idx_start The starting index of the range (inclusive).
 * @return A pointer to the array of elements, or NULL if the inputs are invalid
 *         or the vector was empty.
 */
void * VectorSubRange_GetElementsFromIdx( const struct Vector_S * self,
                                          size_t idx );

/**
 * @brief Copies elements from a specified range in the vector to a provided buffer.
 * 
 * @note Part of the idea of using this is to either not mutate the vector's
 *       data or to use VectorSubRange_CpyElementsInRange() followed by
 *       VectorSubRange_SetElementsInRange() with the same range.
 * 
 * @note Passing in a start and end index that are the same is equivalent to
 *       VectorCpyElementAt that index.
 *
 * @param self Vector handle.
 * @param idx_start The starting index of the range (inclusive).
 * @param idx_end The ending index of the range (inclusive).
 * @param buffer Pointer to the destination buffer where the elements will be copied.
 *               The buffer must be large enough to hold all elements in the specified range.
 * 
 * @return true if the elements were successfully copied;
 *         false otherwise (e.g., if indices are out of bounds).
 */
bool VectorSubRange_CpyElementsInRange( const struct Vector_S * self,
                                        size_t idx_start,
                                        size_t idx_end,
                                        void * buffer );

/**
 * @brief Copies elements from the start of the vector to the specified index into a buffer.
 *
 * @param self Vector handle.
 * @param idx The index up to which elements will be copied (inclusive).
 * @param buffer Pointer to the destination buffer where the elements will be copied.
 *               The buffer must be large enough to hold the copied elements.
 * 
 * @return true if the operation is successful,
 *         false otherwise (e.g., if idx is out of bounds).
 */
bool VectorSubRange_CpyElementsFromStartToIdx( const struct Vector_S * self,
                                               size_t idx,
                                               void * buffer );

/**
 * @brief Copies elements from the specified index to the end of the vector into a provided buffer.
 *
 * @param self Vector handle.
 * @param idx The starting index from which elements will be copied (inclusive).
 * @param buffer Pointer to the destination buffer where the elements will be copied.
 *               The buffer must be large enough to hold all elements from idx to the end.
 * 
 * @return true if the operation is successful,
 *         false otherwise (e.g., if idx is out of bounds)
 */
bool VectorSubRange_CpyElementsFromIdxToEnd( const struct Vector_S * self,
                                             size_t idx,
                                             void * buffer );

/**
 * @brief Sets the elements of a vector within a specified range to the given data.
 *
 * This function updates the elements of the vector from the index `idx_start`
 * to `idx_end` (inclusive) with the value pointed to by `data`.
 *
 * @param self Vector handle.
 * @param idx_start The starting index of the range (inclusive).
 * @param idx_end The ending index of the range (inclusive).
 * @param data Pointer to the data to set for the specified range.
 * @return true if the operation is successful, false otherwise (e.g., if the
 *         indices are out of bounds or the vector is NULL).
 */
bool VectorSubRange_SetElementsInRange( struct Vector_S * self,
                                        size_t idx_start,
                                        size_t idx_end,
                                        const void * data );

/**
 * @brief Sets the elements of the vector from the start up to the specified index.
 *
 * This function updates the elements of the vector from the beginning (index 0)
 * up to the specified index `idx` with the provided data. The data is applied
 * to all elements in the specified range.
 *
 * @param self Vector handle.
 * @param idx The index up to which the elements will be set (inclusive).
 * @param data Pointer to the data to be set for the elements.
 * @return true if the operation is successful, false otherwise.
 */
bool VectorSubRange_SetElementsFromStartToIdx( struct Vector_S * self,
                                               size_t idx,
                                               const void * data );

/**
 * @brief Sets the elements of the vector from the specified index to the end.
 *
 * This function updates the elements of the vector starting from the given
 * index `idx` to the end of the vector with the provided data. The behavior
 * of this function depends on the implementation of the `Vector_S` structure.
 *
 * @param self Vector handle.
 * @param idx The starting index from which elements will be updated.
 * @param data Pointer to the data to be set in the vector elements.
 * @return `true` if the operation is successful, `false` otherwise.
 */
bool VectorSubRange_SetElementsFromIdxToEnd( struct Vector_S * self,
                                             size_t idx,
                                             const void * data );

/**
 * @brief Removes elements from the vector within the specified range.
 *
 * This function removes elements from the vector starting from the index 
 * `idx_start` up to, and including, the index `idx_end`. The indices 
 * must be within the bounds of the vector, and `idx_start` must be less 
 * than `idx_end`.
 *
 * @param self Vector handle.
 * @param idx_start The starting index of the range (inclusive).
 * @param idx_end The ending index of the range (inclusive).
 * @return true if the elements were successfully removed, false otherwise.
 */
bool VectorSubRange_RemoveElementsInRange( struct Vector_S * self,
                                           size_t idx_start,
                                           size_t idx_end );

bool VectorSubRange_RemoveElementsFromStartToIdx( struct Vector_S * self,
                                                  size_t idx,
                                                  const void * data );

bool VectorSubRange_RemoveElementsFromIdxToEnd( struct Vector_S * self,
                                                size_t idx,
                                                const void * data );

bool VectorSubRange_ClearElementsInRange( struct Vector_S * self,
                                          size_t idx_start,
                                          size_t idx_end );

bool VectorSubRange_ClearElementsFromStartToIdx( struct Vector_S * self,
                                                 size_t idx,
                                                 const void * data );

bool VectorSubRange_ClearElementsFromIdxToEnd( struct Vector_S * self,
                                               size_t idx,
                                               const void * data );
