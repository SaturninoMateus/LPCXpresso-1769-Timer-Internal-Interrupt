/*
 * Sensor.c
 *
 *  Created on: 19/09/2016
 *      Author: edielson
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "oled.h"
#include "light.h"


static void Sensor_initI2C(void);

typedef struct tag_sensor
{
	uint32_t sensor_data;
	uint32_t I2C_Interrupt_Done_Flag;
}ttag_sensor;


ttag_sensor ClassHandle;


void Sensor_new(void)
{
	Sensor_initI2C();
	//oled_init();

	//Initialize sensor
	light_init(); //Unnecessary
	light_enable();
}

uint32_t Sensor_read(void)
{
	ClassHandle.I2C_Interrupt_Done_Flag = 0;
	NVIC_EnableIRQ(I2C2_IRQn);
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

