#ifndef _LED_H
#define _LED_H

#include "stm32f10x_tim.h"
#include "system_stm32f10x.h"
#include "misc.h"
#include "define.h"

typedef enum{
	LED_RED,
	LED_GREEN,
	LED_BLUE
} LedColor;

void led_init(void);
void led_set(LedColor color, BitAction state);
void led_blink(LedColor color);

#endif
