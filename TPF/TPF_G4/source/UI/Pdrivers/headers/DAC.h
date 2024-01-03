/***************************************************************************/ /**
  @file     DAC.h
  @brief    DAC Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/
#ifndef _DAC_H_
#define _DAC_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "hardware.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef DAC_Type *DAC_t;
typedef uint16_t DACData_t;

typedef enum {
	DAC_0,
	DAC_1,
} DAC_n;
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void DAC_Init (DAC_n dacN);

void DAC_SetData (DAC_n dacN, DACData_t data);

void DacEnableDMA(DAC_n dacN);

void DAC_Enable(DAC_n dacN, bool enable);
/*******************************************************************************
 ******************************************************************************/

#endif /* void _DAC_H_ */
