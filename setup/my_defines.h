#ifndef MY_DEFINES_H
#define MY_DEFINES_H




void Init_SPI(void);
void Initialise_LED_and_button(void); // Declaration for the function to initialise the LED
void Blink_LED(uint8_t,uint8_t); // Declaration for the function to blink the LED
void Red_LED(uint8_t); // Declaration for the function to blink the LED


// orientation code
typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_LEFT,
    UP_RIGHT,
    DOWN_LEFT,
    DOWN_RIGHT,
    UNKNOWN
} Orientation;

		
Orientation get_orientation(void);
uint16_t read_accel(uint8_t low_reg_axis_address, uint8_t high_reg_axis_address);
void update_LEDs(void);


# endif //MY_DEFINES_H

