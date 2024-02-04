#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "moves.h"

Piece sprite_to_piece(int sprite) {
    Piece p = {
        .type = (sprite < 0) ? NO_PIECE : (sprite > 23) ? sprite - 24 : sprite - 8,
        .color = (sprite < 0) ? NO_COLOR : (sprite > 23) ? BLACK : WHITE
    };
    return p;
};

int v2i_to_board_idx(const v2i v) {
    return (v.y * 8) + v.x;
}

int xy_to_board_idx(const int x, const int y) {
    return (y * 8) + x;
}

void copy_board(int dst[64], const int src[64]) {
    memcpy(dst, src, 64 * sizeof(int));
}

void copy_game(game_t *game_copy, const game_t *game, bool skip_check_check) {
    copy_board(game_copy->board, game->board);
    game_copy->skip_check_check = skip_check_check;
    const int move_buf_cap = 1024;
    game_copy->avail = malloc(move_buf_cap * sizeof(v2i));
    game_copy->avail_cap = move_buf_cap;
    game_copy->avail_len = 0;
    UT_icd move_icd = {sizeof(move_t), NULL, NULL, NULL};
    utarray_new(game_copy->moves, &move_icd);
    for (move_t *m=(move_t *)utarray_front(game->moves); m != NULL; m=(move_t *)utarray_next(game->moves, m)) {
        move_t mc = { .from = {.x = m->from.x, .y = m->from.y}, .to = {.x = m->to.x, .y = m->to.y}, .piece_id = m->piece_id };
        utarray_push_back(game_copy->moves, &mc);
    }
    /*
    if (utarray_len(game->moves) == 0) {
        printf("copied game has %d move and parent had %d\n", utarray_len(game_copy->moves), utarray_len(game->moves));
    }
    */
}

void free_game(game_t *game) {
    utarray_free(game->moves);
    free(game->avail);
}

void set_board(int board[64], v2i pos, int piece_id) {
    const int idx = v2i_to_board_idx(pos);
    board[idx] = piece_id;
}

void unset_board(int board[64], v2i pos) {
    set_board(board, pos, -1);
}

Piece piece_at(int board[64], int x, int y) {
    const int sprite = board[(y * 8) + x];
    return sprite_to_piece(sprite);
}

PieceColor color_at(int board[64], int x, int y) {
    const int sprite = board[(y * 8) + x];
    return (sprite < 0) ? NO_COLOR : ((sprite > 23) ? BLACK : WHITE);
}

PieceType type_at(int board[64], int x, int y) {
    const int sprite = board[(y * 8) + x];
    return (sprite < 0) ? NO_PIECE : ((sprite > 23) ? sprite - 24 : sprite - 8);
}

int find_king_idx(int board[64], PieceColor color) {
    int king_idx = -1;
    for (int i=0; i<64; i++) {
        int x = i % 8;
        int y = i / 8;
        Piece op = piece_at(board, x, y);
        if (op.type == KING && op.color == color) {
            king_idx = i;
            break;
        }
    }
    return king_idx;
}

v2i find_king_pos(int board[64], PieceColor color) {
    int king_idx = find_king_idx(board, color);
    if (king_idx == -1) return (v2i){ .x = -1, .y = -1 };
    return (v2i){ .x = king_idx % 8, .y = king_idx / 8 };
}


// this function assumes everything is kosher with the string
move_t str_to_move(int board[64], const char *mstr) {
    move_t m = { .from = {.x = mstr[0] - 97, .y = mstr[1] - 49}, .to = {.x = mstr[2] - 97, .y = mstr[3] - 49} };
    m.piece_id = board[v2i_to_board_idx(m.from)];
    return m;
}

bool is_move_castle(move_t move) {
    if (move.piece_id != KING_W && move.piece_id != KING_B) return false;
    if (move.from.x == 4 && move.from.y == 0 && move.to.x == 6 && move.to.y == 0) return true;
    if (move.from.x == 4 && move.from.y == 0 && move.to.x == 2 && move.to.y == 0) return true;
    if (move.from.x == 4 && move.from.y == 7 && move.to.x == 6 && move.to.y == 7) return true;
    if (move.from.x == 4 && move.from.y == 7 && move.to.x == 2 && move.to.y == 7) return true;
    return false;
}

bool is_move_en_passant(int board[64], move_t move) {
    Piece moving_piece = piece_at(board, move.from.x, move.from.y);
    // only if moving a pawn
    if (moving_piece.type != PAWN) return false;
    // only if moving diagonally
    if (move.from.x == move.to.x) return false;
    // only if moving to a blank square
    Piece end_piece = piece_at(board, move.to.x, move.to.y);
    if (end_piece.type != NO_PIECE) return false;
    // only if there's a pawn behind the end position
    int dy = (move.from.y - move.to.y) / abs(move.from.y - move.to.y);
    Piece taken_piece = piece_at(board, move.to.x, move.to.y + dy);
    if (taken_piece.type != PAWN) return false;
    return true;
}

void add_move(game_t *game, v2i pos, int x, int y) {
    int start_idx = v2i_to_board_idx(pos);
    int end_idx = xy_to_board_idx(x, y);
    Piece p = piece_at(game->board, pos.x, pos.y);
    if (!game->skip_check_check && is_moving_into_check(game, p, start_idx, end_idx)) return;
    game->avail[game->avail_len].x = x;
    game->avail[game->avail_len].y = y;
    game->avail_len++;
}

void valid_moves(game_t *game, v2i pos) {
    game->avail_len = 0;
    Piece p = piece_at(game->board, pos.x, pos.y);
    if (!game->skip_check_check && p.type != KING && is_king_double_checked(game, find_king_pos(game->board, p.color))) return;
    switch (p.type) {
        case NO_PIECE: {
            break;
        }
        case ROOK: {
            rook_moves(game, pos);
            break;;
        }
        case KNIGHT: {
            knight_moves(game, pos);
            break;
        }
        case BISHOP: {
            bishop_moves(game, pos);
            break;
        }
        case QUEEN: {
            queen_moves(game, pos);
            break;
        }
        case KING: {
            king_moves(game, pos);
            break;
        }
        case PAWN: {
            pawn_moves(game, pos);
            break;
        }
    }
}

void rook_moves(game_t *game, v2i pos) {
    const PieceColor moving_color = color_at(game->board, pos.x, pos.y);
    int x = pos.x-1;
    int y = pos.y;
    while (x >= 0) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        x--;
    }
    x = pos.x+1;
    y = pos.y;
    while (x < 8) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        x++;
    }
    x = pos.x;
    y = pos.y-1;
    while (y >= 0) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        y--;
    }
    x = pos.x;
    y = pos.y+1;
    while (y < 8) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        y++;
    }
}

void knight_moves(game_t *game, v2i pos) {
    const PieceColor moving_color = color_at(game->board, pos.x, pos.y);
    // all the ways the knight could move
    v2i km[8] = {
        {.x = pos.x-1, .y = pos.y+2},
        {.x = pos.x+1, .y = pos.y+2},
        {.x = pos.x-1, .y = pos.y-2},
        {.x = pos.x+1, .y = pos.y-2},
        {.x = pos.x+2, .y = pos.y+1},
        {.x = pos.x-2, .y = pos.y+1},
        {.x = pos.x+2, .y = pos.y-1},
        {.x = pos.x-2, .y = pos.y-1},
    };
    for (int i=0; i<8; i++) {
        const v2i m = km[i];
        // don't go off the board
        if (m.x < 0 || m.x > 7 || m.y < 0 || m.y > 7) continue;
        Piece op = piece_at(game->board, m.x, m.y);
        if (op.type == NO_PIECE || op.color != moving_color) {
            add_move(game, pos, m.x, m.y);
        }
    }
}

void bishop_moves(game_t *game, v2i pos) {
    const PieceColor moving_color = color_at(game->board, pos.x, pos.y);
    int x = pos.x-1;
    int y = pos.y-1;
    while (x >= 0 && y >= 0) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        x--;
        y--;
    }
    x = pos.x+1;
    y = pos.y+1;
    while (x < 8 && y < 8) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        x++;
        y++;
    }
    x = pos.x+1;
    y = pos.y-1;
    while (x < 8 && y >= 0) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        x++;
        y--;
    }
    x = pos.x-1;
    y = pos.y+1;
    while (x >= 0 && y < 8) {
        Piece op = piece_at(game->board, x, y);
        if (op.type == NO_PIECE) {
            add_move(game, pos, x, y);
        } else if (op.color != moving_color) {
            add_move(game, pos, x, y);
            break;
        } else {
            break;
        }
        x--;
        y++;
    }
}

void queen_moves(game_t *game, v2i pos) {
    rook_moves(game, pos);
    bishop_moves(game, pos);
}

void king_moves(game_t *game, v2i pos) {
    const PieceColor moving_color = color_at(game->board, pos.x, pos.y);
    // all the ways the king could move, except castling
    v2i km[8] = {
        {.x = pos.x-1, .y = pos.y+1},
        {.x = pos.x, .y = pos.y+1},
        {.x = pos.x+1, .y = pos.y+1},
        {.x = pos.x-1, .y = pos.y},
        {.x = pos.x+1, .y = pos.y},
        {.x = pos.x-1, .y = pos.y-1},
        {.x = pos.x, .y = pos.y-1},
        {.x = pos.x+1, .y = pos.y-1},
    };
    for (int i=0; i<8; i++) {
        const v2i m = km[i];
        // don't go off the board
        if (m.x < 0 || m.x > 7 || m.y < 0 || m.y > 7) continue;
        Piece op = piece_at(game->board, m.x, m.y);
        if (op.type == NO_PIECE || op.color != moving_color) {
            add_move(game, pos, m.x, m.y);
        }
    }
    if (can_king_castle(game, moving_color, true)) {
        move_t move = get_castle_move(moving_color, true);
        add_move(game, move.from, move.to.x, move.to.y);
    }
    if (can_king_castle(game, moving_color, false)) {
        move_t move = get_castle_move(moving_color, false);
        add_move(game, move.from, move.to.x, move.to.y);
    }
}

void pawn_moves(game_t *game, v2i pos) {
    const PieceColor moving_color = color_at(game->board, pos.x, pos.y);
    int dy = (moving_color == WHITE) ? 1 : -1;
    int st_rank = (moving_color == WHITE) ? 1 : 6;
    // a pawn can move one space forward into an empty space
    int y = pos.y + dy;
    if (y >= 0 && y < 8) {
        Piece op_sa = piece_at(game->board, pos.x, y);
        if (op_sa.type == NO_PIECE) {
            add_move(game, pos, pos.x, y);
        }
    }
    // a pawn can move two spaces forward from its starting position
    if (pos.y == st_rank) {
        int yy = pos.y + (2 * dy);
        if (yy >= 0 && yy < 8) {
            Piece op_sa = piece_at(game->board, pos.x, y);
            Piece op_da = piece_at(game->board, pos.x, yy);
            if (op_da.type == NO_PIECE && op_sa.type == NO_PIECE) {
                add_move(game, pos, pos.x, yy);
            }
        }
    }
    // a pawn can move one diagonal space ahead to capture
    y = pos.y + dy;
    if (pos.x > 0 && y >= 0 && y < 8) {
        Piece op_cap1 = piece_at(game->board, pos.x-1, y);
        if (op_cap1.type != NO_PIECE && op_cap1.color != moving_color) {
            add_move(game, pos, pos.x-1, y);
        }
    }
    if (pos.x < 7 && y >= 0 && y < 8) {
        Piece op_cap2 = piece_at(game->board, pos.x+1, y);
        if (op_cap2.type != NO_PIECE && op_cap2.color != moving_color) {
            add_move(game, pos, pos.x+1, y);
        }
    }
    if (can_pawn_move_en_passant(game, pos, true)) {
        add_move(game, pos, pos.x-1, y);
    }
    if (can_pawn_move_en_passant(game, pos, false)) {
        add_move(game, pos, pos.x+1, y);
    }
    // TODO: indicate to the caller in the case that a promotion would occur
    // int promo_rank = (moving_color == WHITE) ? 7 : 0;
}

move_t get_castle_move(PieceColor color_moving, bool shortCastle) {
    // short castle is e1g1 white or e8g8 black
    // long castle is e1c1 white or e8c8 black
    switch (color_moving) {
        case WHITE: {
            if (shortCastle) {
                const move_t m = {.from = {.x = 4, .y = 0}, .to = {.x = 6, .y = 0}, .piece_id = KING_W };
                return m;
            } else {
                const move_t m = {.from = {.x = 4, .y = 0}, .to = {.x = 2, .y = 0}, .piece_id = KING_W };
                return m;
            }
        }
        case BLACK: {
            if (shortCastle) {
                const move_t m = {.from = {.x = 4, .y = 7}, .to = {.x = 6, .y = 7}, .piece_id = KING_B };
                return m;
            } else {
                const move_t m = {.from = {.x = 4, .y = 7}, .to = {.x = 2, .y = 7}, .piece_id = KING_B };
                return m;
            }
        }
        default: {
            const move_t m = {.from = {.x = -1, .y = -1}, .to = {.x = -1, .y = -1}, .piece_id = -1 };
            return m;
        }
    }
}

bool can_pawn_move_en_passant(game_t *game, v2i pawn_pos, bool negative_x) {
    Piece pawn = piece_at(game->board, pawn_pos.x, pawn_pos.y);
    if (pawn.type != PAWN) return false;
    int allowed_pawn_y = (pawn.color == WHITE) ? 4 : 3;
    int dy = (pawn.color == WHITE) ? 1 : -1;
    int other_pawn_piece_id = (pawn.color == WHITE) ? PAWN_B : PAWN_W;
    if (pawn_pos.y != allowed_pawn_y) return false;
    move_t *last_move = utarray_back(game->moves);
    // the last move must have been a pawn move
    if (last_move->piece_id != other_pawn_piece_id) return false;
    // on the last move the opposing pawn must have landed on the same rank as the pawn that's moving
    if (last_move->to.y != pawn_pos.y) return false;
    // on the last move the opposing pawn must have moved two spaces
    if (last_move->from.y != pawn_pos.y + (2 * dy)) return false;
    // the last move must have landed beside our pawn
    int allowed_pawn_x = (negative_x) ? pawn_pos.x - 1 : pawn_pos.x + 1;
    if (last_move->from.x == allowed_pawn_x) {
        return true;
    }
    return false;
}

bool can_king_castle(game_t *game, PieceColor color_moving, bool shortCastle) {
    move_t move = get_castle_move(color_moving, shortCastle);
    // make sure the king is not currently in check
    if (!game->skip_check_check && is_check(game, move.from)) return false;
    // make sure the king is in the starting position
    Piece king = piece_at(game->board, move.from.x, move.from.y);
    if (king.type != KING || king.color != color_moving) return false;
    // make sure all the spaces between the king and the rook are empty
    int start_blank_x = move.from.x + 1;
    int end_blank_x = 7;
    if (move.from.x > move.to.x) {
        start_blank_x = 1;
        end_blank_x = move.from.x;
    }
    for (int x=start_blank_x; x<end_blank_x; x++) {
        Piece tp = piece_at(game->board, x, move.from.y);
        if (tp.type != NO_PIECE) return false;
    }
    // make sure the rook is in the corner
    int rook_x = (move.from.x < move.to.x) ? 7 : 0;
    Piece rook = piece_at(game->board, rook_x, move.from.y);
    if (rook.type != ROOK || rook.color != color_moving) return false;
    // make sure the king and rook have not previously moved
    for (move_t *m=(move_t *)utarray_front(game->moves); m != NULL; m=(move_t *)utarray_next(game->moves, m)) {
        if (m->from.x == move.from.x && m->from.y == move.from.y) return false;
        if (m->from.x == rook_x && m->from.y == move.from.y) return false;
    }
    // hypothetically move the king and make sure it doesn't move through check
    int move_inc = (move.from.x < move.to.x) ? 1 : -1;
    int start_idx = xy_to_board_idx(move.from.x, move.from.y);
    int end_idx_1 = start_idx + move_inc;
    if (is_moving_into_check(game, king, start_idx, end_idx_1)) return false;
    int end_idx_2 = end_idx_1 + move_inc;
    if (is_moving_into_check(game, king, start_idx, end_idx_2)) return false;
    return true;
}

// you want to use a copied game to call this function
int check_count(game_t *game, v2i king_pos) {
    game_t game_copy;
    copy_game(&game_copy, game, true);
    // copy the game so we can make the hypothetical move
    Piece king = piece_at(game_copy.board, king_pos.x, king_pos.y);
    // collect the possible moves of the other side's pieces and see if any of them land on the king
    int cnt = 0;
    for (int i=0; i<64; i++) {
        v2i pos = {.x = i % 8, .y = i / 8};
        Piece op = piece_at(game_copy.board, pos.x, pos.y);
        if (op.type != NO_PIECE && op.color != king.color) {
            game_copy.avail_len = 0;
            valid_moves(&game_copy, pos);
            for (int j=0; j<game_copy.avail_len; j++) {
                if (game_copy.avail[j].x == king_pos.x && game_copy.avail[j].y == king_pos.y) {
                    cnt++;
                }
            }
        }
    }
    free_game(&game_copy);
    return cnt;
}

bool is_king_double_checked(game_t *game, v2i king_pos) {
    return (check_count(game, king_pos) > 1);
}

// you want to use a copied game to call this function
bool is_check(game_t *game, v2i king_pos) {
    return (check_count(game, king_pos) > 0);
}

bool is_moving_into_check(game_t *game, Piece piece_moving, int start_idx, int end_idx) {
    game_t test_game;
    // copy the game so we can make the hypothetical move
    copy_game(&test_game, game, true);
    // make the hypothetical move on the copied board
    test_game.board[end_idx] = test_game.board[start_idx];
    test_game.board[start_idx] = -1;
    // find the king of the same color as the piece being moved
    v2i king_pos = find_king_pos(test_game.board, piece_moving.color);
    bool ret = is_check(&test_game, king_pos);
    free_game(&test_game);
    return ret;
}

bool is_checkmate(game_t *game, PieceColor color_to_check) {
    v2i king_pos = find_king_pos(game->board, color_to_check);
    game_t test_game;
    // copy the game so we can test moves
    copy_game(&test_game, game, true);
    valid_moves(&test_game, king_pos);
    bool ret = (test_game.avail_len == 0) ? true : false;
    free_game(&test_game);
    return ret;
}