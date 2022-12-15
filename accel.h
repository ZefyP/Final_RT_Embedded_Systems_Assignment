#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

// Enum for board orientation
enum Orientation {
  HORIZONTAL,
  DOWN_LED1,
  DOWN_LED2,
  DOWN_LED3,
  DOWN_LED4
};

// Function to initialize accelerometer
void initAccelerometer();

// Function to get current board orientation
Orientation getOrientation();


// Initialise accelerometer
void initAccelerometer()
{    // Set I2C address of accelerometer
    LIS3DSH_InitTypeDef accelerometer_init;
    accelerometer_init.Axes_Enable = LIS3DSH_XYZ_ENABLE;
    accelerometer_init.Continous_Update = LIS3DSH_ContinousUpdate_Disabled;
    accelerometer_init.AA_Filter_BW = LIS3DSH_AA_BW_50;
    accelerometer_init.Full_Scale = LIS3DSH_FULLSCALE_2;
    accelerometer_init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_100;
    accelerometer_init.Self_Test = LIS3DSH_SELFTEST_NORMAL;
    LIS3DSH_Init(&accelerometer_init);

    // Enable clock for I2C1 peripheral
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Configure I2C1 pins (PB8 and PB9)
    GPIOB->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
    GPIOB->OTYPER |= GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9;
    GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0;
    GPIOB->AFR[1] |= (4 << 4) | (4 << 8);
}


// Read accelerometer data
accelerometer_data read_accelerometer()
{    
    accelerometer_data data;
    // Read accelerometer data registers
    LIS3DSH_ReadACC(data.raw);
    // Convert raw accelerometer data to g-force values
    data.x = (float)data.raw[0] / 16384.0;
    data.y = (float)data.raw[1] / 16384.0;
    data.z = (float)data.raw[2] / 16384.0;
    return data; 
}




#endif  // ACCELEROMETER_H
