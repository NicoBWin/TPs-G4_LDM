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

/*******************************************************************************
 * PIN ASSIGNMENT
 ******************************************************************************/
/***** BOARD Lab. Micros V1.00 defines ****************************************/
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


/***** Card Reader defines *****************************************************/
#define ENABLE_PIN PORTNUM2PIN(PC, 10)
#define CLOCK_PIN PORTNUM2PIN(PC, 11)
#define DATA_PIN PORTNUM2PIN(PB, 11)


/***** UART defines ************************************************************/
#define UART0_TX   PORTNUM2PIN(PB,17)	//ALT3	-> USB UART
#define UART0_RX   PORTNUM2PIN(PB,16) 	//ALT3	-> USB UART

#define UART1_TX   PORTNUM2PIN(PC,4)	//ALT3
#define UART1_RX   PORTNUM2PIN(PC,3) 	//ALT3

#define UART2_TX   PORTNUM2PIN(PD,20)	//ALT3
#define UART2_RX   PORTNUM2PIN(PD,21) 	//ALT3

#define UART3_TX   PORTNUM2PIN(PC,17)	//ALT3
#define UART3_RX   PORTNUM2PIN(PC,16) 	//ALT3

#define UART4_TX   PORTNUM2PIN(PC,15)	//ALT3
#define UART4_RX   PORTNUM2PIN(PC,14) 	//ALT3

#define UART5_TX   PORTNUM2PIN(PE,8)	//ALT3
#define UART5_RX   PORTNUM2PIN(PE,9) 	//ALT3


/***** DEBUG *******************************************************************/
#define PIN_IRQ PORTNUM2PIN(PB, 10)

/******************************************************************************/

#endif // _BOARD_H_
