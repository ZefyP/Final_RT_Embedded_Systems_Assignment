// In this code, the main program includes the necessary libraries for the accelerometer, LEDs, button, timer, CS43L22 audio DAC, 
// I2C, DMA, and I2S interfaces. It also defines the threshold and deadzone angles for each LED, the low-pass filter coefficient, 
// the maximum angle of tilt in any direction, and the audio parameters (sample rate, bits per sample, channels, and volume).
// The setup function initializes the accelerometer, LEDs, button, timer, I2C, CS43L22, DMA, and I2S interfaces, sets the audio buffer 
// for the DMA, and starts the audio output. The loop function continuously reads and processes the accelerometer values, detects and 
// handles the button press, switches between the tilt switch and LED blinking modes, and plays the "beep" sound every time the mode is 
// switched from the tilt switch to the LED blinking mode.