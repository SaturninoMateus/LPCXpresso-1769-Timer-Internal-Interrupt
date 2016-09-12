/*****************************************************************************
 *   Value read from BNC is written to the OLED display (nothing graphical
 *   yet only value).
 *
 *   Copyright(C) 2010, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************/

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

#include "oled.h"

//For luminosity sensor
#include "light.h"

static uint8_t buf[10];

static void intToString(int value, uint8_t* pBuf, uint32_t len, uint32_t base)
{
	static const char* pAscii = "0123456789abcdefghijklmnopqrstuvwxyz";
	int pos = 0;
	int tmpValue = value;

	// the buffer must not be null and at least have a length of 2 to handle one
	// digit and null-terminator
	if (pBuf == NULL || len < 2)
	{
		return;
	}

	// a valid base cannot be less than 2 or larger than 36
	// a base value of 2 means binary representation. A value of 1 would mean only zeros
	// a base larger than 36 can only be used if a larger alphabet were used.
	if (base < 2 || base > 36)
	{
		return;
	}

	// negative value
	if (value < 0)
	{
		tmpValue = -tmpValue;
		value    = -value;
		pBuf[pos++] = '-';
	}

	// calculate the required length of the buffer
	do {
		pos++;
		tmpValue /= base;
	} while(tmpValue > 0);


	if (pos > len)
	{
		// the len parameter is invalid.
		return;
	}

	pBuf[pos] = '\0';

	do {
		pBuf[--pos] = pAscii[value % base];
		value /= base;
	} while(value > 0);

	return;
}

static void init_ssp(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

}


static void init_i2c(void)
{
	PINSEL_CFG_Type PinCfg;

	/* Initialize I2C2 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);
}

void Read_packet(void)
{
	/* delay */
	//5sec
	Timer0_Wait(1000*5);
}

static void init_adc(void)
{
	PINSEL_CFG_Type PinCfg;

	/*
	 * Init ADC pin connect
	 * AD0.5 on P1.31
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 31;
	PinCfg.Portnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	/* Configuration for ADC :
	 * 	Frequency at 1Mhz
	 *  ADC channel 5, no Interrupt
	 */
	ADC_Init(LPC_ADC, 1000000);


	NVIC_EnableIRQ(ADC_IRQn);
	ADC_IntConfig(LPC_ADC,ADC_CHANNEL_5,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_5,ENABLE);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);
	ADC_StartCmd(LPC_ADC,ADC_START_NOW);

}

static void led2_init(void) {
	LPC_PINCON->PINSEL1 &= (~(3 << 12));
	LPC_GPIO0->FIODIR |= (1 << 22);
}
static void led2_on(void) {
	LPC_GPIO0->FIOSET = (1 << 22);
}

static void led2_off(void) {
	LPC_GPIO0->FIOCLR = (1 << 22);
}



uint32_t ADC_Interrupt_Done_Flag = 0;
uint32_t val_luminosity = 0;

int main (void) {

	init_i2c();
	init_ssp();
	init_adc();

	oled_init();

	//Initialize sensor
	light_init(); //Unnecessary
	light_enable();

	oled_clearScreen(OLED_COLOR_WHITE);

	oled_putString(1,1,  (uint8_t*)"LUZ: ", OLED_COLOR_BLACK, OLED_COLOR_WHITE);


	while(1) {


		if (ADC_Interrupt_Done_Flag == 1)
		{
			intToString(val_luminosity, buf, 10, 10);
			oled_fillRect((1+6*6),1, 80, 8, OLED_COLOR_WHITE);
			oled_putString((1+6*6),1, buf, OLED_COLOR_BLACK, OLED_COLOR_WHITE);

			ADC_Interrupt_Done_Flag = 0;
			NVIC_EnableIRQ(ADC_IRQn);

			/* analog input connected to BNC */
			ADC_StartCmd(LPC_ADC,ADC_START_NOW);
		}

		Read_packet();

	}

}

void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}


/**
 * @brief	ADC0 interrupt handler sub-routine
 * @return	Nothing
 */
void ADC_IRQHandler(void)
{
	/* Interrupt mode: Call the stream interrupt handler */
	NVIC_DisableIRQ(ADC_IRQn);

	val_luminosity = light_read();
	ADC_Interrupt_Done_Flag = 1;
}
















