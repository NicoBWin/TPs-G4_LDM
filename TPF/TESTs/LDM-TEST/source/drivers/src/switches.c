/***************************************************************************//**
  @file     switches.h
  @brief    Switches driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../../timer/timer.h"
#include "../headers/switches.h"
#include "../../MCAL/gpio.h"
#include "../board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//El switch tiene un pullup externo
#define SWACTIVE       LOW

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
//Estados de la Encoder FSM
enum states {NONE, LEFT, RIGHT, PLAY, MENU, ONOFF, VOL};
//Son 3 estados porque analizamos 3 flancos

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void swCallback(void);
static void readPins(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
//Data de encoder
static bool status;
static swResult_t swEvent;

//Timer para el encoder
static tim_id_t swTimer;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/*
 * @brief Initialize encoder
 */
void SW_Init() {
  //Inicializo Data
  swEvent = SW_NONE;
  status = false;

  //Seteo los Pines
  gpioMode(SW1, INPUT);
  gpioMode(SW2, INPUT);
  gpioMode(SW3, INPUT);
  gpioMode(SW4, INPUT);
  gpioMode(SW5, INPUT);
  gpioMode(SW6, INPUT);

  //Seteo el timer para que llame periodicamente a la callback con 1ms
  swTimer = timerGetId();
  timerStart(swTimer, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, &swCallback);
}

/*
 * @brief Check for new encoder events
 * @return true if there is an encoder event
 */
bool swGetStatus() {
	if(status) {
		status = false;
		return true;
	}
	else {
		return false;
	}
}

/*
 * @brief Get event type
 * @return event
 */
swResult_t swGetEvent() {
  return swEvent;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void swCallback(void){
	readPins();
}

// Lee todos los pins e interpreta cual se presionó
static void readPins(void) {
	static enum states swFSM= NONE;

	switch (swFSM) {
		case NONE:
			if ( gpioRead(SW1) == SW_ACTIVE ){swFSM = LEFT;}
			if ( gpioRead(SW2) == SW_ACTIVE ){swFSM = RIGHT;}
			if ( gpioRead(SW3) == SW_ACTIVE ){swFSM = PLAY;}
			if ( gpioRead(SW4) == SW_ACTIVE ){swFSM = MENU;}
			if ( gpioRead(SW5) == SW_ACTIVE ){swFSM = ONOFF;}
			if ( gpioRead(SW6) == SW_ACTIVE ){swFSM = VOL;}
			break;
		case LEFT:
			if ( gpioRead(SW1) == !SW_ACTIVE ){
				status = true;
				swEvent = SW_LEFT;
				swFSM = NONE;
			}
			break;
		case RIGHT:
			if ( gpioRead(SW2) == !SW_ACTIVE ){
				status = true;
				swEvent = SW_RIGHT;
				swFSM = NONE;
			}
			break;
		case PLAY:
			if ( gpioRead(SW3) == !SW_ACTIVE ){
				status = true;
				swEvent = SW_PLAY;
				swFSM = NONE;
			}
			break;
		case MENU:
			if ( gpioRead(SW4) == !SW_ACTIVE ){
				status = true;
				swEvent = SW_MENU;
				swFSM = NONE;
			}
			break;
		case ONOFF:
			if ( gpioRead(SW5) == !SW_ACTIVE ){
				status = true;
				swEvent = SW_ONOFF;
				swFSM = NONE;
			}
			break;
		case VOL:
			if ( gpioRead(SW6) == !SW_ACTIVE ){
				status = true;
				swEvent = SW_VOL;
				swFSM = NONE;
			}
			break;
	}
}
