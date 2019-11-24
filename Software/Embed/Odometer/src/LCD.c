#include "LCD.h"
#include "stm32f10x.h"


#define LCD_IR 0
#define LCD_DR 1


static void LCDLolLVLInit();

//платформо зависиммые функции
__attribute__((always_inline)) static inline void LCDSetRegister(uint8_t reg);
__attribute__((always_inline)) static inline void LCDWriteHalfWord(uint8_t word);
static void writeOneByteinLSD(uint8_t reg, uint8_t data);

//желательно ~1ms
static void LCDdelay(int delay);



void LCD_Init()
{
	LCDdelay(150);
	LCDLolLVLInit();

	writeOneByteinLSD(LCD_IR,0x33);
	writeOneByteinLSD(LCD_IR,0x32);
	writeOneByteinLSD(LCD_IR,0x28);
	writeOneByteinLSD(LCD_IR,0x0C);
	LCD_Clear();
}


void LCD_WriteChar(char data)
{
	writeOneByteinLSD(LCD_DR,data);
}


void LCD_WriteString(char *data)
{
	for( int i = 0; (i < 15) && (*data != '\0'); i++, data++)
		LCD_WriteChar(*data);
}


void LCD_WriteInt(int data)
{
	char dataArray[6];

	sprintf(dataArray,"%d",data);
	LCD_WriteString(dataArray);
}


void LCD_Clear()
{
	writeOneByteinLSD(LCD_IR,0x01);
	LCDdelay(10);
}


void LCD_SetCursor(uint8_t row, uint8_t column)
{
	writeOneByteinLSD(LCD_IR, ((0xC0 * row) + column) | 0x80);
}






static void LCDLolLVLInit()
{
	RCC->APB2ENR|= RCC_APB2ENR_IOPAEN;
				// RS E D4 D5 D6 D7
	GPIOA->CRL|= GPIO_CRL_MODE0|GPIO_CRL_MODE1|GPIO_CRL_MODE4|GPIO_CRL_MODE5|GPIO_CRL_MODE6|GPIO_CRL_MODE7;
	GPIOA->CRL&= ~(GPIO_CRL_CNF0|GPIO_CRL_CNF1|GPIO_CRL_CNF4|GPIO_CRL_CNF5|GPIO_CRL_CNF6|GPIO_CRL_CNF7);
}


__attribute__((always_inline)) static inline void LCDSetRegister(uint8_t reg)
{
	if(reg == LCD_DR)
		GPIOA->BSRR |= GPIO_BSRR_BS0;
	else
		GPIOA->BSRR |= GPIO_BSRR_BR0;
}


__attribute__((always_inline)) static inline void LCDWriteHalfWord(uint8_t word)
{
	GPIOA->BRR |= GPIO_BRR_BR4|GPIO_BRR_BR5|GPIO_BRR_BR6|GPIO_BRR_BR7;

	GPIOA->ODR |= word<<4;

	GPIOA->BSRR |= GPIO_BSRR_BS1;
	LCDdelay(1);
	GPIOA->BSRR |= GPIO_BSRR_BR1;
	LCDdelay(1);

}


static void writeOneByteinLSD(uint8_t reg, uint8_t data)
{
	LCDSetRegister(reg);

	//MSB
	LCDWriteHalfWord(data>>4);
	LCDWriteHalfWord(data & 0x0f);
}


static void LCDdelay(int delay)
{
	for(; delay; delay--)
		for(int j=0;j<0x5ff;j++) asm("nop");
}
