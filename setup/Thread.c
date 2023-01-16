#include "cmsis_os.h"                            // CMSIS RTOS header file
#include "stm32f4xx.h"
#include "my_defines.h"


//Global
bool flag_state = false;

// Thread Declarations

void Button_Thread (void const *argument); 
osThreadId tid_Button_Thread; 
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); 

void Tilt_Thread (void const *argument);
osThreadId tid_Tilt_Thread; 
osThreadDef (Tilt_Thread, osPriorityNormal, 1, 0); 

void Blink_Pattern_Thread (void const *argument);
osThreadId tid_Blink_Pattern_Thread; 
osThreadDef (Blink_Pattern_Thread, osPriorityNormal, 1, 0); 

// register addresses for Accelerations 
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
 
// Code to define the thread function to initialise the main thread - this initialise function is called from the ?main.c? file to start the thread.
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
	
	
	int Y_mode = 0; //Declare a variable which stores y-axis tilt mode (0 is neutral, -1 is down, 1 is up)
	int X_mode = 0; //Declare a variable which stores x-axis tilt mode (0 is neutral, -1 is down, 1 is up)
	
	LIS3DSH_enable(CTRL4_REG, ENABLE_AXES); 	// Enable the axes for the LIS3DSH accelerometer

	osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the blink LED thread so that it resumes next time wait is called

  while (1) { // Creates an infinite loop 
		
		if (flag_state == false){ // if tilt mode ON
		
				osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called

				// read x and y axis values from the accelerometer
				
				x_high_val = read_accel(OUT_X_H_REG);	 				// read high byte of the x-axis value from the accelerometer
				x_low_val = read_accel(OUT_X_L_REG); 			 	  // read low byte of the x-axis value from the accelerometer
				x_val = ((x_high_val << 8) | x_low_val);	 		// Combine the high and low values into a single 16-bit value 
				
				y_high_val = read_accel(OUT_Y_H_REG); 				// read high byte of the y-axis value from the accelerometer
				y_low_val = read_accel(OUT_Y_L_REG);  				// read low byte of the y-axis value from the accelerometer
				y_val = ((y_high_val << 8) | y_low_val);			// Combine the high and low bytes
				
				// Checking y-axis tilt mode
					if (y_val > Y_DEADZONE_H){
						Y_mode = 1;  // (tilt up) if y-axis value is above the high deadzone set Y_mode to 1 
					}
					else if (y_val < Y_DEADZONE_L){ //  if y-axis value is below the low deadzone set Y_mode to -1 (tilted down)
						Y_mode = -1; //(tilt down) if y-axis value is below the low deadzone set Y_mode to -1 
					}
					else {
						Y_mode = 0;   //(neutral)if y-axis value is within the deadzone set Y_mode to 0 
					}
				
				// Checking x-axis tilt mode
					if (x_val > X_DEADZONE_H){
						X_mode = 1;  // (tilt right) if x-axis value is above the high deadzone set X_mode to 1 
					}
					else if (x_val < X_DEADZONE_L){
						X_mode = -1; //(tilt left) if x-axis value is below the low deadzone set X_mode to -1 
					}
					else {
						X_mode = 0;  // (neutral) if x-axis value is within the deadzone set X_mode to 0 
					}
					
					//Resetting all the LEDs before checking new states
						Blink_LED(LED_off, green_LED); 
						Blink_LED(LED_off, red_LED); 
						Blink_LED(LED_off, orange_LED);
						Blink_LED(LED_off, blue_LED);

					//Checking Y and X axes accelerations to determine which LEDs to turn on
					// Y Axis
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
					
					// X Axis
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
					else {							// all LEDs off if the board is placed horizontally
						Blink_LED(LED_off, green_LED); 
						Blink_LED(LED_off, red_LED); 
						Blink_LED(LED_off, orange_LED);
						Blink_LED(LED_off, blue_LED);
					}
			
		}else { // if tilt mode OFF, turn Pattern mode ON
			
			osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the blink LED thread so that it resumes next time wait is called
			osSignalWait(0x01,osWaitForever); // Waits until flag 0x01 of this thread is set

		}

		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
		
  }//end while loop

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
			
		uint8_t LED_on = 1;				//Parameter to turn on LED
		uint8_t LED_off = 0; 			//Parameter to turn off LED 
		uint8_t green_LED = 12; 	//Parameter for green LED (GPIOD 12)
		uint8_t orange_LED = 13;	//Parameter for orange LED (GPIOD 13)
		uint8_t red_LED = 14; 		//Parameter for red LED (GPIOD 14)
		uint8_t blue_LED = 15; 		//Parameter for blue LED (GPIOD 15)
			
		osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called

	
  while (1) { //infinite loop for the function to never terminate
			
		osSignalWait(0x01,osWaitForever); // Waits until flag 0x01 of this thread is set 
		//check the flag state set by the push button
		if ( flag_state == true){  // if Pattern mode On
		
			osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the thread so that it resumes next time wait is called
			
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
		else { // if Pattern mode Off
			
			// Turn all LEDs OFF
			Blink_LED(LED_off, green_LED); //Turns off green LED
			Blink_LED(LED_off, red_LED); //Turns off red LED
			Blink_LED(LED_off, orange_LED); //Turns on orange LED
			Blink_LED(LED_off, blue_LED); //Turns on blue LED
		
			osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the blink LED thread so that it resumes next time wait is called
			osSignalWait(0x01,osWaitForever); // Waits until flag 0x01 of this thread is set
			
			
		}
		osThreadYield ();                                           // suspend thread
	}// end while loop
}


/*----------------------------------------------------------------------------
 *      Button thread
 *---------------------------------------------------------------------------*/
int Init_Button_Thread (void) {

  tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); //Create main thread object and assign thread ID to it
  if (!tid_Button_Thread) return(-1); //Check if the thread is created
  
  return(0);
}
void Button_Thread (void const *argument) {

		while (1){ //Creating an infinite loop 
		
			if(is_button_pressed()){ // Checking if the button is pressed 
				while(is_button_pressed()){
				Blink_LED(1,13); //debug indicator that the button is pressed
				} // Debounce - do nothing while the button is pressed
			
			Blink_LED(0,13); //turn off indicator
				
			flag_state = !flag_state; // toggle the flag state
			
				
				if(!flag_state){ // handover to tilt mode
					osSignalSet(tid_Tilt_Thread,0x01);// Set flag 0x01 of the blink LED thread so that it resumes next time wait is called				
					osSignalClear(tid_Blink_Pattern_Thread,0x01); // Clear the signal flag to resume the Blink_LED_Thread
		
				}else { // sustain pattern mode
					osSignalClear(tid_Tilt_Thread,0x01); // Clear the signal flag to resume the Blink_LED_Thread
					osSignalSet(tid_Blink_Pattern_Thread,0x01);// Set flag 0x01 of the blink LED thread so that it resumes next time wait is called				
					
				}
			
			}// end if 
			

		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.	
		}	 // end while loop
		
}

