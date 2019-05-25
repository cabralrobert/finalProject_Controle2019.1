#include "dynamic_system.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#define OUTMAX 900
#define OUTMIN -900

Pid::Pid(float kp, float ki, float kd, float setpoint){
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->setpoint = setpoint;
};

float Pid::getSetpoint(){
	return setpoint;
}

void Pid::setPidSignal(float pid_signal){
	this->pid_signal = pid_signal;
}

float Pid::getPidSignal(){
	return pid_signal;
}

float Pid::CalcPid(float input){

	erro = this->setpoint - input;

	// Proportional
	proportional = this->kp * erro;

	// Integral
	integral += (this->ki * erro);

	if(integral > OUTMAX){
		integral = OUTMAX;
	} else if(integral <= OUTMIN){
		integral = OUTMIN;
	}

	// Derivative
	derivative = this->kd * (input - lastMeasure);

	// Sum of controls
	output = proportional + integral + derivative;

	/*if(output > OUTMAX)
		output = OUTMAX;
	else if(output <= OUTMIN)
		output = OUTMIN;*/

	lastMeasure = input;

	/*PRINTF("Erro: %f\r\n", erro);
	PRINTF("P: %.2f\r\n", proportional);
	PRINTF("I: %.2f\r\n", integral);
	PRINTF("D: %.2f\r\n", derivative);*/

	return output;

}
