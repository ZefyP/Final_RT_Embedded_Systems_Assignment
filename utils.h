#ifndef UTILS_H
#define UTILS_H

// #include <stdint.h>

// LED states
#define LED1 (1 << 0)
#define LED2 (1 << 1)
#define LED3 (1 << 2)
#define LED4 (1 << 3)

void initLEDs();                    // Function to initialise LED
void setLEDState(uint8_t state);    // Function to set LED state
void initButton();                  // Function to initialise button
bool isButtonClicked();             // Function to check if button is clicked


//accel.h and audio.h contain the rest


#endif  // UTILS_H