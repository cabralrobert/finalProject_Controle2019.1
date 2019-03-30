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
xTaskHandle task2_handle = NULL;
xTaskHandle task3_handle = NULL;
/*
void task1(void *params){

	volatile int x = 0;
	while(true) {
		MMA8451Q accel;
		accel.setFilter();
		accel.calibrate();
		x = accel.getX();



		PRINTF("X: %d\r\n", x);

		//vTaskResume(task2_handle);
		//vTaskSuspend(task1_handle);
	}

}
*/

short last_data = 0;
short out = 0;

/*
short blow_filter(short signal){

	out = last_data + (signal - last_data)*1;

	last_data = signal;

	return out;
	// y = y + (x-y)*a

	//float numbers[sample], acc = 0;

	//for(int i = sample-1; i > 0; i--) numbers[i] = numbers[i-1];

	//numbers[0] = signal;

	//for(int i = 0; i < sample; i++) acc += numbers[i];

	//return acc/sample;
}*/

/*
void task2(void *params){

	float pitch_filt = 0;
	//PRINTF("Cheguei:\r\n");

	short Xout_14_bit = 0, Yout_14_bit = 0, Zout_14_bit = 0;
	float Xout_g = 0, Yout_g = 0, Zout_g = 0, Pitch = 0;

	short X_blow = 0, Y_blow = 0, Z_blow = 0;

	MMA8451Q accel;

	while(true) {

		accel.setFilter();
		accel.calibrate();

		Xout_14_bit = accel.getX();
		Yout_14_bit = accel.getY();
		Zout_14_bit = accel.getZ();

		X_blow = blow_filter(Xout_14_bit);
		Y_blow = blow_filter(Yout_14_bit);
		Z_blow = blow_filter(Zout_14_bit);

		Xout_g = ((float) Xout_14_bit) / SENSITIVITY_2G;		// Compute X-axis output value in g's
		Yout_g = ((float) Yout_14_bit) / SENSITIVITY_2G;		// Compute Y-axis output value in g's
		Zout_g = ((float) Zout_14_bit) / SENSITIVITY_2G;		// Compute Z-axis output value in g's
		//PRINTF("Accel_X: %f\r\n", Xout_g);

		Pitch = atan2 (-Xout_g, sqrt (Yout_g*Yout_g + Zout_g*Zout_g)) * 180 / PI;       // Equation 37 in the AN3461

		//Pitch_blow = blow_filter(Pitch);

		PRINTF("Pith: %f\r\n", Pitch);

		//if (Zout_g > 0){

		//	Roll = atan2 (Yout_g, sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
		//	PRINTF("Roll1: %f\r\n\n", Roll);
		//} else{

		//	Roll = atan2 (Yout_g, - sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
		//	PRINTF("Roll2: %f\r\n\n", Roll);
		//}

		//vTaskResume(task3_handle);
		//vTaskSuspend(task2_handle);
	}

}*/

/*
void task3(void *params){

	volatile int z = 0;
	while(true) {
		MMA8451Q accel;
		accel.setFilter();
		accel.calibrate();
		z = accel.getZ();
		PRINTF("Z: %d\r\n", z);

		//vTaskResume(task1_handle);
		//vTaskSuspend(task3_handle);
	}

}
*/

int main(void) {

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	//xTaskCreate(task1, "X", 500, NULL, 1, &task1_handle);
	//xTaskCreate(task2, "Roll", 500, NULL, 1, &task2_handle);
	//xTaskCreate(task3, "Z", 500, NULL, 1, &task3_handle);

	//vTaskStartScheduler();

		short Xout_14_bit = 0, Yout_14_bit = 0, Zout_14_bit = 0;
		float Xout_g = 0, Yout_g = 0, Zout_g = 0, Pitch = 0;
		uint32_t x = 0;
		float pid_signal = 0;

		x = DUTY_MIN;

		Pwm ledred(TPM2, GPIOB, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM, CHANNEL0);
		MMA8451Q accel;
		Pid pid(27, 0.001, 20, 45);

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

			//Pitch_blow = blow_filter(Pitch);
			//PRINTF("Raw_int: %d\r\n", Xout_14_bit);
			//PRINTF("Raw_float: %f\r\n", Xout_g);
			PRINTF("Pith: %f\r\n", Pitch);

			pid_signal = pid.CalcPid(Pitch);

			PRINTF("PID: %.2f\r\n\n\n", pid_signal);

			if(Pitch > 45){
				ledred.setDuty(TPM_MODULE-pid_signal);
			} else{
				ledred.setDuty(DUTY_MIN);
			}



			//if(pid_signal/4 < pid.getSetpoint()){
				//ledred.setDuty(pid_signal/4);
			//} else{
				//ledred.setDuty(TPM_MODULE);
			//}
			// Set value Duty Cycle


			//if (Zout_g > 0){

			//	Roll = atan2 (Yout_g, sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
			//	PRINTF("Roll1: %f\r\n\n", Roll);
			//} else{

			//	Roll = atan2 (Yout_g, - sqrt (0.01*Xout_g*Xout_g + Zout_g*Zout_g)) * 180 / PI;
			//	PRINTF("Roll2: %f\r\n\n", Roll);
			//}

			//vTaskResume(task3_handle);
			//vTaskSuspend(task2_handle);
		}

	return 0 ;
}
