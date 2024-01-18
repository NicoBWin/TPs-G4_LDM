/***************************************************************************//**
  @file     equalizer.h
  @brief    Equalizer, filter and vumeter functions
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "../CMSIS/arm_math.h"

// Main Lib
#include "equalizer.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_POWER_MAX_SIGNAL 8391402

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
* Prototypes
******************************************************************************/
/*
	// Vumetro
	startAnalyzer(audio_buff);
	static uint8_t vumetValues[8];
	getAnalyzer(vumetValues);
	for(int i=0; i<8; i++){
		static color_t VUColor = {.r=255,.b=0,.g=0};
		VUmeter(i, vumetValues[i], VUColor);
	}
 */
/*******************************************************************************
* Variables
******************************************************************************/
arm_rfft_fast_instance_f32 S;
float32_t pOut[1024];
float32_t modulo[1024/2];
double powerBins[8];

/*******************************************************************************
* Code
******************************************************************************/
void InitVUAnalyzer(float32_t * tableStart) {
	arm_rfft_fast_init_f32(&S, 1024);
	arm_rfft_fast_f32(&S, tableStart, pOut, 0);
	arm_cmplx_mag_f32(pOut, modulo, 1024/2);

	powerBins[0] = modulo[2] + modulo[3] + modulo[4];
	powerBins[1] = modulo[5] + modulo[6] + modulo[7];
	powerBins[2] = modulo[8] + modulo[9] + modulo[10];
	powerBins[3] = modulo[11] + modulo[12] + modulo[13];
	powerBins[4] = modulo[14] + modulo[15] + modulo[16];
	powerBins[5] = modulo[17] + modulo[18] + modulo[19];
	powerBins[6] = modulo[20] + modulo[21] + modulo[22];
	powerBins[7] = modulo[23] + modulo[24] + modulo[25] + modulo[26] + modulo[27];
}

void getAnalyzer(uint8_t data [8]){
	for(uint8_t i = 0; i < 8; i++){
		if( powerBins[i] >= MAX_POWER_MAX_SIGNAL * 0.8){
			data[i] = 100;
		}
		else if(powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.8 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.7){
			data[i] = 90;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.7 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.6){
			data[i] = 85;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.6 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.5){
			data[i] = 75;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.5 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.4){
			data[i] = 50;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.4 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.3){
			data[i] = 40;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.3 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.2){
			data[i] = 30;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.2 && powerBins[i] > MAX_POWER_MAX_SIGNAL * 0.1){
			data[i] = 20;
		}
		else if (powerBins[i] <= MAX_POWER_MAX_SIGNAL * 0.1){
			data[i] = 0;
		}
	}
}
