#ifndef VIZABLE_TRAIT_H
#define VIZABLE_TRAIT_H

#include <stdint.h>
#include <stdbool.h>

#include "viz-msgs.pb.c.h"

struct Vizable
{
   size_t (*GetArenaLayout)( struct ArenaVizList *, size_t );
   size_t (*GetArenaSize)(void);
};

#endif // VIZABLE_TRAIT_H 
