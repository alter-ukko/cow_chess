#ifndef MAP_DEFS_H
#define MAP_DEFS_H

typedef struct {
    int layers;
    int cols;
    int rows;
    int map[4][57600];
} t_tilemap;

t_tilemap tm;

#endif //MAP_DEFS_H
