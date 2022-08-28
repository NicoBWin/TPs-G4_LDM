/***************************************************************************//**
  @file     display.c
  @brief    Driver display
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../headers/display.h"
#include "../headers/7seg.h"
#include "../../MCAL/gpio.h"
#include "../board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
const uint8_t SEGMENTS[SEVEN_SEGMENTS_PINS] = {PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D, PIN_SEG_E,
											PIN_SEG_F, PIN_SEG_G, PIN_SEG_DT};

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Selec display between 0-3
bool dispSelect(int8_t disp);


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief dispInit: Initializes the display
 *
 */
void dispInit(void){
	gpioMode(PINA_SEG, OUTPUT);
	gpioWrite(PINA_SEG, LOW);
	gpioMode(PINB_SEG, OUTPUT);
	gpioWrite(PINB_SEG, HIGH);
	for (int i = 0; i < SEVEN_SEGMENTS_PINS; i++)
		{
			gpioMode(SEGMENTS[i], OUTPUT);
			gpioWrite(SEGMENTS[i], HIGH);
		}
	for (int j = 0; j < 4; j++) {
		dispSelect(j);
	}
}


/**
 * @brief dispSendChar: sends to the selected 7 seg the character:
 * @param ch : character to be sent coded in ascii.
 * seven_seg_module: numer of seven segment, it ranges from 0 to 3
 *
 */
void dispSendChar(char ch, uint8_t seven_seg_module){

}

/**
 * @brief dispClearAll: Clears ALL the display.
 *
 */
void dispClearAll(void){

}

/**
 * @brief dispBrightness: Changes the brightness of all displays.
 * @params brightness: the brightness value to be set. Lives between 0<b<100
 *
 */
void dispBrightness(uint8_t brightness){
  
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
bool dispSelect(int8_t disp) {
	int8_t ret;
	switch (disp)
	{
	case 0:
		gpioWrite(PINA_SEG, LOW);
		gpioWrite(PINB_SEG, LOW);
		ret = true;
		break;
	case 1:
		gpioWrite(PINA_SEG, HIGH);
		gpioWrite(PINB_SEG, LOW);
		ret = true;
		break;
	case 2:
		gpioWrite(PINA_SEG, LOW);
		gpioWrite(PINB_SEG, HIGH);
		ret = true;
		break;
	case 3:
		gpioWrite(PINA_SEG, HIGH);
		gpioWrite(PINB_SEG, HIGH);
		ret = true;
		break;
	default:
		ret = false;
	}
	return ret;
}


bool dispSetChar(char ch) {

}
