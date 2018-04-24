#include "servo.h"
#include "mymath.h"

void servo_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	//PWM Pins
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = SERVO1_Pin;
  GPIO_Init(SERVO1_GPIOx, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SERVO2_Pin;
  GPIO_Init(SERVO2_GPIOx, &GPIO_InitStructure);
	
	SERVO_TIM_RCC_init();
	
	//Now init pwm output
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	//TIM_DeInit(SERVO_TIM);
	
	TIM_TimeBaseStructure.TIM_Period = 20000;
	TIM_TimeBaseStructure.TIM_ClockDivision =  TIM_CKD_DIV1;
	
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(SERVO_TIM, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC1Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);
	
	TIM_OC2Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);
	
	TIM_OC3Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);
	
	TIM_OC4Init(SERVO_TIM, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(SERVO_TIM, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(SERVO_TIM, ENABLE);

	TIM_Cmd(SERVO_TIM, ENABLE);
	TIM_CtrlPWMOutputs(SERVO_TIM, ENABLE);
	
	servo_set_deg(SERVO_1, 80);
	servo_set_deg(SERVO_2, 100);
}

void servo_set_deg(ServoID servo, s32 deg){
	switch(servo){
		case SERVO_1:
			TIM_SetCompare1(SERVO_TIM, deg*1200/180 + 900);
			break;
		
		case SERVO_2:
			TIM_SetCompare2(SERVO_TIM, deg*1200/180 + 900);
			break;
	}
}

