/***************************************************************************//**
  @file     PIT.h
  @brief    Header encoder
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _PIT_H_
#define _PIT_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void (*pit_callback_t)(void);

typedef enum {
	PIT_CH0,
	PIT_CH1,
	PIT_CH2,
	PIT_CH3,
} PITch_n;

#define BUS_CLOCK	50000000U
#define PITLDVAL_MStoTICKS(x) (  ( (BUS_CLOCK * x) / 1000 ) - 1U)
/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void PIT_Init(uint32_t time_ms, uint8_t channel, bool chained);

void PIT_Start(uint8_t channel);
void PIT_Stop(uint8_t channel);
void PIT_TIEen(uint8_t channel);

void Pit_SetCallback(uint8_t channel, pit_callback_t callback_fn);
/*******************************************************************************
 ******************************************************************************/
#endif /* PIT_H */
