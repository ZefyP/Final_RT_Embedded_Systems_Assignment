#ifndef UTILS_H
#define UTILS_H

// Include necessary header files
#include <stdint.h>
#include "stm32f4xx.h"

// Define the threshold angles for each LED
#define LED1_THRESHOLD 10
#define LED2_THRESHOLD 100
#define LED3_THRESHOLD 170
// Define the deadzone range for each LED
#define LED1_DEADZONE 5
#define LED2_DEADZONE 10
#define LED3_DEADZONE 5
// Define the low-pass filter coefficient
#define LPF_COEFF 0.8
// Define the maximum angle of tilt in any direction
#define MAX_ANGLE 180
// Define the audio parameters
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define CHANNELS 2
#define VOLUME 50

// LED states
#define LED1 (1 << 0)
#define LED2 (1 << 1)
#define LED3 (1 << 2)
#define LED4 (1 << 3)

// Function prototypes
void initTimer();
void initLEDs();                    // Function to initialise LED
void setLEDState(uint8_t state);    // Function to set LED state                
void initButton();                  // Function to initialise buttons
bool isButtonClicked();             // Function to check if button is clicked

// The following are implemented in the audio.h
// void initI2C();
// void initCS43L22(int sample_rate, int bits_per_sample, int channels, int volume);
// void initDMA();
// void initI2S();

// void initAudio();
// void playBeep();

float readAccelerometerX();
float readAccelerometerY();
float readAccelerometerZ();

void turnOnLED(int led_num);
void turnOffLED(int led_num);

// //accel.h and audio.h contain the rest


#endif  // UTILS_H