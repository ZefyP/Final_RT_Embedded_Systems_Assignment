#ifndef UTILS_H
#define UTILS_H


// Include the necessary libraries (needing implementation)

#include "accel.h""  // Header file for accelerometer library
#include "audio.h"  // Header file for audio library (assuming this is the library for the CS43L22)

#include <stdint.h>

// LED states
#define LED1 (1 << 0)
#define LED2 (1 << 1)
#define LED3 (1 << 2)
#define LED4 (1 << 3)



// Function to initialise LED
void initLEDs() {
  // Enable clock for GPIOD peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  // Configure GPIOD pins 12, 13, 14, and 15 as output
  GPIOD->MODER |= (GPIO_MODER_MODER12_0 |
                   GPIO_MODER_MODER13_0 |
                   GPIO_MODER_MODER14_0 |
                   GPIO_MODER_MODER15_0);
}


// Function to set LED state
void setLEDState(uint8_t state) {
  // Set or clear the appropriate bits in the GPIOD output data register
  GPIOD->ODR = (GPIOD->ODR & ~(GPIO_ODR_ODR_12 |
                                GPIO_ODR_ODR_13 |
                                GPIO_ODR_ODR_14 |
                                GPIO_ODR_ODR_15)) |
                (state << 12);
}


// Function to initialize button
void initButton() {
  // Enable clock for GPIOA peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // Configure GPIOA pin 0 as input
  GPIOA->MODER &= ~GPIO_MODER_MODER0;

  // Configure GPIOA pin 0 to use a pull-up resistor
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0;
}

// Function to check if button is clicked
bool isButtonClicked() {
  // Check if button is pressed (low) and return true if it was pressed and released (high)
  if ((GPIOA->IDR & GPIO_IDR_IDR_0) == 0) {
    while ((GPIOA->IDR & GPIO_IDR_IDR_0) == 0) {}  // Wait for button release
    return true;
  }
  return false;
}


// Function to initialize accelerometer
void initAccelerometer() {
  // Enable clock for GPIOE peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

  // Configure GPIOE pins 0, 1, and 3 as output
  GPIOE->MODER |= (GPIO_MODER_MODER0_0 |
                   GPIO_MODER_MODER1_0 |
                   GPIO_MODER_MODER3_0);
}




#endif  // UTILS_H