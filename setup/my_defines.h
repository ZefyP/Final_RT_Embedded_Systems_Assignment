#ifndef MY_DEFINES_H
#define MY_DEFINES_H

#include <stdbool.h>

void Init_SPI(void);
void Initialise_LED(void); // Declaration for the function to initialise the LED
// void Red_LED(uint8_t); // Declaration for the function to blink the LED
void Initialise_button(void);
void Blink_LED(uint8_t,uint8_t); // Declaration for the function to blink the LED

void LIS3DSH_enable(uint8_t register_address, uint8_t enable_address);
uint16_t read_accel(uint8_t reg_axis_address);

bool is_button_pressed(void);

# endif //MY_DEFINES_H

