#include "stm32f4xx.h"
#include "stm32f4xx_hal.h" 								// Define the hal modules
#include "my_defines.h"										// Include headers


// #include "audio.h"


// Declarations
SPI_HandleTypeDef SPI_Params;
GPIO_InitTypeDef GPIOA_Params;
GPIO_InitTypeDef GPIOE_Params;

// for LED types and status definitions
uint8_t LED_on = 1; // Defines parameter for LED on
uint8_t LED_off = 0; // Defines parameter for LED off
	
uint8_t green_LED = 12; // Defines parameter for green LED (GPIOD pin 12)
uint8_t orange_LED = 13; // Defines parameter for orange LED (GPIOD pin 13)
uint8_t red_LED = 14; // Defines parameter for red LED (GPIOD pin 14)
uint8_t blue_LED = 15; // Defines parameter for blue LED (GPIOD pin 15)


// Definition for the function to initialise the LED and button

void Initialise_LED(void){
	
	// Initialise GPIO Port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock 
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Port D.12 output - green LED
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // Port D.13 output - orange LED
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Port D.14 output - red LED
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // Port D.15 output - blue LED
	
	}

	
void Initialise_button(void){
	
	//Initialise GPIO for push-button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable Port A clock
	GPIOA->MODER &= ~GPIO_MODER_MODER0;  // Set GPIOA pin 0 as an input 

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
	
//Defining a function to write LIS3DSH data
void LIS3DSH_enable(uint8_t register_address, uint8_t enable_address){
	uint8_t data_to_send_receive[1];  //M Declare an array to store the required LIS3DSH register address or data in. It has only one element as only one address will be accessed in each SPI transaction
	uint16_t data_size = 1; //M A variable to specify that only one address is accessed in each transaction
	uint32_t data_timeout = 1000; //M Set maximum time for SPI transaction to be completed whcih ensures that the program will not freeze in case of problems with SPI channel
	
	data_to_send_receive[0] = register_address; //M LIS3DSH CR4 address
	GPIOE->BSRR = GPIO_PIN_3<<16; //M Initiate SPI communication (enable line low) 
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); //M Send CR4 register address to LIS3DSH
	data_to_send_receive[0] = enable_address; //M 0xE8 Enables x, y and z-axes, sets a sample rate of 3.125Hz and continuous update (11101000 binary)
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); //M Send new register value
	GPIOE->BSRR = GPIO_PIN_3; //M End SPI communication (enable line high)
	
}
// Code to read LIS3DSH accelerometer data
uint16_t read_accel(uint8_t reg_axis_address)
{
	//definitions
	uint16_t val;
	uint8_t data_to_send_receive[1];  //Array to store the required LIS3DSH register address or data in. It has only one element as only one address will be accessed in each SPI transaction
	uint16_t data_size = 1; //Specify that only one address is accessed in each transaction
	uint32_t data_timeout = 1000; // Set max time for SPI transactions to be completed. Ensures that the program will not freeze in case of problem with the SPI communication.
	
	// read from register
	data_to_send_receive[0] = 0x80| reg_axis_address; //M y-axis MSB data register address 
	GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH	
	data_to_send_receive[0] = 0x00; // Set a blank address because we are waiting to receive data
	HAL_SPI_Receive(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Get the data from the LIS3DSH through the SPI channel
	GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	
	val = data_to_send_receive[0]; // Read the data from the SPI data array into our internal?variable.

	return val; 
}


// Code to check the button status
bool is_button_pressed(void){
	
//	Button_Beep(); // testing the Beep functionality
	
	bool state = false; // Declaring a variable which stores button state (true if pressed, false if released)
	int ticks;
	
	for(ticks=0;ticks<15;ticks++){
			
		   // If the button is pressed
			if(((GPIOA->IDR & 0x00000001) == 0x00000001) & ((GPIOD->ODR & (1<<14)) != (1<<14)))
				{ //checking if the button is pressed 
						state = true; // yes
				}else {
					state = false;  // no
			}	//end if 
				
	} // end for loop
	
	return state;
}

