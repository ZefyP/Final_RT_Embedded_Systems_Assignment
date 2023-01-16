/*-------------------------------------------------------------------------
 * CMSIS-RTOS main function
 *-----------------------------------------------------------------------*/

#define osObjectsPublic                   // Define objects in main module
#include "osObjects.h"                    // RTOS object definitions
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h" 								// Define the hal modules

#include "my_defines.h"										// Define the utilities library
#include "Thread.h"

int main (void) {
	
	SystemCoreClockUpdate();
  
	/* Initialise the SPI communication with the LIS3DSH accelerometer */
	Init_SPI();
	
  /* Initialize CMSIS-RTOS */
	osKernelInitialize ();	
	
	
	/* Initialise any peripherals */
	
// Initialise the LED and button	
	Initialise_LED();
	Initialise_button();
	
	
	/* Initialise the threads */

// Initialise the main thread to blink the LED?s
  Init_Tilt_Thread();
	Init_Blink_Pattern_Thread();
	Init_Button_Thread();
	
	

	osKernelStart ();	// start the thread execution 
	
	while(1){}; // While loop so the program doesn?t terminate
}








