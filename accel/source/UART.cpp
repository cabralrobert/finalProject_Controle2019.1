#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include <lib/I2C.h>
#include <lib/MMA8451Q.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include "task.h"
#include "lib/pwm.h"
#include "dynamic_system.h"
#include "gpio.h"

#define TPM_MODULE 		3276
#define DUTY_MIN 		1

xTaskHandle task1_handle = NULL;

/* Objects definitions */
Pwm motors(TPM2, GPIOB, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM, CHANNEL0);
Gpio gpio1_motors(GPIOB, 0), gpio2_motors(GPIOB, 1);

enum direction_t {
	FORWARD = 0,
	BACK = 1
};

typedef direction_t direction_t;

void directionMotors(direction_t dir){
	switch(dir){
		case FORWARD:
			gpio1_motors.setValue(LOW);
			gpio2_motors.setValue(HIGH);
		break;
		case BACK:
			gpio1_motors.setValue(HIGH);
			gpio2_motors.setValue(LOW);
		break;
	}
}

void pwmMotors(float pitch, float value){
	if(pitch >= 0)
		directionMotors(BACK);
	else
		directionMotors(FORWARD);

	motors.setDuty(abs(value));
}

void task1(void *params){

	float controle_pwm;

	MMA8451Q accel;
	float v1 = 10, v2 = 0.01, v3 = 1;
//	SCANF("%f", &v1);
//	SCANF("%f", &v2);
//	SCANF("%f", &v3);
//	PRINTF("\r%f %f %f\n", v1, v2, v3);
	Pid pid(v1, v2, v3, 90);

	accel.setFilter();
	accel.calibrate();

	// Initialize pins of directions
	gpio1_motors.setDirection(OUTPUT);
	gpio2_motors.setDirection(OUTPUT);

	/* Initializes TPM */
	motors.pwmInit();
	/* Initializes I / O with the PWM signal */
	motors.setMod(2, TPM_PWM_H);

	while(true){
		pid.setPidSignal(pid.CalcPid(accel.getPitch()));
		controle_pwm = pid.CalcPid(accel.getPitch());
		pwmMotors(accel.getPitch(), controle_pwm);;
	}
}

int main(void) {

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	xTaskCreate(task1, "PID_Control", 500, NULL, 1, &task1_handle);

	vTaskStartScheduler();

	return 0 ;
}
