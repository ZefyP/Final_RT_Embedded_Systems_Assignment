/*-------------------------------------------------------------------------
* CMSIS-RTOS 'main' function - Blinky
*-----------------------------------------------------------------------*/
#define osObjectsPublic // Define objects in main module
#include "osObjects.h" // RTOS object definitions
#include "stm32f4xx.h"
#include "my_defines.h"
#include "Thread.h"

int main (void) {
	
			osKernelInitialize (); //Initialize CMSIS-RTOS
				
			/* Initialise any peripherals or system components */
			
			// Initialize the LED
			Initialise_LED();
				
				
			/* Initialise any threads */
			
			// Initialise the main thread to blink the LED�s
			Init_Blink_LED_Thread();
				
				
			osKernelStart (); // start thread execution
				
			while(1){}; // While loop so the program doesn�t terminate

}