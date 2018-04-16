#include "control.h"
#include "mymath.h"

s32 motor_accel[MOTOR_COUNT] = {100, 100, 100};
s32 motor_max_v[MOTOR_COUNT] = {5000, 5000, 5000};

s32 motor_tar_pos[MOTOR_COUNT] = {0, 0, 0};
s32 motor_tar_vel[MOTOR_COUNT] = {0, 0, 0};

void control_update(){
	for (u8 i=0; i<MOTOR_COUNT; i++){
		
	}
}
