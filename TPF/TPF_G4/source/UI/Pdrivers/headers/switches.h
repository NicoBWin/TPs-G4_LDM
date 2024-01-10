/***************************************************************************//**
  @file     switches.h
  @brief    Switches driver header
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _SWITCHES_H_
#define _SWITCHES_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {
	SW_LEFT,
	SW_RIGHT,
	SW_PLAY,
	SW_PAUSE,
	SW_MENU,
	SW_VOL,
	SW_NONE
}swResult_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialize encoder
 */
void SW_Init();

/**
 * @brief Check for new encoder events
 * @return true if there is an encoder event
 */
bool swGetStatus();

/**
 * @brief Get event type
 * @return event
 */
swResult_t swGetEvent();

/*******************************************************************************
 ******************************************************************************/
 #endif // _SWITCHES_H_
