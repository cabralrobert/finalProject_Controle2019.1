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
#include <math.h>

#define TPM_MODULE 		3276
#define DUTY_MIN 		1
#define DUTY_MAX 		325
#define PI				3.14159
#define SENSITIVITY_2G	4096

xTaskHandle task1_handle = NULL;

void task1(void *params){

	short Xout_14_bit = 0, Yout_14_bit = 0, Zout_14_bit = 0;
	float Xout_g = 0, Yout_g = 0, Zout_g = 0, Pitch = 0;
	float pid_signal = 0;

	Pwm ledred(TPM2, GPIOB, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM, CHANNEL0);
	MMA8451Q accel;
	Pid pid(10, 0.009, 20, 45);

	/* Initializes TPM */
	ledred.pwmInit();
	/* Initializes I / O with the PWM signal */
	ledred.setMod(18, TPM_PWM_H);

	ledred.setDuty(DUTY_MIN);

	while(true) {

		//accel.setFilter();
		//accel.calibrate();

		Xout_14_bit = accel.getX();
		Yout_14_bit = accel.getY();
		Zout_14_bit = accel.getZ();

		Xout_g = ((float) Xout_14_bit) / SENSITIVITY_2G;		// Compute X-axis output value in g's
		Yout_g = ((float) Yout_14_bit) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
		Zout_g = ((float) Zout_14_bit) / SENSITIVITY_2G;		// Compute Z-axis output value in g's
		//PRINTF("Accel_X: %f\r\n", Xout_g);

		Pitch = atan2 (-Xout_g, sqrt (Yout_g*Yout_g + Zout_g*Zout_g)) * 180 / PI;       // Equation 37 in the AN3461

		//PRINTF("Raw_int: %d\r\n", Xout_14_bit);
		//PRINTF("Raw_float: %f\r\n", Xout_g);
		PRINTF("Pith: %f\r\n", Pitch);

		pid_signal = pid.CalcPid(Pitch);

		PRINTF("PID: %.2f\r\n\n\n", pid_signal);

		if(Pitch > pid.getSetpoint()){
			ledred.setDuty(TPM_MODULE-pid_signal);
		} else{
			ledred.setDuty(DUTY_MIN);
		}
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
