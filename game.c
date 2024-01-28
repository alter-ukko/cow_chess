#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef __JETBRAINS_IDE__
#define SOKOL_IMPL
#if defined(_WIN32)
#define SOKOL_D3D11
#elif defined(__EMSCRIPTEN__)
#define SOKOL_GLES2
#elif defined(__APPLE__)
// NOTE: on macOS, sokol.c is compiled explicitely as ObjC
#define SOKOL_METAL
#else
#define SOKOL_GLCORE33
#endif
#endif

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_time.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"
#ifdef __JETBRAINS_IDE__
#define SOKOL_IMGUI_IMPL
#endif

#include "util/sokol_imgui.h"
#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "HandmadeMath.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

typedef HMM_Mat4 hmm_mat4;

#include "game.glsl.h"
#include "utarray.h"
//#include "map_defs.h"
#include "uci.h"
#include "str.h"
#include "chess_types.h"
#include "moves.h"
#include "easing.h"
#include "data.h"

#if defined(__APPLE__)
const uint32_t MODIFIER_KEY = SAPP_MODIFIER_SUPER;
#else
const uint32_t MODIFIER_KEY = SAPP_MODIFIER_CTRL;
#endif

const uint32_t num_quads = 100000;
const uint32_t num_verts = num_quads * 4;
const uint32_t num_indices = num_quads * 6;
const double move_time_ms = 500.0;

typedef struct {
    v2i pos;
    int tile_id;
} tp;

typedef struct {
    int tile_id;
    float x, y;
    int doing;
    int frame;
} obj_g;

typedef struct {
    float x, y, z;
    uint32_t color;
    int16_t u, v;
} vertex_g;

vertex_g vertex(float x, float y, float z, uint32_t color, int16_t u, int16_t v) {
    vertex_g vtx = { .x = x, .y = y, .z = z, .color = color, .u = u, .v = v };
    return vtx;
}

typedef struct {
    vertex_g verts[4];
    uint16_t indices[6];
} quad_g;

typedef struct {
    bool up;
    bool down;
    bool left;
    bool right;
    bool esc;
    bool paste;
    bool zoom_in;
    bool zoom_out;
    float scroll_amt;
    bool mouse_down;
    float mx, my;
    uint64_t md_time;
    uint64_t mu_time;
    bool mouse_clicked;
    float mdx, mdy;
    float mcx, mcy;
    v2i tile_clicked;
} input_g;

const u_int32_t DN = 0;
const u_int32_t DL = 0x00000001;
const u_int32_t DU = 0x00000002;
const u_int32_t DR = 0x00000004;
const u_int32_t DD = 0x00000008;

typedef enum {
    AWAITING_MOVE,
    MOVING_PLAYER,
    MOVING_OPPONENT,
    CHECKMATE,
} GameStatus;

static struct {
    int screen_w;
    int screen_h;
    float rx, ry;
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
    float cx, cy;
    HMM_Vec3 cpos;
    HMM_Mat4 mvp;
    float sprites_across;
    vertex_g *verts;
    uint16_t *indices;
    uint32_t vidx;
    uint32_t iidx;
    input_g input;
    int build_item_idx;
    int spritesheet_w;
    int spritesheet_h;
    int sprite_size;
    int sprite_cols;
    int sprite_rows;
    uci_client client;
    bool white_move;
    move_t cur_move;
    game_t game;
    GameStatus status;
    uint64_t delta_time;
    uint64_t event_time;
    bool player_is_black;
    char *opening_buf;
} state;

// x,y: destination coords
// w,h: width and height of sprite (in tiles)
// row,col: the row and column of the sprite's bottom right corner
quad_g make_quad(float x, float y, int w, int h, int row, int col, int layer) {
    uint16_t uvw_unit = (uint16_t)(32768 / state.sprite_cols);
    uint16_t uvh_unit = (uint16_t)(32768 / state.sprite_rows);
    quad_g q;
    const float ww = (float)w;
    const float hh = (float)h;
    const float xx = x;
    const float yy = y;
    const float zz = (float)layer * 0.1;

    q.verts[0] = vertex( xx, yy, zz, 0xFFFFFFFF, col * uvw_unit, (row + 1) * uvh_unit );
    q.verts[1] = vertex( xx + ww, yy, zz, 0xFFFFFFFF, (col + w) * uvw_unit,  (row + 1) * uvh_unit );
    q.verts[2] = vertex( xx + ww, yy + hh, zz, 0xFFFFFFFF, (col + w) * uvw_unit,  (row - hh + 1) * uvh_unit);
    q.verts[3] = vertex( xx, yy + hh, zz, 0xFFFFFFFF, col * uvw_unit, (row - hh + 1) * uvh_unit );

    q.indices[0] = 0;
    q.indices[1] = 1;
    q.indices[2] = 2;
    q.indices[3] = 0;
    q.indices[4] = 2;
    q.indices[5] = 3;

    return q;
}

void add_quad_to_buffer(quad_g q) {
    uint32_t vi = state.vidx;
    uint32_t ii = state.iidx;
    state.verts[vi] = q.verts[0];
    state.verts[vi + 1] = q.verts[1];
    state.verts[vi + 2] = q.verts[2];
    state.verts[vi + 3] = q.verts[3];

    uint32_t uvi = vi;
    state.indices[ii] = q.indices[0] + uvi;
    state.indices[ii + 1] = q.indices[1] + uvi;
    state.indices[ii + 2] = q.indices[2] + uvi;
    state.indices[ii + 3] = q.indices[3] + uvi;
    state.indices[ii + 4] = q.indices[4] + uvi;
    state.indices[ii + 5] = q.indices[5] + uvi;

    state.vidx = vi + 4;
    state.iidx = ii + 6;
}

HMM_Mat4 ortho_camera_mat(HMM_Vec3 cpos, float pixels_per_sprite) {
    // If you don't move the camera from the origin (0,0,0), then vmat is the inverse
    // of the identity matrix, which is the identity matrix. So if you don't move the
    // camera from the origin, the view-projection matrix is just the projection matrix.
    // Just a thought. The benefit of not moving the camera is that you don't have
    // to ever recalculate the view-projection matrix. The drawback is that you have
    // to translate your sprites every time you draw.

    // HMM_Mat4 identity = HMM_M4D(1.0f);
    float half_w = ( (float)state.screen_w / pixels_per_sprite) * 0.5f;
    float half_h = ( (float)state.screen_h / pixels_per_sprite) * 0.5f;
    HMM_Mat4 p_mat = HMM_Orthographic_RH_ZO(-half_w, half_w, -half_h, half_h, -1.0f, 1.0f);
    HMM_Mat4 transform = HMM_Translate(cpos);
    HMM_Mat4 v_mat = HMM_InvOrthographic(transform);
    return HMM_MulM4(p_mat, v_mat);
}

void compute_mvp() {
    state.cpos = HMM_V3(state.cx, state.cy, 0.0f);
    float pixels_per_sprite = ((float)state.screen_w / state.sprites_across);
    state.mvp = ortho_camera_mat(state.cpos, pixels_per_sprite);
}

void imgui_style() {
    ImGuiStyle *style = igGetStyle();
    ImVec4 *colors = style->Colors;
    colors[ImGuiCol_Text]                   = (ImVec4){0.90f, 0.90f, 0.90f, 1.00f};
    colors[ImGuiCol_TextDisabled]           = (ImVec4){0.60f, 0.60f, 0.60f, 1.00f};
    colors[ImGuiCol_WindowBg]               = (ImVec4){0.17f, 0.17f, 0.17f, 1.00f};
    colors[ImGuiCol_ChildBg]                = (ImVec4){0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_PopupBg]                = (ImVec4){0.11f, 0.11f, 0.14f, 0.92f};
    colors[ImGuiCol_Border]                 = (ImVec4){0.50f, 0.50f, 0.50f, 0.50f};
    colors[ImGuiCol_BorderShadow]           = (ImVec4){0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_FrameBg]                = (ImVec4){0.43f, 0.43f, 0.43f, 0.39f};
    colors[ImGuiCol_FrameBgHovered]         = (ImVec4){0.79f, 0.57f, 0.37f, 0.40f};
    colors[ImGuiCol_FrameBgActive]          = (ImVec4){0.64f, 0.51f, 0.41f, 0.69f};
    colors[ImGuiCol_TitleBg]                = (ImVec4){0.17f, 0.17f, 0.17f, 1.00f};
    colors[ImGuiCol_TitleBgActive]          = (ImVec4){0.08f, 0.08f, 0.08f, 1.00f};
    colors[ImGuiCol_TitleBgCollapsed]       = (ImVec4){0.17f, 0.17f, 0.17f, 1.00f};
    colors[ImGuiCol_MenuBarBg]              = (ImVec4){0.14f, 0.14f, 0.14f, 0.71f};
    colors[ImGuiCol_ScrollbarBg]            = (ImVec4){0.30f, 0.25f, 0.20f, 0.60f};
    colors[ImGuiCol_ScrollbarGrab]          = (ImVec4){0.80f, 0.59f, 0.40f, 0.30f};
    colors[ImGuiCol_ScrollbarGrabHovered]   = (ImVec4){0.80f, 0.59f, 0.40f, 0.40f};
    colors[ImGuiCol_ScrollbarGrabActive]    = (ImVec4){0.80f, 0.59f, 0.39f, 0.60f};
    colors[ImGuiCol_CheckMark]              = (ImVec4){0.90f, 0.90f, 0.90f, 0.50f};
    colors[ImGuiCol_SliderGrab]             = (ImVec4){1.00f, 1.00f, 1.00f, 0.30f};
    colors[ImGuiCol_SliderGrabActive]       = (ImVec4){0.80f, 0.54f, 0.39f, 0.60f};
    colors[ImGuiCol_Button]                 = (ImVec4){0.80f, 0.49f, 0.16f, 0.62f};
    colors[ImGuiCol_ButtonHovered]          = (ImVec4){0.81f, 0.43f, 0.19f, 0.79f};
    colors[ImGuiCol_ButtonActive]           = (ImVec4){0.85f, 0.48f, 0.25f, 1.00f};
    colors[ImGuiCol_Header]                 = (ImVec4){0.34f, 0.34f, 0.34f, 0.45f};
    colors[ImGuiCol_HeaderHovered]          = (ImVec4){0.36f, 0.36f, 0.36f, 0.80f};
    colors[ImGuiCol_HeaderActive]           = (ImVec4){0.43f, 0.43f, 0.43f, 0.80f};
    colors[ImGuiCol_Separator]              = (ImVec4){0.50f, 0.50f, 0.50f, 0.60f};
    colors[ImGuiCol_SeparatorHovered]       = (ImVec4){0.67f, 0.67f, 0.67f, 1.00f};
    colors[ImGuiCol_SeparatorActive]        = (ImVec4){0.87f, 0.87f, 0.87f, 1.00f};
    colors[ImGuiCol_ResizeGrip]             = (ImVec4){1.00f, 1.00f, 1.00f, 0.10f};
    colors[ImGuiCol_ResizeGripHovered]      = (ImVec4){1.00f, 0.86f, 0.78f, 0.60f};
    colors[ImGuiCol_ResizeGripActive]       = (ImVec4){1.00f, 0.86f, 0.78f, 0.90f};
    colors[ImGuiCol_Tab]                    = (ImVec4){0.36f, 0.31f, 0.28f, 0.79f};
    colors[ImGuiCol_TabHovered]             = (ImVec4){0.51f, 0.39f, 0.33f, 0.80f};
    colors[ImGuiCol_TabActive]              = (ImVec4){0.56f, 0.44f, 0.34f, 0.84f};
    colors[ImGuiCol_TabUnfocused]           = (ImVec4){0.57f, 0.40f, 0.28f, 0.82f};
    colors[ImGuiCol_TabUnfocusedActive]     = (ImVec4){0.65f, 0.45f, 0.35f, 0.84f};
    colors[ImGuiCol_DockingPreview]         = (ImVec4){0.90f, 0.58f, 0.40f, 0.31f};
    colors[ImGuiCol_DockingEmptyBg]         = (ImVec4){0.20f, 0.20f, 0.20f, 1.00f};
    colors[ImGuiCol_PlotLines]              = (ImVec4){1.00f, 1.00f, 1.00f, 1.00f};
    colors[ImGuiCol_PlotLinesHovered]       = (ImVec4){0.90f, 0.70f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotHistogram]          = (ImVec4){0.90f, 0.70f, 0.00f, 1.00f};
    colors[ImGuiCol_PlotHistogramHovered]   = (ImVec4){1.00f, 0.60f, 0.00f, 1.00f};
    colors[ImGuiCol_TableHeaderBg]          = (ImVec4){0.44f, 0.44f, 0.44f, 1.00f};
    colors[ImGuiCol_TableBorderStrong]      = (ImVec4){0.54f, 0.53f, 0.53f, 1.00f};
    colors[ImGuiCol_TableBorderLight]       = (ImVec4){0.33f, 0.33f, 0.33f, 1.00f};
    colors[ImGuiCol_TableRowBg]             = (ImVec4){0.00f, 0.00f, 0.00f, 0.00f};
    colors[ImGuiCol_TableRowBgAlt]          = (ImVec4){1.00f, 1.00f, 1.00f, 0.07f};
    colors[ImGuiCol_TextSelectedBg]         = (ImVec4){1.00f, 0.40f, 0.00f, 0.35f};
    colors[ImGuiCol_DragDropTarget]         = (ImVec4){1.00f, 1.00f, 0.00f, 0.90f};
    colors[ImGuiCol_NavHighlight]           = (ImVec4){0.90f, 0.90f, 0.45f, 0.80f};
    colors[ImGuiCol_NavWindowingHighlight]  = (ImVec4){1.00f, 1.00f, 1.00f, 0.70f};
    colors[ImGuiCol_NavWindowingDimBg]      = (ImVec4){0.80f, 0.80f, 0.80f, 0.20f};
    colors[ImGuiCol_ModalWindowDimBg]       = (ImVec4){0.20f, 0.20f, 0.20f, 0.35f};
}

v2i tile_id_to_row_col(int tile_id) {
    v2i v = {.col = tile_id % state.sprite_cols, .row = tile_id / state.sprite_cols };
    return v;
}

void init_board() {
    copy_board(state.game.board, initial_board);
}

void clear_move(move_t *m) {
    m->from.x = -1;
    m->from.y = -1;
    m->to.x = -1;
    m->to.y = -1;
}

bool moved_from(move_t m) {
    return (m.from.x >= 0 && m.from.y >= 0);
}

bool moved_to(move_t m) {
    return (m.to.x >= 0 && m.to.y >= 0);
}

void move_rook_if_castle() {
    if (is_move_castle(state.cur_move)) {
        // also move the rook
        v2i rook_from = { .x = (state.cur_move.from.x < state.cur_move.to.x) ? 7 : 0, .y = state.cur_move.from.y };
        v2i rook_to = { .x = (state.cur_move.from.x < state.cur_move.to.x) ? 5 : 3, .y = state.cur_move.from.y };
        PieceColor color = color_at(state.game.board, rook_from.x, rook_from.y);
        int piece_id = (color == WHITE) ? ROOK_W : ROOK_B;
        set_board(state.game.board, rook_to, piece_id);
        unset_board(state.game.board, rook_from);
    }
}

void remove_pawn_if_en_passant() {
    if (is_move_en_passant(state.game.board, state.cur_move)) {
        int dy = (state.cur_move.from.y - state.cur_move.to.y) / abs(state.cur_move.from.y - state.cur_move.to.y);
        v2i pos = { .x = state.cur_move.to.x, .y = state.cur_move.to.y + dy };
        unset_board(state.game.board, pos);
    }
}

void complete_move(move_t m) {
    utarray_push_back(state.game.moves, &m);
    remove_pawn_if_en_passant(); // this has to happen before making the move
    set_board(state.game.board, m.to, m.piece_id);
    move_rook_if_castle();
    PieceColor moved_color = color_at(state.game.board, m.to.x, m.to.y);
    PieceColor other_color = (moved_color == WHITE) ? BLACK : WHITE;
    if (is_checkmate(&state.game, other_color)) {
        state.status = CHECKMATE;
    }
}

void complete_cur_move() {
    if (!moved_from(state.cur_move) && !moved_to(state.cur_move)) return;
    complete_move(state.cur_move);
    /*
    utarray_push_back(state.game.moves, &state.cur_move);
    remove_pawn_if_en_passant(); // this has to happen before making the move
    set_board(state.game.board, state.cur_move.to, state.cur_move.piece_id);
    move_rook_if_castle();
    PieceColor moved_color = color_at(state.game.board, state.cur_move.to.x, state.cur_move.to.y);
    PieceColor other_color = (moved_color == WHITE) ? BLACK : WHITE;
    if (is_checkmate(&state.game, other_color)) {
        state.status = CHECKMATE;
    }
    */
    clear_move(&state.cur_move);
}

void initiate_engine_move() {
    int alen = utarray_len(state.game.moves);
    const char *prefix = "position startpos moves ";
    char cmd[(alen * 5)+26];
    char *c = cmd;
    strcpy(c, prefix);
    c += strlen(prefix);
    int i = 0;
    for (move_t *m=(move_t *)utarray_front(state.game.moves); m != NULL; m=(move_t *)utarray_next(state.game.moves, m)) {
        c[i++] = files[m->from.x];
        c[i++] = ranks[m->from.y];
        c[i++] = files[m->to.x];
        c[i++] = ranks[m->to.y];
        c[i++] = ' ';
    }
    c[i++] = '\n';
    c[i++] = '\0';
    printf("%s\n", cmd);
    fputs(cmd, state.client.out);
    fflush(state.client.out);
    fputs("go depth 3\n", state.client.out);
    fflush(state.client.out);
    str_t line = str_init();
    while (true) {
        str_getline(&line, state.client.in);
        printf("%s\n", line.buf);
        if (str_starts_with(line.buf, "bestmove")) break;
    }
    const char *emove = str_prefix(line.buf, "bestmove ");
    if (emove) {
        move_t m = str_to_move(state.game.board, emove);
        state.cur_move.piece_id = state.game.board[xy_to_board_idx(m.from.x, m.from.y)];
        state.cur_move.from.x = m.from.x;
        state.cur_move.from.y = m.from.y;
        state.cur_move.to.x = m.to.x;
        state.cur_move.to.y = m.to.y;
        unset_board(state.game.board, state.cur_move.from);
    }
}

void play_opening(const char *opening_moves_str) {
    init_board();
    utarray_clear(state.game.moves);
    size_t len = strlen(opening_moves_str);
    char *s = (char *)opening_moves_str;
    while (s < opening_moves_str+len) {
        move_t m = str_to_move(state.game.board, s);
        complete_move(m);
        unset_board(state.game.board, m.from);
        s += 5;
    }
    state.event_time = 0;
    int turn = (utarray_len(state.game.moves) % 2) + ((state.player_is_black) ? 2 : 1);
    clear_move(&state.cur_move);
    if ((turn % 2) == 0) {
        // engine's move
        initiate_engine_move();
        state.status = MOVING_OPPONENT;
    } else {
        // player's move
        state.status = AWAITING_MOVE;
    }
}

void print_piece(Piece p, const char *prefix) {
    printf("%s ", prefix);
    switch (p.color) {
        case WHITE: {
            printf("WHITE ");
            break;
        }
        case BLACK: {
            printf("BLACK ");
            break;
        }
        case NO_COLOR: {
            printf("NO_COLOR ");
        }
    }
    switch (p.type) {
        case ROOK: {
            printf("ROOK");
            break;
        }
        case KNIGHT: {
            printf("KNIGHT");
            break;
        }
        case BISHOP: {
            printf("BISHOP");
            break;
        }
        case QUEEN: {
            printf("QUEEN");
            break;
        }
        case KING: {
            printf("KING");
            break;
        }
        case PAWN: {
            printf("PAWN");
            break;
        }
        case NO_PIECE: {
            printf("NO_PIECE");
            break;
        }
    }
    printf("\n");
}

void play_test_moves() {
    init_board();
    const char *tm[24] = {
        "e2e4",
        "e7e6",
        "b1c3",
        "f8b4",
        "c3b5",
        "d7d5",
        "f2f3",
        "d8h4",
        "e1e2",
        "h4d8",
        "d2d4",
        "d5e4",
        "f3e4",
        "a7a6",
        "c2c3",
        "b4f8",
        "a2a4",
        "e6e5",
        "g2g4",
        "c8g4",
        "g1f3",
        "a6b5",
        "a4b5",
        "a8a1",
        //"f3e5",
    };
    PieceColor pc = WHITE;
    for (int i=0; i<24; i++) {
        const char * clr = (pc == WHITE) ? "WHITE" : "BLACK";
        const char *emove = tm[i];
        move_t m = {.from = {.x = emove[0] - 97, .y = emove[1] - 49}, .to = {.x = emove[2] - 97, .y = emove[3] - 49}};
        int fidx = v2i_to_board_idx(m.from);
        int tidx = v2i_to_board_idx(m.to);
        Piece from_piece = sprite_to_piece(state.game.board[fidx]);
        Piece to_piece = sprite_to_piece(state.game.board[tidx]);
        printf("\n-=-= %s %s (%d,%d)-(%d,%d)\n", clr, emove, m.from.x, m.from.y, m.to.x, m.to.y);
        print_piece(from_piece, "from_piece: ");
        print_piece(to_piece, "to_piece: ");
        utarray_push_back(state.game.moves, &m);
        state.game.board[tidx] = state.game.board[fidx];
        state.game.board[fidx] = -1;
        pc = (pc == WHITE) ? BLACK : WHITE;
    }
}

static void init(void) {
    srand(4580958);
    stm_setup();
    state.delta_time = 0;
    state.event_time = 0;
    state.opening_buf = malloc(16384);

    state.verts = (vertex_g  *)malloc(num_verts * sizeof(vertex_g));
    state.indices = (uint16_t *) malloc(num_indices * sizeof(uint16_t));

    sg_setup(&(sg_desc){ .context = sapp_sgcontext() });
    simgui_setup(&(simgui_desc_t){ .no_default_font = true });
    ImGuiIO* io = igGetIO();

    ImFontConfig *fontConfig = ImFontConfig_ImFontConfig();
    fontConfig->FontDataOwnedByAtlas = false;

    ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, __barlow_regular_ttf, __barlow_regular_ttf_len, 20.0f, fontConfig, NULL);
    printf("%d\n", fontConfig->FontNo);
    unsigned char* font_pixels;
    int font_width, font_height;
    int bytes_per_pixel;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &font_pixels, &font_width, &font_height, &bytes_per_pixel);
    sg_image_desc img_desc = { };
    img_desc.width = font_width;
    img_desc.height = font_height;
    img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    img_desc.data.subimage[0][0].ptr = font_pixels;
    img_desc.data.subimage[0][0].size = font_width * font_height * 4;
    sg_image font_img = sg_make_image(&img_desc);
    sg_sampler_desc smp_desc = { };
    smp_desc.min_filter = SG_FILTER_LINEAR;
    smp_desc.mag_filter = SG_FILTER_LINEAR;
    smp_desc.wrap_u = SG_WRAP_CLAMP_TO_EDGE;
    smp_desc.wrap_v = SG_WRAP_CLAMP_TO_EDGE;
    sg_sampler font_smp = sg_make_sampler(&smp_desc);
    simgui_image_desc_t font_desc = { };
    font_desc.image = font_img;
    font_desc.sampler = font_smp;
    io->Fonts->TexID = simgui_imtextureid(simgui_make_image(&font_desc));
    ImFontConfig_destroy(fontConfig);
    imgui_style();

    // initial clear color
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.0f, 0.5f, 1.0f, 1.0f } }
    };

    state.bind.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = num_verts * sizeof(vertex_g),
        .usage = SG_USAGE_DYNAMIC,
        .label = "cube-vertices"
    });

    state.bind.index_buffer = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .size = num_indices * sizeof(uint16_t),
        .usage = SG_USAGE_DYNAMIC,
        .label = "cube-indices"
    });

    int tw,th,tn;
    uint8_t *image_data = stbi_load_from_memory(__chess_png, __chess_png_len, &tw, &th, &tn, 0);
    sg_range pixels = { .ptr = image_data, .size = tw * th * tn * sizeof(uint8_t) };
    // NOTE: SLOT_tex is provided by shader code generation
    state.bind.fs.images[SLOT_tex] = sg_make_image(&(sg_image_desc){
        .width = tw,
        .height = th,
        .data.subimage[0][0] = pixels,
        .label = "cube-texture"
    });
    stbi_image_free(image_data);
    state.spritesheet_w = tw;
    state.spritesheet_h = th;
    state.sprite_size = 15;
    state.sprite_cols = state.spritesheet_w / state.sprite_size;
    state.sprite_rows = state.spritesheet_h / state.sprite_size;

    state.bind.fs.samplers[SLOT_smp] = sg_make_sampler(&(sg_sampler_desc){
        .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
        .wrap_w = SG_WRAP_CLAMP_TO_EDGE,
    });

    /* a shader */
    sg_shader shd = sg_make_shader(texcube_shader_desc(sg_query_backend()));

    /* a pipeline state object */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
        .layout = {
            .attrs = {
                [ATTR_vs_pos].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_color0].format = SG_VERTEXFORMAT_UBYTE4N,
                [ATTR_vs_texcoord0].format = SG_VERTEXFORMAT_SHORT2N
            }
        },
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_FRONT,
        .depth = {
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
            .write_enabled = true
        },
        .face_winding = SG_FACEWINDING_CW,
        .label = "cube-pipeline",
        .colors = {
            [0].blend = {
                .enabled = true,
                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                .src_factor_alpha = SG_BLENDFACTOR_ONE,
                .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
            }
        }
    });

    /* default pass action */
    state.pass_action = (sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.25f, 0.5f, 0.75f, 1.0f } }
    };

    state.cx = 1;
    state.cy = 4;
    state.input.tile_clicked.x = -1;
    state.input.tile_clicked.y = -1;
    state.sprites_across = 16.0f;
    compute_mvp();
    init_board();
    UT_icd move_icd = {sizeof(move_t), NULL, NULL, NULL};
    utarray_new(state.game.moves, &move_icd);
    const int move_buf_cap = 1024;
    state.game.avail = malloc(move_buf_cap * sizeof(v2i));
    state.game.avail_cap = move_buf_cap;
    //fork_uci_client("stockfish", &state.client);
    fork_uci_client("lc0", &state.client);

    fputs("uci\n", state.client.out);
    fflush(state.client.out);
    str_t line = str_init();
    while (true) {
        str_getline(&line, state.client.in);
        printf("%s\n", line.buf);
        if (str_starts_with(line.buf, "uciok")) break;
    }
    // lc0 maia option
    // fputs("setoption name WeightsFile value /Users/dmk/code/external/maia-chess/maia_weights/maia-1100.pb.gz\n", state.client.out);
    // fflush(state.client.out);
    // stockfish skill options
    // fputs("setoption name UCI_LimitStrength value true\n", state.client.out);
    // fflush(state.client.out);
    // fputs("setoption name UCI_Elo value 1320\n", state.client.out);
    // fflush(state.client.out);

    fputs("ucinewgame\n", state.client.out);
    fflush(state.client.out);
    fputs("isready\n", state.client.out);
    fflush(state.client.out);
    line = str_init();
    while (true) {
        str_getline(&line, state.client.in);
        printf("%s\n", line.buf);
        if (str_starts_with(line.buf, "readyok")) break;
    }
    clear_move(&state.cur_move);
    state.player_is_black = false;
    state.white_move = true;
    state.status = AWAITING_MOVE;
    //play_test_moves();
}

u_int32_t input_dir() {
    u_int32_t dir = 0;
    if (state.input.up) dir = dir | DU;
    if (state.input.down) dir = dir | DD;
    if (state.input.left) dir = dir | DL;
    if (state.input.right) dir = dir | DR;
    return dir;
}

v2i screen_to_tilemap(const float x, const float y) {
    const float pixels_per_sprite = (float)state.screen_w / state.sprites_across;
    const float sprites_updown = (float)state.screen_h / pixels_per_sprite;
    const float sx = state.cx - (state.sprites_across * 0.5f);
    const float sy = state.cy - (sprites_updown * 0.5f);
    const float scr_pct_across = x / (float)state.screen_w;
    // mouse screen coords start at top left and positive Y goes down, so we need to flip
    const float scr_pct_up = ((float)state.screen_h - y) / (float)state.screen_h;
    const float tiles_across = sx + (scr_pct_across * state.sprites_across);
    const float tiles_updown = sy + (scr_pct_up * sprites_updown);

    const v2i v = {.x = floorf(tiles_across), .y = floorf(tiles_updown) };
    return v;
}

bool is_available_move(const game_t *game, v2i pos) {
    if (game->avail_len == 0) return false;
    for (int i=0; i<game->avail_len; i++) {
        if (game->avail[i].x == pos.x && game->avail[i].y == pos.y) {
            return true;
        }
    }
    return false;
}

static void frame(void) {
    uint64_t lap_time = stm_laptime(&state.delta_time);
    state.event_time += lap_time;
	if (state.input.esc) {
	    printf("escape pressed\n");
		sapp_request_quit();
	}
    /*
    if (state.input.zoom_in) {
        state.sprites_across -= 1.0f;
        if (state.sprites_across < 0.0f) state.sprites_across = 20.0f;
    }
    if (state.input.zoom_out) {
        state.sprites_across += 1.0f;
        if (state.sprites_across > 200.0f) state.sprites_across = 200.0f;
    }
    if (state.input.scroll_amt > 0.01f || state.input.scroll_amt < -0.01f) {
        state.sprites_across -= state.input.scroll_amt;
        state.input.scroll_amt = 0.0f;
    }
    u_int32_t d = input_dir();
    if ((d & DU) == DU) state.cy += 1.0f;
    if ((d & DD) == DD) state.cy -= 1.0f;
    if ((d & DR) == DR) state.cx += 1.0f;
    if ((d & DL) == DL) state.cx -= 1.0f;
    */
    compute_mvp();

    if (state.input.paste) {
        printf("pasting...\n");
        const char *clip = igGetClipboardText();
        printf("clipboard: %s\n", clip);
        strcpy(state.opening_buf, clip);
        state.input.paste = false;
    }
    // handle user clicking on the board to move pieces
    if (state.status == AWAITING_MOVE && state.input.mouse_clicked) {
        if (state.white_move) {
            v2i tc = screen_to_tilemap(state.input.mcx, state.input.mcy);
            if (tc.x >= 0 && tc.x < 8 && tc.y >= 0 && tc.y < 8) {
                int bidx = xy_to_board_idx(tc.x, tc.y);
                if (moved_from(state.cur_move) && is_available_move(&state.game, tc)) {
                    // set end position - start moving piece that player is moving
                    state.cur_move.piece_id = state.game.board[xy_to_board_idx(state.cur_move.from.x, state.cur_move.from.y)];
                    state.cur_move.to.x = tc.x;
                    state.cur_move.to.y = tc.y;
                    state.input.tile_clicked.x = tc.x;
                    state.input.tile_clicked.y = tc.y;
                    state.game.avail_len = 0;
                    state.status = MOVING_PLAYER;
                    state.event_time = 0;
                    unset_board(state.game.board, state.cur_move.from);
                } else if (state.game.board[bidx] >= 0) {
                    // printf("calling valid_moves with tile_clicked: %d,%d\n", tc.x, tc.y);
                    // set start position - select piece that player is moving and calculate available moves
                    valid_moves(&state.game, tc);
                    /*
                    printf("available: %d ", state.game.avail_len);
                    for (int mi=0; mi<state.game.avail_len; mi++) {
                        printf(" %d,%d", state.game.avail[mi].x, state.game.avail[mi].y);
                    }
                    printf("\n");
                    */
                    state.cur_move.from.x = tc.x;
                    state.cur_move.from.y = tc.y;
                    state.input.tile_clicked.x = tc.x;
                    state.input.tile_clicked.y = tc.y;
                }
            }
        }
        state.input.mouse_clicked = false;
    }

    if (state.status == MOVING_PLAYER && stm_ms(state.event_time) >= move_time_ms) {
        // complete player move, select opponent move, and start moving opponent piece
        complete_cur_move();
        initiate_engine_move();
        state.status = MOVING_OPPONENT;
        state.event_time = 0;
    } else if (state.status == MOVING_OPPONENT && stm_ms(state.event_time) >= move_time_ms) {
        // complete opponent move and start awaiting player move
        complete_cur_move();
        state.status = AWAITING_MOVE;
        state.event_time = 0;
    }

    simgui_new_frame(&(simgui_frame_desc_t){
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });
    /*=== UI CODE STARTS HERE ===*/
    igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){400, 200}, ImGuiCond_Once);
    igBegin("test_window", 0, ImGuiWindowFlags_None);
    /*
    igBeginGroup();
    igRadioButton_IntPtr("walls", &state.build_item_idx, 0);
    igRadioButton_IntPtr("floors", &state.build_item_idx, 1);
    igRadioButton_IntPtr("belts", &state.build_item_idx, 2);
    igEndGroup();
    */
    /*
    igText("mouse: %0.2f,%0.2f", state.input.mx, state.input.my);
    igText("scroll: %0.2f", state.input.scroll_amt);
    */
    igText("tile clicked: %d, %d", state.input.tile_clicked.x, state.input.tile_clicked.y);
    igInputText("opening", state.opening_buf, 16384, ImGuiInputTextFlags_EscapeClearsAll, NULL, NULL);
    if (igButton("play opening", (ImVec2){.x = 120, .y = 40})) {
        int opening_len = strlen(state.opening_buf);
        if (opening_len > 0) {
            play_opening(state.opening_buf);
        }
    }
    igEnd();
    /*=== UI CODE ENDS HERE ===*/

    if (state.input.mouse_down) {
        v2i v = screen_to_tilemap(state.input.mx, state.input.my);
        //int tile_id = tile_id_at(v.x, v.y);
    }

    int color = 0;
    state.vidx = 0;
    state.iidx = 0;
    for (int y=0; y<8; y++) {
        color = (y + 1) % 2;
        for (int x=0; x<8; x++) {
            const v2i rc = tile_id_to_row_col((color == 0) ? SQUARE_B : SQUARE_W);
            add_quad_to_buffer(make_quad(x, y, 1, 1, rc.row, rc.col, 0));
            color = (color + 1) % 2;
        }
    }
    v2i crc = tile_id_to_row_col(FRAME_TL);
    add_quad_to_buffer(make_quad(-1, 8, 1, 1, crc.row, crc.col, 0));
    crc = tile_id_to_row_col(FRAME_TR);
    add_quad_to_buffer(make_quad(8, 8, 1, 1, crc.row, crc.col, 0));
    crc = tile_id_to_row_col(FRAME_BL);
    add_quad_to_buffer(make_quad(-1, -1, 1, 1, crc.row, crc.col, 0));
    crc = tile_id_to_row_col(FRAME_BR);
    add_quad_to_buffer(make_quad(8, -1, 1, 1, crc.row, crc.col, 0));
    for (int i=0; i<8; i++) {
        crc = tile_id_to_row_col(FRAME_T);
        add_quad_to_buffer(make_quad(i, 8, 1, 1, crc.row, crc.col, 0));
        crc = tile_id_to_row_col(FRAME_B);
        add_quad_to_buffer(make_quad(i, -1, 1, 1, crc.row, crc.col, 0));
        crc = tile_id_to_row_col(FRAME_L);
        add_quad_to_buffer(make_quad(-1, i, 1, 1, crc.row, crc.col, 0));
        crc = tile_id_to_row_col(FRAME_R);
        add_quad_to_buffer(make_quad(8, i, 1, 1, crc.row, crc.col, 0));
        crc = tile_id_to_row_col(LBL_1 + i);
        add_quad_to_buffer(make_quad(8, i, 1, 1, crc.row, crc.col, 0));
        crc = tile_id_to_row_col(LBL_A + i);
        add_quad_to_buffer(make_quad(i, -1, 1, 1, crc.row, crc.col, 0));
    }
    for (int i=0; i<64; i++) {
        const int piece_id = state.game.board[i];
        if (piece_id >= 0) {
            const int xx = i % 8;
            const int yy = i / 8;
            crc = tile_id_to_row_col(piece_id);
            add_quad_to_buffer(make_quad(xx, yy, 1, 1, crc.row, crc.col, 1));
        }
    }
    if (state.status == MOVING_PLAYER || state.status == MOVING_OPPONENT) {
        float pct_moved = stm_ms(state.event_time) / move_time_ms;
        float eased = CubicEaseOut(pct_moved);
        float xx = (float)state.cur_move.from.x + ((float)(state.cur_move.to.x - state.cur_move.from.x) * eased);
        float yy = (float)state.cur_move.from.y + ((float)(state.cur_move.to.y - state.cur_move.from.y) * eased);
        crc = tile_id_to_row_col(state.cur_move.piece_id);
        add_quad_to_buffer(make_quad(xx, yy, 1, 1, crc.row, crc.col, 3));
    } else if (state.status == AWAITING_MOVE) {
        if (state.input.tile_clicked.x >= 0 && state.input.tile_clicked.x < 8 && state.input.tile_clicked.y >= 0 && state.input.tile_clicked.y < 8) {
            crc = tile_id_to_row_col(HIGHLIGHT);
            add_quad_to_buffer(make_quad(state.input.tile_clicked.x, state.input.tile_clicked.y, 1, 1, crc.row, crc.col, 2));
        }
        for (int j=0; j<state.game.avail_len; j++) {
            crc = tile_id_to_row_col(DOT);
            add_quad_to_buffer(make_quad(state.game.avail[j].x, state.game.avail[j].y, 1, 1, crc.row, crc.col, 2));
        }
    }

    sg_range vrange = { state.verts, state.vidx * sizeof(vertex_g) };
    sg_range irange = { state.indices, state.iidx * sizeof(uint16_t) };
    sg_update_buffer(state.bind.vertex_buffers[0], &vrange);
    sg_update_buffer(state.bind.index_buffer, &irange);

    vs_params_t vs_params;
    vs_params.mvp = state.mvp;

    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
    sg_draw(0, (int32_t)state.iidx, 1);
    simgui_render();
    sg_end_pass();
    sg_commit();
}

static void cleanup(void) {
    simgui_shutdown();
    sg_shutdown();
    free(state.verts);
    free(state.indices);
    utarray_free(state.game.moves);
    free(state.opening_buf);
}

static void event_handler(const sapp_event* ev) {
    simgui_handle_event(ev);
    if (!ev) return;

    const bool mouse_scrolled = (ev->type == SAPP_EVENTTYPE_MOUSE_SCROLL);
    if (mouse_scrolled) {
        state.input.scroll_amt = ev->scroll_y;
    }
    if (ev->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
        state.input.md_time = stm_now();
        state.input.mouse_down = true;
        state.input.mx = ev->mouse_x;
        state.input.my = ev->mouse_y;
        state.input.mdx = state.input.mx;
        state.input.mdy = state.input.my;
    }
    if (ev->type == SAPP_EVENTTYPE_MOUSE_UP) {
        float mxd = fabs(state.input.mdx - ev->mouse_x);
        float myd = fabs(state.input.mdy - ev->mouse_y);
        if (mxd < 10 && myd < 10) {
            state.input.mcx = ev->mouse_x;
            state.input.mcy = ev->mouse_y;
            state.input.mouse_clicked = true;
        }
        state.input.mouse_down = false;
        state.input.mx = ev->mouse_x;
        state.input.my = ev->mouse_y;
    }
    if (ev->type == SAPP_EVENTTYPE_MOUSE_MOVE) {
        state.input.mx = ev->mouse_x;
        state.input.my = ev->mouse_y;
    }
    const bool key_pressed = (ev->type == SAPP_EVENTTYPE_KEY_DOWN);
    switch (ev->key_code) {
        case SAPP_KEYCODE_W:
        case SAPP_KEYCODE_UP: {
            state.input.up = key_pressed;
            break;
        }
        case SAPP_KEYCODE_S:
        case SAPP_KEYCODE_DOWN: {
            state.input.down = key_pressed;
            break;
        }
        case SAPP_KEYCODE_A:
        case SAPP_KEYCODE_LEFT: {
            state.input.left = key_pressed;
            break;
        }
        case SAPP_KEYCODE_D:
        case SAPP_KEYCODE_RIGHT: {
            state.input.right = key_pressed;
            break;
        }
        case SAPP_KEYCODE_O: {
            state.input.zoom_in = key_pressed;
            break;
        }
        case SAPP_KEYCODE_L: {
            state.input.zoom_out = key_pressed;
            break;
        }
        case SAPP_KEYCODE_V: {
            if (key_pressed) {
                if ((ev->modifiers & MODIFIER_KEY) == MODIFIER_KEY) {
                    state.input.paste = true;
                }
            }
            break;
        }
        case SAPP_KEYCODE_ESCAPE: {
            state.input.esc = key_pressed;
            break;
        }
        default: {}
    }
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    state.screen_w = 1280;
    state.screen_h = 800;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event_handler,
        .window_title = "chess",
        .width = state.screen_w,
        .height = state.screen_h,
        .sample_count = 1,
        //.gl_force_gles2 = true,
        //.high_dpi = true,
        .icon.sokol_default = true,
        .enable_clipboard = true,
        .clipboard_size = 16384,
    };
}
