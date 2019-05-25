/*
 * dynamic_system.h
 *
 *  Created on: 29 de mar de 2019
 *      Author: matt
 */

#ifndef DYNAMIC_SYSTEM_H_
#define DYNAMIC_SYSTEM_H_

#include "fsl_device_registers.h"

#define MAXCOEF 3
#define MAX_X 	3
#define MAX_Y 	3

class Pid {

private:
		float erro, lastMeasure = 0, output;
		float proportional = 0, integral = 0, derivative = 0;
		float setpoint;
		float kp, ki, kd;
		float pid_signal;

public:
	Pid(float kp, float ki, float kd, float setpoint);
	float CalcPid(float input);
	float getSetpoint();
	void setPidSignal(float pidSignal);
	float getPidSignal();
};

#endif /* DYNAMIC_SYSTEM_H_ */
