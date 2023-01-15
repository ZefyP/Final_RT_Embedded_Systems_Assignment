#ifndef AUDIO_H
#define AUDIO_H


#include "stm32f4xx_hal.h"
#include "stm32f4xx_i2c.h"

// Declare the I2C and GPIO initialization structures
I2C_InitTypeDef I2C_InitStruct;
GPIO_InitTypeDef GPIO_InitStruct;

// Declare the I2C initialization function
void I2C_init(void);
void Button_Beep(void);

void I2C_init(void) {

    // Enable the clock for the I2C1 peripheral
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    // Enable the clock for the GPIOB peripheral
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// Configure the I2C1 pins (SCL and SDA) as alternate function
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;  //Select the pins to be used for I2C
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF; // Set the mode of the pin as alternate function
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz; // Set the speed of the pin
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD; // Set the output type as open drain
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // Enable the pull-up resistor
    GPIO_Init(GPIOB, &GPIO_InitStruct); // Initialise the GPIOB peripheral with the above configurations

    // Connect the I2C1 pins to the I2C1 peripheral
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); // Connect the SCL pin to I2C1 peripheral
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1); // Connect the SDA pin to I2C1 peripheral

    // Configure the I2C1 peripheral
    I2C_InitStruct.I2C_ClockSpeed = 100000; // Set the clock speed to 100kHz
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C; // Set the mode of the I2C peripheral as I2C
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; // Set the duty cycle of the I2C peripheral
    I2C_InitStruct.I2C_OwnAddress1 = 0x00; // Set the address of the device
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable; // Enable the acknowledge feature
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; // Set the acknowledged address to 7-bit
    I2C_Init(I2C1, &I2C_InitStruct); // Initialise the I2C1 peripheral with the above configurations

    // Enable the I2C1 peripheral
    I2C_Cmd(I2C1, ENABLE);
}

// Function to make the CS43L22 beep for 1 second when button is pressed
void Button_Beep(void) {
    // Check if the button is pressed
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0) {
        // Send the beep command to the CS43L22
        I2C_SendData(I2C1, 0x01);
        // Wait for 1 second
        delay_ms(1000);
        // Send the stop beep command to the CS43L22
        I2C_SendData(I2C1, 0x00);
    }
}



#endif // AUDIO_H

