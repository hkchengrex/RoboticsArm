#ifndef _SERVO_H
#define _SERVO_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define SERVO_TIM								TIM1
#define SERVO_TIM_RCC_init()		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE)

#define SERVO1_GPIOx					GPIOA
#define SERVO1_Pin						GPIO_Pin_8

#define SERVO2_GPIOx					GPIOA
#define SERVO2_Pin						GPIO_Pin_9

typedef enum{
	SERVO_1, 
	SERVO_2
}ServoID;

void servo_init(void);
void servo_set_deg(ServoID servo, s32 deg);

#endif
