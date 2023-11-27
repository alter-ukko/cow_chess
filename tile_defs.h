#ifndef TILE_DEFS_H
#define TILE_DEFS_H

#include <stdlib.h>

#define ACTION_IDLE 1
#define ACTION_OPEN 2
#define ACTION_CLOSE 3
#define ACTION_MOOR 4
#define ACTION_FLOAT 5
#define ACTION_SAIL 6
#define ACTION_IDLE_FRONT 7
#define ACTION_PICK_FRONT 8
#define ACTION_IDLE_BACK 9
#define ACTION_PICK_BACK 10
#define ACTION_IDLE_LEFT 11
#define ACTION_PICK_LEFT 12
#define ACTION_IDLE_RIGHT 13
#define ACTION_PICK_RIGHT 14
#define ACTION_WALK_FRONT 15
#define ACTION_AXE_FRONT 16
#define ACTION_WALK_BACK 17
#define ACTION_AXE_BACK 18
#define ACTION_WALK_RIGHT 19
#define ACTION_AXE_LEFT 20
#define ACTION_WALK_LEFT 21
#define ACTION_AXE_RIGHT 22
#define ACTION_RUN_FRONT 23
#define ACTION_CAN_FRONT 24
#define ACTION_RUN_BACK 25
#define ACTION_CAN_BACK 26
#define ACTION_RUN_RIGHT 27
#define ACTION_CAN_LEFT 28
#define ACTION_RUN_LEFT 29
#define ACTION_CAN_RIGHT 30

#define NUM_TILES 896

typedef struct {
    int action;
    uint32_t frame_len;
    int *frames;
} t_anim;

typedef struct {
    int id;
    int tw;
    int th;
    uint32_t anim_len;
    t_anim *anims;
} t_tile;

t_tile *create_tiles();
void free_tiles(t_tile *tiles);

#endif //TILE_DEFS_H
