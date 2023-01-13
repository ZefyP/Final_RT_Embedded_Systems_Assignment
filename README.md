# Final_RT_Embedded_Systems_Assignment
# Embedded Systems Project

This is an embedded systems project that uses a STM32F4 microcontroller to implement a system with two modes: Tilt Switch and Pause.

## Modes

- **Tilt Switch**: In this mode, the system uses the accelerometer to determine the current orientation of the board, and then sets the state of the LEDs accordingly. The state of LEDs will change based on the Orientation of the board.
- **Pause**: In this mode, the system will blink the LEDs in a specific pattern and play a beep through the audio output. 

The mode can be toggled by clicking the button.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

In order to build and run the code, you will need the following:
- A development board with a STM32F4 microcontroller
- A toolchain for building and debugging the code
- A suitable software for flashing the code onto the microcontroller
- A library for the STM32F4xx devices

### Building and Running the Code

1. Clone the repository to your local machine.
2. Build the code using the toolchain and flash it onto the microcontroller.
3. Once the code is running on the microcontroller, you should be able to toggle the mode by clicking the button and see the LEDs change state accordingly.
