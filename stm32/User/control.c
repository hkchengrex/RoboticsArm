#include "control.h"
#include "mymath.h"
#include "encoder.h"
#include "define.h"

volatile s32 motor_accel[MOTOR_COUNT] = {1000, 1000, 1000};
volatile s32 motor_max_v[MOTOR_COUNT] = {7000, 7000, 7000};

volatile s32 motor_final_pos[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_tar_pos[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_tar_vel[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_vel_remain[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_pos_remain[MOTOR_COUNT] = {0, 0, 0};

#define PID_KP 500
#define PID_KI 0 //26
#define PID_KD 0 //44
#define PID_SCALE (32)
#define MAX_I (128)

volatile s32 inte_err[MOTOR_COUNT] = {0, 0, 0};
volatile s32 motor_pwm_value[MOTOR_COUNT] = {0, 0, 0};

void control_update(){
	for (u8 i=0; i<MOTOR_COUNT; i++){
		s32 cur_pos = encoder_cnt[i];
		s32 cur_vel = encoder_vel[i];
		s32 err = motor_final_pos[i] - cur_pos;
		s32 s_at_decel = (motor_max_v[i]*motor_max_v[i])/2/motor_accel[i];
		
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
		motor_tar_pos[i] += (motor_tar_vel[i] + motor_pos_remain[i]) / CONTROL_FREQ;
		motor_vel_remain[i] = (motor_tar_vel[i] + motor_pos_remain[i]) % CONTROL_FREQ;
		
		//Do PID
		s32 curr_err = motor_tar_pos[i] - cur_pos;
		s32 diff_err = motor_tar_vel[i] - cur_vel;
		
		inte_err[i] += curr_err;
		inte_err[i] = CAP(inte_err[i], -MAX_I, MAX_I);
		
		s32 duty = (PID_KP*curr_err + PID_KI*inte_err[i] + PID_KD*diff_err)/PID_SCALE;
		motor_pwm_value[i] = CAP(duty, -MAX_PWM, MAX_PWM);
		//motor_pwm_value[i] = cap_value(duty, -10000, 10000);
		
		motor_set_power((MotorID)i, motor_pwm_value[i]);
		//motor_set_power((MotorID)i, 3000);
	}
}

void motor_set_pos(MotorID id, s32 pos){
	motor_final_pos[id] = pos;
}
