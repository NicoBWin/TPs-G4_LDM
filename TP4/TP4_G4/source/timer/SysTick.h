/***************************************************************************/ /**
  @file     SysTick.h
  @brief    Header SysTick
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _SYSTICK_H_
#define _SYSTICK_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SYSTICK_ISR_FREQUENCY_HZ 1000U //1000U

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void (*systick_callback_t)(void);


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialize SysTic driver
 * @param funcallback Function to be call every SysTick
 * @return Initialization and registration succeed
 */
bool SysTick_Init(systick_callback_t callback);

void G4_SysTick_Handler(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _SYSTICK_H_
