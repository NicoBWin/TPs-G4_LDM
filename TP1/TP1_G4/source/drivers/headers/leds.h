/***************************************************************************//**
  @file     leds.h
  @brief    LEDs driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _LEDS_H_
#define _LEDS_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_LEDS	3

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initializes the LEDs.
 */
void ledsInit();

/**
 * @brief Turn off a LED
 * @param n Index of LED to turn off
 */
void ledClear(int n);

/**
 * @brief Turn on a LED
 * @param n Index of LED to turn on
 */
void ledSet(int n);

/*******************************************************************************
 ******************************************************************************/


#endif /* _LEDS_H_ */