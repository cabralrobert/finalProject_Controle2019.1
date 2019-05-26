#include "dynamic_system.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#define OUTMAX 1000
#define OUTMIN -1000

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

float Pid::CalcPid(float x){

	// Cálculo do erro
	erro = this->setpoint - x;

	// Controle proporcional
	proportional = erro * this->kp;

	// Controle integral
	if(integral > 250)	integral = 250;
	else				integral += erro * this->ki;

	derivative = (lastMeasure - x) * this->kd;

	lastMeasure = x;

	pid = proportional + integral + derivative;

	if(pid > OUTMAX)
		pid = OUTMAX;
	else if(pid <= OUTMIN)
		pid = OUTMIN;

	//PRINTF("Erro: %f\r\n", erro);
	//PRINTF("P: %.2f\r\n", proportional);
	//PRINTF("I: %.2f\r\n", integral);
	//PRINTF("D: %.2f\r\n", derivative);

	return pid;

}
