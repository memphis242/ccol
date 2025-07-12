#ifndef VECTOR_VIZABLE_EXAMPLE_H
#define VECTOR_VIZABLE_EXAMPLE_H

#include "vizable_vtable.h"
#include "../inc/vector.h"

// Forward declaration for arena viz
struct ArenaVizList;

/**
 * @brief Enhanced Vector structure with vizable support
 * 
 * This shows how you could modify your existing Vector struct
 * to support the vizable interface using the vtable approach.
 */
struct VizableVector {
    VIZABLE_HEADER;  // Embeds struct VizableBase vizable_base
    
    // Your existing vector fields would go here
    // (This is just an example structure)
    void* data;
    size_t element_size;
    size_t length;
    size_t capacity;
    size_t max_capacity;
};

/**
 * @brief Vector-specific vizable function implementations
 */
size_t Vector_GetArenaLayout(const void* self, struct ArenaVizList* viz_list, size_t max_entries);
size_t Vector_GetArenaSize(const void* self);
size_t Vector_GetElementCount(const void* self);
size_t Vector_GetElementSize(const void* self);
bool Vector_IsEmpty(const void* self);
void Vector_Destroy(void* self);

/**
 * @brief Vector vtable declaration
 */
DECLARE_VIZABLE_VTABLE(VizableVector);

/**
 * @brief Constructor for vizable vector
 */
struct VizableVector* VizableVector_Create(size_t element_size, size_t initial_capacity);

/**
 * @brief Example usage functions
 */
void Example_VizableUsage(void);

#endif // VECTOR_VIZABLE_EXAMPLE_H
