#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirent.h>
#include <stdbool.h>

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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "jsmn.h"
#include "misc_util.h"
#include "game.glsl.h"
#include "utarray.h"
#include "IconsFontAwesome5.h"
#include "map_defs.h"
#include "tile_defs.h"

#define BLK_VL 0
#define BLK_H 1
#define BLK_VR 2
#define BLK_VBL 3
#define BLK_VBR 4
#define BLK_VC 5
#define BLK_HV 6
#define FLOOR_1 7
#define FLOOR_2 8
#define FLOOR_3 9
#define FLOOR_4 10
#define BELT_LT 16
#define BELT_LR 17
#define BELT_LB 18
#define BELT_RT 19
#define BELT_TB 20
#define BELT_RB 21
#define BELT_LTB 22
#define BELT_LRT 23
#define BELT_RTB 24
#define BELT_LRB 25
#define BELT_LRTB 26
#define SPLT_LTB 32
#define SPLT_LRT 33
#define SPLT_RTB 34
#define SPLT_LRB 35
#define SPLT_LRTB 36
#define OUT_L 37
#define OUT_T 38
#define OUT_R 39
#define OUT_B 40
#define IN_L 41
#define IN_T 42
#define IN_R 43
#define IN_B 44

#define ARROW_BL 48
#define ARROW_BR 64
#define ARROW_BT 80
#define ARROW_LB 96
#define ARROW_LR 112
#define ARROW_LT 128
#define ARROW_RB 144
#define ARROW_RL 160
#define ARROW_RT 176
#define ARROW_TB 192
#define ARROW_TL 208
#define ARROW_TR 224

const uint32_t num_quads = 150000;
const uint32_t num_verts = num_quads * 4;
const uint32_t num_indices = num_quads * 6;
uint32_t frame_num;

static uint32_t alt_img_id = 0;
static float icon_uv_w;
static float icon_uv_h;
static ImFont *reg_font;
static ImFont *icon_font;

/*
typedef struct {
    int len;
    int tiles[NUM_TILES];
} tile_list;

typedef struct {
    int32_t tile_idx;
    int32_t entropy;
    tile_list poss;
} grid_loc;

typedef struct {
    int grid_w;
    int grid_h;
    grid_loc *locs;
} grid;

// a place to keep a list of grid locations
// with the same entropy.
typedef struct {
    int len;
    int *loc_indices;
} entropy_list;

typedef struct {
    int idx;
    uint32_t sides[4];
} tile;
*/

enum Side {
    TopSide,
    RightSide,
    BottomSide,
    LeftSide,
};

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
} input_g;

enum heading {
    Backward,
    Forward,
    Right,
    Left,
    Up,
    Down
};

const u_int32_t DN = 0;
const u_int32_t DL = 0x00000001;
const u_int32_t DU = 0x00000002;
const u_int32_t DR = 0x00000004;
const u_int32_t DD = 0x00000008;

const u_int32_t RED = 0xFF0000FF;
const u_int32_t GREEN = 0xFF00FF00;
const u_int32_t BLUE = 0xFFFF0000;
const u_int32_t PURPLE = 0xFFFF007F;
const u_int32_t CYAN = 0xFFFFFF00;
const u_int32_t YELLOW = 0xFF00FFFF;

static struct {
    int screen_w;
    int screen_h;
    float rx, ry;
    sg_pass_action pass_action;
    sg_pipeline pip;
    sg_bindings bind;
    float cang;
    float cdist;
    float cx, cy;
    hmm_v3 cpos;
    hmm_mat4 mvp;
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
    t_tile *tiles;
    int *tile_idxs;
    //grid g;
    //entropy_list elist;
} state;

/*
void set_tile_bools_to_all(bool tb[NUM_TILES]) {
    for (int ti=0; ti<NUM_TILES; ti++) {
        tb[ti] = true;
    }
}

void set_tiles_to_all(tile_list *tl) {
    if (tl->len == NUM_TILES) return;
    tl->len = NUM_TILES;
    for (int ti=0; ti<NUM_TILES; ti++) {
        tl->tiles[ti] = ti;
    }
}

void set_tiles_to_none(tile_list *tl) {
    tl->len = 0;
}

// Get the set of tiles that a grid location could be given the constraints
// of one of its adjacent locations.
void get_possible_tiles(grid *g, int x, int y, enum Side side, bool tile_bools[NUM_TILES]) {
    int dx = x;
    int dy = y;
    int dside = 0;
    switch (side) {
        case TopSide: {
            dy--;
            dside = BottomSide;
            break;
        }
        case RightSide: {
            dx++;
            dside = LeftSide;
            break;
        }
        case BottomSide: {
            dy++;
            dside = TopSide;
            break;
        }
        case LeftSide: {
            dx--;
            dside = RightSide;
            break;
        }
    }
    if ((dx < 0) || (dx >= g->grid_w) || (dy < 0) || (dy >= g->grid_h)) {
        set_tile_bools_to_all(tile_bools);
        return;
    }
    int didx = (dy * g->grid_w) + dx;
    int dlen = g->locs[didx].poss.len;
    // if the grid location we're comparing to can be any tile, we can too
    if (dlen == NUM_TILES) {
        set_tile_bools_to_all(tile_bools);
        return;
    }
    for (int i=0; i<dlen; i++) {
        int tidx = g->locs[didx].poss.tiles[i];
        uint32_t match = tiles[tidx].sides[dside];
        for (int j=0; j<NUM_TILES; j++) {
            if (tiles[j].sides[side] == match) {
                tile_bools[j] = true;
            }
        }
    }
}

int32_t update_grid_loc(grid *g, int x, int y) {
    int idx = (y * g->grid_w) + x;
    bool tbt[NUM_TILES] = { 0 };
    bool tbr[NUM_TILES] = { 0 };
    bool tbb[NUM_TILES] = { 0 };
    bool tbl[NUM_TILES] = { 0 };
    // find the possible tiles on each side
    get_possible_tiles(g, x, y, TopSide, tbt);
    get_possible_tiles(g, x, y, RightSide, tbr);
    get_possible_tiles(g, x, y, BottomSide, tbb);
    get_possible_tiles(g, x, y, LeftSide, tbl);
    // the final list will be the intersection of the lists from the sides
    int cnt = 0;
    for (int i=0; i<NUM_TILES; i++) {
        if (tbt[i] && tbr[i] && tbb[i] && tbl[i]) {
            g->locs[idx].poss.tiles[cnt] = i;
            cnt++;
        }
    }
    g->locs[idx].poss.len = cnt;
    g->locs[idx].entropy = cnt; // this could be weighted
    // if there's only one possibility left, we've resolved the grid location
    if (cnt == 1) g->locs[idx].tile_idx =  g->locs[idx].poss.tiles[0];
    return g->locs[idx].entropy;
}

int32_t get_min_entropy(grid *g, entropy_list *el) {
    int32_t min_entropy = NUM_TILES * 1000; // todo: fix this - it's arbitrary
    int cnt = 0;
    int num_locs = g->grid_w * g->grid_h;
    int i;
    for (i=0; i<num_locs; i++) {
        if (g->locs[i].entropy > 1 && g->locs[i].entropy < min_entropy) {
            min_entropy = g->locs[i].entropy;
        }
    }
    for (i=0; i<num_locs; i++) {
        if (g->locs[i].entropy == min_entropy) {
            el->loc_indices[cnt] = i;
            cnt++;
        }
    }
    el->len = cnt;
    return min_entropy;
}

int count_undecided(grid *g) {
    int cnt = 0;
    int num_locs = g->grid_w * g->grid_h;
    for (int i=0; i<num_locs; i++) {
        if (g->locs[i].entropy > 1) {
            cnt++;
        }
    }
    return cnt;
}

void collapse_some_wave_functions() {
    int undecided_locs = (state.g.grid_w * state.g.grid_h);
    int iter_cnt = 0;
    while (undecided_locs > 0) {
        // get the minimum entropy and choose a grid loc with that entropy
        int32_t min_entropy = get_min_entropy(&state.g, &state.elist);
        int idx_to_collapse = (state.elist.len == 1) ? state.elist.loc_indices[0] : state.elist.loc_indices[(rand() % state.elist.len)];

        // update the chosen tile with its decided information
        int poss_len = state.g.locs[idx_to_collapse].poss.len;
        int *poss_tiles = state.g.locs[idx_to_collapse].poss.tiles;
        int tile_idx = (poss_len == 1) ? state.g.locs[idx_to_collapse].poss.tiles[0] : state.g.locs[idx_to_collapse].poss.tiles[(rand() % poss_len)];
        state.g.locs[idx_to_collapse].tile_idx = tile_idx;
        state.g.locs[idx_to_collapse].entropy = 1;
        state.g.locs[idx_to_collapse].poss.len = 1;
        state.g.locs[idx_to_collapse].poss.tiles[0] = tile_idx;

        // update the constraints/entropy of the neighboring tiles
        int x = (idx_to_collapse % state.g.grid_w);
        int y = (idx_to_collapse / state.g.grid_w);
        // above
        int xx = x;
        int yy = y-1;
        bool entropy_didnt_change = true;
        while (yy >= 0 && entropy_didnt_change) {
            int32_t entropy_before = state.g.locs[(yy * state.g.grid_w)+xx].entropy;
            if (entropy_before == 1) break;
            int32_t entropy_after = update_grid_loc(&state.g, xx, yy);
            entropy_didnt_change = (entropy_before == entropy_after);
            yy--;
        }
        // right
        xx = x+1;
        yy = y;
        entropy_didnt_change = true;
        while (xx < state.g.grid_w && entropy_didnt_change) {
            int32_t entropy_before = state.g.locs[(yy * state.g.grid_w)+xx].entropy;
            if (entropy_before == 1) break;
            int32_t entropy_after = update_grid_loc(&state.g, xx, yy);
            entropy_didnt_change = (entropy_before == entropy_after);
            xx++;
        }
        // below
        xx = x;
        yy = y+1;
        entropy_didnt_change = true;
        while (yy < state.g.grid_h && entropy_didnt_change) {
            int32_t entropy_before = state.g.locs[(yy * state.g.grid_w)+xx].entropy;
            if (entropy_before == 1) break;
            int32_t entropy_after = update_grid_loc(&state.g, xx, yy);
            entropy_didnt_change = (entropy_before == entropy_after);
            yy++;
        }
        // right
        xx = x-1;
        yy = y;
        entropy_didnt_change = true;
        while (xx >= 0 && entropy_didnt_change) {
            int32_t entropy_before = state.g.locs[(yy * state.g.grid_w)+xx].entropy;
            if (entropy_before == 1) break;
            int32_t entropy_after = update_grid_loc(&state.g, xx, yy);
            entropy_didnt_change = (entropy_before == entropy_after);
            xx--;
        }
        undecided_locs = count_undecided(&state.g);
        //printf("iter: %d, undecided: %d\n", iter_cnt, undecided_locs);
        iter_cnt++;
    }
    for (int i=0; i<(state.g.grid_w * state.g.grid_h); i++) {
        if (state.g.locs[i].tile_idx < 0) {
            printf("idx: %d, entropy: %d, tile cnt: %d\n", i, state.g.locs[i].entropy, state.g.locs[i].poss.len);
        }
    }
    for (int y=0; y<state.g.grid_h; y++) {
        for (int x=0; x<state.g.grid_w; x++) {
            int lidx = (y * state.g.grid_w) + x;
            int tidx = state.g.locs[lidx].tile_idx;
            if (tidx >= 0) {
                printf("tile at %d,%d is: %d %d\n", x, y, state.g.locs[lidx].tile_idx,
                       tiles[state.g.locs[lidx].tile_idx].idx);
            }
        }
    }
}
*/

// x,y: destination coords
// w,h: width and height of sprite (in tiles)
// row,col: the row and column of the sprite's bottom right corner
quad_g make_quad(int x, int y, int w, int h, int row, int col) {
    uint16_t uvw_unit = (uint16_t)(32768 / state.sprite_cols);
    uint16_t uvh_unit = (uint16_t)(32768 / state.sprite_rows);
    quad_g q;
    float ww = (float)w;
    float hh = (float)h;
    float xx = (float)x;
    float yy = (float)y;
    //int16_t uu = (int16_t)(col * 2048);
    //int16_t vv = (int16_t)(row * 2048);
    /*
    q.verts[0] = vertex( xx - ww, yy, 0.0f, YELLOW, (col + w) * uvw_unit, (row + 1) * uvh_unit );
    q.verts[1] = vertex( xx, yy, 0.0f, YELLOW, col * uvw_unit,  (row + 1) * uvh_unit );
    q.verts[2] = vertex( xx, yy + hh, 0.0f, YELLOW, col * uvw_unit,  (row - hh + 1) * uvh_unit);
    q.verts[3] = vertex( xx - ww, yy + hh, 0.0f, YELLOW, (col + w) * uvw_unit, (row - hh + 1) * uvh_unit );
    */

    q.verts[0] = vertex( xx, yy, 0.0f, YELLOW, col * uvw_unit, (row + 1) * uvh_unit );
    q.verts[1] = vertex( xx + ww, yy, 0.0f, YELLOW, (col + w) * uvw_unit,  (row + 1) * uvh_unit );
    q.verts[2] = vertex( xx + ww, yy + hh, 0.0f, YELLOW, (col + w) * uvw_unit,  (row - hh + 1) * uvh_unit);
    q.verts[3] = vertex( xx, yy + hh, 0.0f, YELLOW, col * uvw_unit, (row - hh + 1) * uvh_unit );

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

hmm_mat4 ortho_camera_mat(hmm_v3 cpos, float pps) {
    float half_w = ((float)state.screen_w / pps) * 0.5f;
    float half_h = ((float)state.screen_h / pps) * 0.5f;
    hmm_v3 cat = HMM_Vec3(state.cx, state.cy, 0.0f);
    hmm_v3 up = HMM_Vec3(0.0f, 1.0f, 0.0f);
    hmm_mat4 p_mat = HMM_Orthographic(-half_w, half_w, -half_h, half_h, 1.0f, 200.0f);
    hmm_mat4 m_mat = HMM_Mat4d(1.0f);
    hmm_mat4 v_mat = HMM_LookAt(cpos, cat, up);
    return HMM_MultiplyMat4(HMM_MultiplyMat4(p_mat, v_mat), m_mat);
}

void compute_mvp() {
    //state.cpos = HMM_Vec3(state.cdist * cosf(state.cang), state.cy, state.cdist * sinf(state.cang));
    state.cpos = HMM_Vec3(state.cx, state.cy, 5.0f);
    float pps = (float)state.screen_w / 6.0f;
    state.mvp = ortho_camera_mat(state.cpos, pps);
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

static void init(void) {
    frame_num = 0;
    srand(4580958);
    /*
    state.g.grid_w = 32;
    state.g.grid_h = 18;
    state.g.locs = (grid_loc *)malloc(state.g.grid_w * state.g.grid_h * sizeof(grid_loc));
    state.elist.loc_indices = (int32_t *) malloc(state.g.grid_w * state.g.grid_h * sizeof(int32_t));

    for (int y=0; y<state.g.grid_h; y++) {
        for (int x=0; x<state.g.grid_w; x++) {
            int idx = (y*state.g.grid_w) + x;
            state.g.locs[idx].tile_idx = -1;
            state.g.locs[idx].entropy = NUM_TILES;
            set_tiles_to_all(&state.g.locs[idx].poss);
        }
    }
    */

    state.verts = (vertex_g  *)malloc(num_verts * sizeof(vertex_g));
    state.indices = (uint16_t *) malloc(num_indices * sizeof(uint16_t));

    sg_setup(&(sg_desc){ .context = sapp_sgcontext() });
    simgui_setup(&(simgui_desc_t){ .no_default_font = true });
    ImGuiIO* io = igGetIO();


    static ImFontConfig config;
    // #define ICON_MIN_FA 0xe005
    // #define ICON_MAX_FA 0xf8ff

    static ImWchar icon_ranges[] = { 0x0020, ICON_MAX_FA, 0 };
    config.FontDataOwnedByAtlas = false;
    //config.OversampleH = 2;
    //config.OversampleV = 2;
    //config.RasterizerMultiply = 1.5f;

    ImFontAtlas_AddFontFromFileTTF(io->Fonts, "/Users/dmk/code/gaem/data/barlow.regular.ttf", 20.0f, NULL, icon_ranges);
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

    const char *sprite_filename = "/Users/dmk/code/factory/data/sproutlands.png";
    int tw,th,tn;
    uint8_t *image_data = stbi_load(sprite_filename, &tw, &th, &tn, 0);
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
    state.sprite_size = 16;
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

    state.cx = 0.0f;
    state.cy = 0.0f;
    state.cang = TWO_PI / 4.0f;
    state.cdist = 7.0717f;

    compute_mvp();
    /*
    collapse_some_wave_functions();

    const int tt = tm.cols * tm.rows;
    for (int i=0; i<tm.layers; i++) {
        const int *layer = tm.map[i];
        for (int j=0; j<tt; j++) {
            printf("%d ", layer[j]);
        }
        printf("\n");
    }
    */
    state.tiles = create_tiles();
    int max_tile_id = 0;
    int i;
    for (i=0; i<NUM_TILES; i++) {
        if (state.tiles[i].id > max_tile_id) {
            max_tile_id = state.tiles[i].id;
        }
    }
    state.tile_idxs = malloc(max_tile_id * sizeof(int));
    for (i=0; i<max_tile_id; i++) {
        state.tile_idxs[i] = -1;
    }
    for (i=0; i<NUM_TILES; i++) {
        state.tile_idxs[state.tiles[i].id] = i;
    }
}

u_int32_t input_dir() {
    u_int32_t dir = 0;
    if (state.input.up) dir = dir | DU;
    if (state.input.down) dir = dir | DD;
    if (state.input.left) dir = dir | DL;
    if (state.input.right) dir = dir | DR;
    return dir;
}

void load_json() {
    const char *json_data = load_file("/Users/dmk/code/game/data/dogish.json");
    unsigned long json_data_len = strlen(json_data);
    printf("json data is %ld bytes\n", json_data_len);
    jsmn_parser p;
    jsmntok_t t[1024];
    int ntoks = jsmn_parse(&p, json_data, json_data_len, t, 1024);
    printf("got %d tokens\n", ntoks);
    char s[1024*8];
    for (int i=0; i<ntoks; i++) {
        switch (t[i].type) {
            case 1: {
                printf("%d\tOBJECT\t%d\t%d\n", i, t[i].start, t[i].end);
                break;
            }
            case 2: {
                printf("%d\tARRAY  \t%d\t%d\n",i, t[i].start, t[i].end);
                break;
            }
            case 4: {
                printf("%d\tSTRING \t%d\t%d\n",i, t[i].start, t[i].end);
                int len = (t[i].end - t[i].start);
                strncpy(s, json_data + t[i].start, len);
                s[len] = '\0';
                printf("%s\n",s);
                break;
            }
            case 8: {
                printf("%d\tPRIMIT \t%d\t%d\n",i, t[i].start, t[i].end);
                int len = (t[i].end - t[i].start);
                strncpy(s, json_data + t[i].start, len);
                s[len] = '\0';
                printf("%s\n",s);
                break;
            }
            default: {
                printf("%d\tUNKNOWN \t%d\n", i, t[i].type);
                break;
            }
        }
    }
    free((void *)json_data);
}

void find_dirs(DIR *dir, const char *path, size_t pathlen, UT_array *array) {
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_namlen > 0 && entry->d_type == DT_DIR && entry->d_name[0] != '.') {
            // 2 is 1 for / and one for terminator
            size_t namelen = (pathlen + entry->d_namlen + 2) * sizeof(char);
            char *name = (char *) malloc(namelen);
            strcpy(name, path);
            strcat(name, "/");
            strcat(name, entry->d_name);
            utarray_push_back(array, &name);
            DIR *ndir = opendir(name);
            if (ndir != NULL) {
                find_dirs(ndir, name, namelen, array);
                closedir(ndir);
            }
        }
    }
}

static void frame(void) {
    frame_num++;
    if ((frame_num % 60) == 0) {
        //printf("item: %d\n", state.build_item_idx);
        frame_num = 0;
    }
	if (state.input.esc) {
		sapp_request_quit();
	}
    u_int32_t d = input_dir();
    if ((d & DU) == DU) state.cy += 1.0f;
    if ((d & DD) == DD) state.cy -= 1.0f;
    if ((d & DR) == DR) state.cx += 1.0f;
    if ((d & DL) == DL) state.cx -= 1.0f;
    compute_mvp();
    simgui_new_frame(&(simgui_frame_desc_t){
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });

    /*=== UI CODE STARTS HERE ===*/
    igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){200, 200}, ImGuiCond_Once);
    igBegin("test_window", 0, ImGuiWindowFlags_None);
    igBeginGroup();
    igRadioButton_IntPtr("walls", &state.build_item_idx, 0);
    igRadioButton_IntPtr("floors", &state.build_item_idx, 1);
    igRadioButton_IntPtr("belts", &state.build_item_idx, 2);
    igEndGroup();

    //igPushFont(font);
    //igPushFont(reg_font);
    //if (igButton(ICON_FA_MUSIC "midi", (ImVec2){100.0f, 30.0f})) {
    //}
    //if (igButton(ICON_FA_MUSIC "gen", (ImVec2){100.0f, 30.0f})) {
    //}
    // igImage(ImTextureID user_texture_id,const ImVec2 size,const ImVec2 uv0,const ImVec2 uv1,const ImVec4 tint_col,const ImVec4 border_col)
    // igImage((ImTextureID)alt_img_id, (ImVec2){48.0f, 48.0f}, (ImVec2){0.0f, 0.0f}, (ImVec2){1.0f, 1.0f}, (ImVec4){1.0f,1.0f,1.0f,1.0f}, (ImVec4){0.0f,0.0f,0.0f,0.0f});
    //igColorEdit3("Background", &state.pass_action.colors[0].value.r, ImGuiColorEditFlags_None);
    //igPopFont();
    //igShowFontSelector("font selector");
    // igLabelText("portmidi error", "%d", pm_error);
    // igLabelText("portmidi in id", "%d", pm_in_id);
    // igLabelText("portmidi out id", "%d", pm_out_id);
    //igLabelText("Does this font look any good?", "I have no idea");

    igEnd();
    /*=== UI CODE ENDS HERE ===*/


    state.vidx = 0;
    state.iidx = 0;
    for (int l=0; l<4; l++) {
        for (int y=0; y<tm.rows; y++) {
            for (int x=0; x<tm.cols; x++) {
                int tidx = (y * tm.cols) + x;
                int tile_id = tm.map[l][tidx];
                if (tile_id >= 0) {
                    /*
                    if (tile_id != 54) {
                        printf("%d %d %d\n", x, y, tile_id);
                    }
                    */
                    int real_y = tm.rows - y;
                    t_tile tile = state.tiles[state.tile_idxs[tile_id]];
                    int row = tile_id / state.sprite_rows;
                    int col = tile_id % state.sprite_rows;
                    add_quad_to_buffer(make_quad(x, real_y, tile.tw, tile.th, row, col));
                }
            }
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
    free(state.tile_idxs);
    free_tiles(state.tiles);
    //free(state.g.locs);
    //free(state.elist.loc_indices);
}

static void event_handler(const sapp_event* ev) {
    simgui_handle_event(ev);
    if (!ev) return;
    bool key_pressed = (ev->type == SAPP_EVENTTYPE_KEY_DOWN);
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
    state.screen_h = 720;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event_handler,
        .window_title = "gaem",
        .width = state.screen_w,
        .height = state.screen_h,
        .sample_count = 4,
        //.gl_force_gles2 = true,
        .icon.sokol_default = true,
    };
}
