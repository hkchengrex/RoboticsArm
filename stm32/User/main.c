#include "stm32f10x.h"
#include "lcd.h"
#include "math.h"

#include "ticks.h"
#include "led.h"
#include "gpio.h"

#include "encoder.h"
#include "motor.h"
#include "servo.h"
#include "control.h"

#include "cam/bsp_ov7725.h"
#include "cam/bsp_sccb.h"

#include "button.h"
#include "cam_algo.h"

static s32 motor_init_pos[MOTOR_COUNT] = {-540, -300, -300};
static s32 motor_pos_1[MOTOR_COUNT] = {-800, -300, -600};
static s32 motor_pos_2[MOTOR_COUNT] = {-400, -450, -400};

int main(void)
{
	gpio_rcc_init_all();

	//provided
  LCD_INIT();
	
	ticks_init();
	//led_init();
	encoder_init();
	motor_init();
	button_init();
	
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
	
	servo_init();
	
	motor_set_pos(MOTOR_1, 2000);
  while (1) {
		u32 this_ticks = get_ticks();
		
		static u32 last_led_ticks = 0;
		if (this_ticks - last_led_ticks >= 200){
			//led_blink(LED_GREEN);
			last_led_ticks = this_ticks;
		}
		
		static s32 last_ctrl_ticks = 0;
		/**
		* 0 - init motor 3
		* 1 - init motor 1
		* 2 - init motor 2
		* 3 - motor 2 restore
		* 4 - motor 3 restore
		* 5 - motor 1 restore
		* 6 - Ready
		*/
		static u8 motor_init_state = 0;
		if (this_ticks - last_ctrl_ticks >= (1000/CONTROL_FREQ)){
			encoder_update();
			switch(motor_init_state){
				case 0:
					motor_control_on[MOTOR_1] = 0;
					motor_control_on[MOTOR_2] = 0;
					motor_control_on[MOTOR_3] = 0;
					motor_set_power(MOTOR_1, 0);
					motor_set_power(MOTOR_2, 0);
					motor_set_power(MOTOR_3, 3500);
					if (!GPIO_ReadInputDataBit(ENCODER_3_RESET_PORT, ENCODER_3_RESET_PIN)){
						motor_init_state++;
						motor_set_power(MOTOR_3, 0);
						encoder_reset(ENCODER_3);
					}
					break;
				case 1:
					motor_set_power(MOTOR_1, 3500);
					motor_set_power(MOTOR_2, 0);
					motor_set_power(MOTOR_3, 0);
					if (!GPIO_ReadInputDataBit(ENCODER_1_RESET_PORT, ENCODER_1_RESET_PIN)){
						motor_init_state++;
						motor_set_power(MOTOR_1, 0);
						encoder_reset(ENCODER_1);
					}
					break;
				case 2:
					motor_set_power(MOTOR_1, 0);
					motor_set_power(MOTOR_2, 3500);
					motor_set_power(MOTOR_3, 0);
					if (!GPIO_ReadInputDataBit(ENCODER_2_RESET_PORT, ENCODER_2_RESET_PIN)){
						motor_init_state++;
						motor_set_power(MOTOR_1, 0);
						motor_set_power(MOTOR_2, 0);
						motor_set_power(MOTOR_3, 0);
						encoder_reset(ENCODER_2);
						motor_control_on[MOTOR_2] = 1;
						motor_set_pos(MOTOR_2, motor_init_pos[MOTOR_2]);
					}
					break;
				case 3:
					if (motor_arrived[MOTOR_2]){
						motor_init_state++;
						motor_control_on[MOTOR_2] = 0;
						motor_set_power(MOTOR_2, 0);
						
						motor_control_on[MOTOR_1] = 1;
						motor_set_pos(MOTOR_1, motor_init_pos[MOTOR_1]);
					}
					break;
					
				case 4:
					if (motor_arrived[MOTOR_2]){
						motor_init_state++;
						motor_control_on[MOTOR_1] = 0;
						motor_set_power(MOTOR_1, 0);
						
						motor_control_on[MOTOR_3] = 1;
						motor_set_pos(MOTOR_3, motor_init_pos[MOTOR_3]);
					}
					break;
					
				case 5:
					if (motor_arrived[MOTOR_3]){
						motor_init_state++;
						motor_control_on[MOTOR_1] = 1;
						motor_control_on[MOTOR_2] = 1;
						motor_control_on[MOTOR_3] = 1;
					}
					break;
					
				case 6:
					//Demo
					if (button_pressed(BUTTON_K1)){
						motor_set_pos(MOTOR_1, motor_pos_1[MOTOR_1]);
						motor_set_pos(MOTOR_2, motor_pos_1[MOTOR_2]);
						motor_set_pos(MOTOR_3, motor_pos_1[MOTOR_3]);
					}else if (button_pressed(BUTTON_K2)){
						motor_set_pos(MOTOR_1, motor_pos_2[MOTOR_1]);
						motor_set_pos(MOTOR_2, motor_pos_2[MOTOR_2]);
						motor_set_pos(MOTOR_3, motor_pos_2[MOTOR_3]);
					}
			}

			control_update();			
			last_ctrl_ticks = this_ticks;
		}

		static u16 locate_x = 0, locate_y = 0;
		static u32 last_flash_ticks = 0;
		if (this_ticks - last_flash_ticks >= 50){
			LCD_Clear();
			LCD_Printf(0, 0, "%d", this_ticks);
			LCD_Printf(0, 1, "P: %d V: %d", encoder_cnt[0], encoder_vel[0]);
			LCD_Printf(0, 2, "P: %d V: %d", encoder_cnt[1], encoder_vel[1]);
			LCD_Printf(0, 3, "P: %d V: %d", encoder_cnt[2], encoder_vel[2]);
			LCD_Printf(0, 4, "P: %d V:%d", motor_tar_pos[0], motor_tar_vel[0]);
			LCD_Printf(0, 5, "X: %d Y: %d", locate_x, locate_y);
			LCD_Printf(0, 6, "Value: %d", CameraData[locate_x][locate_y]);
			LCD_Printf(0, 7, "S:%d %d %d %d", motor_init_state, GPIO_ReadInputDataBit(ENCODER_1_RESET_PORT, ENCODER_1_RESET_PIN),
													button_pressed(BUTTON_K1), button_pressed(BUTTON_K2));
			LCD_Update();
			
			last_flash_ticks = this_ticks;
		}
		
		if (button_pressed(BUTTON_K1)){
			servo_set_deg(SERVO_1, 1000);
			servo_set_deg(SERVO_2, 2000);
			//while(1){}
		}else if (button_pressed(BUTTON_K2)){
			servo_set_deg(SERVO_1, 2000);
			servo_set_deg(SERVO_2, 3100);
			//while(1){}
		}
		
		static s32 last_cam_ticks = 0;
		if (this_ticks - last_cam_ticks >= 100){
			if(Ov7725_vsync == 2){
				FIFO_PREPARE;
				ImagLoadAndDisp(cam_mode.lcd_sx,
									cam_mode.lcd_sy,
									READ_RED);
				
				locate_red(&locate_x, &locate_y, 5, 10);
				draw_locator(locate_x+cam_mode.lcd_sx, locate_y+cam_mode.lcd_sy,
											cam_mode.lcd_sx, cam_mode.lcd_sy,
											cam_mode.lcd_sx+CAM_WIDTH, cam_mode.lcd_sy+CAM_HEIGHT);
				
				Ov7725_vsync = 0;	
				last_cam_ticks = this_ticks;
			}
		}
  }
}
