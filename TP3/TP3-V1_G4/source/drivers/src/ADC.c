/***************************************************************************//**
  @file     ADC.c
  @brief    ADC Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main lib
#include "../headers/ADC.h"

// Internal libs
#include "../../MCAL/gpio.h"
#include "../board.h"

// MCU libs
#include "MK64F12.h"
#include "hardware.h"

// Standar libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define TWO_POW_NUM_OF_CAL (1 << 4)

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef ADC_Type *ADC_t;	//Puntero para trabajar con un ADC o el otro

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
bool ADC_interrupt[2] = {false, false};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static ADCData_t currentValue[2] = {0, 0};
static ADCChannel_t channels[2]= {0, 0};

static adcCallback_t adcCallbacks[2];


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
//**************** INIT CONFIG ***************************
void ADC_Init (ADC_Config_t adcCon) {
	ADC_t adc = (adcCon.adcN == ADC_0) ? ADC0 : ADC1;
	if(adcCon.adcN == ADC_0){
		SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
		NVIC_EnableIRQ(ADC0_IRQn);
	}
	else {
		SIM->SCGC3 |= SIM_SCGC3_ADC1_MASK;
		NVIC_EnableIRQ(ADC1_IRQn);
	}

	adc->CFG1 = (adc->CFG1 & ~ADC_CFG1_ADICLK_MASK) | ADC_CFG1_ADICLK(00) ; //Use bus clock
	adc->CFG1 |= ADC_CFG1_ADIV(adcCon.divide_select);        //Clock Divide Select

	ADC_SetResolution(adcCon.adcN, adcCon.resolution);
	ADC_SetCycles(adcCon.adcN, adcCon.cycles);
	ADC_Calibrate (adcCon.adcN);

	adc->SC3 |= ADC_SC3_ADCO(true) ;  //Continous mode & ADC_SC3_AVGE(false)
	adc->CFG2 = (adc->CFG2 & ~ADC_CFG2_MUXSEL_MASK) | ADC_CFG2_MUXSEL(adcCon.mux);

	channels[adcCon.adcN] = adcCon.channel;
}

void ADC_SetResolution (ADC_n adcN, ADCBits_t bits) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	adc->CFG1 |= (adc->CFG1 & ~ADC_CFG1_MODE_MASK) | ADC_CFG1_MODE(bits);
}

void ADC_SetCycles (ADC_n adcN, ADCCycles_t cycles) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	if (cycles & ~ADC_CFG2_ADLSTS_MASK)	{
		adc->CFG1 &= ~ADC_CFG1_ADLSMP_MASK;
	}
	else {
		adc->CFG1 |= ADC_CFG1_ADLSMP_MASK;
		adc->CFG2 |= (adc->CFG2 & ~ADC_CFG2_ADLSTS_MASK) | ADC_CFG2_ADLSTS(cycles);
	}
}


//**************** INTERRUPT Fn ***************************
void ADC_SetInterruptMode (ADC_n adcN, bool mode) {
	if (adcN == ADC_0)
		ADC_interrupt[0] = mode;
	else if (adcN == ADC_1)
		ADC_interrupt[1] = mode;
}

bool ADC_IsInterruptPending (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	return adc->SC1[0] & ADC_SC1_COCO_MASK;
}

void ADC_ClearInterruptFlag (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	adc->SC1[0] = 0x00;
}


//**************** Getters ***************************
ADCBits_t ADC_GetResolution (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	return adc->CFG1 & ADC_CFG1_MODE_MASK;
}

ADCCycles_t ADC_GetSCycles (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	if (adc->CFG1 & ADC_CFG1_ADLSMP_MASK)
		return ADC_c4;
	else
		return adc->CFG2 & ADC_CFG2_ADLSTS_MASK;
}

ADCTaps_t ADC_GetHardwareAverage (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	if (adc->SC3 & ADC_SC3_AVGE_MASK)
		return ADC_t1;
	else
		return adc->SC3 & ADC_SC3_AVGS_MASK;
}

//**************** Setter ***************************
void ADC_SetHardwareAverage (ADC_n adcN, ADCTaps_t taps) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	if (taps & ~ADC_SC3_AVGS_MASK)	{
		adc->SC3 &= ~ADC_SC3_AVGE_MASK;
	}
	else {
		adc->SC3 |= ADC_SC3_AVGE_MASK;
		adc->SC3 = (adc->SC3 & ~ADC_SC3_AVGS_MASK) | ADC_SC3_AVGS(taps);
	}
}

//**************** Calibration ***************************
bool ADC_Calibrate (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	int32_t  Offset		= 0;
	uint32_t Minus	[7] = {0,0,0,0,0,0,0};
	uint32_t Plus	[7] = {0,0,0,0,0,0,0};
	uint8_t  i;
	uint32_t scr3;

	/// SETUP
	adc->SC1[0] = 0x1F;
	scr3 = adc->SC3;
	adc->SC3 &= (ADC_SC3_AVGS(0x03) | ADC_SC3_AVGE_MASK);

	/// INITIAL CALIBRATION
	adc->SC3 &= ~ADC_SC3_CAL_MASK;
	adc->SC3 |=  ADC_SC3_CAL_MASK;
	while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));
	if (adc->SC3 & ADC_SC3_CALF_MASK)
	{
		adc->SC3 |= ADC_SC3_CALF_MASK;
		return false;
	}
	adc->PG  = (0x8000 | ((adc->CLP0+adc->CLP1+adc->CLP2+adc->CLP3+adc->CLP4+adc->CLPS) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->MG  = (0x8000 | ((adc->CLM0+adc->CLM1+adc->CLM2+adc->CLM3+adc->CLM4+adc->CLMS) >> (1 + TWO_POW_NUM_OF_CAL)));

	// FURTHER CALIBRATIONS
	for (i = 0; i < TWO_POW_NUM_OF_CAL; i++)
	{
		adc->SC3 &= ~ADC_SC3_CAL_MASK;
		adc->SC3 |=  ADC_SC3_CAL_MASK;
		while (!(adc->SC1[0] & ADC_SC1_COCO_MASK));
		if (adc->SC3 & ADC_SC3_CALF_MASK)
		{
			adc->SC3 |= ADC_SC3_CALF_MASK;
			return 1;
		}
		Offset += (short)adc->OFS;
		Plus[0] += (unsigned long)adc->CLP0;
		Plus[1] += (unsigned long)adc->CLP1;
		Plus[2] += (unsigned long)adc->CLP2;
		Plus[3] += (unsigned long)adc->CLP3;
		Plus[4] += (unsigned long)adc->CLP4;
		Plus[5] += (unsigned long)adc->CLPS;
		Plus[6] += (unsigned long)adc->CLPD;
		Minus[0] += (unsigned long)adc->CLM0;
		Minus[1] += (unsigned long)adc->CLM1;
		Minus[2] += (unsigned long)adc->CLM2;
		Minus[3] += (unsigned long)adc->CLM3;
		Minus[4] += (unsigned long)adc->CLM4;
		Minus[5] += (unsigned long)adc->CLMS;
		Minus[6] += (unsigned long)adc->CLMD;
	}
	adc->OFS = (Offset >> TWO_POW_NUM_OF_CAL);
	adc->PG  = (0x8000 | ((Plus[0] +Plus[1] +Plus[2] +Plus[3] +Plus[4] +Plus[5] ) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->MG  = (0x8000 | ((Minus[0]+Minus[1]+Minus[2]+Minus[3]+Minus[4]+Minus[5]) >> (1 + TWO_POW_NUM_OF_CAL)));
	adc->CLP0 = (Plus[0] >> TWO_POW_NUM_OF_CAL);
	adc->CLP1 = (Plus[1] >> TWO_POW_NUM_OF_CAL);
	adc->CLP2 = (Plus[2] >> TWO_POW_NUM_OF_CAL);
	adc->CLP3 = (Plus[3] >> TWO_POW_NUM_OF_CAL);
	adc->CLP4 = (Plus[4] >> TWO_POW_NUM_OF_CAL);
	adc->CLPS = (Plus[5] >> TWO_POW_NUM_OF_CAL);
	adc->CLPD = (Plus[6] >> TWO_POW_NUM_OF_CAL);
	adc->CLM0 = (Minus[0] >> TWO_POW_NUM_OF_CAL);
	adc->CLM1 = (Minus[1] >> TWO_POW_NUM_OF_CAL);
	adc->CLM2 = (Minus[2] >> TWO_POW_NUM_OF_CAL);
	adc->CLM3 = (Minus[3] >> TWO_POW_NUM_OF_CAL);
	adc->CLM4 = (Minus[4] >> TWO_POW_NUM_OF_CAL);
	adc->CLMS = (Minus[5] >> TWO_POW_NUM_OF_CAL);
	adc->CLMD = (Minus[6] >> TWO_POW_NUM_OF_CAL);

	/// UN-SETUP
	adc->SC3 = scr3;

	return true;
}

//**************** ADC ***************************
void ADC_Start (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	adc->SC1[adcN] = ADC_SC1_AIEN(ADC_interrupt[adcN]) | ADC_SC1_ADCH(channels[adcN]);
}

bool ADC_IsReady(ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	return adc->SC1[0] & ADC_SC1_COCO_MASK;
}

ADCData_t ADC_getData (ADC_n adcN) {
	ADC_t adc = (adcN == ADC_0) ? ADC0 : ADC1;
	return adc->R[0];
}

ADCData_t ADC_getValue(ADC_n adcN){
	return currentValue[adcN];
}

void ADC_SetInterruptCallback(ADC_n adcN, adcCallback_t callback_fn){
	adcCallbacks[adcN] = callback_fn;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
__ISR__ ADC0_IRQHandler(void){
	currentValue[ADC_0] = ADC_getData(ADC_0);

	if(adcCallbacks[ADC_0]){
		adcCallbacks[ADC_0]();
	}
}

__ISR__ ADC1_IRQHandler(void){
	currentValue[ADC_1] = ADC_getData(ADC_1);

	if(adcCallbacks[ADC_1]){
		adcCallbacks[ADC_1]();
	}
}
