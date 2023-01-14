#include "stm32f4xx.h"
#include "stm32f4xx_hal.h" 								// Define the hal modules
#include "my_defines.h"										// Include headers


// Declarations
SPI_HandleTypeDef SPI_Params;
GPIO_InitTypeDef GPIOA_Params;
GPIO_InitTypeDef GPIOE_Params;

uint8_t data_to_send_receive[1]; //Declares an array to store the required LIS3DSH register address or data in. 
uint16_t data_size=1; //Declares a variable that specifies that only a single address is accessed in each transaction.
uint32_t data_timeout=1000; //Sets a maximum time to wait for the SPI transaction to complete in.


// for Accel register and SPI comms
#define CTRL4_REG	0x20
#define ENABLE_AXES	0x17
#define OUT_Y_H_REG	0x2B
#define OUT_Y_L_REG	0x2A
#define OUT_X_H_REG	0x29
#define OUT_X_L_REG	0x28

#define Y_THRESHOLD_L -50
#define Y_THRESHOLD_H  50
#define X_THRESHOLD_L -50
#define X_THRESHOLD_H 50
		
// for LED types and status definitions
uint8_t LED_on = 1; // Defines parameter for LED on
uint8_t LED_off = 0; // Defines parameter for LED off
	
uint8_t green_LED = 12; // Defines parameter for green LED (GPIOD pin 12)
uint8_t orange_LED = 13; // Defines parameter for orange LED (GPIOD pin 13)
uint8_t red_LED = 14; // Defines parameter for red LED (GPIOD pin 14)
uint8_t blue_LED = 15; // Defines parameter for blue LED (GPIOD pin 15)

	


// Definition for the function to initialise the LED and button

void Initialise_LED_and_button(void){
	
	// Initialise GPIO Port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock 
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Port D.12 output - green LED
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Port D.13 output - orange LED
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Port D.14 output - red LED
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Port D.15 output - blue LED
	
	//Initialise GPIO for push-button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable Port A clock
}

// Definition for the function to blink the LED
void Blink_LED(uint8_t LED_state,uint8_t LED_colour){
	
	if(LED_state == 1){ // Checks to see if the request is to turn the LED on or off
			GPIOD->BSRR = 1<<LED_colour; // Turn on the LED
	}
	else{
			GPIOD->BSRR = 1<<(LED_colour+16); // Turn off the LED
	}
}

// Definition for the function to blink the LED
void Red_LED(uint8_t LED_state){
	
	if(LED_state == 1){ // Checks to see if the request is to turn the LED on or off
			GPIOD->BSRR = 1<<14; // Turn on the red LED
	}
	else{
			GPIOD->BSRR = 1<<(14+16); // Turn off the red LED
	}
			
}

// INIT SPI
void Init_SPI(void){

				// Code to initialise the SPI
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //Enable the clock for SPI1
		
		SPI_Params.Instance = SPI1; // Selects which SPI interface to use
		SPI_Params.Init.Mode = SPI_MODE_MASTER; // Sets the STM32F407 to act as the master
		SPI_Params.Init.NSS = SPI_NSS_SOFT; // Sets the slave to be controlled by software
		SPI_Params.Init.Direction = SPI_DIRECTION_2LINES; // Sets the SPI to full-duplex
		SPI_Params.Init.DataSize = SPI_DATASIZE_8BIT; // Sets the data packet size to 8-bit
		SPI_Params.Init.CLKPolarity = SPI_POLARITY_HIGH; // Sets the idle polarity for the clock line to high
		SPI_Params.Init.CLKPhase = SPI_PHASE_2EDGE; // Sets the data line to be sampled on the second transition of the clock line
		SPI_Params.Init.FirstBit = SPI_FIRSTBIT_MSB; // Sets the transmission to MSB first
		SPI_Params.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Sets the clock prescaler to divide the main APB2 clock (previously set to 84MHz) by 32 to give a SPI clock of 2.625MHz, which is less the maximum value of 10MHz for the SPI.

		HAL_SPI_Init(&SPI_Params); // Configures the SPI using the specified parameters
		
		// Code to initialise pins 5-7 of GPIOA
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //Enable the clock for GPIOA
		 
		GPIOA_Params.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // Selects pins 5,6 and 7
		GPIOA_Params.Alternate = GPIO_AF5_SPI1; //Selects alternate function 5 which corresponds to SPI1
		GPIOA_Params.Mode = GPIO_MODE_AF_PP; //Selects alternate function push-pull mode
		GPIOA_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
		GPIOA_Params.Pull = GPIO_NOPULL; //Selects no pull-up or pull-down activation

		HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Sets GPIOA into the modes specified in GPIOA_Params

		// Code to initialise pin 3 of GPIOE
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; //Enable the clock for GPIOE
		GPIOE_Params.Pin = GPIO_PIN_3; // Selects pin 3
		GPIOE_Params.Mode = GPIO_MODE_OUTPUT_PP; //Selects normal push-pull mode
		GPIOE_Params.Speed = GPIO_SPEED_FAST; //Selects fast speed
		GPIOE_Params.Pull = GPIO_PULLUP; //Selects pull-up activation
		HAL_GPIO_Init(GPIOE, &GPIOE_Params); // Sets GPIOE into the modes specified in GPIOE_Params


		GPIOE->BSRR = GPIO_PIN_3; //Sets the serial port enable pin CS high (idle)
		__HAL_SPI_ENABLE(&SPI_Params); //Enable the SPI

}
	


// Code to read LIS3DSH accelerometer data
uint16_t read_accel(uint8_t low_reg_axis_address, uint8_t high_reg_axis_address)
{
		uint8_t high_val, low_val;
	
		data_to_send_receive[0] = 0x80 | high_reg_axis_address; // Address for the high register of the axis on LIS3DSH
    GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
    HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the high register and receive the high value of the axis through the SPI channel
    GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
    
    data_to_send_receive[0] = 0x80 | low_reg_axis_address; // Address for the low register of the axis on LIS3DSH
    GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
    HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the low register and receive the low value of the axis through the SPI channel
    GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
 
	// Combine the high and low values into a single 16-bit value   
	return (high_val << 8) | low_val; 
}



Orientation get_orientation(void) {
   // read x and y axis values from the accelerometer
		int16_t x_val = read_accel(OUT_X_H_REG, OUT_X_L_REG);
    int16_t y_val = read_accel(OUT_Y_H_REG, OUT_Y_L_REG);
	  
	// RESET all LEDs
			Blink_LED(LED_off,green_LED);
			Blink_LED(LED_off,orange_LED);
			Blink_LED(LED_off,red_LED);
			Blink_LED(LED_off,blue_LED);
	
	//Checking Y-axis acceleration
			if (y_val < Y_THRESHOLD_L){
				Blink_LED(LED_on, blue_LED); //M Turn on blue LED if the value is negative
				
			}
			else if (y_val > Y_THRESHOLD_H){
				Blink_LED(LED_on, orange_LED); //M Turn on blue LED if the value is negative
			
			}
			else {							//all LEDs off if the board is placed horizontally
				Blink_LED(LED_off, red_LED); //M Turn on blue LED if the value is negative
			
			}
	
return UNKNOWN;
}




//void update_LEDs(void)
//{
//    // Code to update LED states
//	
//		// Read axis accel
//		Orientation current_orientation = get_orientation();
//			
		
//	
	
	 
//		// Update LEDs states
//		switch (current_orientation) {
//			
//    case UP:
//        // Turn on blue
//		Blink_LED(LED_on,green_LED);
//        break;
//    case DOWN:
//        // Turn on LED2
//		Blink_LED(LED_on,blue_LED);			
//        break;
//    case LEFT:
//        // Turn on LED3
//		Blink_LED(LED_on,orange_LED);		
//        break;
//    case RIGHT:
//        // Turn on LED4
//		Red_LED(1);
//        break;
//    case UP_LEFT:
//        // Turn on LED1 and LED3
//		Blink_LED(LED_on,green_LED);
//		Blink_LED(LED_on,orange_LED);
//        break;
//    case UP_RIGHT:
//        // Turn on LED1 and LED4
//		Blink_LED(LED_on,green_LED);
//		Blink_LED(LED_on,red_LED);
//        break;
//    case DOWN_LEFT:
//        // Turn on LED2 and LED3
//		Blink_LED(LED_on,orange_LED);
//		Blink_LED(LED_on,blue_LED);
//        break;
//    case DOWN_RIGHT:
//        // Turn on LED2 and LED4
//		Blink_LED(LED_on,red_LED);
//		Blink_LED(LED_on,blue_LED);
//        break;
//    default:
//			Blink_LED(LED_on, blue_LED);
//        // No LEDs should be illuminated in the case of horizontal orientation or an unknown orientation
//        break;
//		}
//		

		// Implement small deadzone and sensitivity
		
//}



