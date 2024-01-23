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
#define MAX_SIGNAL 8000000

/*******************************************************************************
* Variables EQ
******************************************************************************/
static int Equalizer_ON;

static float32_t pState[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static float32_t pCoeffs [5*4];
static float32_t pCoeffs_filter [12][5] ={{1.00453968207833,-2,0.995917140680743,1.97554429635406,-0.975995533152126}, // 150Hz -9db
											{1.007695793510805,-2,0.992761029248211 ,1.970195284624802,-0.970645299647719 }, //-6dB
											{1.021740027305337,-2,0.978716795453736 ,1.937205750397515,-0.937648230235409 }, // -3.5dB
											{1.015730293762013,-2,0.989355301752624 ,1.923612924125309,-0.928504282754724}, // 500Hz -9dB
											{1.025384340054628,-2,0.979701255460009 ,1.908182701493999,-0.913034824187912}, // -6dB
											{1.068343432627087,-2,0.936742162887549 ,1.816536937638274,-0.821156023689386}, //-3.5
											{1.063771500843777,-2,0.982787728876055 ,1.758394402069101,-0.799329146521103 }, // 1500Hz -9dB
											{1.093414015360886,-2,0.953145214358947 ,1.719370914163331,-0.759397206846395 },// -6dB
											{1.225318865060547,-2,0.821240364659285 ,1.508775857076812,-0.543899577939500 }, // -3dB
											{1.315471063170089,-2,1.059958207403383 ,1.292172328674553,-0.534731986079310 },// 4000Hz -9dB
											{1.408996510952285,-2,0.966432759621188 ,1.227573744668495,-0.458007302436515}, // -6dB
											{1.825171060708643,-2,0.550258209864829 ,0.933924963748510,-0.109236347703740}}; // -3dB
static arm_biquad_casd_df1_inst_f32 Sequ;

/*******************************************************************************
* Variables VUMETRO
******************************************************************************/
arm_rfft_fast_instance_f32 S;
float32_t pOut[1024];
float32_t modulo[1024/2];
double Bins[8];

/*******************************************************************************
* Code
******************************************************************************/
// FILTROS
void setUpFilter(int N_Frequency, int GaindB) {
	int i;
	if (GaindB > 0) {
		i = (N_Frequency*3) + (9/GaindB) - 1; // Asumo ganancia positiva
		pCoeffs[N_Frequency*5+0] = pCoeffs_filter [i][0] ; // b0
		pCoeffs[N_Frequency*5+1] = pCoeffs_filter [i][1] ; // b1
		pCoeffs[N_Frequency*5+2] = pCoeffs_filter [i][2] ; // b2

		pCoeffs[N_Frequency*5+3] = pCoeffs_filter [i][3] ; // a1
		pCoeffs[N_Frequency*5+4] = pCoeffs_filter [i][4] ; // a2
	}
	else {
		pCoeffs[N_Frequency*5] = 1;
		pCoeffs[N_Frequency*5 + 1] = 0;
		pCoeffs[N_Frequency*5 + 2] = 0;
		pCoeffs[N_Frequency*5 + 3] = 0;
		pCoeffs[N_Frequency*5 + 4] = 0;
	}
	arm_biquad_cascade_df1_init_f32(&Sequ, 4, pCoeffs, pState);
}

void setUpCascadeFilter(char* GaindB) {
	int i;
	for(i = 0; i<4; i++) {
		setUpFilter(i, GaindB[i]-'0');
	}
}

void On_Off_equalizer(int on) {
	Equalizer_ON = on;
}

int GetOnOffEq() {
	return Equalizer_ON;
}

void blockEqualizer(const float * pSrc, float * pDst, int blockSize){
//########################################
	arm_biquad_cascade_df1_f32(&Sequ, pSrc, pDst, blockSize);
//########################################
}

//--------------------------------------------------------------------

// VUMETRO
void InitVUAnalyzer(float32_t * tableStart) {
	arm_rfft_fast_init_f32(&S, 1024);
	arm_rfft_fast_f32(&S, tableStart, pOut, 0);
	arm_cmplx_mag_f32(pOut, modulo, 1024/2);

	Bins[0] = modulo[2] + modulo[3] + modulo[4];
	Bins[1] = modulo[5] + modulo[6] + modulo[7];
	Bins[2] = modulo[8] + modulo[9] + modulo[10];
	Bins[3] = modulo[11] + modulo[12] + modulo[13];
	Bins[4] = modulo[14] + modulo[15] + modulo[16];
	Bins[5] = modulo[17] + modulo[18] + modulo[19];
	Bins[6] = modulo[20] + modulo[21] + modulo[22];
	Bins[7] = modulo[23] + modulo[24] + modulo[25] + modulo[26] + modulo[27];
}

void VUAnalyze(uint8_t data [8]){
	for(uint8_t i = 0; i < 8; i++){
		if(Bins[i] >= MAX_SIGNAL * 0.8){
			data[i] = 100;
		}
		else if(Bins[i] <= MAX_SIGNAL * 0.8 && Bins[i] > MAX_SIGNAL * 0.7){
			data[i] = 90;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.7 && Bins[i] > MAX_SIGNAL * 0.6){
			data[i] = 85;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.6 && Bins[i] > MAX_SIGNAL * 0.5){
			data[i] = 75;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.5 && Bins[i] > MAX_SIGNAL * 0.4){
			data[i] = 50;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.4 && Bins[i] > MAX_SIGNAL * 0.3){
			data[i] = 40;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.3 && Bins[i] > MAX_SIGNAL * 0.2){
			data[i] = 30;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.2 && Bins[i] > MAX_SIGNAL * 0.1){
			data[i] = 20;
		}
		else if (Bins[i] <= MAX_SIGNAL * 0.1){
			data[i] = 0;
		}
	}
}
