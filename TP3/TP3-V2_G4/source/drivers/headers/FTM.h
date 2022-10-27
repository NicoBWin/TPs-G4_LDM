/***************************************************************************/ /**
  @file     DAC.h
  @brief    DAC Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/
#ifndef _FTM_H_
#define _FTM_H_

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
	FTM_0,
	FTM_1,
	FTM_2,
	FTM_3,
} FTM_n;

typedef enum {
	FTM_mInputCapture,
	FTM_mOutputCompare,
	FTM_mPWM,
} FTMMode_t;

typedef enum {
	FTM_eRising 		= 0x01,
	FTM_eFalling 		= 0x02,
	FTM_eEither 		= 0x03,
} FTMEdge_t;

typedef enum {
	FTM_eToggle 		= 0x01,
	FTM_eClear 			= 0x02,
	FTM_eSet 			= 0x03,
} FTMEffect_t;

typedef enum {
	FTM_High	= 0x02,
	FTM_Low 	= 0x03,
} FTMLogic_t;

typedef enum {
	FTM_PSC_x1		= 0x00,
	FTM_PSC_x2		= 0x01,
	FTM_PSC_x4		= 0x02,
	FTM_PSC_x8		= 0x03,
	FTM_PSC_x16		= 0x04,
	FTM_PSC_x32		= 0x05,
	FTM_PSC_x64		= 0x06,
	FTM_PSC_x128	= 0x07,
} FTM_Prescal_t;	// Divide todo por el número

typedef enum {
	FTM_Channel_0,
	FTM_Channel_1,
	FTM_Channel_2,
	FTM_Channel_3,
	FTM_Channel_4,
	FTM_Channel_5,
	FTM_Channel_6,
	FTM_Channel_7
} FTM_Channel_t;

typedef enum {
	FTM_NoCLK,	// No clock selected. This in effect disables the FTM counter.
	FTM_SysCLK,	// System Clock
	FTM_FixedFreqCLK,	// Fixed frequency clock
	FTM_ExtCLK
} FTM_CLKSource_t;

typedef FTM_Type *FTM_t;
typedef uint16_t FTMData_t;
typedef uint32_t FTMChannel_t; /* FTM0/FTM3: Channel 1-8; FTM1/FTM2: Channel 1-2 */

typedef struct {
	FTM_Channel_t channel;
	FTMMode_t mode;
	FTM_Prescal_t prescale;
	FTM_CLKSource_t CLK_source;
	uint8_t ext_clock;
	union{
		FTMEdge_t IC_edge;
		FTMEffect_t OC_effect;
		FTMLogic_t PWM_logic;
	};
	uint16_t modulo;
	union{
		uint16_t counter;
		uint16_t PWM_DC;
	};
	bool active_low;
	bool DMA_on;
	bool interrupt_on;
} FTMConfig_t;


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void FTM_Init (FTM_n FTMn, FTMConfig_t config);

void FTM_start(FTM_n FTMn);

void FTM_stop(FTM_n FTMn);

uint16_t FTM_getCounter(FTM_n FTMn);

//DC de 0% a 100%
void FTM_modifyDC(FTM_n FTMn, uint16_t DC);

/*******************************************************************************
 ******************************************************************************/

#endif /* void _FTM_H_ */
