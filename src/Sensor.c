#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

#include "oled.h"
#include "light.h"


static void Sensor_initI2C(void);
static void Sensor_initSSP(void);
static void Sensor_initADC(void);


typedef struct tag_sensor
{
	uint32_t sensor_data;
	uint32_t I2C_Interrupt_Done_Flag;
}ttag_sensor;


ttag_sensor ClassHandle;


void Sensor_new(void)
{
	Sensor_initI2C();
	Sensor_initSSP();
	Sensor_initADC();

	oled_init();


	//Initialize sensor
	light_init(); //Unnecessary
	light_enable();

}

uint32_t Sensor_read(void)
{
	ClassHandle.I2C_Interrupt_Done_Flag = 0;
	NVIC_EnableIRQ(I2C2_IRQn);
	/* analog input connected to BNC */
	ADC_StartCmd(LPC_ADC,ADC_START_NOW);
	return ClassHandle.sensor_data;
}


void I2C2_IRQHandler(void)
{
	/* Interrupt mode: Call the stream interrupt handler */

	//Disable I2C2 Interrupt
	NVIC_DisableIRQ(I2C2_IRQn);

	ClassHandle.sensor_data = light_read();
	ClassHandle.I2C_Interrupt_Done_Flag = 1;
}

static void Sensor_initI2C(void)

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

	NVIC_EnableIRQ(I2C2_IRQn);
	ClassHandle.I2C_Interrupt_Done_Flag = 0;


}
static void Sensor_initSSP(void)
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
static void Sensor_initADC(void)
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

	//NVIC_EnableIRQ(ADC_IRQn);
	ADC_IntConfig(LPC_ADC,ADC_CHANNEL_5,ENABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_5,ENABLE);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);
	ADC_StartCmd(LPC_ADC,ADC_START_NOW);

}


