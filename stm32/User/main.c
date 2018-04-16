#include "stm32f10x.h"
#include "lcd.h"
#include "math.h"

#include "ticks.h"
#include "led.h"
#include "gpio.h"

#include "encoder.h"
#include "motor.h"

int main(void)
{
	//provided
  LCD_INIT();
	
	//my init
	gpio_rcc_init_all();
	ticks_init();
	led_init();
	encoder_init();
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
			LCD_Printf(0, 0, "%d", this_ticks);
			LCD_Printf(0, 1, "P: %d V: %d", encoder_cnt[0], encoder_vel[0]);
			LCD_Printf(0, 2, "P: %d V: %d", encoder_cnt[1], encoder_vel[1]);
			LCD_Printf(0, 3, "P: %d V: %d", encoder_cnt[2], encoder_vel[2]);
			LCD_Update();
			
			last_flash_ticks = this_ticks;
		}
		
		motor_set_power(MOTOR_1, 5000);
		motor_set_power(MOTOR_2, 0);
		motor_set_power(MOTOR_3, 10000);
		motor_set_power(MOTOR_4, 10000);
  }
}
