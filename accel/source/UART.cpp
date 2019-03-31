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

#define TPM_MODULE 		3276
#define DUTY_MIN 		1

xTaskHandle task1_handle = NULL;

void task1(void *params){

	Pwm ledred(TPM2, GPIOB, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM, CHANNEL0);
	MMA8451Q accel;
	Pid pid(10, 0.009, 20, 45);

	/* Initializes TPM */
	ledred.pwmInit();
	/* Initializes I / O with the PWM signal */
	ledred.setMod(18, TPM_PWM_H);

	ledred.setDuty(DUTY_MIN);

	while(true) {

		PRINTF("Pitch: %f\r\n", accel.getPitch());

		pid.setPidSignal(pid.CalcPid(accel.getPitch()));

		PRINTF("PID: %.2f\r\n\n\n", pid.getPidSignal());

		if(accel.getPitch() > pid.getSetpoint()){
			ledred.setDuty(TPM_MODULE-(pid.getPidSignal()));
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
