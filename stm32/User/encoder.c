#include "encoder.h"
#include "define.h"

volatile s32 encoder_cnt[ENCODER_COUNT] = {0};
volatile s32 encoder_vel[ENCODER_COUNT] = {0};

static volatile s32 temp_enc_store[ENCODER_COUNT] = {0};

static u8 lastReading[ENCODER_COUNT][2] = {0};

void encoder_init(){
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Externel interrupt init
	EXTI_InitStructure.EXTI_Line = ENCODER_1_A_LINE | ENCODER_1_B_LINE | 
			ENCODER_2_A_LINE | ENCODER_2_B_LINE | ENCODER_3_A_LINE | ENCODER_3_B_LINE;
	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	
	EXTI_Init(&EXTI_InitStructure);
	
	GPIO_EXTILineConfig(ENCODER_1_A_PORT_SRC, ENCODER_1_A_PIN_SRC);
	GPIO_EXTILineConfig(ENCODER_1_B_PORT_SRC, ENCODER_1_B_PIN_SRC);
	GPIO_EXTILineConfig(ENCODER_2_A_PORT_SRC, ENCODER_2_A_PIN_SRC);
	GPIO_EXTILineConfig(ENCODER_2_B_PORT_SRC, ENCODER_2_B_PIN_SRC);
	GPIO_EXTILineConfig(ENCODER_3_A_PORT_SRC, ENCODER_3_A_PIN_SRC);
	GPIO_EXTILineConfig(ENCODER_3_B_PORT_SRC, ENCODER_3_B_PIN_SRC);
	
	//NVIC init
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);	

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_1_A_PIN;
	GPIO_Init(ENCODER_1_A_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_1_B_PIN;
	GPIO_Init(ENCODER_1_B_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_2_A_PIN;
	GPIO_Init(ENCODER_2_A_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_2_B_PIN;
	GPIO_Init(ENCODER_2_B_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_3_A_PIN;
	GPIO_Init(ENCODER_3_A_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_3_B_PIN;
	GPIO_Init(ENCODER_3_B_PORT, &GPIO_InitStructure);
	
	//Init reset pins
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_InitStructure.GPIO_Pin = ENCODER_1_RESET_PIN;
	GPIO_Init(ENCODER_1_RESET_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ENCODER_2_RESET_PIN;
	GPIO_Init(ENCODER_2_RESET_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ENCODER_3_RESET_PIN;
	GPIO_Init(ENCODER_3_RESET_PORT, &GPIO_InitStructure);
}

void encoder_update(){
	for (u8 i=0; i<ENCODER_COUNT; i++){
		encoder_vel[i] = temp_enc_store[i]*CONTROL_FREQ;
		encoder_cnt[i] += temp_enc_store[i];
		temp_enc_store[i] = 0;
	}
}

void encoder_reset(EncoderID id){
	encoder_vel[id] = 0;
	encoder_cnt[id] = 0;
	temp_enc_store[id] = 0;
}

void EXTI9_5_IRQHandler(void){
	if (EXTI_GetITStatus(ENCODER_1_A_LINE) != RESET || EXTI_GetITStatus(ENCODER_1_B_LINE) != RESET) {
		u8 aReading, bReading;
		aReading = GPIO_ReadInputDataBit(ENCODER_1_A_PORT, ENCODER_1_A_PIN);
		bReading = GPIO_ReadInputDataBit(ENCODER_1_B_PORT, ENCODER_1_B_PIN);

		if (bReading ^ lastReading[0][0]){
			temp_enc_store[0]++;
		}
		
		if (aReading ^ lastReading[0][1]){
			temp_enc_store[0]--;
		}
		
		lastReading[0][0] = aReading;
		lastReading[0][1] = bReading;
		
		EXTI_ClearITPendingBit(ENCODER_1_A_LINE);
		EXTI_ClearITPendingBit(ENCODER_1_B_LINE);
		
	}else if (EXTI_GetITStatus(ENCODER_2_B_LINE) != RESET){
		u8 aReading, bReading;
		aReading = GPIO_ReadInputDataBit(ENCODER_2_A_PORT, ENCODER_2_A_PIN);
		bReading = GPIO_ReadInputDataBit(ENCODER_2_B_PORT, ENCODER_2_B_PIN);

		if (bReading ^ lastReading[1][0]){
			temp_enc_store[1]++;
		}
		
		if (aReading ^ lastReading[1][1]){
			temp_enc_store[1]--;
		}
		
		lastReading[1][0] = aReading;
		lastReading[1][1] = bReading;
		
		EXTI_ClearITPendingBit(ENCODER_2_B_LINE);
		
	}else if (EXTI_GetITStatus(ENCODER_3_A_LINE) != RESET || EXTI_GetITStatus(ENCODER_3_B_LINE) != RESET){
		u8 aReading, bReading;
		aReading = GPIO_ReadInputDataBit(ENCODER_3_A_PORT, ENCODER_3_A_PIN);
		bReading = GPIO_ReadInputDataBit(ENCODER_3_B_PORT, ENCODER_3_B_PIN);

		if (bReading ^ lastReading[2][0]){
			temp_enc_store[2]++;
		}
		
		if (aReading ^ lastReading[2][1]){
			temp_enc_store[2]--;
		}
		
		lastReading[2][0] = aReading;
		lastReading[2][1] = bReading;
		
		EXTI_ClearITPendingBit(ENCODER_3_A_LINE);
		EXTI_ClearITPendingBit(ENCODER_3_B_LINE);
	}
}

void EXTI4_IRQHandler(void){
	if (EXTI_GetITStatus(ENCODER_2_A_LINE) != RESET) {
		u8 aReading, bReading;
		aReading = GPIO_ReadInputDataBit(ENCODER_2_A_PORT, ENCODER_2_A_PIN);
		bReading = GPIO_ReadInputDataBit(ENCODER_2_B_PORT, ENCODER_2_B_PIN);

		if (bReading ^ lastReading[1][0]){
			temp_enc_store[1]++;
		}
		
		if (aReading ^ lastReading[1][1]){
			temp_enc_store[1]--;
		}
		
		lastReading[1][0] = aReading;
		lastReading[1][1] = bReading;
		
		EXTI_ClearITPendingBit(ENCODER_2_A_LINE);
	}
}
