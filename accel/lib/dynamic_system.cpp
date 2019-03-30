#include "dynamic_system.h"
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

Pid::Pid(float kp, float ki, float kd, float setpoint){
	this->kp = kp;
	this->ki = ki;
	this->kd = kd;
	this->setpoint = setpoint;
};

float Pid::getSetpoint(){
	return setpoint;
}

float Pid::CalcPid(float x){

	erro = this->setpoint - x;

	proportional = erro * this->kp;

	integral += erro * this->ki;

	derivative = (lastMeasure - x) * this->kd;

	lastMeasure = x;

	pid = proportional + integral + derivative;

	if(integral > 250)	integral = 250;

	PRINTF("Erro: %f\r\n", erro);
	PRINTF("P: %.2f\r\n", proportional);
	PRINTF("I: %.2f\r\n", integral);
	PRINTF("D: %.2f\r\n", derivative);

	//if(pid > 1000)	pid = 800;
	// pwm = PID + VALOR_PWM;

	return pid;

}
