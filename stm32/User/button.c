#include "button.h"

void button_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

u8 button_pressed(ButtonID id){
	if (id == BUTTON_K1){
		return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
	}else{
		return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
	}
}

