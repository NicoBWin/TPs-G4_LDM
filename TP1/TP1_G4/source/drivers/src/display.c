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


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Selec display between 0-3
bool dispSelect(int8_t disp);

// Display update
static void displayCallback(void);
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static const uint8_t SEGMENTS[SEVEN_SEGMENTS_PINS] = {PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D, PIN_SEG_E,
											PIN_SEG_F, PIN_SEG_G, PIN_SEG_DT};

static int fps = 6;
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
typedef struct
{
	bool enable;
	char ch;
} letter_t;

static letter_t displays[4] = {{false, '0'}, {false, '0'}, {false, '0'}, {false, '0'}};

//Timer para el encoder
static tim_id_t rate_id;
static tim_id_t bright_id;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief dispInit: Initializes the display
 */
void dispInit(void){
	gpioMode(PINA_SEG, OUTPUT);
	gpioWrite(PINA_SEG, LOW);
	gpioMode(PINB_SEG, OUTPUT);
	gpioWrite(PINB_SEG, LOW);
	for (int i = 0; i < SEVEN_SEGMENTS_PINS; i++) {
		gpioMode(SEGMENTS[i], OUTPUT);
		gpioWrite(SEGMENTS[i], LOW);
	}

	//Seteo el timer para que llame periodicamente a la callback con 1ms
	display_id = timerGetId();
	bright_id = timerGetId();
	timerStart(display_id, TIMER_MS2TICKS(fps), TIM_MODE_PERIODIC, displayCallback);
}


/**
 * @brief dispSendChar: Writes ONE character in designated display:
 * @param ch : character to be sent coded in ascii.
 * seven_seg_module: numer of seven segment, it ranges from 0 to 3
 */
void dispSendChar(char ch, uint8_t seven_seg_module){
	if (seven_seg_module < 4)
	{
		displays[seven_seg_module].enable = true;
		displays[seven_seg_module].ch = ch;
	}
}


/**
 * @brief dispSendChar: sends to the selected 7 seg the character and scrolls:
 * @param ch : character to be sent coded in ascii.
 */
void dispSendWord(char* ch) {

}


/**
 * @brief dispClearAll: Clears ALL the display.
 *
 */
void dispClearAll(void){
	for (int i = 0; i < SEVEN_SEGMENTS_PINS; i++) {
		gpioWrite(SEGMENTS[i], LOW);
	}
	for (int j = 0; j < MAX_CHARACTERS; j++) {
		dispSelect(j);
	}
}

/**
 * @brief dispBrightness: Changes the brightness of all displays.
 * @params brightness: the brightness value to be set. Lives between 0<b<100
 *
 */
void dispBrightness(uint8_t brightness) {
  
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
	int i;
	bool ret = true;
	for (i = 0; i < MAX_CHARACTERS; i++) {
		if (characters[i].name == ch) {
			for (int j = 0; j < SEVEN_SEGMENTS_PINS; j++) {
				led_set_state(seven_segment_id[j], characters[i].pin_mode[j]);
			}
			ret = true;
			break;
		}
	}
	if (i == MAX_CHARACTERS)
	{
		ret = false;
	}
	return ret;
}

static void displayCallback(void){
	
}