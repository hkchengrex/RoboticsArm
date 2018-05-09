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

static s32 motor_init_pos[MOTOR_COUNT] = {-1000, 400, -200};
static s32 motor_pos_1[MOTOR_COUNT] = {-1000, 800, -750};
static s32 motor_pos_2[MOTOR_COUNT] = {-1000, 600, -550};
static s32 motor_pos_3[MOTOR_COUNT] = {-300, 600, -550};
static s32 motor_pos_4[MOTOR_COUNT] = {-300, 800, -750};

static s32 motor_init_pwm[MOTOR_COUNT] = {5500, -5500, 5500};

//2 is close, 1 is open
static s32 servo_deg_1[2] = {160, 2};
static s32 servo_deg_2[2] = {160, 1};

u8 use_cam = 0;
u8 grab_state = 0;

int main(void)
{
	gpio_rcc_init_all();

	//provided
	LCD_INIT();
	
	ticks_init();
	encoder_init();
	motor_init();
	button_init();
	servo_init();
	
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
		
		static u32 last_ticks = 0;
		if (this_ticks == last_ticks) continue;
		last_ticks = this_ticks;
		
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
		* 4 - motor 1 restore
		* 5 - motor 3 restore
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
					motor_set_power(MOTOR_3, motor_init_pwm[MOTOR_3]);
					if (!GPIO_ReadInputDataBit(ENCODER_3_RESET_PORT, ENCODER_3_RESET_PIN)){
						motor_init_state = 5;
						motor_set_power(MOTOR_3, 0);
						encoder_reset(ENCODER_3);
					}
					break;
				case 1:
					motor_set_power(MOTOR_1, motor_init_pwm[MOTOR_1]);
					motor_set_power(MOTOR_2, 0);
					motor_set_power(MOTOR_3, 0);
					if (!GPIO_ReadInputDataBit(ENCODER_1_RESET_PORT, ENCODER_1_RESET_PIN)){
						motor_init_state = 4;
						motor_set_power(MOTOR_1, 0);
						encoder_reset(ENCODER_1);
					}
					break;
				case 2:
					motor_set_power(MOTOR_1, 0);
					motor_set_power(MOTOR_2, motor_init_pwm[MOTOR_2]);
					motor_set_power(MOTOR_3, 0);
					if (!GPIO_ReadInputDataBit(ENCODER_2_RESET_PORT, ENCODER_2_RESET_PIN)){
						motor_init_state = 3;
						motor_set_power(MOTOR_1, 0);
						motor_set_power(MOTOR_2, 0);
						motor_set_power(MOTOR_3, 0);
						encoder_reset(ENCODER_2);
					}
					break;
				case 3:
					motor_control_on[MOTOR_2] = 1;
					motor_set_pos(MOTOR_2, motor_init_pos[MOTOR_2]);
					if (motor_arrived[MOTOR_2]){
						motor_init_state = 1;
						motor_control_on[MOTOR_2] = 0;
						motor_set_power(MOTOR_2, 0);
					}
					break;
					
				case 4:
					motor_control_on[MOTOR_1] = 1;
					motor_set_pos(MOTOR_1, motor_init_pos[MOTOR_1]);
					if (motor_arrived[MOTOR_1]){
						motor_init_state = 6;
						motor_control_on[MOTOR_1] = 0;
						motor_set_power(MOTOR_1, 0);
					}
					break;
					
				case 5:
					motor_control_on[MOTOR_3] = 1;
					motor_set_pos(MOTOR_3, motor_init_pos[MOTOR_3]);
					if (motor_arrived[MOTOR_3]){
						motor_init_state = 2;
						motor_control_on[MOTOR_3] = 0;
						motor_set_power(MOTOR_3, 0);
					}
					break;
					
				case 6:
					//Demo
					motor_control_on[MOTOR_1] = 1;
					motor_control_on[MOTOR_2] = 1;
					motor_control_on[MOTOR_3] = 1;
					if (button_pressed(BUTTON_K1)){
						grab_state = 1;
					}else if (button_pressed(BUTTON_K2)){
//						motor_set_pos(MOTOR_1, motor_pos_2[MOTOR_1]);
//						motor_set_pos(MOTOR_2, motor_pos_2[MOTOR_2]);
//						motor_set_pos(MOTOR_3, motor_pos_2[MOTOR_3]);
//						
//						servo_set_deg(SERVO_1, servo_deg_2[SERVO_1]);
//						servo_set_deg(SERVO_2, servo_deg_2[SERVO_2]);
//						use_cam = 0;
					}
					
					static u32 g1_down_arrive_ticks = 0;
					static u32 g2_down_arrive_ticks = 0;
					switch (grab_state){
						case 1:
							motor_set_pos(MOTOR_1, motor_pos_1[MOTOR_1]);
							motor_set_pos(MOTOR_2, motor_pos_1[MOTOR_2]);
							motor_set_pos(MOTOR_3, motor_pos_1[MOTOR_3]);
							servo_set_deg(SERVO_1, servo_deg_1[SERVO_1]);
							if (all_motor_arrived()){
								grab_state = 2;
								g1_down_arrive_ticks = get_ticks();
							}
						break;
							
						case 2:
							if (get_ticks() - g1_down_arrive_ticks > 1000){
								servo_set_deg(SERVO_2, 2);
								grab_state = 3;
							}
						break;
							
						case 3:
							if (get_ticks() - g1_down_arrive_ticks > 2000){
								grab_state = 4;
								motor_set_pos(MOTOR_1, motor_pos_2[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_2[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_2[MOTOR_3]);
								servo_set_deg(SERVO_1, servo_deg_1[SERVO_1]);
							}
						break;
							
						case 4:
							if (all_motor_arrived() && get_ticks() - g1_down_arrive_ticks > 3000){
								grab_state = 5;
								motor_set_pos(MOTOR_1, motor_pos_3[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_3[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_3[MOTOR_3]);
							}
						break;
							
						case 5:
							if (all_motor_arrived() && get_ticks() - g1_down_arrive_ticks > 4000){
								grab_state = 6;
								motor_set_pos(MOTOR_1, motor_pos_4[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_4[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_4[MOTOR_3]);
							}
						break;
							
						case 6:
							if (all_motor_arrived()){
								grab_state = 7;
								servo_set_deg(SERVO_2, 1);
								g2_down_arrive_ticks = get_ticks();
							}
						break;
							
						case 7:
							if (get_ticks() - g2_down_arrive_ticks > 2000){
								grab_state = 8;
								motor_set_pos(MOTOR_1, motor_pos_1[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_1[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_1[MOTOR_3]);
							}
						break;
					}
			}

			control_update();			
			last_ctrl_ticks = this_ticks;
		}
		
		if (motor_init_state == 6 && motor_arrived[MOTOR_1] && motor_arrived[MOTOR_2] && motor_arrived[MOTOR_3]){
			use_cam = 1;
		}else{
			use_cam = 0;
		}

		static u16 locate_x = 0, locate_y = 0;
		static u32 last_flash_ticks = 0;
		if (this_ticks - last_flash_ticks >= 100){
			LCD_Clear();
			LCD_Printf(0, 0, "%d", this_ticks);
			LCD_Printf(0, 1, "P:%d V:%d W:%d", encoder_cnt[0], encoder_vel[0], motor_pwm_value[0]);
			LCD_Printf(0, 2, "P:%d V:%d W:%d", encoder_cnt[1], encoder_vel[1], motor_pwm_value[1]);
			LCD_Printf(0, 3, "P:%d V:%d W:%d", encoder_cnt[2], encoder_vel[2], motor_pwm_value[2]);
			LCD_Printf(0, 4, "X: %d Y: %d", locate_x, locate_y);
			LCD_Printf(0, 5, "Data: %d", found_data);
			LCD_Printf(0, 6, "%d %d", point_c.x, point_c.y);
			for (u8 i=0; i<25; i++){
				LCD_Printf(i%10, i/10 + 7, "%d", data_array[i]);
			}
//			LCD_Printf(0, 6, "S:%d %d %d %d", motor_init_state, GPIO_ReadInputDataBit(ENCODER_1_RESET_PORT, ENCODER_1_RESET_PIN),
//													button_pressed(BUTTON_K1), button_pressed(BUTTON_K2));
			LCD_Update();
			
			last_flash_ticks = this_ticks;
		}
		
		static s32 last_cam_ticks = 0;
		if (use_cam){
			if (this_ticks - last_cam_ticks >= 500){
				if(Ov7725_vsync == 2){
					FIFO_PREPARE;
					ImageLoad(cam_mode.lcd_sx,
										cam_mode.lcd_sy,
										READ_RED);
					
					//smooth_red();
					ImageDisp(cam_mode.lcd_sx,
										cam_mode.lcd_sy);
					
					locate_red(&locate_x, &locate_y, 5, 10);
					draw_locator(locate_x+cam_mode.lcd_sx, locate_y+cam_mode.lcd_sy,
												cam_mode.lcd_sx, cam_mode.lcd_sy,
												cam_mode.lcd_sx+CAM_WIDTH, cam_mode.lcd_sy+CAM_HEIGHT);
					//find_pattern();
					
					Ov7725_vsync = 0;	
					last_cam_ticks = this_ticks;
				}
			}
		}
  }
}
