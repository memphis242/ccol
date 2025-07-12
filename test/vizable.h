#ifndef VIZABLE_TRAIT_H
#define VIZABLE_TRAIT_H

#include <stdbool.h>

struct ArenaVizBlk
{
   size_t blk_offset;
   size_t blk_len;
};

struct ArenaVizList
{
   struct ArenaVizBlk * list;
   size_t len;
};

struct Vizable
{
   size_t (*ArenaLayout)(struct ArenaVizList *, size_t);
   size_t (*ArenaSize)(void);
};

#endif // VIZABLE_TRAIT_H 

