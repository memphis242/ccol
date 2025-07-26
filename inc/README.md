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
### API Summary
```c
/*** Constructor/Destructor ***/

struct Vector * VectorNew( size_t element_size, size_t initial_capacity, size_t max_capacity, size_t initial_len, const struct Allocator * mem_mgr );
void VectorFree( struct Vector * self );

/*** Vector-Vector Operations (Copy/Move) ***/

struct Vector * VectorDuplicate( const struct Vector * self );
bool            VectorMove(struct Vector * dest, struct Vector * src);
bool            VectorsAreEqual( const struct Vector * a, const struct Vector * b );
struct Vector * VectorConcatenate( const struct Vector * v1, const struct Vector * v2 );

/*** Basic Stats ***/

size_t VectorLength( const struct Vector * self );
size_t VectorCapacity( const struct Vector * self );
size_t VectorMaxCapacity( const struct Vector * self );
size_t VectorElementSize( const struct Vector * self );
bool   VectorIsEmpty( const struct Vector * self );
bool   VectorIsFull( const struct Vector * self );

/*** Vector Operations ***/

bool   VectorPush( struct Vector * self, const void * element );
bool   VectorInsert( struct Vector * self, size_t idx, const void * element );
void * VectorGet( const struct Vector * self, size_t idx );
void * VectorLastElement( const struct Vector * self );
bool   VectorCpyElementAt( const struct Vector * self, size_t idx, void * data );
bool   VectorCpyLastElement( const struct Vector * self, void * data );
bool   VectorSet( struct Vector * self, size_t idx, const void * element );
bool   VectorRemoveElementAt( struct Vector * self, size_t idx, void * data );
bool   VectorRemoveLastElement( struct Vector * self, void * data );
bool   VectorClearElementAt( struct Vector * self, size_t idx );
bool   VectorClear( struct Vector * self );
bool   VectorReset( struct Vector * self );
bool   VectorHardReset( struct Vector * self );

/*** Range Based Vector Operations ***/

struct Vector * VectorSplitAt( struct Vector * self, size_t idx );
struct Vector * VectorSlice( const struct Vector * self, size_t idx_start, size_t idx_end );

bool VectorRangePush( struct Vector * self, const void * data, size_t dlen );
bool VectorRangeInsert( struct Vector * self, size_t idx,  const void * data, size_t dlen );
bool VectorRangeCpy( const struct Vector * self, size_t idx_start, size_t idx_end, void * buffer );
bool VectorRangeCpyToEnd( const struct Vector * self, size_t idx, void * buffer );
bool VectorRangeSet( struct Vector * self, size_t idx_start, size_t idx_end, const void * data );
bool VectorRangeRemove( struct Vector * self, size_t idx_start, size_t idx_end, void * buf );
bool VectorRangeClear( struct Vector * self, size_t idx_start, size_t idx_end );
```
### Example Usage
```c
struct Vector * vec = VectorNew( sizeof(int), 10, 100, 0, NULL );
(void)VectorPush( vec, &(int){42} ); // occassionally more convenient to use compound literals
int a = 5;
(void)VectorPush( vec, &a );
// TODO
```
