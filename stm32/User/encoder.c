#include "encoder.h"
#include "define.h"

volatile s32 encoder_cnt[ENCODER_COUNT] = {0};
volatile s32 encoder_vel[ENCODER_COUNT] = {0};

static volatile s32 last_enc_cnt[ENCODER_COUNT] = {0};

static u8 lastReading[ENCODER_COUNT][2] = {0};
volatile s32 inter_count = 0;

void encoder_init(){
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Externel interrupt init
	EXTI_InitStructure.EXTI_Line = ENCODER_1_A_LINE | ENCODER_1_B_LINE;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(ENCODER_1_A_PORT_SRC, ENCODER_1_A_PIN_SRC);
	GPIO_EXTILineConfig(ENCODER_1_B_PORT_SRC, ENCODER_1_B_PIN_SRC);
	
	//NVIC init
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_1_A_PIN;
	GPIO_Init(ENCODER_1_A_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_1_B_PIN;
	GPIO_Init(ENCODER_1_B_PORT, &GPIO_InitStructure);
}

void encoder_update(){
	for (u8 i=0; i<3; i++){
		encoder_vel[i] = (encoder_cnt[i] - last_enc_cnt[i])*CONTROL_FREQ;
		last_enc_cnt[i] = encoder_cnt[i];
	}
}

void EXTI9_5_IRQHandler(void){
	if (EXTI_GetITStatus(ENCODER_1_A_LINE) != RESET || EXTI_GetITStatus(ENCODER_1_B_LINE) != RESET) {
		inter_count++;
		u8 aReading, bReading;
		aReading = GPIO_ReadInputDataBit(ENCODER_1_A_PORT, ENCODER_1_A_PIN);
		bReading = GPIO_ReadInputDataBit(ENCODER_1_B_PORT, ENCODER_1_B_PIN);

		if (bReading ^ lastReading[0][0]){
			encoder_cnt[0]++;
		}
		
		if (aReading ^ lastReading[0][1]){
			encoder_cnt[0]--;
		}
		
		lastReading[0][0] = aReading;
		lastReading[0][1] = bReading;
		
		EXTI_ClearITPendingBit(ENCODER_1_A_LINE);
		EXTI_ClearITPendingBit(ENCODER_1_B_LINE);
	}
}
