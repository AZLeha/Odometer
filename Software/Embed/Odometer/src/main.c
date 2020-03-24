#include "stm32f10x.h"
#include "LCD.h"
#include "UART.h"
#include "peripheral.h"
#include "dynamometers.h"



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


#pragma pack(push, 1)
typedef struct
{
	int16_t leftRPM;
	int16_t rightRPM;

	int32_t leftDyno;
	int32_t rightDyno;
} DataStruct;
#pragma pack(pop)

typedef struct
{
	uint32_t leftRPM;
	uint32_t rightRPM;
} RPM;

State GlobalState  = StateSTOP;
DataStruct GlobalData;
RPM GlobalRateSensor;


void StartButtonHandler(PinState pin_event);

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
	Dynamometers_init();
	InitExternalInterrupt();
	InitTimer();


	InitDWT();


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
}




void RightAngularRateSensorHandler(PinState pin_event)
{
	static uint32_t DWT_Count = 0;
	static uint8_t localState = 0;
	NVIC_DisableIRQ(TIM2_IRQn);

	if(pin_event == PIN_UP)
	{

		if(!localState)
		{
			DWT_Count = DWT_CYCCNT;
			localState++;
		}
		else
		{
			if (GlobalRateSensor.rightRPM== 0) //Для гашения выбросов отризаем 1 измерение если оно попало вромежут отсчёта таймера
			{									// иначе могут быть выбросы после 0
				GlobalRateSensor.rightRPM = 1;
				localState = 0;
				return;
			}


			uint32_t time = DWT_CYCCNT - DWT_Count;		//Определяем сколько прошло времени с пролого момента
			if (time > GlobalRateSensor.rightRPM) GlobalRateSensor.rightRPM = time; //тут идёт выбока самого длинного промежутка тоетсь минимальных оборотов

			localState = 0;
		}
	}

	NVIC_EnableIRQ(TIM2_IRQn);
}


void LeftAngularRateSensorHandler(PinState pin_event)
{

	static uint32_t DWT_Count = 0;
	static uint8_t localState = 0;
	NVIC_DisableIRQ(TIM2_IRQn);



	if(pin_event == PIN_UP)
	{

		if(!localState)
		{
			DWT_Count = DWT_CYCCNT;
			localState++;
		}
		else
		{
			if (GlobalRateSensor.leftRPM == 0) //Для гашения выбросов отризаем 1 измерение если оно попало вромежут отсчёта таймера
			{									// иначе могут быть выбросы после 0
				GlobalRateSensor.leftRPM = 1;
				localState = 0;
				return;
			}


			uint32_t time = DWT_CYCCNT - DWT_Count;		//Определяем сколько прошло времени с пролого момента
			if (time > GlobalRateSensor.leftRPM) GlobalRateSensor.leftRPM = time; //тут идёт выбока самого длинного промежутка тоетсь минимальных оборотов

			localState = 0;
		}
	}

	NVIC_EnableIRQ(TIM2_IRQn);

}








void StateMachine(State state, DataStruct *data)
{
	static State  curentState = StateRUN;

	if(state==curentState)
	{
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
			if(TransmitDataViaDMA(data, sizeof(DataStruct))) break;
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
	//rpm= (60*F_CPU)/(PD * 42)

	TIM2->SR &= ~TIM_SR_UIF;
	static const uint32_t divided = 72000000*60;



	if(GlobalState == StateRUN)
	{

		uint32_t local_leftRPM = GlobalRateSensor.leftRPM;
		uint32_t local_rightRPM = GlobalRateSensor.rightRPM;

		GlobalRateSensor.leftRPM = GlobalRateSensor.rightRPM = 0;

		if(local_leftRPM<=1) GlobalData.leftRPM = 0;
		else
		{
			local_leftRPM *= 42;
			GlobalData.leftRPM = divided / local_leftRPM;

		}

		if(local_rightRPM<=1) GlobalData.rightRPM = 0;
		else
		{
			local_rightRPM *= 42;
			GlobalData.rightRPM = divided / local_rightRPM;
		}



		static int8_t cc=0;

		if(Dynamometers_isDataReady())
		{
			Dynamometers_StartMeasurement();
			cc=-1;
		}

		if(cc>3)
		{
			GlobalData.rightDyno = Dynamometers_Data2();
			GlobalData.leftDyno = Dynamometers_Data1();

			Dynamometers_StartMeasurement();

			cc=-1;
		}
		cc++;


		uartWrite(sizeof(DataStruct),&GlobalData);
	}
}


