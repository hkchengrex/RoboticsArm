#ifndef _COLOR_H
#define _COLOR_H

#include "stm32f10x_tim.h"

typedef struct{
	u8 h;
	u8 s;
	u8 v;
}ColorTuple;

ColorTuple rgb_to_hsv(u8 r, u8 g, u8 b);

#endif
