/***************************************************************************//**
  @file     ADC.c
  @brief    ADC Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main lib
#include "../headers/DAC.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define DAC_DATL_DATA0_WIDTH 8

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void DAC_Init (DAC_n dacN) {
	DAC_t dac = (dacN == DAC_0) ? DAC0 : DAC1;

	if(dacN == DAC_0)
		SIM->SCGC2 |= SIM_SCGC2_DAC0_MASK;
	else
		SIM->SCGC2 |= SIM_SCGC2_DAC1_MASK;

	dac->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK | DAC_C0_DACTRGSEL_MASK;
}

//USE DMA?

void DAC_SetData (DAC_n dacN, DACData_t data) {
	DAC_t dac = (dacN == DAC_0) ? DAC0 : DAC1;
	dac->DAT[0].DATL = DAC_DATL_DATA0(data);
	dac->DAT[0].DATH = DAC_DATH_DATA1(data >> DAC_DATL_DATA0_WIDTH);
}
