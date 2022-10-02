/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Drivers
#include "drivers/headers/uart.h"
#include "drivers/headers/I2C.h"
#include "drivers/headers/Accelerometer.h"

// Timer
#include "timer/timer.h"

// App
#include "App.h"

// Standar libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define TIME_POLLING  1000 // en ms

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static int xd = 0;
static test = 0;
static data_already_proccess = true;
static tim_id_t id_polling;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* Todos los init necesarios */
void App_Init(void) {
  timerInit();		// Inicializa timers

  // UART init
  int id = UARTID;
  uart_cfg_t config = {.baudrate = UARTBAUDRATE};
  uartInit(id, config);

  // I2C init
  I2C_init(i2c_baudrate_111607Hz, 0); //  INICIALIZO EL I2C i2c_baudrate_111607Hz
  test = 0;
  id_polling = timerGetId();

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	if ( xd == 0){
		initAccelerometer();
		xd = 1;
		timerStart(id_polling, TIMER_MS2TICKS(TIME_POLLING), 0 , NULL);
		// ID que se usará para el polling de la petición de datos al acelerometro
	}
	//tim_id_t id_polling = timerGetId();
	if(get_alreadyInit())
	{
		if(data_already_proccess && timerExpired(id_polling)) // && timerExpired(id_polling)
		{
			//SRAWDATA mag;
			//SRAWDATA acel;
			//printf("me inicialice carajo");
			ReadAccelMagnData();
			test = 1;
			data_already_proccess = false;
			timerStart(id_polling, TIMER_MS2TICKS(TIME_POLLING), 0 , NULL);
		}
		if (  get_alreadyread_AccelMagnData() )
		{
			SRAWDATA aceleration = get_aceleration();
			proccess_data(aceleration);
			data_already_proccess = true;
			set_alreadyread_AccelMagnData(0);
		}
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
