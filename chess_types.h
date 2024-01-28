#pragma once

#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <stdbool.h>
#include "utarray.h"

typedef union  {
    struct {
        int x, y;
    };
    struct {
        int col, row;
    };
} v2i;

typedef struct {
    v2i from;
    v2i to;
    int piece_id;
} move_t;

typedef enum {
    NO_PIECE = -1,
    ROOK = 0,
    KNIGHT = 1,
    BISHOP = 2,
    QUEEN = 3,
    KING = 4,
    PAWN = 5,
} PieceType;

typedef enum {
    NO_COLOR = -1,
    WHITE = 0,
    BLACK = 9,
} PieceColor;

typedef struct {
    PieceType type;
    PieceColor color;
} Piece;

typedef struct {
    int board[64];
    UT_array *moves;
    v2i cur_sel;
    v2i *avail;
    int avail_len;
    int avail_cap;
    bool skip_check_check;
} game_t;

#define ROOK_W 0
#define KNIGHT_W 1
#define BISHOP_W 2
#define QUEEN_W 3
#define KING_W 4
#define PAWN_W 5
#define SQUARE_W 6
#define SQUARE_PLAIN_W 7
#define ROOK_B 9
#define KNIGHT_B 10
#define BISHOP_B 11
#define QUEEN_B 12
#define KING_B 13
#define PAWN_B 14
#define SQUARE_B 15
#define SQUARE_PLAIN_B 16
#define FRAME_TL 18
#define FRAME_T 19
#define FRAME_TR 20
#define SHADOW 21
#define FRAME_L 27
#define FRAME_R 29
#define DOT 30
#define HIGHLIGHT 32
#define FRAME_BL 36
#define FRAME_B 37
#define FRAME_BR 38
#define LBL_1 45
#define LBL_2 46
#define LBL_3 47
#define LBL_4 48
#define LBL_5 49
#define LBL_6 50
#define LBL_7 51
#define LBL_8 52
#define LBL_A 54
#define LBL_B 55
#define LBL_C 56
#define LBL_D 57
#define LBL_E 58
#define LBL_F 59
#define LBL_G 60
#define LBL_H 61

extern const int initial_board[64];
extern const char files[];
extern const char ranks[];

#endif //CHESS_TYPES_H
