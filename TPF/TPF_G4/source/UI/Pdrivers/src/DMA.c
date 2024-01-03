/***************************************************************************//**
  @file     DMA.c
  @brief    DMA Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main lib
#include "../headers/DMA.h"


// MCU libs
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
static dma_callback_t dma_callbacks[16];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
// ONLY CHN_0 WORKS
void DMA_Init(uint8_t DMA_channel, DMA_config_t config) {
	// Clock Gating
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	if(config.request_source >= 58) {
		DMAMUX->CHCFG[DMA_channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_TRIG_MASK | DMAMUX_CHCFG_SOURCE((uint8_t)config.request_source);
	}
	else {
		DMAMUX->CHCFG[DMA_channel] |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE((uint8_t)config.request_source);
	}

	/* Enable the interrupts for the channel 0. */
	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	/* Enable the DMA interrupts. */
	NVIC_EnableIRQ(DMA0_IRQn);

	// ============= INIT TCD0 ===================//
	/* Set memory address for source and destination. */
	DMA0->TCD[DMA_channel].SADDR = (uint32_t)(config.source_buffer);
	//List of Values to be written to the selected port
	DMA0->TCD[DMA_channel].DADDR = (uint32_t)(config.destination_buffer);

	// Set source and destination data transfer size
	switch(config.transfer_bytes) {
		case(1):
			DMA0->TCD[DMA_channel].ATTR = DMA_ATTR_SSIZE(0) | DMA_ATTR_DSIZE(0);
		break;
		case(2):
			DMA0->TCD[DMA_channel].ATTR = DMA_ATTR_SSIZE(1) | DMA_ATTR_DSIZE(1);
		break;
		case(4):
			DMA0->TCD[DMA_channel].ATTR = DMA_ATTR_SSIZE(2) | DMA_ATTR_DSIZE(2);
		break;
	}

	// Toggle PIN (only if '1' is written)/* Set an offset for source and destination address. */
	DMA0->TCD[DMA_channel].SOFF = config.source_offset;
	DMA0->TCD[DMA_channel].DOFF = config.destination_offset; // Destination address offset is 0. (Siempre al mismo lugar -> PTOR)

	/*Number of bytes to be transferred in each service request of the channel.*/
	DMA0->TCD[DMA_channel].NBYTES_MLNO = config.transfer_bytes;

	/* Autosize for Current major iteration count */
	DMA0->TCD[DMA_channel].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(config.major_cycles);
	DMA0->TCD[DMA_channel].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(config.major_cycles);

	/* Autosize SLAST for Wrap Around. This value is added to SADD at the end of Major Loop */
	DMA0->TCD[DMA_channel].SLAST = -(config.wrap_around);

	/* Scatter and Gather */
	DMA0->TCD[DMA_channel].DLAST_SGA = 0x00; //IMPORTANTE! SETEAR EN 0 (o valor conocido, es random)

	//DMA0->TCD[DMA_channel].CSR &= ~DMA_CSR_INTMAJOR_MASK;
	DMA0->TCD[DMA_channel].CSR = DMA_CSR_INTMAJOR_MASK;	//Enable Major Interrupt.

	/* Enable request signal for channel 0. */
	switch(DMA_channel) {
		case 0:
			DMA0->ERQ = DMA_ERQ_ERQ0_MASK;
		break;
		case 1:
			DMA0->ERQ = DMA_ERQ_ERQ1_MASK;
		break;
		case 2:
			DMA0->ERQ = DMA_ERQ_ERQ2_MASK;
		break;
		case 3:
			DMA0->ERQ = DMA_ERQ_ERQ3_MASK;
		break;
		case 4:
			DMA0->ERQ = DMA_ERQ_ERQ4_MASK;
		break;
		case 5:
			DMA0->ERQ = DMA_ERQ_ERQ5_MASK;
		break;
		case 6:
			DMA0->ERQ = DMA_ERQ_ERQ6_MASK;
		break;
		case 7:
			DMA0->ERQ = DMA_ERQ_ERQ7_MASK;
		break;
		case 8:
			DMA0->ERQ = DMA_ERQ_ERQ8_MASK;
		break;
		case 9:
			DMA0->ERQ = DMA_ERQ_ERQ9_MASK;
		break;
		case 10:
			DMA0->ERQ = DMA_ERQ_ERQ10_MASK;
		break;
		case 11:
			DMA0->ERQ = DMA_ERQ_ERQ11_MASK;
		break;
		case 12:
			DMA0->ERQ = DMA_ERQ_ERQ12_MASK;
		break;
		case 13:
			DMA0->ERQ = DMA_ERQ_ERQ13_MASK;
		break;
		case 14:
			DMA0->ERQ = DMA_ERQ_ERQ14_MASK;
		break;
		case 15:
			DMA0->ERQ = DMA_ERQ_ERQ15_MASK;
		break;
	}
}

void DMA_SetCallback(uint8_t channel, dma_callback_t callback_fn){
	dma_callbacks[DMA_0] = callback_fn;
}

uint32_t DMA_GetRemainingMajorLoopCount(uint8_t DMA_channel) {
    uint32_t remainingCount = 0;

    if (DMA_CSR_DONE_MASK & DMA0->TCD[DMA_channel].CSR) {
        remainingCount = 0;
    }
    else {
        /* Calculate the unfinished bytes */
        if (DMA0->TCD[DMA_channel].CITER_ELINKNO & DMA_CITER_ELINKNO_ELINK_MASK) {
            remainingCount =
                (DMA0->TCD[DMA_channel].CITER_ELINKYES & DMA_CITER_ELINKYES_CITER_MASK) >> DMA_CITER_ELINKYES_CITER_SHIFT;
        }
        else {
            remainingCount =
                (DMA0->TCD[DMA_channel].CITER_ELINKNO & DMA_CITER_ELINKNO_CITER_MASK) >> DMA_CITER_ELINKNO_CITER_SHIFT;
        }
    }
    return remainingCount;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void DMA0_IRQHandler(void) {
	/* Clear all the pending events. */
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	DMA0->CINT |= DMA_CINT_CINT(DMA_0); //clear interrupt ch0
	if(dma_callbacks[DMA_0]){
		dma_callbacks[DMA_0]();
	}
}

void DMA_Error_IRQHandler(void){
	/* Clear all error indicators.*/
	DMA0->CERR = DMA_CERR_CAEI(1);
}
