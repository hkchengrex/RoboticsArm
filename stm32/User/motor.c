#include "motor.h"
#include "mymath.h"

volatile s32 motor_pwm_value[MOTOR_COUNT] = {0, 0, 0};

void motor_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	//PWM Pins
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = MOTOR1_MAG_Pin;
  GPIO_Init(MOTOR1_MAG_GPIOx, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOTOR2_MAG_Pin;
  GPIO_Init(MOTOR2_MAG_GPIOx, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOTOR3_MAG_Pin;
  GPIO_Init(MOTOR3_MAG_GPIOx, &GPIO_InitStructure);
	
	//DIR Pins
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = MOTOR1_DIR_Pin;
  GPIO_Init(MOTOR1_DIR_GPIOx, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOTOR2_DIR_Pin;
  GPIO_Init(MOTOR2_DIR_GPIOx, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = MOTOR3_DIR_Pin;
  GPIO_Init(MOTOR3_DIR_GPIOx, &GPIO_InitStructure);
	
	MOTOR_TIM_RCC_init();
	
	//Now init pwm output
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_DeInit(MOTOR_TIM);
	
	TIM_TimeBaseStructure.TIM_Period = MAX_PWM;
	TIM_TimeBaseStructure.TIM_ClockDivision =  TIM_CKD_DIV1;
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(MOTOR_TIM, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	
	TIM_OC1Init(MOTOR_TIM, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);
	TIM_OC2Init(MOTOR_TIM, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);
	TIM_OC3Init(MOTOR_TIM, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(MOTOR_TIM, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(MOTOR_TIM, ENABLE);

	TIM_Cmd(MOTOR_TIM, ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_TIM, ENABLE);
	
	motor_set_power(MOTOR_1, 0);
	motor_set_power(MOTOR_2, 0);
	motor_set_power(MOTOR_3, 0);
}

void motor_set_power(MotorID motor, s32 power){
	u8 direction = 0;
	if (power < 0) direction = 1;
	
	motor_pwm_value[motor] = power;
	
	power = ABS(power);
	if (power > MAX_PWM_OUTPUT) power = MAX_PWM_OUTPUT;
	
	switch(motor){
		case MOTOR_1:
			TIM_SetCompare1(MOTOR_TIM, MAX_PWM_OUTPUT-power);
			GPIO_WriteBit(MOTOR1_DIR_GPIOx, MOTOR1_DIR_Pin, (BitAction)direction);
			break;
		
		case MOTOR_2:
			TIM_SetCompare2(MOTOR_TIM, MAX_PWM_OUTPUT-power);
			GPIO_WriteBit(MOTOR2_DIR_GPIOx, MOTOR2_DIR_Pin, (BitAction)direction);
			break;
		
		case MOTOR_3:
			TIM_SetCompare3(MOTOR_TIM, MAX_PWM_OUTPUT-power);
			GPIO_WriteBit(MOTOR3_DIR_GPIOx, MOTOR3_DIR_Pin, (BitAction)direction);
			break;
	}
}

