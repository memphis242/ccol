#include "vector_vizable_example.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Vector vtable implementation
 */
const struct VizableVTable VizableVector_VTable = {
    .type_name = "Vector",
    .get_arena_layout = Vector_GetArenaLayout,
    .get_arena_size = Vector_GetArenaSize,
    .get_element_count = Vector_GetElementCount,
    .get_element_size = Vector_GetElementSize,
    .is_empty = Vector_IsEmpty,
    .destroy = Vector_Destroy
};

/**
 * @brief Implementation of Vector's vizable methods
 */
size_t Vector_GetArenaLayout(const void* self, struct ArenaVizList* viz_list, size_t max_entries) {
    const struct VizableVector* vec = (const struct VizableVector*)self;
    if (vec == NULL || viz_list == NULL) {
        return 0;
    }
    
    // TODO: Implement actual arena layout logic
    // This would populate viz_list with information about memory layout
    // For now, return a placeholder
    (void)max_entries; // Suppress unused parameter warning
    return 1; // Number of entries added to viz_list
}

size_t Vector_GetArenaSize(const void* self) {
    const struct VizableVector* vec = (const struct VizableVector*)self;
    if (vec == NULL) {
        return 0;
    }
    
    // Return total memory used by this vector
    return sizeof(struct VizableVector) + (vec->capacity * vec->element_size);
}

size_t Vector_GetElementCount(const void* self) {
    const struct VizableVector* vec = (const struct VizableVector*)self;
    if (vec == NULL) {
        return 0;
    }
    
    return vec->length;
}

size_t Vector_GetElementSize(const void* self) {
    const struct VizableVector* vec = (const struct VizableVector*)self;
    if (vec == NULL) {
        return 0;
    }
    
    return vec->element_size;
}

bool Vector_IsEmpty(const void* self) {
    const struct VizableVector* vec = (const struct VizableVector*)self;
    if (vec == NULL) {
        return true;
    }
    
    return vec->length == 0;
}

void Vector_Destroy(void* self) {
    struct VizableVector* vec = (struct VizableVector*)self;
    if (vec == NULL) {
        return;
    }
    
    if (vec->data != NULL) {
        free(vec->data);
    }
    free(vec);
}

/**
 * @brief Constructor implementation
 */
struct VizableVector* VizableVector_Create(size_t element_size, size_t initial_capacity) {
    struct VizableVector* vec = malloc(sizeof(struct VizableVector));
    if (vec == NULL) {
        return NULL;
    }
    
    // Initialize the vizable base
    VizableVector_InitVizable(vec);
    
    // Initialize vector fields
    vec->element_size = element_size;
    vec->length = 0;
    vec->capacity = initial_capacity;
    vec->max_capacity = initial_capacity * 10; // Example
    
    vec->data = malloc(element_size * initial_capacity);
    if (vec->data == NULL) {
        free(vec);
        return NULL;
    }
    
    return vec;
}

/**
 * @brief Example usage demonstration
 */
void Example_VizableUsage(void) {
    // Create a vizable vector
    struct VizableVector* vec = VizableVector_Create(sizeof(int), 10);
    if (vec == NULL) {
        return;
    }
    
    // Use the vizable interface
    size_t arena_size = VIZABLE_GET_ARENA_SIZE(vec);
    size_t element_count = VIZABLE_GET_ELEMENT_COUNT(vec);
    const char* type_name = VIZABLE_GET_TYPE_NAME(vec);
    
    // Example: Pass to a generic function that works with any vizable object
    // ProcessVizableObject(AS_VIZABLE(vec));
    
    // Clean up
    VIZABLE_CALL(vec, destroy);
    
    // Suppress unused variable warnings for this example
    (void)arena_size;
    (void)element_count;
    (void)type_name;
}
