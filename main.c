// In this code, the main program includes the necessary libraries for the accelerometer, LEDs, button, timer, CS43L22 audio DAC, 
// I2C, DMA, and I2S interfaces. It also defines the threshold and deadzone angles for each LED, the low-pass filter coefficient, 
// the maximum angle of tilt in any direction, and the audio parameters (sample rate, bits per sample, channels, and volume).
// The setup function initializes the accelerometer, LEDs, button, timer, I2C, CS43L22, DMA, and I2S interfaces, sets the audio buffer 
// for the DMA, and starts the audio output. The loop function continuously reads and processes the accelerometer values, detects and 
// handles the button press, switches between the tilt switch and LED blinking modes, and plays the "beep" sound every time the mode is 
// switched from the tilt switch to the LED blinking mode.


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
// Variables to store the accelerometer values
float x_accel, y_accel, z_accel;
// Variables to store the angle of tilt
float x_angle, y_angle;
// Variables to store the filtered accelerometer values
float x_accel_filtered, y_accel_filtered, z_accel_filtered;
// Variables to store the filtered angle of tilt
float x_angle_filtered, y_angle_filtered;
// Variable to track the current mode (tilt switch or LED blinking)
int mode = 0;
// Audio buffer
int16_t audio_buffer[BUFFER_SIZE];
void setup()
{
  // Initialize the accelerometer
  initAccelerometer();
  // Initialize the LEDs
  initLEDs();
  // Initialize the button
  initButton();
  // Initialize the timer
  initTimer();
  // Initialize the I2C interface
  initI2C();
  // Initialize the CS43L22
  initCS43L22(SAMPLE_RATE, BITS_PER_SAMPLE, CHANNELS, VOLUME);
  // Initialize the DMA and I2S interfaces
  initDMA();
  initI2S();
// Set the audio buffer for the DMA
  setAudioBuffer(audio_buffer);
  // Start the audio output
  startAudioOutput();
}

void loop()
{
  // Read the accelerometer values
  x_accel = readAccelerometerX();
  y_accel = readAccelerometerY();
 z_accel = readAccelerometerZ();
  // Calculate the angle of tilt in each axis
  x_angle = atan(x_accel / sqrt(y_accel*y_accel + z_accel*z_accel)) * 180 / PI;
  y_angle = atan(y_accel / sqrt(x_accel*x_accel + z_accel*z_accel)) * 180 / PI;
  // Apply the low-pass filter to the accelerometer values
  x_accel_filtered = x_accel_filtered * LPF_COEFF + (1 - LPF_COEFF) * x_accel;
  y_accel_filtered = y_accel_filtered * LPF_COEFF + (1 - LPF_COEFF) * y_accel;
  z_accel_filtered = z_accel_filtered * LPF_COEFF + (1 - LPF_COEFF) * z_accel;
  // Calculate the filtered angle of tilt in each axis
  x_angle_filtered = atan(x_accel_filtered / sqrt(y_accel_filtered*y_accel_filtered + z_accel_filtered*z_accel_filtered)) * 180 / PI;
  y_angle_filtered = atan(y_accel_filtered / sqrt(x_accel_filtered*x_accel_filtered + z_accel_filtered*z_accel_filtered)) * 180 / PI;
    // Check if the board is tilted in the direction of LED1
  if (x_angle_filtered > LED1_THRESHOLD && x_angle_filtered < MAX_ANGLE - LED1_DEADZONE)
  {
    // Turn on LED1
    turnOnLED(1);
  }
  else
  {
    // Turn off LED1
    turnOffLED(1);
  }

  // Check if the board is tilted in the direction of LED2
  if (y_angle_filtered > LED2_THRESHOLD && y_angle_filtered < MAX_ANGLE - LED2_DEADZONE)
  {
    // Turn on LED2
    turnOnLED(2);
  }
  else
  {
    // Turn off LED2
    turnOffLED(2);
  }

  // Check if the board is tilted in the direction of LED3
  if (y_angle_filtered < -LED3_THRESHOLD && y_angle_filtered > -(MAX_ANGLE - LED3_DEADZONE))
  {
    // Turn on LED3
    turnOnLED(3);
  }
  else
  {
    // Turn off LED3
    turnOffLED(3);
  }
  // Check if the button has been pressed
  if (isButtonPressed())
  {
    // Switch between the tilt switch and LED blinking modes
    if (mode == 0)
    {
      // Switch to LED blinking mode
      mode = 1;
      // Play the "beep" sound to indicate that the system is paused
      playBeep();
    }
    else
    {
      // Switch to tilt switch mode
      mode = 0;
    }
  }
  // Check the current mode
  if (mode == 0)
  {
    // Tilt switch mode
    // Continue reading and processing the accelerometer values
  }
  else
  {
    // LED blinking mode
    // Blink the LEDs in the specified pattern
    if (getTime() % 1.5 < 1)
    {
      // Turn on LED1 and LED2
      turnOnLED(1);
      turnOnLED(2);
    }
    else
    {
      // Turn on LED3 and LED4
      turnOnLED(3);
      turnOnLED(4);
    }
  }
}
