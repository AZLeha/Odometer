#include "stm32f10x.h"
#include "LCD.h"
#include "UART.h"
#include "peripheral.h"
#define COUNT_TO_TIMER

#define  PacketHeader_default {.array={0xAA,0x3C,0x55,0}}
typedef union {
	uint8_t array[4];

	struct {
	    uint8_t separator[3];
		uint8_t comand;
	};

}PacketHeader;



typedef enum
{
	PIN_DOWEN=0,
	PIN_UP
} PinState;


typedef enum
{
	StateRUN=0,
	StateSTOP
} State;

typedef struct
{
	int16_t leftRPM;
	int16_t rightRPM;

	int16_t leftDyno;
	int16_t rightDyno;
} DataStruct;


typedef struct
{
	int64_t leftRPM;
	int64_t rightRPM;
} RPM;

State GlobalState  = StateSTOP;
DataStruct GlobalData;
RPM GlobalRateSensor;

//���������� �����
void StartButtonHandler(PinState pin_event);
//����������� ������ ��������
void RightAngularRateSensorHandler(PinState pin_event);
void LeftAngularRateSensorHandler(PinState pin_event);





void StateMachine(State state, DataStruct *data);
void uartWrite(uint8_t comand,DataStruct *data);




int main(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	GPIOC->CRH |= GPIO_CRH_MODE13;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;


	GlobalRateSensor.leftRPM = 0 ;
	GlobalRateSensor.rightRPM = 0 ;
	LCD_Init();
	UART_Init();
	InitExternalInterrupt();
	InitTimer();


	#ifdef COUNT_TO_TIMER
	#else
	InitDWT();
	#endif



	/*DWT_CYCCNT  = 0;
	uint32_t x = DWT_CYCCNT;*/
	GlobalData.leftDyno = 0;
	GlobalData.rightDyno = 0;
	GlobalData.leftRPM = 0;
	GlobalData.rightRPM = 10;

	for(;;)
	{
		StateMachine(GlobalState, &GlobalData);
	}
}


void StartButtonHandler(PinState pin_event)
{
	if(pin_event==PIN_UP)
	{
		GlobalState = !GlobalState;
	}
	else
	{

	}
}


void RightAngularRateSensorHandler(PinState pin_event)
{
#ifdef COUNT_TO_TIMER

	NVIC_DisableIRQ(TIM2_IRQn);
	GlobalRateSensor.rightRPM = GlobalRateSensor.rightRPM +1;
	NVIC_EnableIRQ(TIM2_IRQn);
#else
	static uint8_t count=0;
	static uint32_t DWT_Count = 0;

	count++;


	if(count == 8)
	{
		uint32_t time = DWT_Count;

		DWT_Count = DWT_CYCCNT;

		time = DWT_Count - time;
								//(time * 14 *60 )/f_clc
	   float x = time*14;
			   x=x/72000000;
			   x=60/x;

		GlobalData.rightRPM = x;
		count = 0;
	}
#endif

}


void LeftAngularRateSensorHandler(PinState pin_event)
{
#ifdef COUNT_TO_TIMER
	NVIC_DisableIRQ(TIM2_IRQn);
	GlobalRateSensor.leftRPM = GlobalRateSensor.leftRPM +1;
	NVIC_EnableIRQ(TIM2_IRQn);
#else

	static uint8_t count=0;
	static uint32_t DWT_Count = 0;

	count++;


	if(count == 8)
	{
		uint32_t time = DWT_Count;

		DWT_Count = DWT_CYCCNT;

		time = DWT_Count - time;

		float x = time*14;
					   x=x/72000000;
					   x=60/x;

		GlobalData.leftRPM = x;
		count = 0;
								//(time * 14 *60 )/f_clc
		//GlobalData.leftRPM = (time *840) / 72000000;
		//count = 0;
	}
	#endif
}








void StateMachine(State state, DataStruct *data)
{
	static State  curentState = StateRUN;

	if(state==curentState)
	{
		//���� ������� STOP � �� �������� ������� ��� ��� ������������ �� ���������
		if(curentState==StateSTOP) return;
	}
	else
	{
		curentState=state;

		if(curentState == StateSTOP)
		{
			GPIOC->BSRR=GPIO_BSRR_BR13;
			TIM2->CR1 &= ~TIM_CR1_CEN;
		}
		else
			GPIOC->BSRR=GPIO_BSRR_BS13;

		//uartWrite(curentState ? 0x80 : 0x81, 0);

		for(int j=0; j<0xfffff;j++) asm("nop");

		if(curentState == StateRUN)  TIM2->CR1 |= TIM_CR1_CEN;



	}

	if(state==StateRUN)
	{

		LCD_SetCursor(0,0);
		LCD_WriteString("LR:     ");
		LCD_SetCursor(0,3);
		LCD_WriteInt(data->leftRPM);



		LCD_SetCursor(0,8);
		LCD_WriteString("RR:     ");
		LCD_SetCursor(0,11);
		LCD_WriteInt(data->rightRPM);

		LCD_SetCursor(1,0);
		LCD_WriteString("LD:     ");
		LCD_SetCursor(1,3);
		LCD_WriteInt(data->leftDyno);


		LCD_SetCursor(1,8);
		LCD_WriteString("RD:     ");
		LCD_SetCursor(1,11);
		LCD_WriteInt(data->rightDyno);


		for(int j=0; j<0xffff;j++) asm("nop");

	}
	else
	{
		LCD_Clear();
		LCD_WriteString("Wait-to-Start");
	}


}

void uartWrite(uint8_t comand,DataStruct *data)
{
  	static PacketHeader comandStatecanget = PacketHeader_default;
	comandStatecanget.comand = comand;


	for(int i = 0; i < 0xfffff; i++)
	{
		if(TransmitDataViaDMA(comandStatecanget.array, 4)) break;
	}

	if(!(comand & 0x80))
	{
		for(int i = 0; i < 0xfffff; i++)
		{
			if(TransmitDataViaDMA(data, 16)) break;
		}
	}

}





void USART1_IRQHandler(void)
{
	USART1->SR=0;
	if(USART1->DR == 0x80) GlobalState = StateSTOP;
	if(USART1->DR == 0x81) GlobalState = StateRUN;
}




void EXTI15_10_IRQHandler()
{
	//right angular rate sensor
	if(EXTI->PR & EXTI_PR_PR11)
	{
		EXTI->PR |= EXTI_PR_PR11;
		RightAngularRateSensorHandler(GPIOA->IDR & EXTI_PR_PR11 ? PIN_UP : PIN_DOWEN);
	}

	//Left angular rate sensor
	if(EXTI->PR & EXTI_PR_PR12)
	{
		EXTI->PR |= EXTI_PR_PR12;
		LeftAngularRateSensorHandler(GPIOA->IDR & EXTI_PR_PR12 ? PIN_UP : PIN_DOWEN);
	}

	//start button
	if(EXTI->PR & EXTI_PR_PR15)
	{
		EXTI->PR |= EXTI_PR_PR15;
		StartButtonHandler(GPIOC->IDR & EXTI_PR_PR15 ? PIN_UP : PIN_DOWEN);
	}

}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF; // ���������� ����������

	static uint8_t tt=0;
	if(tt )
	{
		GPIOC->BSRR=GPIO_BSRR_BR13;
		tt=0;
	}
	else
	{	GPIOC->BSRR=GPIO_BSRR_BS13;
		tt++;
	}



	if(GlobalState == StateRUN)
	{
		#ifdef COUNT_TO_TIMER
		float t=GlobalRateSensor.leftRPM;
		t/=8.4;
		t*=60;
		GlobalData.leftRPM =t;

		t=GlobalRateSensor.rightRPM;
		t/=8.4;
		t*=60;
		GlobalData.rightRPM =t;



		//GlobalData.leftRPM = ((GlobalRateSensor.leftRPM*/84 ;
		//GlobalData.rightRPM = ((GlobalRateSensor.rightRPM*60)*10)/84 ;

		GlobalRateSensor.leftRPM = 0;
		GlobalRateSensor.rightRPM = 0;
		#endif
		uartWrite(8,&GlobalData);
	}
}


