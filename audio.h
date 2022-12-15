#ifndef AUDIO_H
#define AUDIO_H

// Function to initialise audio output
void initAudio() {
  // Enable clock for I2C1, SPI2, and I2S2 peripherals
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  RCC->APB1ENR |= RCC_APB1ENR_I2S2EN;

  // Configure I2C1 pins PB6 (SCL) and PB9 (SDA)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  GPIOB->MODER |= (GPIO_MODER_MODER6_1 |
                   GPIO_MODER_MODER9_1);
  GPIOB->AFR[0] |= (4 << 24);  // AF4 for PB6
  GPIOB->AFR[1] |= (4 << 4);   // AF4 for PB9

  // Configure I2S2 pins PC3 (CK), PC2 (SD), and PC0 (WS)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  GPIOC->MODER |= (GPIO_MODER_MODER3_1 |
                   GPIO_MODER_MODER2_1 |
                   GPIO_MODER_MODER0_1);
  GPIOC->AFR[0] |= (5 << 12);  // AF5 for PC3
  GPIOC->AFR[0] |= (5 << 8);   // AF5 for PC2
  GPIOC->AFR[0] |= (5 << 0);   // AF5 for PC0

  // Configure I2S2 pins PA4 (MCK) and PA5 (SD)
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  GPIOA->MODER |= (GPIO_MODER_MODER4_1 |
                   GPIO_MODER_MODER5_1);
  GPIOA->AFR[0] |= (6 << 16);  // AF6 for PA4
  GPIOA->AFR[0] |= (6 << 20);  // AF6 for PA5

  // Configure I2S2 master clock (I2S2_CKIN) on PA8
  GPIOA->MODER |= GPIO_MODER_MODER8_1;
  GPIOA->AFR[1] |= (5 << 0);  // AF5 for PA8

  // Configure I2S2_MCK pin (I2S2_MCK) on PB10
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  GPIOB->MODER |= GPIO_MODER_MODER10_1;
  GPIOB->AFR[1] |= (5 << 8);  // AF5 for PB10

  // Configure I2S2_SD pin (I2S2_SD) on PB15
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  GPIOB->MODER |= GPIO_MODER_MODER15_1;
  GPIOB->AFR[1] |= (5 << 28);  // AF5 for PB15

    // Set up I2C1 and I2S2 peripherals
    I2C1->CR1 |= I2C_CR1_PE;  // Enable I2C1 peripheral
    I2C1->CR2 |= I2C_CR2_FREQ_2;  // Set clock frequency to 8 MHz
    I2C1->CCR |= I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR;  // Set I2C1 speed to 400 kHz
    I2C1->TRISE = 9;  // Set I2C1 rise time to 300 ns
    I2S2->I2SCFGR |= I2S_I2SCFGR_I2SMOD | I2S_I2SCFGR_I2SE | I2S_I2SCFGR_I2SCFG_0 | I2S_I2SCFGR_I2SCFG_1;  // Configure I2S2 as master transmit
    I2S2->I2SPR |= I2S_I2SPR_MCKOE | I2S_I2SPR_ODD;  // Enable master clock output and set data length to 24 bits

    // Set up CS43L22 audio DAC
    uint8_t initData[] = {0x99, 0x80};  // Initialization data
    I2C1->CR1 |= I2C_CR1_START;  // Generate start condition
    I2C1->DR = 0x94;  // Send slave address + write bit
    while (!(I2C1->SR1 & I2C_SR1_SB)) {}  // Wait for start condition to be sent
    I2C1->DR = 0x01;  // Send register address
    while (!(I2C1->SR1 & I2C_SR1_ADDR)) {}  // Wait for slave address to be sent
    (void)I2C1->SR2;  // Clear ADDR flag
    I2C1->DR = initData[0];  // Send first initialization data byte
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}  // Wait for data to be sent
    I2C1->DR = initData[1];  // Send second initialization data byte
    while (!(I2C1->SR1 & I2C_SR1_TXE)) {}  // Wait for data to be sent
    I2C1->CR1 |= I2C_CR1_STOP;  // Generate stop condition
    while ((I2C1->SR2 & I2C_SR2_BUSY)) {}  // Wait for stop condition to be sent
}


// Function to generate a beep sound
void playBeep() {
  // Set up I2S2 DMA transfer
  DMA1_Stream4->CR |= DMA_SxCR_CHSEL_0;  // Set channel to 0
  DMA1_Stream4->CR |= DMA_SxCR_PL_1;  // Set priority to high
  DMA1_Stream4->CR |= DMA_SxCR_MINC;  // Memory increment mode
  DMA1_Stream4->CR |= DMA_SxCR_DIR_0;  // Memory to peripheral
  DMA1_Stream4->CR |= DMA_SxCR_MSIZE_0;  // Memory size to 16 bits
  DMA1_Stream4->CR |= DMA_SxCR_PSIZE_0;  // Peripheral size to 16 bits
  DMA1_Stream4->CR |= DMA_SxCR_EN;  // Enable DMA stream
  I2S2->I2SCFGR |= I2S_I2SCFGR_I2SE;  // Enable I2S2 peripheral

  // Generate beep sound
  while (!(DMA1->HISR & DMA_HISR_TCIF4)) {}  // Wait for DMA transfer to complete
  I2S2->I2SCFGR &= ~I2S_I2SCFGR_I2SE;  // Disable I2S2 peripheral
  DMA1_Stream4->CR &= ~DMA_SxCR_EN;  // Disable DMA stream
}

 
#endif  // AUDIO_H
