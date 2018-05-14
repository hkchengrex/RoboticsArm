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

u8 cam_captured = 0;
u8 captured_w=0, captured_h=0;

u8 use_cam = 0;
u32 grab_state = 0;
u32 block_height = 0;

u8 red_width = 0;
u8 red_height = 0;
u32 red_area = 0;

static s16 rx=0, ry=0, gx=0, gy=0, bx=0, by=0;
static s16 cx=0, cy=0;

static s32 motor_view_pos[MOTOR_COUNT] = {-1000, 500, -500};
static s32 motor_pos_1[MOTOR_COUNT] = {-900, 850, -750};
static s32 motor_pos_2[MOTOR_COUNT] = {-900, 650, -350};
static s32 motor_pos_3[MOTOR_COUNT] = {-200, 650, -350};
static s32 motor_pos_4[3][MOTOR_COUNT] = {{-200, 800, -650}, {-200, 750, -600}, {-200, 700, -550}};

static s32 motor_init_pwm[MOTOR_COUNT] = {2500, -2500, 2500};
static s32 motor_hard_pwm[MOTOR_COUNT] = {6500, -7500, 8000};

//2 is close, 1 is open
static s32 servo1_grab = 180;
static s32 servo1_view = 60;

s32 get_m1_offset(){
	return 0;
}

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
					if (encoder_vel[ENCODER_3] < 500){
						motor_set_power(MOTOR_3, motor_hard_pwm[MOTOR_3]);
					}else{
						motor_set_power(MOTOR_3, motor_init_pwm[MOTOR_3]);
					}
					if (!GPIO_ReadInputDataBit(ENCODER_3_RESET_PORT, ENCODER_3_RESET_PIN)){
						motor_init_state = 5;
						motor_set_power(MOTOR_3, 0);
						encoder_reset(ENCODER_3);
					}
					break;
				case 1:
					if (encoder_vel[ENCODER_1] < 500){
						motor_set_power(MOTOR_1, motor_hard_pwm[MOTOR_1]);
					}else{
						motor_set_power(MOTOR_1, motor_init_pwm[MOTOR_1]);
					}
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
					if (encoder_vel[ENCODER_2] < 500){
						motor_set_power(MOTOR_2, motor_hard_pwm[MOTOR_2]);
					}else{
						motor_set_power(MOTOR_2, motor_init_pwm[MOTOR_2]);
					}
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
						grab_state = 100;
					}
					
					static u32 g1_down_arrive_ticks = 0;
					static u32 g2_down_arrive_ticks = 0;
					switch (grab_state){
						
						//Goto grab position
						case 1:
							motor_set_pos(MOTOR_1, motor_pos_1[MOTOR_1] + get_m1_offset());
							motor_set_pos(MOTOR_2, motor_pos_1[MOTOR_2]);
							motor_set_pos(MOTOR_3, motor_pos_1[MOTOR_3]);
							servo_set_deg(SERVO_1, servo1_grab);
							servo_set_deg(SERVO_2, 1);
							if (all_motor_arrived()){
								grab_state = 2;
								g1_down_arrive_ticks = get_ticks();
							}
						break;
							
						//Grab
						case 2:
							if (get_ticks() - g1_down_arrive_ticks > 1000){
								servo_set_deg(SERVO_2, 2);
								grab_state = 3;
							}
						break;
							
						//Up
						case 3:
							if (get_ticks() - g1_down_arrive_ticks > 2000){
								grab_state = 4;
								motor_set_pos(MOTOR_1, motor_pos_2[MOTOR_1] + get_m1_offset());
								motor_set_pos(MOTOR_2, motor_pos_2[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_2[MOTOR_3]);
								servo_set_deg(SERVO_1, servo1_grab);
							}
						break;
							
						//Translate
						case 4:
							if (all_motor_arrived() && get_ticks() - g1_down_arrive_ticks > 3000){
								grab_state = 5;
								motor_set_pos(MOTOR_1, motor_pos_3[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_3[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_3[MOTOR_3]);
							}
						break;
							
						//Down
						case 5:
							if (all_motor_arrived() && get_ticks() - g1_down_arrive_ticks > 4000){
								grab_state = 6;
								motor_set_pos(MOTOR_1, motor_pos_4[block_height][MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_4[block_height][MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_4[block_height][MOTOR_3]);
								block_height = (block_height + 1) % 3;
							}
						break;
							
						//Release
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
								motor_set_pos(MOTOR_1, motor_pos_3[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_3[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_3[MOTOR_3]);
							}
						break;
							
						case 8:
							if (all_motor_arrived() && get_ticks() - g2_down_arrive_ticks > 3000){
								grab_state = 100;
								motor_set_pos(MOTOR_1, motor_pos_2[MOTOR_1]);
								motor_set_pos(MOTOR_2, motor_pos_2[MOTOR_2]);
								motor_set_pos(MOTOR_3, motor_pos_2[MOTOR_3]);
							}
						break;
							
//						case 9:
//							if (all_motor_arrived() && get_ticks() - g2_down_arrive_ticks > 4000){
//								grab_state = 100;
//								motor_set_pos(MOTOR_1, motor_pos_1[MOTOR_1]);
//								motor_set_pos(MOTOR_2, motor_pos_1[MOTOR_2]);
//								motor_set_pos(MOTOR_3, motor_pos_1[MOTOR_3]);
//							}
//						break;
							
						//Goto viewing position
						case 100:
							motor_set_pos(MOTOR_1, motor_view_pos[MOTOR_1]);
							motor_set_pos(MOTOR_2, motor_view_pos[MOTOR_2]);
							motor_set_pos(MOTOR_3, motor_view_pos[MOTOR_3]);
							servo_set_deg(SERVO_1, servo1_view);
						
							static u32 last_viewing_ticks = 0;
							if (cam_captured){
								cam_captured = 0;
								grab_state = 1;
							}else{
								if (get_ticks() - last_viewing_ticks > 2000){
									last_viewing_ticks = get_ticks();
									cam_captured = 0;
								}
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

		static u32 last_flash_ticks = 0;
		if (this_ticks - last_flash_ticks >= 100){
			LCD_Clear();
			LCD_Printf(0, 0, "%d %d", this_ticks, grab_state);
			LCD_Printf(0, 1, "1:%d %d %d %d", encoder_cnt[0], encoder_vel[0], motor_pwm_value[0], GPIO_ReadInputDataBit(ENCODER_1_RESET_PORT, ENCODER_1_RESET_PIN));
			LCD_Printf(0, 2, "2:%d %d %d %d", encoder_cnt[1], encoder_vel[1], motor_pwm_value[1], GPIO_ReadInputDataBit(ENCODER_2_RESET_PORT, ENCODER_2_RESET_PIN));
			LCD_Printf(0, 3, "3:%d %d %d %d", encoder_cnt[2], encoder_vel[2], motor_pwm_value[2], GPIO_ReadInputDataBit(ENCODER_3_RESET_PORT, ENCODER_3_RESET_PIN));
			LCD_Printf(0, 4, "R:%d %dG:%d %dB:%d %d", rx, ry, gx, gy, bx, by);
			LCD_Printf(0, 5, "X:%d Y:%d", cx, cy);
			LCD_Printf(0, 5, "W:%d H:%d %d", red_width, red_height, red_area);
			//LCD_Printf(0, 6, "HSV: %d %d %d", CameraData[50][50].h, CameraData[50][50].s, CameraData[50][50].v);
//			LCD_Printf(0, 5, "D: %d", found_data);
//			LCD_Printf(0, 6, "%d %d", point_c.x, point_c.y);
//			for (u8 i=0; i<25; i++){
//				LCD_Printf(i%10, i/10 + 7, "%d", data_array[i]);
//			}
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
					
					locate_rgb(&rx, &ry, &gx, &gy, &bx, &by, 5, 300);
					
					Point p1, p2;
					p1.x = rx;
					p1.y = ry;
					p2 = double_recur_center(p1, 14, &red_width, &red_height);
					rx = p2.x;
					ry = p2.y;
					
					red_area = red_width * red_height;
					
					#define RGB_DIST_THRE 20
					s32 rg_dist = DIST_TIMES_16_XY(rx, ry, gx, gy) / 16;
					s32 gb_dist = DIST_TIMES_16_XY(gx, gy, bx, by) / 16;
					s32 rb_dist = DIST_TIMES_16_XY(rx, ry, bx, by) / 16;
					
//					//if (rg_dist<RGB_DIST_THRE && gb_dist<RGB_DIST_THRE && rb_dist<RGB_DIST_THRE){
//					if (gb_dist<RGB_DIST_THRE){
//						cx = (gx + bx) / 2;
//						cy = (gy + by) / 2;
//						
//						
//					}
					
					if (grab_state==100 && red_area > 80){
							cam_captured = 1;
							cx = rx;
							cy = ry;
						
							captured_w = cx;
							captured_h = cy;
						}else{
						cx = 0;
						cy = 0;
					}
					
					draw_locator(rx+cam_mode.lcd_sx, ry+cam_mode.lcd_sy,
												cam_mode.lcd_sx, cam_mode.lcd_sy,
												cam_mode.lcd_sx+CAM_WIDTH, cam_mode.lcd_sy+CAM_HEIGHT, RED);
					
//					draw_locator(gx+cam_mode.lcd_sx, gy+cam_mode.lcd_sy,
//												cam_mode.lcd_sx, cam_mode.lcd_sy,
//												cam_mode.lcd_sx+CAM_WIDTH, cam_mode.lcd_sy+CAM_HEIGHT, GREEN);
//					
//					draw_locator(bx+cam_mode.lcd_sx, by+cam_mode.lcd_sy,
//												cam_mode.lcd_sx, cam_mode.lcd_sy,
//												cam_mode.lcd_sx+CAM_WIDTH, cam_mode.lcd_sy+CAM_HEIGHT, BLUE);
												
//					draw_locator(50+cam_mode.lcd_sx, 50+cam_mode.lcd_sy,
//												cam_mode.lcd_sx, cam_mode.lcd_sy,
//												cam_mode.lcd_sx+CAM_WIDTH, cam_mode.lcd_sy+CAM_HEIGHT, BLUE);
					//find_pattern();
					
					Ov7725_vsync = 0;	
					last_cam_ticks = this_ticks;
				}
			}
		}
  }
}
