#ifndef _CONTROL_H
#define _CONTROL_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "motor.h"

void control_update(void);
void motor_set_pos(MotorID id, s32 pos);

#endif
