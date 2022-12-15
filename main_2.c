// This is an alternative approach that is focused ontask management / without yet using RTOS.

// The code initialises the necessary peripherals and sets up global variables to track the current mode and state of the system. 
// The handleModeChange function checks if the button is clicked and switches the mode accordingly. The handleTiltSwitchMode function 
// uses the accelerometer to determine the current orientation of the board and updates the LED state accordingly. 
// The handlePauseMode function implements the LED blinking pattern and plays a beep through the audio output in the pause mode. 
// The loop function calls the appropriate mode handling function based on the current mode.
// This is just one possible way to implement the required functionality. There may be other approaches that can also be used to achieve 
// the same result.

#include "stm32f4xx.h"  // Header file for STM32F4xx devices
#include "utils.h"      // Header file for utilities of the system

// Global variables to track current mode and state of the system
enum Mode { TILT_SWITCH, PAUSE };
enum State { HORIZONTAL, LED1, LED2, LED3, LED4 };
Mode mode = TILT_SWITCH;
State state = HORIZONTAL;

// Function to initialise the system
void setup() {
  // Initialise LEDs, button, accelerometer, and audio
  initLEDs();
  initButton();
  initAccelerometer();
  initAudio();
}

// Function to handle mode changes
void handleModeChange() {
  // If button is clicked, switch mode
  if (isButtonClicked()) {
    if (mode == TILT_SWITCH) {
      mode = PAUSE;
    } else {
      mode = TILT_SWITCH;
    }
  }
}

// Function to handle tilt switch mode
void handleTiltSwitchMode() {
  // Get current orientation of the board from accelerometer
  Orientation orientation = getOrientation();

  // Update LED state based on orientation
  if (orientation == DOWN_LED1) {
    state = LED1;
  } else if (orientation == DOWN_LED2) {
    state = LED2;
  } else if (orientation == DOWN_LED3) {
    state = LED3;
  } else if (orientation == DOWN_LED4) {
    state = LED4;
  } else {
    state = HORIZONTAL;
  }
  setLEDState(state);
}

// Function to handle pause mode
void handlePauseMode() {
  // Blink LED in the specified pattern
  static int counter = 0;  // Counter variable to track current step in the pattern
  if (counter == 0) {
    setLEDState(LED1 | LED2);  // Turn on red and green LED
    playBeep();  // Play beep through audio output
  } else if (counter == 10) {
    setLEDState(0);  // Turn off all LED
  } else if (counter == 15) {
    setLEDState(LED3 | LED4);  // Turn on orange and blue LED
  } else if (counter == 20) {
    setLEDState(0);  // Turn off all LED
    counter = -1;  // Reset counter
  }
  counter++;
}

//
// Main loop
void loop() {
  handleModeChange();  // Handle mode change if button is clicked
  if (mode == TILT_SWITCH) {
    handleTiltSwitchMode();  // Handle tilt switch mode
  } else {
    handlePauseMode();  // Handle pause mode
  }
}
