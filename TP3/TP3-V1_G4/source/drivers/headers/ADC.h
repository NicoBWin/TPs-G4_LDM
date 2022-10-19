/***************************************************************************/ /**
  @file     ADC.h
  @brief    ADC Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/
#ifndef _ADC_H_
#define _ADC_H_

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
typedef enum {
	ADC_b8,
	ADC_b12,
	ADC_b10,
	ADC_b16,
} ADCBits_t;

typedef enum {
	ADC_c24,
	ADC_c16,
	ADC_c10,
	ADC_c6,
	ADC_c4,
} ADCCycles_t;

typedef enum {
	ADC_t4,
	ADC_t8,
	ADC_t16,
	ADC_t32,
	ADC_t1,
} ADCTaps_t;

typedef enum {
	ADC_mA,
	ADC_mB,
} ADCMux_t;

typedef enum {
	ADC_0,
	ADC_1,
} ADC_n;

typedef enum {
	input_clock,
	input_clock2,	// Clock/2
	input_clock4,	// Clock/4
	input_clock8,	// Clock/8
} ADCClock_Divide;

typedef ADC_Type *ADC_t;
typedef uint8_t ADCChannel_t; /* Channel 0-23 */
typedef uint16_t ADCData_t;

typedef void (*adcCallback_t)(void);

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void 		ADC_Init (ADC_n, ADCBits_t, ADCCycles_t, ADCClock_Divide, ADCMux_t, ADCChannel_t);
void 		ADC_SetResolution(ADC_t, ADCBits_t);
ADCBits_t 	ADC_GetResolution(ADC_t);

void 		ADC_SetInterruptMode(ADC_t, bool);
bool 		ADC_IsInterruptPending(ADC_t);
void 		ADC_ClearInterruptFlag(ADC_t);

ADCCycles_t ADC_GetCycles(ADC_t);
ADCTaps_t   ADC_GetHardwareAverage(ADC_t);
void 		ADC_SetHardwareAverage(ADC_t, ADCTaps_t);
void 		ADC_SetCycles(ADC_t, ADCCycles_t);

bool 		ADC_Calibrate(ADC_t);

void 		ADC_Start(ADC_t, ADCChannel_t, ADCMux_t);

bool 		ADC_IsReady(ADC_t);

ADCData_t 	ADC_getData(ADC_t);

void ADC_SetInterruptCallback(ADC_n, adc_callback_t);
/*******************************************************************************
 ******************************************************************************/

#endif // _ADC_H_
