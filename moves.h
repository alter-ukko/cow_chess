#ifndef MOVES_H
#define MOVES_H

#include "chess_types.h"

Piece sprite_to_piece(int sprite);
int v2i_to_board_idx(const v2i v);
int xy_to_board_idx(const int x, const int y);
void copy_board(int dst[64], const int src[64]);
void copy_game(game_t *game_copy, const game_t *game, bool skip_check_check);
void free_game(game_t *game);
void set_board(int board[64], v2i pos, int piece_id);
void unset_board(int board[64], v2i pos);
Piece piece_at(int board[64], int x, int y);
PieceColor color_at(int board[64], int x, int y);
PieceType type_at(int board[64], int x, int y);
int find_king_idx(int board[64], PieceColor color);
v2i find_king_pos(int board[64], PieceColor color);
move_t str_to_move(int board[64], const char *mstr);

bool is_move_castle(move_t move);
bool is_move_en_passant(int board[64], move_t move);
int check_count(game_t *game, v2i king_pos);
bool is_king_double_checked(game_t *game, v2i king_pos);
bool is_check(game_t *game, v2i king_pos);
bool is_moving_into_check(game_t *game, Piece piece_moving, int start_idx, int end_idx);
bool is_checkmate(game_t *game, PieceColor color_to_check);
move_t get_castle_move(PieceColor color_moving, bool shortCastle);
bool can_king_castle(game_t *game, PieceColor color_moving, bool shortCastle);
bool can_pawn_move_en_passant(game_t *game, v2i pawn_pos, bool negative_x);

void valid_moves(game_t *game, v2i pos);
void rook_moves(game_t *game, v2i pos);
void knight_moves(game_t *game, v2i pos);
void bishop_moves(game_t *game, v2i pos);
void queen_moves(game_t *game, v2i pos);
void king_moves(game_t *game, v2i pos);
void pawn_moves(game_t *game, v2i pos);

#endif //MOVES_H
