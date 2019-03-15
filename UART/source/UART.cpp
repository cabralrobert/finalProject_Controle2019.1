#include <stdio.h>
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

#define TPM_MODULE 3276
#define DUTY_MIN 1
#define DUTY_MAX 325


xTaskHandle task1_handle = NULL;
xTaskHandle task2_handle = NULL;
xTaskHandle task3_handle = NULL;

void task1(void *params){

	volatile int x = 0;
	while(true) {
		MMA8451Q accel;
		accel.setFilter();
		accel.calibrate();
		x = accel.getX();
		PRINTF("X: %d\r\n", x);

		vTaskResume(task2_handle);
		vTaskSuspend(task1_handle);
	}

}

void task2(void *params){

	volatile int y = 0;
	while(true) {
		MMA8451Q accel;
		accel.setFilter();
		accel.calibrate();
		y = accel.getY();
		PRINTF("Y: %d\r\n", y);

		vTaskResume(task3_handle);
		vTaskSuspend(task2_handle);
	}

}

void task3(void *params){

	volatile int z = 0;
	while(true) {
		MMA8451Q accel;
		accel.setFilter();
		accel.calibrate();
		z = accel.getZ();
		PRINTF("Z: %d\r\n", z);

		vTaskResume(task1_handle);
		vTaskSuspend(task3_handle);
	}

}

int main(void) {

	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
	BOARD_InitDebugConsole();

	xTaskCreate(task1, "X", 500, NULL, 1, &task1_handle);
	xTaskCreate(task2, "Y", 500, NULL, 1, &task2_handle);
	xTaskCreate(task3, "Z", 500, NULL, 1, &task3_handle);

	vTaskStartScheduler();

	return 0 ;
}
