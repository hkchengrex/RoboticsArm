#include "stm32f10x.h"
#include "lcd.h"
#include "math.h"

#include "ticks.h"
#include "led.h"
#include "gpio.h"

#include "encoder.h"
#include "motor.h"

#include "cam/bsp_ov7725.h"
#include "cam/bsp_sccb.h"

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
	
	OV7725_GPIO_Config();
	while(OV7725_Init() != SUCCESS);
	
	OV7725_Special_Effect(cam_mode.effect);
	OV7725_Light_Mode(cam_mode.light_mode);
	OV7725_Color_Saturation(cam_mode.saturation);
	OV7725_Brightness(cam_mode.brightness);
	OV7725_Contrast(cam_mode.contrast);
	OV7725_Special_Effect(cam_mode.effect);
	
	OV7725_Window_Set(cam_mode.cam_sx,
														cam_mode.cam_sy,
														cam_mode.cam_width,
														cam_mode.cam_height,
														cam_mode.QVGA_VGA);
	
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

		static s32 last_cam_ticks = 0;
		if (this_ticks - last_cam_ticks >= 100){
			if(Ov7725_vsync == 2){
				FIFO_PREPARE;
				ImagDisp(cam_mode.lcd_sx,
									cam_mode.lcd_sy,
									cam_mode.cam_width,
									cam_mode.cam_height);
				
				Ov7725_vsync = 0;	
				last_cam_ticks = this_ticks;
			}
		}
		
		motor_set_power(MOTOR_1, 5000);
		motor_set_power(MOTOR_2, 0);
		motor_set_power(MOTOR_3, 10000);
		motor_set_power(MOTOR_4, 10000);
  }
}
