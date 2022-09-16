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
 * SPECIAL PIN ASSIGNMENT
 ******************************************************************************/
// PA,0 -> DO NOT USE
// On Board User LEDs
#define LED_ACTIVE      LOW
#define PIN_LED_RED     PORTNUM2PIN(PB,22)	// PTB22
#define PIN_LED_GREEN   PORTNUM2PIN(PE,26)	// PTE26
#define PIN_LED_BLUE    PORTNUM2PIN(PB,21) 	// PTB21

// On Board User Switches
#define PIN_SW2         PORTNUM2PIN(PC,6)	// PTC6
#define PIN_SW3         PORTNUM2PIN(PA,4)	// PTA4

/*******************************************************************************
 * PIN ASSIGNMENT
 ******************************************************************************/
/***** USB UART **********************************************************/
#define UART0_TX   PORTNUM2PIN(PB,17)	//ALT3
#define UART0_RX   PORTNUM2PIN(PB,16) 	//ALT3

/***** I2C ***************************************************************/
#define I2C0_DATA   PORTNUM2PIN(PB,1)	//ALT2
#define I2C0_CLOCK   PORTNUM2PIN(PB,2) 	//ALT2

/***** SPI ***************************************************************/
//COMPLETAR

/***** DEBUG *******************************************************************/
#define PIN_IRQ PORTNUM2PIN(PB, 10)


#endif // _BOARD_H_
