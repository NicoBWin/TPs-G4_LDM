/***************************************************************************/ /**
  @file     board.h
  @brief    Board management
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/***** BOARD defines **********************************************************/

// On Board User LEDs
#define PIN_LED_RED PORTNUM2PIN(PB, 22)
#define PIN_LED_GREEN PORTNUM2PIN(PE, 26)
#define PIN_LED_BLUE PORTNUM2PIN(PB, 21)
#define PIN_PULSADOR PORTNUM2PIN(PC, 9)
#define PIN_LED_AMA_EXT PORTNUM2PIN(PB, 23)
#define PIN_LED_AMA_EXT PORTNUM2PIN(PB, 23)

#define PIN_B2 PORTNUM2PIN(PB, 2)   //PB2
#define PIN_B3 PORTNUM2PIN(PB, 3)   //PB3
#define PIN_C5 PORTNUM2PIN(PC, 5)   //PB2
#define PIN_C3 PORTNUM2PIN(PC, 3)   //PB3
#define PIN_C7 PORTNUM2PIN(PC, 7)   //PB2
#define PIN_C2 PORTNUM2PIN(PC, 2)   //PB3
#define PIN_B10 PORTNUM2PIN(PB, 10) //PB10

// On Board User Switches
#define PIN_SW2 PORTNUM2PIN(PC, 6) // PTC6
#define PIN_SW3 PORTNUM2PIN(PA, 4) // PTA4

#define DEBUG_PIN PIN_B10 //PB10

#endif // _BOARD_H_
