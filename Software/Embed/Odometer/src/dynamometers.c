#include "dynamometers.h"
#include "stm32f10x.h"

static bool _isDataReadyChanel1 = false;
static bool _isDataReadyChanel2 = false;

static uint32_t _data1 = 0;
static uint32_t _data2 = 0;

static bool TransmitDataViaDMA(DMA_Channel_TypeDef *pxDmaChannel ,const void *aData, uint16_t dataSize);


void Dynamometers_init(void)
{
	RCC->APB2ENR |=  RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_USART3EN;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	//UART2 TX(PA2)  RX(PA3)
	GPIOA->CRL &= ~(GPIO_CRL_MODE2 | GPIO_CRL_MODE3);
	GPIOA->CRL |= GPIO_CRL_MODE2;
	GPIOA->CRL &= ~GPIO_CRL_MODE3;

	GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_CNF3);
	GPIOA->CRL |= GPIO_CRL_CNF2_1 | GPIO_CRL_CNF3_0;



	//UART3 TX(PB10)  RX(PB11)
	GPIOB->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_MODE11);
	GPIOB->CRH |= GPIO_CRH_MODE10;
	GPIOB->CRH &= ~GPIO_CRH_MODE11;

	GPIOB->CRH &= ~(GPIO_CRH_CNF10 | GPIO_CRH_CNF11);
	GPIOB->CRH |= GPIO_CRH_CNF10_1 | GPIO_CRH_CNF11_0;





	//======================UART2=============================
	//9600;
	USART2->BRR = 0xEA6;
	USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_PCE | USART_CR1_M;
	USART2->CR2 = 0;
	USART2->CR3 = USART_CR3_DMAT; //select DMA for tx
	NVIC_SetPriority(USART2_IRQn,4);
	NVIC_EnableIRQ (USART2_IRQn);




	//4 channel
	DMA1_Channel7->CPAR =(uint32_t)(&USART2->DR);
	DMA1_Channel7->CCR = DMA_CCR7_PL | DMA_CCR7_MINC | DMA_CCR7_DIR | DMA_CCR7_TCIE;
	NVIC_SetPriority(DMA1_Channel7_IRQn,4);
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);

	USART2->CR1 |= USART_CR1_UE;



	//======================UART3=============================
	//9600;
	USART3->BRR = 0xEA6;
	USART3->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_PCE | USART_CR1_M;
	USART3->CR2 = 0;
	USART3->CR3 = USART_CR3_DMAT; //select DMA for tx
	NVIC_SetPriority(USART3_IRQn,4);
	NVIC_EnableIRQ (USART3_IRQn);


	//4 channel
	DMA1_Channel2->CPAR =(uint32_t)(&USART3->DR);
	DMA1_Channel2->CCR = DMA_CCR2_PL | DMA_CCR2_MINC | DMA_CCR2_DIR | DMA_CCR2_TCIE;
	NVIC_SetPriority(DMA1_Channel2_IRQn,4);
	NVIC_EnableIRQ (DMA1_Channel2_IRQn);

	USART3->CR1 |= USART_CR1_UE;

}



bool TransmitDataViaDMA(DMA_Channel_TypeDef *pxDmaChannel ,const void *aData, uint16_t dataSize)
{
	if(pxDmaChannel->CCR&DMA_CCR1_EN) return false;

	pxDmaChannel->CNDTR = dataSize;
	pxDmaChannel->CMAR = (uint32_t)aData;
	pxDmaChannel->CCR |= DMA_CCR1_EN;

	return true;
}


bool Dynamometers_isDataReady(void)
{
	if(_isDataReadyChanel1 && _isDataReadyChanel2) return true;
	else return false;
}


void Dynamometers_StartMeasurement(void)
{
	_isDataReadyChanel1 = false;
	_isDataReadyChanel2 = false;
	//  Algorithm           Poly    Init    RefIn 	RefOut 	XorOut
	//	CRC-16/CCITT-FALSE	0x1021  0xFFFF	false	false	0x0000
	static const uint8_t startSequence[4] = {0x00,0x55,0x17,0x5f};

	TransmitDataViaDMA(DMA1_Channel2, startSequence, 4);
	TransmitDataViaDMA(DMA1_Channel7, startSequence, 4);
}


uint32_t Dynamometers_Data1(void)
{
	return _data1;
}


uint32_t Dynamometers_Data2(void)
{
	return _data2;
}





void USART2_IRQHandler(void)
{
	static uint8_t count = 0;
	static uint8_t localData[8];
	USART2->SR=0;

	localData[count++] = (uint8_t)USART2->DR;


	if(count>7)
	{
		uint8_t *localPtr = (uint8_t*)(&_data1);
		localPtr[0] = localData[1];
		localPtr[1] = localData[2];
		localPtr[2] = localData[3];
		if(localPtr[2] & (1<<7)) localPtr[3] = 0xff;
		else localPtr[3] = 0;
		_isDataReadyChanel1 = true;
		count = 0;
	}



}

void USART3_IRQHandler(void)
{
	static uint8_t count = 0;
	static uint8_t localData[8];
	USART3->SR=0;

	localData[count++] = (uint8_t)USART3->DR;


	if(count>7)
	{
		uint8_t *localPtr = (uint8_t*)(&_data2);
		localPtr[0] = localData[1];
		localPtr[1] = localData[2];
		localPtr[2] = localData[3];

		if(localPtr[2] & (1<<7)) localPtr[3] = 0xff;
		else localPtr[3] = 0;

		_isDataReadyChanel2 = true;
		count = 0;
	}

}




void DMA1_Channel2_IRQHandler()
{
	DMA1->IFCR=DMA_IFCR_CTCIF2;
	DMA1_Channel2->CCR&=~DMA_CCR1_EN;

}

void DMA1_Channel7_IRQHandler()
{
	DMA1->IFCR=DMA_IFCR_CTCIF7;
	DMA1_Channel7->CCR&=~DMA_CCR1_EN;

}



