/*-------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function - Blinky
 *-----------------------------------------------------------------------*/

#define osObjectsPublic                   // Define objects in main module
#include "osObjects.h"                    // RTOS object definitions
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h" 								// Define the hal modules

#include "my_defines.h"
#include "Thread.h"


int main (void) {
  	
	
  /* Initialise the SPI for the LIS3SHD  */
	Init_SPI();
	
  /* Initialize CMSIS-RTOS */
	osKernelInitialize ();	
	
	
	/* Initialise any peripherals or system components */
	
// Initialise the LED and button	
	Initialise_LED_and_button();

// Initialise accellerometer
	Init_accel();
	/* Initialise any threads */

// Initialise the main thread to blink the LED’s
	Init_Blink_LED_Thread();
	Init_Button_Thread();
	Init_Tilt_Thread();
	

	osKernelStart ();	// start thread execution 
	
	while(1){}; // While loop so the program doesn’t terminate
}









