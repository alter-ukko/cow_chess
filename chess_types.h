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
    KING = 0,
    QUEEN = 1,
    BISHOP = 2,
    KNIGHT = 3,
    ROOK = 4,
    PAWN = 5,
} PieceType;

typedef enum {
    NO_COLOR = -1,
    WHITE = 0,
    BLACK = 8,
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

#define KING_W 8
#define QUEEN_W 9
#define BISHOP_W 10
#define KNIGHT_W 11
#define ROOK_W 12
#define PAWN_W 13
#define DOT 14
#define KING_B 24
#define QUEEN_B 25
#define BISHOP_B 26
#define KNIGHT_B 27
#define ROOK_B 28
#define PAWN_B 29
#define HIGHLIGHT 30
#define LBL_1_DK 32
#define LBL_2_DK 33
#define LBL_3_DK 34
#define LBL_4_DK 35
#define LBL_5_DK 36
#define LBL_6_DK 37
#define LBL_7_DK 38
#define LBL_8_DK 39
#define LBL_1_LT 40
#define LBL_2_LT 41
#define LBL_3_LT 42
#define LBL_4_LT 43
#define LBL_5_LT 44
#define LBL_6_LT 45
#define LBL_7_LT 46
#define LBL_8_LT 47
#define LBL_A_DK 48
#define LBL_B_DK 49
#define LBL_C_DK 50
#define LBL_D_DK 51
#define LBL_E_DK 52
#define LBL_F_DK 53
#define LBL_G_DK 54
#define LBL_H_DK 55
#define LBL_A_LT 56
#define LBL_B_LT 57
#define LBL_C_LT 58
#define LBL_D_LT 59
#define LBL_E_LT 60
#define LBL_F_LT 61
#define LBL_G_LT 62
#define LBL_H_LT 63

extern const int initial_board[64];
extern const char files[];
extern const char ranks[];

#endif //CHESS_TYPES_H
