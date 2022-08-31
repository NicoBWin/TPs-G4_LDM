/***************************************************************************//**
  @file     encoder.c
  @brief    Driver encoder
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../headers/leds.h"
#include "../board.h"
#include "../../timer/timer.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
const uint8_t LEDSELECTOR[2] = {PINA_LEDS, PINB_LEDS};
//#define OFF 4
//#define D1  1
//#define D2  2
//#define D3  3

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void ledSelect(int disp);
// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
//Timer para el los leds
static tim_id_t leds_id;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Initializes the LEDs.
 */
void ledsInit() {
    gpioMode(PINA_LEDS, OUTPUT);
	gpioMode(PINB_LEDS, OUTPUT);
	//ledsClear(OFF);
	int n;
    for (int i=0;i<3;i++){
    	ledClear(i);
    }
	//leds_id = timerGetId();
	//timerStart(leds_id, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, &update_leds);
}

/**
 * @brief Turn off ONLY ONE LED
 * @param n Index of LED to turn off
 */
void ledClear(int n) {
    ledSelect(OFF);
}

/**
 * @brief Turn on a LED
 * @param n Index of LED to turn on
 */
void ledSet(int n) {
    ledSelect(n);
}   


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Selec the led to turn on
// Case 0: D1
// Case 1: D2
// Case 2: D3
// Case 3: OFF
void ledSelect(int disp) {
	int ret;
	switch (disp)
	{
	case D1:
		gpioWrite(PINA_SEG, HIGH);
		gpioWrite(PINB_SEG, LOW);
		ret = true;
		break;
	case D2:
		gpioWrite(PINA_SEG, LOW);
		gpioWrite(PINB_SEG, HIGH);
		ret = true;
		break;
	case D3:
		gpioWrite(PINA_SEG, HIGH);
		gpioWrite(PINB_SEG, HIGH);
		ret = true;
		break;
	case OFF:
		gpioWrite(PINA_SEG, LOW);
		gpioWrite(PINB_SEG, LOW);
		ret = true;
		break;
	default:
		ret = false;
	}
	//return ret;
}
