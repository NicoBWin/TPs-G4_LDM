/***************************************************************************/ /**
  @file     DMA.h
  @brief    DMA Header
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/
#ifndef DMA_H
#define DMA_H

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define DELTA0 8
#define DELTA1 4

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef void (*dma_callback_t)(void);

typedef enum {
	FTM0CH0 = 20, 	FTM0CH1 = 21, FTM0CH2 = 22, FTM0CH3 = 23,
	FTM0CH4 = 24, 	FTM0CH5 = 25, FTM0CH6 = 26, FTM0CH7 = 27,
	DMADAC0 = 45,	DMADAC1 = 46, DMAALWAYS63 = 63
} DMA_request_t;

typedef enum {
	DMA_0,
	DMA_1,
	DMA_2,
	DMA_3,
} DMA_n;

typedef struct DMA_config_t {
	void * source_buffer;
	void * destination_buffer;
	DMA_request_t request_source;
	uint8_t source_offset;
	uint8_t destination_offset;
	uint8_t transfer_bytes;
	uint16_t major_cycles;
	uint32_t wrap_around;
} DMA_config_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void DMA_Init(uint8_t DMA_channel, DMA_config_t config);

void DMA_SetCallback(uint8_t channel, dma_callback_t callback_fn);


/*******************************************************************************
 ******************************************************************************/
#endif /* DMA_h */
