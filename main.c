/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes. */
#include "basic_io.h"


#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "display.h"


xQueueHandle Global_Queue_handle = 0;

void task_reader(void *p){
	Sensor_new();
	display_new();
	uint32_t val = 0;
	while(1){
		puts("TaskRead");
		val = Sensor_read();

		//1000 means 1sec of timeout
		if(!xQueueSend(Global_Queue_handle, &val, 1000)){
			printf("Failed to send to queue\n");
		}
		else{
			printf("Data sent sucessfull!\n");
		}
		taskYIELD();
	}
}

void task_printer(void *p){
	uint32_t val;
	while(1){
		val = 0;
		puts("TaskPrint");

		//1000 means 1sec of timeout
		if(xQueueReceive(Global_Queue_handle, &val, 1000)){
			display_write(val);
			printf("Data received successful!\n");
		}
		else{
			puts("Failed to receive data from queue");
		}


	}
}


/*-----------------------------------------------------------*/

int main( void )
{


	Global_Queue_handle = xQueueCreate(3, sizeof(uint32_t));

	/* Create two instances of the continuous processing task, with priority 5 and 1. */
	xTaskCreate( task_reader, (signed char*)"Task 1 Read", 240, NULL, 2, NULL );
	xTaskCreate( task_printer, (signed char*)"Task 2 Print", 240, NULL, 1, NULL );
	/* Create one instance of the periodic task at priority 2. */
	//xTaskCreate( vPeriodicTask, "Task 3", 240, (void*)pcTextForPeriodicTask, 2, NULL );

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();

	/* If all is well we will never reach here as the scheduler will now be
	running.  If we do reach here then it is likely that there was insufficient
	heap available for the idle task to be created. */
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/



void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing. */
}


