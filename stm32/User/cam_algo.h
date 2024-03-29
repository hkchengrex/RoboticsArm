#ifndef _CAM_ALGO_H
#define _CAM_ALGO_H

#include "stm32f10x_tim.h"
#include "system_stm32f10x.h"
#include "misc.h"
#include "define.h"
#include "mymath.h"
#include "cam/bsp_ov7725.h"

#define MAX_RED_TO_BE_FOUND 4
#define SEARCH_RED_STEP 2
#define DIST_THRE 20

#define CENTER_DIST_DIFF_THRE 20
#define NUM_OF_GRIDS 5

#define DIST_TIMES_16(p1, p2) (Sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y)) / 64)

#define DIST_TIMES_16_XY(x1, y1, x2, y2) (Sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)) / 64)

typedef struct{
    s16 x;
    s16 y;
} Point;

extern Point red_located[MAX_RED_TO_BE_FOUND];
extern u8 cur_red_located_pt;
extern s32 rel_dist[MAX_RED_TO_BE_FOUND][MAX_RED_TO_BE_FOUND];
extern s32 found_data;
extern u8 data_array[25];
extern Point point_c, point_1, point_2;

Point double_recur_center(const Point p, u8 thre, u8* width, u8* height);
void locate_rgb(s16 *rx, s16 *ry, s16 *gx, s16 *gy, s16 *bx, s16 *by, u16 size, u32 threshold);
void draw_locator(u16 x, u16 y, u16 min_x, u16 min_y, u16 max_x, u16 max_y, u16 color);
void smooth_red(void);

void find_pattern(void);

#endif
