/***************************************************************************//**
  @file     PIT.c
  @brief    PIT Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../headers/PIT.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define BUS_CLOCK	50000000U
#define PITLDVAL_MStoTICKS(x) (  ( (BUS_CLOCK * x) / 1000 ) - 1U)

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static pit_callback_t pit_callback[4];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
//Channel 0 a 3. Time is in MilliSeconds
void PIT_Init(uint32_t time_ms, uint8_t channel, bool chained) {
    static _Bool firstCall=true;
    if(firstCall) {
        // Clock Gating for PIT
        SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
        NVIC_EnableIRQ(PIT0_IRQn);
        NVIC_EnableIRQ(PIT1_IRQn);
        NVIC_EnableIRQ(PIT2_IRQn);
        NVIC_EnableIRQ(PIT3_IRQn);
        firstCall = false;
    }

    // Turn on PIT
    PIT->MCR = 0x00;

    // PIT interrupt enable (not used)
    PIT->CHANNEL[channel].LDVAL = PITLDVAL_MStoTICKS(time_ms);
    PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TIE_MASK;
    PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
    PIT->CHANNEL[channel].TCTRL |= ~PIT_TCTRL_CHN_MASK;

    if(chained) {
        PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_CHN_MASK;    //Restara solo cuando el anterior termine
    }
}

void PIT_Start(uint8_t channel) {
	PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void PIT_Stop(uint8_t channel){
	PIT->CHANNEL[channel].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}

void Pit_SetCallback(uint8_t channel, pit_callback_t callback_fn) {
	pit_callback[channel] = callback_fn;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
//ISR of the PIT timers.
void PIT0_IRQHandler(void) {
	// Clear interrupt
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
	if(pit_callback[0]){
		pit_callback[0]();
	}
}

void PIT1_IRQHandler(void) {
	// Clear interrupt
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF_MASK;

	if(pit_callback[1]){
		pit_callback[1]();
	}
}

void PIT2_IRQHandler(void) {
	// Clear interrupt
	PIT->CHANNEL[2].TFLG = PIT_TFLG_TIF_MASK;

	if(pit_callback[2]){
		pit_callback[2]();
	}
}

void PIT3_IRQHandler(void) {
	// Clear interrupt
	PIT->CHANNEL[3].TFLG = PIT_TFLG_TIF_MASK;

	if(pit_callback[3]){
		pit_callback[3]();
	}
}
