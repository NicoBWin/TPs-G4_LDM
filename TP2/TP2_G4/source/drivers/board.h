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

#define UART1_TX   PORTNUM2PIN(PC,4)	//ALT3
#define UART1_RX   PORTNUM2PIN(PC,3) 	//ALT3

#define UART2_TX   PORTNUM2PIN(PD,3)	//ALT3
#define UART2_RX   PORTNUM2PIN(PD,2) 	//ALT3

#define UART3_TX   PORTNUM2PIN(PC,17)	//ALT3
#define UART3_RX   PORTNUM2PIN(PC,16) 	//ALT3

#define UART4_TX   PORTNUM2PIN(PC,15)	//ALT3
#define UART4_RX   PORTNUM2PIN(PC,14) 	//ALT3

#define UART5_TX   PORTNUM2PIN(PE,8)	//ALT3
#define UART5_RX   PORTNUM2PIN(PE,9) 	//ALT3


/*************** SPI **********************************************************/


/*************** I2C **********************************************************/
#define I2C0_DATA 	PORTNUM2PIN(PE, 25) // ALT5		SDA
#define I2C0_CLK	PORTNUM2PIN(PE, 24)	// ALT5	SCL

#define I2C1_DATA 	PORTNUM2PIN(PC, 11) // ALT2
#define I2C1_CLK 	PORTNUM2PIN(PC, 10) // ALT2

#define I2C2_DATA 	PORTNUM2PIN(PA, 11) // ALT5
#define I2C2_CLK	PORTNUM2PIN(PA, 12) // ALT5

/***** DEBUG *******************************************************************/
#define PIN_IRQ PORTNUM2PIN(PB, 10)


#endif // _BOARD_H_
