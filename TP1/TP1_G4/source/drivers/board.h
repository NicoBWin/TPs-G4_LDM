/***************************************************************************/ /**
  @file     board.h
  @brief    Board management
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../MCAL/gpio.h"

/*******************************************************************************
 * PIN ASSIGNMENT
 ******************************************************************************/

/***** BOARD Lab Micros V1.00 define ******************************************/
// 7seg Display
#define PIN_SEG_A PORTNUM2PIN(PE, 24)
#define PIN_SEG_B PORTNUM2PIN(PD, 1)
#define PIN_SEG_C PORTNUM2PIN(PD, 2)
#define PIN_SEG_D PORTNUM2PIN(PC, 4)
#define PIN_SEG_E PORTNUM2PIN(PC, 3)
#define PIN_SEG_F PORTNUM2PIN(PA, 2)
#define PIN_SEG_G PORTNUM2PIN(PA, 1)
#define PIN_SEG_DT PORTNUM2PIN(PC, 17)

// 7seg Decoder
#define PINA_SEG PORTNUM2PIN(PE, 25)
#define PINB_SEG PORTNUM2PIN(PD, 3)

// 3LEDs Decoder
#define PINA_LEDS PORTNUM2PIN(PB, 23)
#define PINB_LEDS PORTNUM2PIN(PB, 9)

// Encoder + SW
#define PIN_SW PORTNUM2PIN(PC, 0)
#define PIN_RCHA PORTNUM2PIN(PC, 5)
#define PIN_RCHB PORTNUM2PIN(PC, 7)

// On Board User Switches
#define PIN_SW2 PORTNUM2PIN(PC, 6)
#define PIN_SW3 PORTNUM2PIN(PA, 4)

/***** DEBUG *******************************************************************/
#define PIN_IRQ PORTNUM2PIN(PB, 10)

/***** Card Reader defines *****************************************************/
#define ENABLE_PIN PORTNUM2PIN(PC, 10)
#define CLOCK_PIN PORTNUM2PIN(PC, 11)
#define DATA_PIN PORTNUM2PIN(PB, 11)


#endif // _BOARD_H_
