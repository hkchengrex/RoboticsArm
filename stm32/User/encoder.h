#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"

#define ENCODER_COUNT 4

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

typedef enum{
	ENCODER_1,
	ENCODER_2,
	ENCODER_3,
	ENCODER_4
}EncoderID;

extern volatile s32 encoder_cnt[ENCODER_COUNT];
extern volatile s32 encoder_vel[ENCODER_COUNT];

extern volatile s32 inter_count;

void encoder_init(void);
void encoder_update(void);

#endif
