/***************************************************************************//**
  @file     encoder.h
  @brief    Header encoder
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _ENCODER_H_
#define _ENCODER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

//OS
#include <os.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {
	ENC_LEFT,
	ENC_RIGHT,
	ENC_CLICK,
	ENC_NONE
}encResult_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialize encoder
 */
void encInit(OS_SEM *EncSem);

/**
 * @brief Check for new encoder events
 * @return true if there is an encoder event
 */
bool encGetStatus();

/**
 * @brief Get event type
 * @return event
 */
encResult_t encGetEvent();

/*******************************************************************************
 ******************************************************************************/
 #endif // _ENCODER_H_
