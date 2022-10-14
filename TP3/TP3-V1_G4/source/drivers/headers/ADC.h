
#ifndef SOURCES_TEMPLATE_ADC_H_
#define SOURCES_TEMPLATE_ADC_H_

#include "hardware.h"

typedef enum
{
	ADC_b8,
	ADC_b12,
	ADC_b10,
	ADC_b16,
} ADCBits_t;

typedef enum
{
	ADC_c24,
	ADC_c16,
	ADC_c10,
	ADC_c6,
	ADC_c4,
} ADCCycles_t;

typedef enum
{
	ADC_t4,
	ADC_t8,
	ADC_t16,
	ADC_t32,
	ADC_t1,
} ADCTaps_t;

typedef enum
{
	ADC_mA,
	ADC_mB,
} ADCMux_t;

typedef ADC_Type *ADC_t;
typedef uint8_t ADCChannel_t; /* Channel 0-23 */
typedef uint16_t ADCData_t;

void 		ADC_Init 			   (void);

void 		ADC_SetInterruptMode   (ADC_t, bool);
bool 		ADC_IsInterruptPending (ADC_t);
void 		ADC_ClearInterruptFlag (ADC_t);

void 		ADC_SetResolution 	   (ADC_t, ADCBits_t);
ADCBits_t 	ADC_GetResolution 	   (ADC_t);
void 		ADC_SetCycles	 	   (ADC_t, ADCCycles_t);
ADCCycles_t ADC_GetCycles	 	   (ADC_t);
void 		ADC_SetHardwareAverage (ADC_t, ADCTaps_t);
ADCTaps_t   ADC_GetHardwareAverage (ADC_t);

bool 		ADC_Calibrate 		   (ADC_t);

void 		ADC_Start 			   (ADC_t, ADCChannel_t, ADCMux_t);

bool 		ADC_IsReady 	       (ADC_t);

ADCData_t 	ADC_getData 		   (ADC_t);

#endif /* SOURCES_TEMPLATE_ADC_H_ */
