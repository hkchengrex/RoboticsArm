#include "ticks.h"
#include "control.h"
#include "encoder.h"

volatile u32 ms_ticks = 0;

void ticks_init(void){
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(TICKS_RCC, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 1000;
	TIM_TimeBaseStructure.TIM_Prescaler = 143; 
	TIM_TimeBaseInit(TICKS_TIM, &TIM_TimeBaseStructure); 
	
	TIM_ClearITPendingBit(TICKS_TIM, TIM_IT_Update);
	TIM_ITConfig(TICKS_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(TICKS_TIM, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TICKS_IRQn;   
	NVIC_Init(&NVIC_InitStructure);
}

TICKS_IRQHandler{
	//What is ticking? 
	//Is it you, or is it the universe itself?
	//What defines time?
	//What if time itself is slowing down, and you simply cannot tell?
	ms_ticks++;
	
//	if (ms_ticks % (1000/CONTROL_FREQ) == 0){
//		//encoder_update();
//		//control_update();
//	}
	
	TIM_ClearITPendingBit(TICKS_TIM, TIM_IT_Update);
}
