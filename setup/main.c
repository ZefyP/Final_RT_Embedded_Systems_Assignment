#include "stm32f4xx.h"
int main(void){
// Initialize GPIO Port for LEDs
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // Enable GPIOD clock 
GPIOD->MODER |= GPIO_MODER_MODER12_0; // GPIOD pin 12 output - green LED
//Initialize Timer 2
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable timer 2 clock 
TIM2->CR1 &= ~0x00000016; /*Sets the counter as an upcounter*/
TIM2->CR1 |= 0x00000008; /*Turn off repeat - i.e. use one pulse mode. |= 0x8 
(binary 01000) is used to activate only bit 3 (note the use of |) so that 
the timer works in one-pulse (non auto repeating) mode.*/
TIM2->PSC = 8400-1; /*Prescaler value - the prescaler clock defaults to 
twice the APB1 which is running at 42MHz - so the timer clock is 84MHz*/
TIM2->ARR = 10000-1; /*sets the value in the auto-reload register*/
TIM2->EGR = 1; /*Re-initialises the timer*/
for(;;){
GPIOD->BSRR = 1<<12; // Turn on the green LED
TIM2->CR1 |= 1; //Enables the counter
while((TIM2->SR&0x0001)!=1){}; //TIM2 to implement a 1 second delay
TIM2->SR &= ~1; //Resets the flag
GPIOD->BSRR = 1<<(12+16); // Turn off the green LED
TIM2->CR1 |= 1; //Enables the counter
while((TIM2->SR&0x0001)!=1){}; //TIM2 to implement a 1 second delay
TIM2->SR &= ~1; //Resets the flag
}
}
