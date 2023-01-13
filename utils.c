// Include the necessary libraries (needing implementation)
#include "accel.h" // Header file for accelerometer library
#include "audio.h"  // Header file for audio library (assuming this is the library for the CS43L22)


// Function to initialise timer
void initTimer() {
  // Enable clock for TIM3
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

  // Configure TIM3 to generate update events every 1ms
  TIM3->PSC = 42000 - 1;
  TIM3->ARR = 1000 - 1;
  TIM3->EGR |= TIM_EGR_UG;
  TIM3->CR1 |= TIM_CR1_URS | TIM_CR1_CEN;
}

// Function to initialise LED
void initLEDs() {
  // Enable clock for GPIOD peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  // Configure LED pins as output
  GPIOD->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 |
                   GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);

  // Configure LED pins as low-speed
  GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR12 | GPIO_OSPEEDR_OSPEEDR13 |
                     GPIO_OSPEEDR_OSPEEDR14 | GPIO_OSPEEDR_OSPEEDR15);

  // Configure LED pins as no pull-up, no pull-down
  GPIOD->PUPDR |= (GPIO_PUPDR_PUPDR12_0 | GPIO_PUPDR_PUPDR13_0 |
                   GPIO_PUPDR_PUPDR14_0 | GPIO_PUPDR_PUPDR15_0);
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

// Function to initialise button
void initButton() {
  // Enable clock for GPIOA peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // Configure GPIOA pin 0 as input
  GPIOA->MODER &= ~GPIO_MODER_MODER0;

 // Configure button pin as low-speed
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR0;

  // Configure button pin as pull-up
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0;

  // Enable clock for SYSCFG
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // Configure EXTI0 to trigger on rising edge
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
  EXTI->RTSR |= EXTI_RTSR_TR0;
  EXTI->IMR |= EXTI_IMR_MR0;
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

// // Function to initialise I2C
// void initI2C() {
//   // Enable clock for GPIOB
//   RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
//   // Configure I2C1_SCL and I2C1_SDA pins as AF
//   GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER9_1);
//   // Configure I2C1_SCL and I2C1_SDA pins as high-speed
//   GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR9);
//   // Configure I2C1_SCL and I2C1_SDA pins as open-drain
//   GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_9);
//   // Configure I2C1_SCL and I2C1_SDA pins as no pull-up, no pull-down
//   GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR9_0);
//   // Configure I2C1_SCL and I2C1_SDA pins as AF4
//   GPIOB->AFR[0] |= (4 << 24);
//   GPIOB->AFR[1] |= (4 << 4);
//   // Enable clock for I2C1
//   RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
//   // Configure I2C1_SCL and I2C1_SDA pins as alternate function
//   I2C1->CR1 |= I2C_CR1_PE;
//   // Configure I2C1 clock speed
//   I2C1->CR2 |= (42 << I2C_CR2_FREQ_Pos);
//   I2C1->CCR |= (0x1E << I2C_CCR_CCR_Pos);
//   I2C1->TRISE = (42 + 1) * 300 / 1000 + 1;
// }

// // Function to initialise DMA
// void initDMA() {
//   // Enable clock for DMA1
//   RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
//   // Configure DMA1_Stream7 to transfer data from memory to I2S
//   DMA1_Stream7->CR |= (7 << DMA_SxCR_CHSEL_Pos);
//   DMA1_Stream7->CR |= DMA_SxCR_MINC;
//   DMA1_Stream7->CR |= (2 << DMA_SxCR_MSIZE_Pos);
//   DMA1_Stream7->CR |= (2 << DMA_SxCR_PSIZE_Pos);
//   DMA1_Stream7->CR |= DMA_SxCR_DIR_0;
//   DMA1_Stream7->CR |= DMA_SxCR_TCIE;
//   // Enable DMA1_Stream7 interrupt
//   NVIC_SetPriority(DMA1_Stream7_IRQn, 0);
//   NVIC_EnableIRQ(DMA1_Stream7_IRQn);
// }

// // Function to set the audio buffer for the DMA
// void setAudioBuffer(int16_t *buffer) {
//   DMA1_Stream7->PAR = (uint32_t)&SPI2->DR;
//   DMA1_Stream7->M0AR = (uint32_t)buffer;
//   DMA1_Stream7->NDTR = BUFFER_SIZE;
// }

// // Function to start the audio output
// void startAudioOutput() {
//   // Enable I2S
//   SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
//   // Enable DMA request
//   SPI2->CR2 |= SPI_CR2_TXDMAEN;
//   // Enable DMA transfer
//   DMA1_Stream7->CR |= DMA_SxCR_EN;
// }

// // DMA1_Stream7 interrupt handler
// void DMA1_Stream7_IRQHandler() {
//   if (DMA1->LISR & DMA_LISR_TCIF7) {
//     // Clear the transfer complete flag
//     DMA1->LIFCR |= DMA_LIFCR_CTCIF7;
//     // Generate the audio data
//     generateAudioData(audio_buffer);
//   }
// }

// Function to turn on a LED
void turnOnLED(int led_num) {
  GPIOD->BSRRL = (1 << (led_num + 12));
}

// Function to turn off a LED
void turnOffLED(int led_num) {
  GPIOD->BSRRH = (1 << (led_num + 12));
}