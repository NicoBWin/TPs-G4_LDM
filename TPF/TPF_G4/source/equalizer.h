/***************************************************************************//**
  @file     equalizer.h
  @brief    Equalizer, filter and vumeter functions
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _EQUALIZER_H_
#define _EQUALIZER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Standar lib
#include <stdint.h>
#include <stdlib.h>
#include "../CMSIS/arm_math.h"

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
//FILTROS
void On_Off_equalizer(int on);

int GetOnOffEq();

void setUpCascadeFilter(char* GaindB);

void setUpFilter(int N_Frequency, int GaindB);

void blockEqualizer(const float * pSrc, float * pDst, int blockSize);

//VUMETRO
void InitVUAnalyzer(float32_t * tableStart);

void VUAnalyze(uint8_t data[8]);

/*******************************************************************************
 ******************************************************************************/

#endif // _EQUALIZER_H_
