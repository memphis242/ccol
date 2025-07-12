#ifndef VIZABLE_VTABLE_H
#define VIZABLE_VTABLE_H

#include <stdint.h>
#include <stdbool.h>

// Forward declarations
struct ArenaVizList;

/**
 * @brief Vizable vtable - similar to C++ vtables
 * 
 * This approach mimics OOP vtables where each data structure type
 * has a static vtable with function pointers.
 */
struct VizableVTable {
    const char* type_name;
    size_t (*get_arena_layout)(const void* self, struct ArenaVizList* viz_list, size_t max_entries);
    size_t (*get_arena_size)(const void* self);
    size_t (*get_element_count)(const void* self);
    size_t (*get_element_size)(const void* self);
    bool (*is_empty)(const void* self);
    void (*destroy)(void* self);  // Optional destructor
};

/**
 * @brief Base "class" for all vizable objects
 * 
 * Any data structure that wants to be vizable should embed this
 * as the first member of their struct.
 */
struct VizableBase {
    const struct VizableVTable* vtable;
};

/**
 * @brief Macro to declare a vizable data structure
 * 
 * Usage: 
 * struct Vector {
 *     VIZABLE_HEADER;
 *     // ... other vector fields
 * };
 */
#define VIZABLE_HEADER struct VizableBase vizable_base

/**
 * @brief Macro to get the vizable base from any data structure
 */
#define AS_VIZABLE(obj) (&((obj)->vizable_base))

/**
 * @brief Macro to call vizable methods safely
 */
#define VIZABLE_CALL(obj, method, ...) \
    (AS_VIZABLE(obj)->vtable->method ? \
     AS_VIZABLE(obj)->vtable->method((obj), ##__VA_ARGS__) : 0)

/**
 * @brief Helper macros for common operations
 */
#define VIZABLE_GET_ARENA_LAYOUT(obj, viz_list, max_entries) \
    VIZABLE_CALL(obj, get_arena_layout, viz_list, max_entries)

#define VIZABLE_GET_ARENA_SIZE(obj) \
    VIZABLE_CALL(obj, get_arena_size)

#define VIZABLE_GET_ELEMENT_COUNT(obj) \
    VIZABLE_CALL(obj, get_element_count)

#define VIZABLE_GET_TYPE_NAME(obj) \
    (AS_VIZABLE(obj)->vtable->type_name)

/**
 * @brief Initialize a vizable object
 */
static inline void VizableBase_Init(struct VizableBase* base, const struct VizableVTable* vtable) {
    base->vtable = vtable;
}

/**
 * @brief Example macro to declare vtable for a specific type
 */
#define DECLARE_VIZABLE_VTABLE(TypeName) \
    extern const struct VizableVTable TypeName##_VTable; \
    static inline void TypeName##_InitVizable(struct TypeName* obj) { \
        VizableBase_Init(&obj->vizable_base, &TypeName##_VTable); \
    }

#endif // VIZABLE_VTABLE_H
