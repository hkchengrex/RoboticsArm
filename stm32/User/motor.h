#ifndef _MOTOR_H
#define _MOTOR_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

#define MAX_PWM									10000
#define MAX_PWM_OUTPUT				  10000

#define MOTOR_TIM								TIM4
#define MOTOR_TIM_RCC_init()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)

//PWM: TIM4, PB6789
//DIR: PA2, PA6
#define MOTOR1_MAG_GPIOx					GPIOB
#define MOTOR1_MAG_Pin						GPIO_Pin_6
#define MOTOR1_DIR_GPIOx					GPIOA
#define MOTOR1_DIR_Pin						GPIO_Pin_2

#define MOTOR2_MAG_GPIOx					GPIOB
#define MOTOR2_MAG_Pin						GPIO_Pin_7
#define MOTOR2_DIR_GPIOx					GPIOA
#define MOTOR2_DIR_Pin						GPIO_Pin_5

#define MOTOR3_MAG_GPIOx					GPIOB
#define MOTOR3_MAG_Pin						GPIO_Pin_8
#define MOTOR3_DIR_GPIOx					GPIOC
#define MOTOR3_DIR_Pin						GPIO_Pin_6

#define MOTOR4_MAG_GPIOx					GPIOB
#define MOTOR4_MAG_Pin						GPIO_Pin_9
#define MOTOR4_DIR_GPIOx					GPIOC
#define MOTOR4_DIR_Pin						GPIO_Pin_7

typedef enum{
	MOTOR_1, 
	MOTOR_2, 
	MOTOR_3, 
	MOTOR_4
}MotorID;

void motor_init(void);
void motor_set_power(MotorID motor, s32 power);

#endif
