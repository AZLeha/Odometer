
#include "peripheral.h"



/*! Initializing external interrupt
 *
 * - PC15 this Start Button
 * - PA11 this right angular rate sensor
 * - PA12 this left angular rate sensor
 * */
void InitExternalInterrupt(void)
{


	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPAEN |RCC_APB2ENR_AFIOEN;

	GPIOC->CRH &= ~GPIO_CRH_MODE15;
	GPIOA->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_MODE11);

	// Configure external interrupt the start button
	AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI15_PC;


	// Configure external interrupt the right angular rate sensor
	AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI11_PA;

	// Configure external interrupt the left angular rate sensor
	AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI12_PA;

	// Selecting signal edge
	EXTI->FTSR |= EXTI_FTSR_TR15 | EXTI_FTSR_TR12 | EXTI_FTSR_TR11;
	EXTI->RTSR |= EXTI_RTSR_TR15 | EXTI_RTSR_TR12 | EXTI_RTSR_TR11;

	//Enable interrupt
    EXTI->IMR |= EXTI_IMR_MR15 | EXTI_IMR_MR12 | EXTI_IMR_MR11;


	NVIC_EnableIRQ (EXTI15_10_IRQn);

}


void InitTimer(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;



	TIM2->PSC = (72000000 / 2) / 1000 - 1;
	TIM2->ARR = 200 - 1;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;

	NVIC_SetPriority(TIM2_IRQn,5);
	NVIC_EnableIRQ(TIM2_IRQn);
}

void InitDWT(void)
{
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Включаем TRACE
	SCB_DEMCR |= 0x01000000;
	DWT_CONTROL|= 1; // включаем счётчик

}



