#include "cmsis_os.h"                            // CMSIS RTOS header file
#include "stm32f4xx.h"
#include "my_defines.h"

// Global variable for mode switch 
bool flag_state = false;

// Thread Declarations

//void Blink_LED_Thread (void const *argument); // Declares the main thread function that is defined later in the code
//osThreadId tid_Blink_LED_Thread; // Declares an ID that we will associate with the thread and which allows easy reference to it when using some of the OS functions.
//osThreadDef (Blink_LED_Thread, osPriorityNormal, 1, 0); // Declares the main thread object that we will use later. The parameters can be used to adjust certain properties, such as the priority of a thread and how many instances of it exist.

//void Button_Thread (void const *argument); 
//osThreadId tid_Button_Thread; 
//osThreadDef (Button_Thread, osPriorityNormal, 1, 0); 


// osEvent signal_event;

void Tilt_Thread (void const *argument);
osThreadId tid_Tilt_Thread; 
osThreadDef (Tilt_Thread, osPriorityNormal, 1, 0); 

void Blink_Pattern_Thread (void const *argument);
osThreadId tid_Blink_Pattern_Thread; 
osThreadDef (Blink_Pattern_Thread, osPriorityNormal, 1, 0); 

// for Accel register and SPI comms
#define CTRL4_REG	0x20
#define ENABLE_AXES	0x17
#define OUT_Y_H_REG	0x2B
#define OUT_Y_L_REG	0x2A
#define OUT_X_H_REG	0x29
#define OUT_X_L_REG	0x28

// deadzone values differ due to accelerometer calibration
#define Y_DEADZONE_L -2000 // Blue LED  
#define Y_DEADZONE_H  2000 // Orange LED
#define X_DEADZONE_L -1000  // Green LED
#define X_DEADZONE_H  1000  // Red LED



/*-------------------------------------------------------------------------
 *     Tilt Switch Thread
 *-----------------------------------------------------------------------*/
 
// Code to define the thread function to initialise the main thread - this initialise function is called from the “main.c” file to start the thread.
int Init_Tilt_Thread (void) {

  tid_Tilt_Thread = osThreadCreate (osThread(Tilt_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
  if(!tid_Tilt_Thread) return(-1); // Checks to make sure the thread has been created.
  
  return(0);
}

// Code to define the operation of the main thread. This is effectively the code that was in the infinite FOR loop of our previous blinky program.
void Tilt_Thread (void const *argument) {
		
	uint8_t LED_on = 1; // Defines parameter for LED on
	uint8_t LED_off = 0; // Defines parameter for LED off
  
	uint8_t green_LED = 12; // Defines parameter for green LED (GPIOD pin 12)
	uint8_t orange_LED = 13; // Defines parameter for orange LED (GPIOD pin 13)
	uint8_t red_LED = 14; // Defines parameter for red LED (GPIOD pin 14)
	uint8_t blue_LED = 15; // Defines parameter for blue LED (GPIOD pin 15)

	
	int16_t x_high_val = 0; 
	int16_t x_low_val = 0;
	int16_t x_val = 0; // combined value of high and low x-axis values
	
	int16_t y_high_val = 0;
	int16_t y_low_val = 0;
	int16_t y_val = 0; // combined value of high and low y-axis values
	
	
	int Y_mode = 0; //Declare a variable which stores y-axis tilt mode (0 - neutral, -1 - down, 1 - up)
	int X_mode = 0; //Declare a variable which stores x-axis tilt mode (0 - neutral, -1 - down, 1 - up)
	
	LIS3DSH_enable(CTRL4_REG, ENABLE_AXES); 	// Enable the axes for the LIS3DSH accelerometer
	
 // osSignalClear(tid_Tilt_Thread,0x01);// Clear the flag 0x01 of the Tilt_Thread before entering the while loop
	osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
  
  while (1) { // Creates an infinite loop so that the blinking never terminates
		
	// osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set for this thread	
		
	// Code to toggle the mode after button click 
		if(is_button_pressed()){ // Checking if the button is pressed 
			
			 // Debounce: do nothing while the button is pressed
				while(is_button_pressed()){
					Blink_LED(1,13); //debug indicator that the button is pressed
				} 
			
				Blink_LED(0,13); //turn off indicator
				
				// TOGGLE THE FLAG
				if (flag_state == true){ //handover from TILT to LED
					
						flag_state = false ; 
						osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
					  osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set before continuing
					
				}else {
						// handover from LED to TILT
						flag_state = true ; 
						osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
						//osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set before continuing
						
				} //end toggle
				
		}else if( !is_button_pressed() ){
				 //do nothing	until it's pressed again to toggle the flag state
		}//end is button pressed
		
		
		if (flag_state == true){ // handover to Blink Pattern Thread
				
			osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called		
				
			osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set for this thread
				// osSignalClear(tid_Tilt_Thread,0x01); // Clear the flag 0x01 of the Tilt_Thread		
				// osThreadYield(); // Pause the current thread and switch to the next one.
	
		// This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
	} else { 
		
		//osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
  
		// read x and y axis values from the accelerometer
		
		x_high_val = read_accel(OUT_X_H_REG);	 				// read high byte of the x-axis value from the accelerometer
		x_low_val = read_accel(OUT_X_L_REG); 			 	  // read low byte of the x-axis value from the accelerometer
		x_val = ((x_high_val << 8) | x_low_val);	 		// Combine the high and low values into a single 16-bit value 
		
    y_high_val = read_accel(OUT_Y_H_REG); // read high byte of the y-axis value from the accelerometer
		y_low_val = read_accel(OUT_Y_L_REG);  // read low byte of the y-axis value from the accelerometer
		y_val = ((y_high_val << 8) | y_low_val);
		
		//Checking y-axis tilt mode
			if (y_val > Y_DEADZONE_H){
				Y_mode = 1;  // if y-axis value is above the high deadzone set Y_mode to 1 (tilted up)
			}
			else if (y_val < Y_DEADZONE_L){ //  if y-axis value is below the low deadzone set Y_mode to -1 (tilted down)
				Y_mode = -1; // if y-axis value is below the low deadzone set Y_mode to -1 (tilted down)
			}
			else {
				Y_mode = 0;   // if y-axis value is within the deadzone set Y_mode to 0 (neutral)
			}
		
		//Checking x-axis tilt mode
			if (x_val > X_DEADZONE_H){
				X_mode = 1;  // if x-axis value is above the high deadzone set X_mode to 1 (tilted right)
			}
			else if (x_val < X_DEADZONE_L){
				X_mode = -1; // if x-axis value is below the low deadzone set X_mode to -1 (tilted left)
			}
			else {
				X_mode = 0;  // if x-axis value is within the deadzone set X_mode to 0 (neutral)
			}
			
			//Resetting all the LEDs before checking new states
				Blink_LED(LED_off, green_LED); 
				Blink_LED(LED_off, red_LED); 
				Blink_LED(LED_off, orange_LED);
				Blink_LED(LED_off, blue_LED);

			//Checking Y axis accelerations to determine which LEDs to turn on
			if (Y_mode == -1 && X_mode == 1){
					Blink_LED(LED_on, blue_LED);  
					Blink_LED(LED_on, red_LED);
			}
			else if (Y_mode == -1){
				Blink_LED(LED_on, blue_LED); // Turn on blue LED if the value is negative
				
			}
			else if (Y_mode == 1){
				Blink_LED(LED_on, orange_LED); // Turn on blue LED if the value is negative
		
			}
			
			// Check for X axis 
			if (X_mode == -1){
				Blink_LED(LED_on, green_LED); // Turn on blue LED if the value is negative
			}
			else if (X_mode == 1){
				Blink_LED(LED_on, red_LED); // Turn on blue LED if the value is negative
			}
			else if (Y_mode == -1 && X_mode == 1){
					Blink_LED(LED_on, blue_LED);  
					Blink_LED(LED_on, red_LED);
			}
			else {							//all LEDs off if the board is placed horizontally
				
				Blink_LED(LED_off, green_LED); 
				Blink_LED(LED_off, red_LED); 
				Blink_LED(LED_off, orange_LED);
				Blink_LED(LED_off, blue_LED);
		
			}
			
			
		} // end flag state if 
			
			osThreadYield(); // Pause the current thread and switch to the next one. 
			
  }// end while loop

}

/*----------------------------------------------------------------------------
 *      LED Blinking Pattern thread
 *---------------------------------------------------------------------------*/
int Init_Blink_Pattern_Thread (void) {

  tid_Blink_Pattern_Thread = osThreadCreate (osThread(Blink_Pattern_Thread), NULL); //Create main thread object and assign thread ID to it
  if (!tid_Blink_Pattern_Thread) return(-1); //Check if the thread is created
  
  return(0);
}

void Blink_Pattern_Thread (void const *argument) {
			
		uint8_t LED_on = 1; //Parameter to turn on LED
		uint8_t LED_off = 0; //Parameter to turn off LED 
		uint8_t green_LED = 12; //Parameter for green LED (GPIOD 12)
		uint8_t orange_LED = 13; //Parameter for orange LED (GPIOD 13)
		uint8_t red_LED = 14; //Parameter for red LED (GPIOD 14)
		uint8_t blue_LED = 15; //Parameter for blue LED (GPIOD 15)
			
	//osSignalClear(tid_Blink_Pattern_Thread,0x01);// Clear the flag 0x01 of the Blink_Pattern_Thread before entering the while loop
	osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
  // osSignalClear(tid_Blink_Pattern_Thread,0x01);// Clear the flag 0x01 of the Tilt_Thread before entering the while loop
	
  while (1) { //infinite loop for the function to never terminate
		//	osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set before continuing
		
		// Code to toggle the mode after button click 
		if(is_button_pressed()){ // Checking if the button is pressed 
			
			 // Debounce: do nothing while the button is pressed
				while(is_button_pressed()){
					Blink_LED(1,13); //debug indicator that the button is pressed
				} 
			
				Blink_LED(0,13); //turn off indicator
				
				// TOGGLE THE FLAG
				if (flag_state == true){ //handover from TILT to LED
					
						flag_state = false ; 
						osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
					  // osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set before continuing
				}else {
						// handover from LED to TILT
						flag_state = true ; 
					
						Blink_LED(LED_off, green_LED); //Turns off green LED
						Blink_LED(LED_off, red_LED); //Turns off red LED
						Blink_LED(LED_off, orange_LED); //Turns on orange LED
						Blink_LED(LED_off, blue_LED); //Turns on blue LED
					
		
						osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
						osSignalWait(0x01, osWaitForever); // Wait for the signal flag to be set before continuing
						
				} //end toggle
				
		}else if( !is_button_pressed() ){
				 //do nothing	until it's pressed again to toggle the flag state
		}//end is button pressed
			
		
		if (flag_state ==false )
			{ //checking the flag state set by the push button
			
	//		osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called	
  
			Blink_LED(LED_on, green_LED); //Turns on green LED
			Blink_LED(LED_on, red_LED); //Turns on red LED
			Blink_LED(LED_off, orange_LED); //Turns off orange LED
			Blink_LED(LED_off, blue_LED); //Turns off blue LED
			
			osDelay(1000); //Built in delay function for 1 second delay
					
			Blink_LED(LED_off, green_LED); //Turns off green LED
			Blink_LED(LED_off, red_LED); //Turns off red LED
			Blink_LED(LED_on, orange_LED); //Turns on orange LED
			Blink_LED(LED_on, blue_LED); //Turns on blue LED
			
			osDelay(500); //0.5 second delay 
		
			}
	

		osThreadYield ();                                           // suspend thread
			
	}//end while loop
		
//		osSignalWait(0x01,osWaitForever); // Waits until flag 0x01 of this thread is set 			
//		osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the blink LED thread so that it resumes next time wait is called
//		
}
	
