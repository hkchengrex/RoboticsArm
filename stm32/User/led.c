#include "led.h"

void led_init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_0, Bit_SET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, Bit_SET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
}

void led_set(LedColor color, BitAction state){
	switch (color){
		case LED_RED:
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction)!state);
			break;
		case LED_GREEN:
			GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction)!state);
			break;
		case LED_BLUE:
			GPIO_WriteBit(GPIOB, GPIO_Pin_1, (BitAction)!state);
			break;
	}
}

void led_blink(LedColor color){	
	
	/*
		Twinkle, twinkle, little star,
		How we wonder what you are.
		Up above the world so high,
		Like a diamond in the sky.
	*/
	
	switch (color){
		case LED_RED:
			GPIO_WriteBit(GPIOB, GPIO_Pin_5, (BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_5));
			break;
		case LED_GREEN:
			GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0));
			break;
		case LED_BLUE:
			GPIO_WriteBit(GPIOB, GPIO_Pin_1, (BitAction) !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1));
			break;
	}
}
