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
#include "../../timer/timer.h"
#include "../board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Display update
static void displayCallback(void);

//Turns off all seven segment
static void OFFseven_segment();

// Selec display between 0-3
bool dispSelect(int8_t disp);
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static const uint8_t SEGMENTS[SEVEN_SEGMENTS_PINS] = {PIN_SEG_A, PIN_SEG_B, PIN_SEG_C, PIN_SEG_D, PIN_SEG_E,
											PIN_SEG_F, PIN_SEG_G, PIN_SEG_DT};

static int fps = 5;

static int brightness = (5) * (8.0 / 10.0);

static int segmentCnt = 0;
/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
typedef struct {
	bool enable;
	char ch;
} letter_t;

static letter_t displays[4] = {{false, '0'}, {false, '0'}, {false, '0'}, {false, '0'}}; //{ON|OFF, CHAR}

//Timer para el encoder
static tim_id_t display_id;
static tim_id_t bright_id;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// Display  ********************************************************************
/**
 * @brief dispInit: Initializes the display
 */
void dispInit(void){
	gpioMode(PINA_SEG, OUTPUT);
	gpioMode(PINB_SEG, OUTPUT);
	gpioWrite(PINA_SEG, LOW);
	gpioWrite(PINB_SEG, LOW);
	for (int i = 0; i < SEVEN_SEGMENTS_PINS; i++) {
		gpioMode(SEGMENTS[i], OUTPUT);
		gpioWrite(SEGMENTS[i], LOW);
	}

	//Seteo el timer para que llame periodicamente a la callback con 1ms
	display_id = timerGetId();
	bright_id = timerGetId();
	timerStart(display_id, TIMER_MS2TICKS(fps), TIM_MODE_PERIODIC, &displayCallback);
}

/**
 * @brief dispSendChar: Writes ONE character in designated display:
 * @param ch : character to be sent coded in ascii.
 * seven_seg_module: numer of seven segment, it ranges from 0 to 3
 */
void dispSendChar(char ch, uint8_t seven_seg_module){
	if (seven_seg_module < 4) {
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
 */
void dispClearAll(void){
	for (int i = 0; i < SEVEN_SEGMENTS_PINS; i++) {
		gpioWrite(SEGMENTS[i], LOW);
	}
	for (int j = 0; j < MAX_CHARACTERS; j++) {
		dispSelect(j);
	}
}

// Configuracion de Brillo *****************************************************
/**
 * @brief dispBrightness: Changes the brightness of all displays.
 * @params level: the brightness value to be set. Lives between 0<b<100
 */
void SetdispBrightness(int level) {	//1<LEVEL<9
	if(level > 9) level = 9;
	if(level < 1) level = 1;
	brightness = (fps) * ((level+1) / 10.0);
}

int GetdispBrightness(){
	return (brightness * 10.0) / fps - 1;
}



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void displayCallback(void){
	// Update Display
	static int i=0;
	int n = 0;

	if(segmentCnt == MAX_DISPLAY){
		segmentCnt = 0;
		i = segmentCnt;
	}

	timerStart(bright_id, TIMER_MS2TICKS(brightness), TIM_MODE_SINGLESHOT, &OFFseven_segment);

	if(displays[i].enable == true) {
		dispSelect(segmentCnt);
		for (n = 0; n < MAX_CHARACTERS; n++) {
			if (characters[n].name == displays[i].ch) {
				for (int p = 0; p < SEVEN_SEGMENTS_PINS; p++) {
					gpioWrite(SEGMENTS[p], characters[n].pin_mode[p]);
				}
				break;
			}
		}
		if (n == MAX_CHARACTERS) {	//Si es un caracter que no esta se prende el punto y D
			for (int p = 0; p < SEVEN_SEGMENTS_PINS; p++) {
				gpioWrite(SEGMENTS[i], LOW);
			}
			gpioWrite(SEGMENTS[PIN_SEG_DT], HIGH);
			gpioWrite(SEGMENTS[PIN_SEG_D], HIGH);
		}
	}
	else {
		OFFseven_segment();
	}
	segmentCnt++;
	i++;
}

static void OFFseven_segment() {
	for (int i = 0; i < SEVEN_SEGMENTS_PINS; i++) {
		gpioWrite(SEGMENTS[i], LOW);
	}
}

// Set the mux according to de display
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
