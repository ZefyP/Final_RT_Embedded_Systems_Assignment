#ifndef MY_DEFINES_H
#define MY_DEFINES_H

#include <stdbool.h>

// Declare LED functions
void Initialise_LED(void); // Declaration for the function to initialise the LED
void Blink_LED(uint8_t,uint8_t); // Declaration for the function to blink the LED
// void Red_LED(uint8_t); // Declaration for the function to blink the LED

// Declare Button Interrupt functions
void Initialise_button(void);
void Init_Interrupt(void);
void Enable_Interrupt(void);
// void EXTI0_IRQHandler(void);
bool is_button_pressed(void);

// Declare Accellerometer functions
void Init_SPI(void);
void LIS3DSH_enable(uint8_t register_address, uint8_t enable_address);
uint16_t read_accel(uint8_t reg_axis_address);

# endif //MY_DEFINES_H

