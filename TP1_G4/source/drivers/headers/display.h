/***************************************************************************/ /**
  @file     display.h
  @brief    Driver display de 7 segmentos
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 *****************************************************************************/
#define MAX_DISPLAY  4
#define SEVEN_SEGMENTS_PINS 8

#define MIN_BRIGHT 0
#define MAX_BRIGHT 9
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief dispInit: Initializes the display
 *
 */
void dispInit(void);

/**
 * @brief dispSendChar: sends to the selected 7 seg the character:
 * @param ch : character to be sent coded in ascii.
 * seven_seg_module: numer of seven segment, it ranges from 0 to 3
 *
 */
void dispSendChar(char ch, uint8_t seven_seg_module);

/**
 * @brief dispSendChar: sends to the selected 7 seg the character:
 * @param ch : character to be sent coded in ascii.
 */
void dispSendWord(char* ch);

/**
 * @brief dispClearAll: Clears ALL the display.
 *
 */
void dispClearAll(void);

/**
 * @brief SetdispBrightness: Changes the brightness of all displays.
 * @params brightness: the brightness value to be set.
 */
void SetdispBrightness(int level);

/**
 * @brief GetdispBrightness: Gets the brightness of all displays.
 */
int GetdispBrightness();

/**
 * @brief SetdispDP: Turns on DP.
 */
void SetdispDP();

/**
 * @brief CleardispDP: Turns off DP.
 */
void CleardispDP();
/*******************************************************************************
 ******************************************************************************/

#endif // _DISPLAY_H_
