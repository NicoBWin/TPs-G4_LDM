/***************************************************************************//**
  @file     display.c
  @brief    Driver display
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../headers/card_reader.h"
#include "../../MCAL/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
const static character_t characters[] = {
	{'0', {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW, LOW}},
	{'1', {LOW, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW}},
	{'2', {HIGH, HIGH, LOW, HIGH, HIGH, LOW, HIGH, LOW}},
	{'3', {HIGH, HIGH, HIGH, HIGH, LOW, LOW, HIGH, LOW}},
	{'4', {LOW, HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW}},
	{'5', {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW}},
	{'6', {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}},
	{'7', {HIGH, HIGH, HIGH, LOW, LOW, LOW, LOW, LOW}},
	{'8', {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}},
	{'9', {HIGH, HIGH, HIGH, HIGH, LOW, HIGH, HIGH, LOW}},
	{'A', {HIGH, HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW}},
	{'B', {LOW, LOW, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}},
	{'C', {HIGH, LOW, LOW, HIGH, HIGH, HIGH, LOW, LOW}},
	{'D', {LOW, HIGH, HIGH, HIGH, HIGH, LOW, HIGH, LOW}},
	{'E', {HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH, LOW}},
	{'F', {HIGH, LOW, LOW, LOW, HIGH, HIGH, HIGH, LOW}},
	{'G', {HIGH, LOW, HIGH, HIGH, HIGH, HIGH, LOW, LOW}},
	{'H', {LOW, HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW}},
	{'I', {LOW, LOW, LOW, LOW, HIGH, HIGH, LOW, LOW}},
	{'J', {LOW, HIGH, HIGH, HIGH, HIGH, LOW, LOW, LOW}},
	{'K', {LOW, LOW, HIGH, LOW, HIGH, HIGH, HIGH, LOW}},
	{'L', {LOW, LOW, LOW, HIGH, HIGH, HIGH, LOW, LOW}},
	{'M', {HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW, LOW}},
	{'N', {HIGH, HIGH, HIGH, LOW, HIGH, HIGH, LOW, LOW}},
	{'O', {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW, LOW}},
	{'P', {HIGH, HIGH, LOW, LOW, HIGH, HIGH, HIGH, LOW}},
	{'Q', {HIGH, HIGH, HIGH, LOW, LOW, HIGH, HIGH, LOW}},
	{'R', {HIGH, LOW, LOW, LOW, HIGH, HIGH, LOW, LOW}},
	{'r', {LOW, LOW, LOW, LOW, HIGH, LOW, HIGH, LOW}},
	{'S', {HIGH, LOW, HIGH, HIGH, LOW, HIGH, HIGH, LOW}},
	{'T', {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, LOW}},
	{'U', {LOW, HIGH, HIGH, HIGH, HIGH, HIGH, LOW, LOW}},
	{'V', {LOW, HIGH, HIGH, HIGH, LOW, HIGH, LOW, LOW}},
	{'W', {LOW, HIGH, LOW, HIGH, LOW, HIGH, LOW, LOW}},
	{'X', {LOW, HIGH, HIGH, LOW, HIGH, HIGH, HIGH, LOW}},
	{'Y', {LOW, HIGH, HIGH, HIGH, LOW, HIGH, HIGH, LOW}},
	{'Z', {HIGH, HIGH, LOW, HIGH, LOW, LOW, HIGH, LOW}},

	{'-', {LOW, LOW, LOW, LOW, LOW, LOW, HIGH, LOW}},
	{' ', {LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW}},
	{'^', {LOW, LOW, LOW, LOW, LOW, LOW, HIGH, LOW}},
	{'>', {LOW, LOW, HIGH, LOW, LOW, LOW, LOW, LOW}},
	{'_', {LOW, LOW, LOW, HIGH, LOW, LOW, LOW, LOW}},
	{'<', {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, LOW}},
	{'.', {LOW, LOW, LOW, LOW, LOW, LOW, LOW, HIGH}},
	{'=', {LOW, LOW, LOW, HIGH, LOW, LOW, HIGH, LOW}},

	//Segmentos
	{'a', {HIGH, LOW, LOW, LOW, LOW, LOW, LOW, LOW}},
	{'b', {LOW, HIGH, LOW, LOW, LOW, LOW, LOW, LOW}},
	{'c', {LOW, LOW, HIGH, LOW, LOW, LOW, LOW, LOW}},
	{'d', {LOW, LOW, LOW, HIGH, LOW, LOW, LOW, LOW}},
	{'e', {LOW, LOW, LOW, LOW, HIGH, LOW, LOW, LOW}},
	{'f', {LOW, LOW, LOW, LOW, LOW, HIGH, LOW, LOW}},
	{'g', {LOW, LOW, LOW, LOW, LOW, LOW, HIGH, LOW}},


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
	char name;
	uint8_t pin_mode[SEVEN_SEGMENTS_PINS]; // PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DOT
} character_t;



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


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