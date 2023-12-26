/***************************************************************************/ /**
  @file     board.h
  @brief    PIN Board assignment file
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _BOARD_H_
#define _BOARD_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../MCAL/gpio.h"

/*******************************************************************************
 * SPECIAL PIN ASSIGNMENT
 ******************************************************************************/
// ALERT
// PA,0 -> DO NOT USE
// ALERT
// On Board User LEDs
#define LED_ACTIVE      LOW
#define PIN_LED_RED     PORTNUM2PIN(PB,22)	// PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26)	// PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) 	// PTB21

// On Board User Switches
#define PIN_SW2 PORTNUM2PIN(PC, 6)
#define PIN_SW3 PORTNUM2PIN(PA, 4)

// FOR DEBUG IQRs
#define PIN_IRQ PORTNUM2PIN(PB, 10)

#define SW_ACTIVE       LOW

/*******************************************************************************
 * PIN ASSIGNMENT
 ******************************************************************************/

/***** Encoder *****************************************************************/
#define PIN_SW 		PORTNUM2PIN(PB, 11)
#define PIN_RCHB 	PORTNUM2PIN(PC, 11)
#define PIN_RCHA 	PORTNUM2PIN(PC, 10)

/***** SWs or Buttons **********************************************************/
#define SW1 		PORTNUM2PIN(PC, 16)
#define SW2 		PORTNUM2PIN(PC, 17)
#define SW3 		PORTNUM2PIN(PB, 9)
#define SW4 		PORTNUM2PIN(PA, 1)
#define SW5 		PORTNUM2PIN(PB, 23)
#define SW6 		PORTNUM2PIN(PA, 2)

/***** LED Matrix **************************************************************/
#define LED_MATRIX	PORTNUM2PIN(PC, 1)

/***** 16*2 LCD Display ********************************************************/
#define LCD_RS 		PORTNUM2PIN(PC, 4)
#define LCD_EN	 	PORTNUM2PIN(PD, 0)
#define LCD_D4 		PORTNUM2PIN(PD, 2)
#define LCD_D5 		PORTNUM2PIN(PD, 3)
#define LCD_D6 		PORTNUM2PIN(PD, 1)
#define LCD_D7 		PORTNUM2PIN(PC, 3)

/***** UART defines ************************************************************/
#define UART0_TX   PORTNUM2PIN(PB,17)	//ALT3	-> USB UART
#define UART0_RX   PORTNUM2PIN(PB,16) 	//ALT3	-> USB UART

/******************************************************************************/

#endif // _BOARD_H_
