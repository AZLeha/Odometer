#include "UART.h"
#include "stm32f10x.h"
#include "LCD.h"
#include <stdbool.h>

bool isDMAFree = true;

void UART_Init(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

	//UART1 TX(PA9)  RX(PA10)

	GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_MODE10);
	GPIOA->CRH |= GPIO_CRH_MODE9;
	GPIOA->CRH &= ~GPIO_CRH_MODE10;

	GPIOA->CRH &= ~(GPIO_CRH_CNF9 | GPIO_CRH_CNF10);
	GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_0;


	//0x1D4C 9600;
	USART1->BRR = 0x270;
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE ;
	USART1->CR2 = 0;
	USART1->CR3 = USART_CR3_DMAT; //select DMA for tx
	USART1->CR1 |= USART_CR1_UE;

	NVIC_EnableIRQ (USART1_IRQn);

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	//4 channel
	DMA1_Channel4->CPAR =(uint32_t)(&USART1->DR);
	DMA1_Channel4->CCR = DMA_CCR4_PL | DMA_CCR4_MINC | DMA_CCR4_DIR | DMA_CCR4_TCIE;
	NVIC_SetPriority(DMA1_Channel4_IRQn,4);
	NVIC_EnableIRQ (DMA1_Channel4_IRQn);

}
bool TransmitDataViaDMA(void * aData, uint16_t dataSize)
{
	if(!isDMAFree) return false;
	isDMAFree = false;

	while(DMA1_Channel4->CCR&DMA_CCR1_EN) asm("nop");

	DMA1_Channel4->CNDTR = dataSize;
	DMA1_Channel4->CMAR = (uint32_t)aData;

	DMA1_Channel4->CCR |= DMA_CCR4_EN;


	return true;
}


void DMA1_Channel4_IRQHandler()
{
	DMA1->IFCR=DMA_IFCR_CTCIF4;
	DMA1_Channel4->CCR&=~DMA_CCR1_EN;
	isDMAFree=true;

}

