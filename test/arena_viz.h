#include <stdint.h>
#include <stdbool.h>

#include "arena-viz-msgs.pb.c.h"

struct ArenaListItem
{

}

struct ArenaViz_List
{
   
   size_t len;
}

size_t ArenaViz_Get_ArenaLayout( struct ArenaViz_List *, size_t max_len );
size_t ArenaViz_Get_ArenaSize(void);

