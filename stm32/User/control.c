#include "control.h"
#include "mymath.h"
#include "encoder.h"
#include "define.h"

volatile s32 motor_accel[MOTOR_COUNT] = {2000, 1000, 2000};
volatile s32 motor_max_v[MOTOR_COUNT] = {2000, 1000, 2000};

volatile s32 motor_final_pos[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_tar_pos[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_tar_vel[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_vel_remain[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_pos_remain[MOTOR_COUNT] = {0, 0, 0};

volatile u8 motor_arrived[MOTOR_COUNT] = {0};
volatile u8 motor_control_on[MOTOR_COUNT] = {0};

static const s32 PID_KP[MOTOR_COUNT] = {450, 300, 200};
static const s32 PID_KI[MOTOR_COUNT] = {0, 10, 10};
static const s32 PID_KD[MOTOR_COUNT] = {710, 320, 550};

u8 all_motor_arrived(){
	return motor_arrived[MOTOR_1] && motor_arrived[MOTOR_2] && motor_arrived[MOTOR_3];
}

#define PID_SCALE (32)
#define MAX_I (128)

#define THRESHOLD 10

volatile s32 inte_err[MOTOR_COUNT] = {0, 0, 0};

void control_update(){
	for (u8 i=0; i<MOTOR_COUNT; i++){
		if (motor_control_on[i]){
			s32 cur_pos = encoder_cnt[i];
			s32 cur_vel = encoder_vel[i];
			s32 err = motor_final_pos[i] - cur_pos;
			s32 s_at_decel = (motor_max_v[i]*motor_max_v[i])/2/motor_accel[i];
			s32 pwm = 0;
			
			if (ABS(err) > THRESHOLD){
				motor_arrived[i] = 0;
				
				//Update velocity
				if (err > 0 && err < s_at_decel){
					motor_tar_vel[i] = (s32)(Sqrt(motor_accel[i]/2*err)/512);
				}else if (err < 0 && err > -s_at_decel){
					motor_tar_vel[i] = -(s32)(Sqrt(ABS(motor_accel[i]/2*err))/512);
				}else if (err > 0){
					motor_tar_vel[i] += (motor_accel[i] + motor_vel_remain[i]) / CONTROL_FREQ;
					motor_vel_remain[i] = (motor_accel[i] + motor_vel_remain[i]) % CONTROL_FREQ;
				}else if (err < 0){
					motor_tar_vel[i] += (-motor_accel[i] + motor_vel_remain[i]) / CONTROL_FREQ;
					motor_vel_remain[i] = (-motor_accel[i] + motor_vel_remain[i]) % CONTROL_FREQ;
				}
				
				//Clamping
				motor_tar_vel[i] = CAP(motor_tar_vel[i], -motor_max_v[i], motor_max_v[i]);
				
				//Progress
				motor_tar_pos[i] += (motor_tar_vel[i] + motor_pos_remain[i]) * 2 / CONTROL_FREQ;
				motor_pos_remain[i] = (motor_tar_vel[i] + motor_pos_remain[i]) * 2 % CONTROL_FREQ;
				
				//Do PID
				s32 curr_err = motor_tar_pos[i] - cur_pos;
				s32 diff_err = motor_tar_vel[i] - cur_vel;
				
				inte_err[i] += curr_err;
				inte_err[i] = CAP(inte_err[i], -MAX_I, MAX_I);
				
				s32 duty = (PID_KP[i]*curr_err + PID_KI[i]*inte_err[i] + PID_KD[i]*diff_err)/PID_SCALE;
				pwm = CAP(duty, -MAX_PWM, MAX_PWM);
				
			}else{
				motor_arrived[i] = 1;
				pwm = 0;
				inte_err[i] = 0;
				motor_tar_pos[i] = encoder_cnt[i];
				motor_tar_vel[i] = motor_tar_vel[i];
				motor_pos_remain[i] = motor_vel_remain[i] = 0;
			}
			
			motor_set_power((MotorID)i, pwm);
			//motor_set_power((MotorID)i, -10000);
		}else{
			inte_err[i] = 0;
		}
	}
}

void motor_set_pos(MotorID id, s32 pos){
	motor_final_pos[id] = pos;
}
