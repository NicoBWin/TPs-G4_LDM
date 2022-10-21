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

typedef struct{
	ADC_n adcN;
	ADCBits_t resolution;
	ADCCycles_t cycles;
	ADCClock_Divide divide_select;
	ADCMux_t mux;
	ADCChannel_t channel;	// 12 -> A0 = PTB2
} ADC_Config_t;

typedef void (*adcCallback_t)(void);

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void 		ADC_Init (ADC_Config_t);
void 		ADC_SetResolution(ADC_n adcN, ADCBits_t bits);
void 		ADC_SetCycles(ADC_n adcN, ADCCycles_t cycles);

void ADC_SetInterruptMode (ADC_n adcN, bool mode);
bool ADC_IsInterruptPending (ADC_n adcN);
void ADC_ClearInterruptFlag (ADC_n adcN);

ADCBits_t ADC_GetResolution (ADC_n adcN);
ADCCycles_t ADC_GetSCycles (ADC_n adcN);
ADCTaps_t ADC_GetHardwareAverage (ADC_n adcN);

void ADC_SetHardwareAverage (ADC_n adcN, ADCTaps_t taps);

bool ADC_Calibrate (ADC_n adcN);

void ADC_Start (ADC_n adcN, ADCMux_t mux);
ADCData_t ADC_getData (ADC_n adcN);
ADCData_t ADC_getValue(ADC_n adcN);

void ADC_SetInterruptCallback(ADC_n adcN, adcCallback_t callback_fn);
/*******************************************************************************
 ******************************************************************************/

#endif // _ADC_H_
