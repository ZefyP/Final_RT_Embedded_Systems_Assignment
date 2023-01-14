#include "cmsis_os.h"                            // CMSIS RTOS header file
#include "stm32f4xx.h"
#include "my_defines.h"

// Thread Declarations

void Blink_LED_Thread (void const *argument); // Declares the main thread function that is defined later in the code
osThreadId tid_Blink_LED_Thread; // Declares an ID that we will associate with the thread and which allows easy reference to it when using some of the OS functions.
osThreadDef (Blink_LED_Thread, osPriorityNormal, 1, 0); // Declares the main thread object that we will use later. The parameters can be used to adjust certain properties, such as the priority of a thread and how many instances of it exist.

void Button_Thread (void const *argument); // Declares the main thread function that is defined later in the code
osThreadId tid_Button_Thread; // Declares an ID that we will associate with the thread and which allows easy reference to it when using some of the OS functions.
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); // Declares the main thread object that we will use later. The parameters can be used to adjust certain properties, such as the priority of a thread and how many instances of it exist.

void Tilt_Thread (void const *argument); // Declares the main thread function that is defined later in the code
osThreadId tid_Tilt_Thread; // Declares an ID that we will associate with the thread and which allows easy reference to it when using some of the OS functions.
osThreadDef (Tilt_Thread, osPriorityNormal, 1, 0); // Declares the main thread object that we will use later. The parameters can be used to adjust certain properties, such as the priority of a thread and how many instances of it exist.



// for Accel register and SPI comms
#define CTRL4_REG	0x20
#define ENABLE_AXES	0x17
#define OUT_Y_H_REG	0x2B
#define OUT_Y_L_REG	0x2A
#define OUT_X_H_REG	0x29
#define OUT_X_L_REG	0x28


#define Y_DEADZONE_L -2000
#define Y_DEADZONE_H  2000 //deadzone values differ due to accelerometer calibration 
#define X_DEADZONE_L -1500
#define X_DEADZONE_H  1500
/*-------------------------------------------------------------------------
 *      Blink LED Thread
 *-----------------------------------------------------------------------*/
 
// Code to define the thread function to initialise the main thread - this initialise function is called from the “main.c” file to start the thread.
int Init_Blink_LED_Thread (void) {

  tid_Blink_LED_Thread = osThreadCreate (osThread(Blink_LED_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
  if(!tid_Blink_LED_Thread) return(-1); // Checks to make sure the thread has been created.
  
  return(0);
}

// Code to define the operation of the main thread. This is effectively the code that was in the infinite FOR loop of our previous blinky program.
void Blink_LED_Thread (void const *argument) {
		

	// for LED types and status definitions
		uint8_t LED_on = 1; // Defines parameter for LED on
		uint8_t LED_off = 0; // Defines parameter for LED off		
		uint8_t green_LED = 12; // Defines parameter for green LED (GPIOD pin 12)


  while (1) { // Creates an infinite loop so that the blinking never terminates
		
		Blink_LED(LED_on,green_LED); // Blinks the green LED on once
		
		osDelay(1000); // Uses the built in delay function for the OS to create a 1 second delay. The fundamental delay is specified in the “RTX_conf_CM.c” file and usually defaults to 1ms.
		
		Blink_LED(LED_off,green_LED); // Blinks the green LED on once
		
		osDelay(1000); // Uses the built in delay function for the OS to create a 1 second delay. The fundamental delay is specified in the “RTX_conf_CM.c” file and usually defaults to 1ms.	
		
		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
		
  }

}

/*-------------------------------------------------------------------------
 *      Red LED on when button pressed thread Thread
 *-----------------------------------------------------------------------*/
 
// Code to define the thread function to initialise the main thread - this initialise function is called from the “main.c” file to start the thread.
int Init_Button_Thread (void) {

  tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
  if(!tid_Button_Thread) return(-1); // Checks to make sure the thread has been created.
  
  return(0);
}

// Code to define the operation of the main thread. This is effectively the code that was in the infinite FOR loop of our previous blinky program.
void Button_Thread (void const *argument) {

	uint8_t LED_on = 1; // Defines parameter for LED on
	uint8_t LED_off = 0; // Defines parameter for LED off
  
	uint8_t green_LED = 12; // Defines parameter for green LED (GPIOD pin 12)
	uint8_t orange_LED = 13; // Defines parameter for orange LED (GPIOD pin 13)
	uint8_t red_LED = 14; // Defines parameter for red LED (GPIOD pin 14)
	uint8_t blue_LED = 15; // Defines parameter for blue LED (GPIOD pin 15)

	
// Intro_blink !
	Blink_LED(LED_on,blue_LED);
	Blink_LED(LED_on,red_LED);
	Blink_LED(LED_on,orange_LED);
	Blink_LED(LED_on,green_LED);
	osDelay(1000);
	Blink_LED(LED_off,blue_LED);
	Blink_LED(LED_off,red_LED);
	Blink_LED(LED_off,orange_LED);
	Blink_LED(LED_off,green_LED);
	
  while (1) { // Creates an infinite loop so that the blinking never terminates
		
		// Checks the state of the push-button and only turns the red LED on if the button has only just been pressed, which is indicated by the state of the red LED. 
		if(((GPIOA->IDR & 0x00000001) == 0x00000001) & ((GPIOD->ODR & (1<<red_LED)) != (1<<red_LED))){
					
			Blink_LED(LED_on,red_LED); // Blinks the red LED on once
		
		}
		// Checks the state of the push-button and only turns the red LED off if the button has only just been released, which is indicated by the state of the red LED. 		
		else if(((GPIOA->IDR & 0x00000001) != 0x00000001) & ((GPIOD->ODR & (1<<red_LED)) == (1<<red_LED))){
		
			Blink_LED(LED_off,red_LED); // Turn red LED off
			
		}		
		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
		
  }

}




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
	int16_t x_val = 0; //combined value 
	
	int16_t y_high_val = 0;
	int16_t y_low_val = 0;
	int16_t y_val = 0; //combined value 
	
	
	int Y_mode = 0; //Declare a variable which stores y-axis tilt mode (0 - neutral, -1 - down, 1 - up)
	int X_mode = 0; //Declare a variable which stores x-axis tilt mode (0 - neutral, -1 - down, 1 - up)
	LIS3DSH_enable(CTRL4_REG, ENABLE_AXES); 	//M Writing new value to LIS3DSH control register 4 (CR4)

  while (1) { // Creates an infinite loop so that the blinking never terminates
		
		//get_orientation();
		// update_LEDs();
		 // read x and y axis values from the accelerometer

		
		x_high_val = read_accel(OUT_X_H_REG);
		x_low_val = read_accel(OUT_X_L_REG);
	
		// Combine the high and low values into a single 16-bit value 
		x_val = ((x_high_val << 8) | x_low_val);
		
    y_high_val = read_accel(OUT_Y_H_REG);
		y_low_val = read_accel(OUT_Y_L_REG);
		y_val = ((y_high_val << 8) | y_low_val);
		
		//Checking y-axis tilt mode
			if (y_val > Y_DEADZONE_H){
				Y_mode = 1;
			}
			else if (y_val < Y_DEADZONE_L){
				Y_mode = -1;
			}
			else {
				Y_mode = 0; 
			}
		
			//Checking x-axis tilt mode
			if (x_val > X_DEADZONE_H){
				X_mode = 1;
			}
			else if (x_val < X_DEADZONE_L){
				X_mode = -1;
			}
			else {
				X_mode = 0; 
			}
			//Resetting all the LEDs before checking new states
				Blink_LED(LED_off, green_LED); 
				Blink_LED(LED_off, red_LED); 
				Blink_LED(LED_off, orange_LED);
				Blink_LED(LED_off, blue_LED);

			//Checking Y and X axes accelerations 
			if (Y_mode == -1 && X_mode == 1){
					Blink_LED(LED_on, blue_LED);  
					Blink_LED(LED_on, red_LED);
			}
			else if (Y_mode == -1){
				Blink_LED(LED_on, blue_LED); //M Turn on blue LED if the value is negative
				
			}
			else if (Y_mode == 1){
				Blink_LED(LED_on, orange_LED); //M Turn on blue LED if the value is negative
		
			}
			else if (X_mode == -1){
				Blink_LED(LED_on, green_LED); //M Turn on blue LED if the value is negative
			}
			else if (X_mode == 1){
				Blink_LED(LED_on, red_LED); //M Turn on blue LED if the value is negative
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

//	//Checking Y-axis acceleration
//			if (y_val < Y_THRESHOLD_L){
//				Blink_LED(LED_on, blue_LED); //M Turn on blue LED if the value is negative
//				
//			}
//			else if (y_val > Y_THRESHOLD_H){
//				Blink_LED(LED_on, blue_LED); //M Turn on blue LED if the value is negative
//			
//			}
//			else {							//all LEDs off if the board is placed horizontally
//				Blink_LED(LED_on, red_LED); //M Turn on blue LED if the value is negative
//			
//			}
//	
		osThreadYield(); // This function tells the RTOS that when the thread gets to this stage the RTOS should suspend this thread and run the next thread that is ready to run. If there is no other thread ready (which is the case with this simple program since we only have one thread) then the calling thread continues. This function effectively forces the RTOS to reschedule and is useful in more complex systems and scheduling policies.
		
  }

}

