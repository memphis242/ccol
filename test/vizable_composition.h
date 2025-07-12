#ifndef VIZABLE_COMPOSITION_H
#define VIZABLE_COMPOSITION_H

#include <stdint.h>
#include <stdbool.h>

// Forward declaration for arena viz (assuming it exists)
struct ArenaVizList;

/**
 * @brief Vizable interface using composition
 * 
 * Each data structure that wants to be vizable embeds this struct
 * and initializes the function pointers to point to their implementations.
 */
struct Vizable {
    // Function pointers for required methods
    size_t (*GetArenaLayout)(const void* self, struct ArenaVizList* viz_list, size_t max_entries);
    size_t (*GetArenaSize)(const void* self);
    
    // Optional metadata
    const char* type_name;
    size_t (*GetElementCount)(const void* self);
    size_t (*GetElementSize)(const void* self);
    
    // Pointer to the actual data structure instance
    void* instance;
};

/**
 * @brief Helper macro to initialize a vizable interface
 */
#define INIT_VIZABLE(vizable_ptr, instance_ptr, type_name_str, \
                     layout_fn, size_fn, count_fn, elem_size_fn) \
    do { \
        (vizable_ptr)->instance = (instance_ptr); \
        (vizable_ptr)->type_name = (type_name_str); \
        (vizable_ptr)->GetArenaLayout = (layout_fn); \
        (vizable_ptr)->GetArenaSize = (size_fn); \
        (vizable_ptr)->GetElementCount = (count_fn); \
        (vizable_ptr)->GetElementSize = (elem_size_fn); \
    } while(0)

/**
 * @brief Generic functions that work with any vizable object
 */
static inline size_t Vizable_GetArenaLayout(const struct Vizable* vizable, 
                                           struct ArenaVizList* viz_list, 
                                           size_t max_entries) {
    return vizable->GetArenaLayout(vizable->instance, viz_list, max_entries);
}

static inline size_t Vizable_GetArenaSize(const struct Vizable* vizable) {
    return vizable->GetArenaSize(vizable->instance);
}

static inline const char* Vizable_GetTypeName(const struct Vizable* vizable) {
    return vizable->type_name;
}

#endif // VIZABLE_COMPOSITION_H
