#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "motor.h"

extern volatile s32 motor_accel[MOTOR_COUNT];
extern volatile s32 motor_max_v[MOTOR_COUNT];

extern volatile s32 motor_final_pos[MOTOR_COUNT];
extern volatile s32 motor_tar_pos[MOTOR_COUNT];
extern volatile s32 motor_tar_vel[MOTOR_COUNT];
extern volatile s32 motor_vel_remain[MOTOR_COUNT];
extern volatile s32 motor_pos_remain[MOTOR_COUNT];

extern volatile u8 motor_arrived[MOTOR_COUNT];
extern volatile u8 motor_control_on[MOTOR_COUNT];

u8 all_motor_arrived(void);

void control_update(void);
void motor_set_pos(MotorID id, s32 pos);

#endif
