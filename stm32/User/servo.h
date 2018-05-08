#ifndef _SERVO_H
#define _SERVO_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define SERVO_TIM								TIM4
#define SERVO_TIM_RCC_init()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)

#define SERVO1_GPIOx					GPIOB
#define SERVO1_Pin						GPIO_Pin_6

#define SERVO2_GPIOx					GPIOC
#define SERVO2_Pin						GPIO_Pin_1

typedef enum{
	SERVO_1, 
	SERVO_2
}ServoID;

void servo_init(void);
void servo_set_deg(ServoID servo, s32 deg);
void servo_update(void);

#endif
