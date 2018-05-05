#ifndef _BUTTON_H
#define _BUTTON_H

#include "stm32f10x_tim.h"
#include "system_stm32f10x.h"
#include "misc.h"
#include "define.h"

typedef enum{
	BUTTON_K1,
	BUTTON_K2
} ButtonID;

void button_init(void);
u8 button_pressed(ButtonID id);

#endif
