#include "stm32f10x.h"
#include "lcd.h"
#include "math.h"

#include "ticks.h"
#include "led.h"
#include "gpio.h"

#include "motor.h"

int main(void)
{
	//provided
  LCD_INIT();
	
	//my init
	gpio_rcc_init_all();
	ticks_init();
	led_init();
	motor_init();
	
  while (1) {
		u32 this_ticks = get_ticks();
		
		static u32 last_led_ticks = 0;
		if (this_ticks - last_led_ticks >= 200){
			//led_blink(LED_GREEN);
			last_led_ticks = this_ticks;
		}
		
		static u32 last_flash_ticks = 0;
		if (this_ticks - last_flash_ticks >= 100){
			LCD_Clear();
			LCD_Printf(0, 0, "Hello %d", this_ticks);
			LCD_Update();
			
			last_flash_ticks = this_ticks;
		}
		
		motor_set_power(MOTOR_1, 5000);
  }
}
