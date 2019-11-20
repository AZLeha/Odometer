#include "stm32f10x.h"
#include "LCD.h"
#include "UART.h"
#include "peripheral.h"


#define  PacketHeader_default {.array={0xAA,0x3C,0x55,0}}
typedef union {
	uint32_t header;
	struct {
	    uint8_t separator[3];
		uint8_t comand;
	};
	uint8_t array[4];
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


State GlobalState  = StateSTOP;
DataStruct GlobalData={0};

//обработчик кноки
void StartButtonHandler(PinState pin_event);
//обработчики дачика оборотов
void RightAngularRateSensorHandler(PinState pin_event);
void LeftAngularRateSensorHandler(PinState pin_event);


void StateMachine(State state, DataStruct *data);





int main(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

	GPIOC->CRH |= GPIO_CRH_MODE13;
	GPIOC->CRH &= ~GPIO_CRH_CNF13;


	LCD_Init();
	UART_Init();
	InitExternalInterrupt();
	InitTimer();
	InitDWT();



	/*DWT_CYCCNT  = 0;
	uint32_t x = DWT_CYCCNT;*/



	for(;;)
	{
		StateMachine(GlobalState, &GlobalData);
	}
}


void StartButtonHandler(PinState pin_event)
{
	if(pin_event==PIN_UP)
	{
		//USART1->DR='1';
		GlobalState = !GlobalState;

	}
	else
	{
		//USART1->DR='2';
	}
}


void RightAngularRateSensorHandler(PinState pin_event)
{
	if(pin_event==PIN_UP)
	{
		USART1->DR='3';
	}
	else
	{
		USART1->DR='4';
	}

}


void LeftAngularRateSensorHandler(PinState pin_event)
{
	if(pin_event==PIN_UP)
	{
		USART1->DR='5';
	}
	else
	{
		USART1->DR='6';
	}
}



void StateMachine(State state, DataStruct *data)
{
	static State  curentState = StateRUN;

	if(state==curentState)
	{
		//≈сли сто€ние SYOP и не мен€лось выходим так как перотрисовка не требуетс€
		if(curentState==StateSTOP) return;
	}
	else
	{
		curentState=state;

		PacketHeader comandStatecanget =PacketHeader_default;


		comandStatecanget.comand= curentState ? 0x80 : 0x81;
		for(uint8_t i=0; i < 4; i++)
		{

			USART1->DR=  comandStatecanget.array[i];
			for(int j=0; j<0xffff;j++) asm("nop");
		}
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
	}
	else
	{
		LCD_Clear();
		LCD_WriteString("Wait-to-Start");
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
	TIM2->SR &= ~TIM_SR_UIF; // сбрасываем прерывание

	//USART1->DR='t';
	if(GPIOC->IDR & GPIO_IDR_IDR13)
	GPIOC->BSRR=GPIO_BSRR_BR13;
		//for (int i = 0; i < 0x9ffff; i++) asm("nop");
	else GPIOC->BSRR=GPIO_BSRR_BS13;
	//	for (int i = 0; i < 0x9ffff; i++) asm("nop");
}


