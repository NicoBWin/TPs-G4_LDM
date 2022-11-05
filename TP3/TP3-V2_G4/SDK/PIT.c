
#include "PIT.h"
#include "MK64F12.h"



void PIT_init(uint32_t ticks,uint8_t channel)
{
	// Clock Gating for PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT_Type* pit =
	// PIT Module enable
	PIT->MCR &= ~PIT_MCR_MDIS_MASK;


	/* ===================================== */
	/* Configure timer operation when in debug mode */

        PIT->MCR &= ~PIT_MCR_FRZ_MASK;

      //  PIT->MCR |= PIT_MCR_FRZ_MASK;

    /* ===================================== */

    NVIC_EnableIRQ(PIT0_IRQn);
    NVIC_EnableIRQ(PIT1_IRQn);
    NVIC_EnableIRQ(PIT2_IRQn);
    NVIC_EnableIRQ(PIT3_IRQn);


    PIT->CHANNEL[0].LDVAL = ticks-1;
//  PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;  // PIT interrupt enable (not used)
    PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
    PIT->MCR |= PIT_MCR_FRZ_MASK;
}



void PIT_start(id)
{
	PIT->MCR |= PIT_MCR_FRZ_MASK;
}
