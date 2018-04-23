#ifndef _CAM_ALGO_H
#define _CAM_ALGO_H

#include "stm32f10x_tim.h"
#include "system_stm32f10x.h"
#include "misc.h"
#include "define.h"
#include "cam/bsp_ov7725.h"

void locate_red(u16 *x, u16 *y, u16 size, u32 threshold);
void draw_locator(u16 x, u16 y, u16 min_x, u16 min_y, u16 max_x, u16 max_y);

#endif
