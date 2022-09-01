/***************************************************************************//**
  @file     leds.c
  @brief    Driver leds
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
#define UPDATE_RATE 2

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Update LEDs.
static void ledsCallback();

// MUX for leds
bool ledSelect(int8_t disp);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
const uint8_t LEDSELECTOR[2] = {PINA_LEDS, PINB_LEDS};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
//Timer para el los leds
static tim_id_t leds_id;

//LEDS array if is ON or OFF
static bool leds[MAX_LEDS];

//LED COUNTER
static int ledCnt;

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
    for (int i=0;i<MAX_LEDS;i++){
        leds[i]=false;
    }
	leds_id = timerGetId();
	timerStart(leds_id, TIMER_MS2TICKS(UPDATE_RATE), TIM_MODE_PERIODIC, &ledsCallback);
}

/**
 * @brief Turn off ONLY ONE LED
 * @param n Index of LED to turn off
 */
void ledClear(int n) {
	leds[n]=false;
}

/**
 * @brief Turn on a LED
 * @param n Index of LED to turn on
 */
void ledSet(int n) {
    leds[n]=true;
}   

/**
 * @brief Turn on a LED
 * @param n Index of LED to turn on
 */
void ledToggle(int n) {
    leds[n]=!leds[n];
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Update LEDS
static void ledsCallback() {
	if(ledCnt == MAX_LEDS) {
		ledCnt = 0;
	}
	if(leds[ledCnt])
		ledSelect(ledCnt);
	else
		ledSelect(OFF);
	ledCnt++;
}

// Selec the led to turn on
// Case 0: D1
// Case 1: D2
// Case 2: D3
// Case 3: OFF
bool ledSelect(int8_t disp) {
	int8_t ret;
	switch (disp) {
		case D1:
			gpioWrite(PINA_LEDS, HIGH);
			gpioWrite(PINB_LEDS, LOW);
			ret = true;
			break;
		case D2:
			gpioWrite(PINA_LEDS, LOW);
			gpioWrite(PINB_LEDS, HIGH);
			ret = true;
			break;
		case D3:
			gpioWrite(PINA_LEDS, HIGH);
			gpioWrite(PINB_LEDS, HIGH);
			ret = true;
			break;
		case OFF:
			gpioWrite(PINA_LEDS, LOW);
			gpioWrite(PINB_LEDS, LOW);
			ret = true;
			break;
		default:
			ret = false;
	}
	return ret;
}
