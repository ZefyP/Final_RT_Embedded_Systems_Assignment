#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

	
// Definitions of LIS3DSH Addresses Registers
#define CTRL4_REG		0x20
#define	ENABLE_AXES	0X17	
#define OUT_Y_H_REG	0x2B
#define	OUT_Y_L_REG 0x2A
#define OUT_X_H_REG 0x29
#define OUT_X_L_REG 0x28

// Declarations	
	SPI_HandleTypeDef SPI_Params; // Declares the structure handle for the parameters of SPI1
	GPIO_InitTypeDef GPIOA_Params; // Declares the structure handle for the parameters of GPIOA
	GPIO_InitTypeDef GPIOE_Params; // Declares the structure handle for the parameters of GPIOE
	uint8_t data_to_send_receive[1]; //Declares an array to store the required LIS3DSH register address or data in. It has a single element since we will only be accessing a single address in each SPI transaction.
	uint16_t data_size=1; //Declares a variable that specifies that only a single address is accessed in each transaction.
	uint32_t data_timeout=1000; //Sets a maximum time to wait for the SPI transaction to complete in - this mean that our program won?t freeze if there is a problem with the SPI communication channel.
	uint8_t Z_Reg_H; //Declares the variable to store the z-axis MS 8-bits in
	
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

	// Initialize GPIO Port for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable Port D clock 
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // Port D.14 output - red LED
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // Port D.12 output - green LED
	
	// Write a new value to control register 4 of the LIS3DSH
	data_to_send_receive[0] = 0x20; // Address for control register 4 on LIS3DSH
	GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH	
	data_to_send_receive[0] = 0x14; // Set register value to give a sample rate of 3.125Hz, continuous update and z-axis enabled only
	HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the new register value to the LIS3DSH through the SPI channel	GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
	
	for(;;){
	
		// Read the value from the MSB (h) z-axis data register of the LIS3DSH ? Only the higher 8-bits are needed as this contains the sign bit in 2?s complement and only the sign of the z-axis acceleration is needed to determine the orientation of the board in this case.
		data_to_send_receive[0] = 0x80|0x2D; // Address for the MSB z-axis (H) data register on the LIS3DSH
		GPIOE->BSRR = GPIO_PIN_3<<16; // Set the SPI communication enable line low to initiate communication
		HAL_SPI_Transmit(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Send the address of the register to be read on the LIS3DSH	
		data_to_send_receive[0] = 0x00; // Set a blank address because we are waiting to receive data
		HAL_SPI_Receive(&SPI_Params,data_to_send_receive,data_size,data_timeout); // Get the data from the LIS3DSH through the SPI channel
		GPIOE->BSRR = GPIO_PIN_3; // Set the SPI communication enable line high to signal the end of the communication process
		Z_Reg_H = data_to_send_receive[0]; // Read the data from the SPI data array into our internal variable.
					
		if((Z_Reg_H&0x80) == 0x80){ // Check to see if the received value is positive or negative - the acceleration is a signed 16-bit number so the MSB is the sign bit - 1 is negative, 0 is positive. Refer to the C Programming guide document if you are unclear about this.
			GPIOD->BSRR |= (1<<12); // If the receive value is negative turn on the green LED
			GPIOD->BSRR |= (1<<(14+16)); // If the receive value is negative turn off the red LED
		}
		else{
			GPIOD->BSRR |= (1<<14); // If the received value is positive turn on the red LED
			GPIOD->BSRR |= (1<<(12+16)); // If the received value is positive turn off the green LED
		}
		
	}
	
