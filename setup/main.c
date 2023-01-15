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
	
	SystemCoreClockUpdate();
  
	// Initialise and enable the button interrupt to switch modes
	Init_Interrupt();
	Enable_Interrupt();
	
	/* Initialise the SPI communication with the LIS3DSH accelerometer */
	Init_SPI();
	
  /* Initialise CMSIS-RTOS */
	osKernelInitialize ();	
	
	
	/* Initialise any peripherals */
	
// Initialise the LED and button	
	Initialise_LED();
	Initialise_button();
	
	
	/* Initialise the threads */

	// Init_Blink_LED_Thread();
  Init_Tilt_Thread();
	
	// Init_Button_Thread();
	Init_Blink_Pattern_Thread();
		

	osKernelStart ();	// start thread execution 
	
	while(1){}; // While loop so the program doesn�t terminate
}









