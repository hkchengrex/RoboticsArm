#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"

#define ENCODER_COUNT 3

#define ENCODER_1_A_PORT GPIOC
#define ENCODER_1_A_PIN GPIO_Pin_8
#define ENCODER_1_A_PORT_SRC GPIO_PortSourceGPIOC
#define ENCODER_1_A_PIN_SRC GPIO_PinSource8
#define ENCODER_1_A_LINE EXTI_Line8

#define ENCODER_1_B_PORT GPIOC
#define ENCODER_1_B_PIN GPIO_Pin_9
#define ENCODER_1_B_PORT_SRC GPIO_PortSourceGPIOC
#define ENCODER_1_B_PIN_SRC GPIO_PinSource9
#define ENCODER_1_B_LINE EXTI_Line9

#define ENCODER_1_RESET_PORT GPIOC
#define ENCODER_1_RESET_PIN GPIO_Pin_11

#define ENCODER_2_A_PORT GPIOA
#define ENCODER_2_A_PIN GPIO_Pin_4
#define ENCODER_2_A_PORT_SRC GPIO_PortSourceGPIOA
#define ENCODER_2_A_PIN_SRC GPIO_PinSource4
#define ENCODER_2_A_LINE EXTI_Line4

#define ENCODER_2_B_PORT GPIOA
#define ENCODER_2_B_PIN GPIO_Pin_5
#define ENCODER_2_B_PORT_SRC GPIO_PortSourceGPIOA
#define ENCODER_2_B_PIN_SRC GPIO_PinSource5
#define ENCODER_2_B_LINE EXTI_Line5

#define ENCODER_2_RESET_PORT GPIOA
#define ENCODER_2_RESET_PIN GPIO_Pin_10

#define ENCODER_3_A_PORT GPIOB
#define ENCODER_3_A_PIN GPIO_Pin_1
#define ENCODER_3_A_PORT_SRC GPIO_PortSourceGPIOB
#define ENCODER_3_A_PIN_SRC GPIO_PinSource1
#define ENCODER_3_A_LINE EXTI_Line1

#define ENCODER_3_B_PORT GPIOD
#define ENCODER_3_B_PIN GPIO_Pin_2
#define ENCODER_3_B_PORT_SRC GPIO_PortSourceGPIOD
#define ENCODER_3_B_PIN_SRC GPIO_PinSource2
#define ENCODER_3_B_LINE EXTI_Line2

#define ENCODER_3_RESET_PORT GPIOA
#define ENCODER_3_RESET_PIN GPIO_Pin_12

typedef enum{
	ENCODER_1,
	ENCODER_2,
	ENCODER_3
}EncoderID;

extern volatile s32 encoder_cnt[ENCODER_COUNT];
extern volatile s32 encoder_vel[ENCODER_COUNT];

void encoder_init(void);
void encoder_update(void);
void encoder_reset(EncoderID id);

#endif
