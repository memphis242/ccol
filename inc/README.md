# API Quick Reference

## Allocator
```c
struct Allocator
{
   void * (*alloc)(size_t req_sz, void * arena);
   void * (*realloc)(void * old_ptr, size_t new_sz, size_t old_sz, void * arena);
   void   (*reclaim)(void * old_ptr, size_t old_sz, void * arena);
   void   (*alloca_init)(void * arena);
   void * arena;
};
```

## Vector
```c
/*** Constructor/Destructor ***/

struct Vector * VectorNew( size_t element_size, size_t initial_capacity, size_t max_capacity, size_t initial_len, const struct Allocator * mem_mgr );
void VectorFree( struct Vector * self );

/*** Vector-Vector Operations (Copy/Move) ***/

struct Vector * VectorDuplicate( const struct Vector * self );
// TODO: bool VectorMove(struct Vector * old, struct Vector * new);
bool            VectorsAreEqual( const struct Vector * a, const struct Vector * b );
struct Vector * VectorConcatenate( const struct Vector * v1, const struct Vector * v2 );

/*** Basic Stats Get Functions ***/

size_t VectorLength( const struct Vector * self );
size_t VectorCapacity( const struct Vector * self );
size_t VectorMaxCapacity( const struct Vector * self );
size_t VectorElementSize( const struct Vector * self );
bool   VectorIsEmpty( const struct Vector * self );
bool   VectorIsFull( const struct Vector * self );

/*** Vector Operations ***/

bool   VectorPush( struct Vector * self, const void * element );
bool   VectorInsertAt( struct Vector * self, size_t idx, const void * element );
void * VectorGetElementAt( const struct Vector * self, size_t idx );
void * VectorLastElement( const struct Vector * self );
bool   VectorCpyElementAt( const struct Vector * self, size_t idx, void * data );
bool   VectorCpyLastElement( const struct Vector * self, void * data );
bool   VectorSetElementAt( struct Vector * self, size_t idx, const void * element );
bool   VectorRemoveElementAt( struct Vector * self, size_t idx, void * data );
bool   VectorRemoveLastElement( struct Vector * self, void * data );
bool   VectorClearElementAt( struct Vector * self, size_t idx );
bool   VectorClear( struct Vector * self );
bool   VectorReset( struct Vector * self );
bool   VectorHardReset( struct Vector * self );

/*** Sub-Range Based Vector Operations ***/

struct Vector * VectorSplitAt( struct Vector * self, size_t idx );
struct Vector * VectorSlice( const struct Vector * self, size_t idx_start, size_t idx_end );

bool   VectorSubRange_PushElements( struct Vector * self, const void * data, size_t dlen );
bool   VectorSubRange_InsertElementsAt( struct Vector * self, size_t idx,  const void * data, size_t dlen );
void * VectorSubRange_GetElementsFromIdx( const struct Vector * self, size_t idx );
bool   VectorSubRange_CpyElementsInRange( const struct Vector * self, size_t idx_start, size_t idx_end, void * buffer );
bool   VectorSubRange_CpyElementsFromStartToIdx( const struct Vector * self, size_t idx, void * buffer );
bool   VectorSubRange_CpyElementsFromIdxToEnd( const struct Vector * self, size_t idx, void * buffer );
bool   VectorSubRange_SetElementsInRange( struct Vector * self, size_t idx_start, size_t idx_end, const void * data );
bool   VectorSubRange_SetElementsFromStartToIdx( struct Vector * self, size_t idx, const void * data );
bool   VectorSubRange_SetElementsFromIdxToEnd( struct Vector * self, size_t idx, const void * data );
bool   VectorSubRange_RemoveElementsInRange( struct Vector * self, size_t idx_start, size_t idx_end, void * buf );
bool   VectorSubRange_RemoveElementsFromStartToIdx( struct Vector * self, size_t idx, void * buf );
bool   VectorSubRange_RemoveElementsFromIdxToEnd( struct Vector * self, size_t idx, void * buf );
bool   VectorSubRange_ClearElementsInRange( struct Vector * self, size_t idx_start, size_t idx_end );
bool   VectorSubRange_ClearElementsFromStartToIdx( struct Vector * self, size_t idx );
bool   VectorSubRange_ClearElementsFromIdxToEnd( struct Vector * self, size_t idx );
```
