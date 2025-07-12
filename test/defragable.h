#ifdef DEFRAGABLE_TRAIT_H
#define DEFRAGABLE_TRAIT_H

#include <stdbool.h>

struct Defragable
{
   bool (*IsFragmented)(void);
   bool (*Defragment)(void);
}

#endif // DEFRAGABLE_TRAIT_H

