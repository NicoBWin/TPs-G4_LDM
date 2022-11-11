
#include "../headers/PIT.h"
#include "MK64F12.h"
#include  <os.h>

#define PIT_LDVAL_TICKS(x) ( (CORE_CLOCK/2*x) - 1U)

static OS_SEM *Sem;
static OS_ERR pit_err;



void PIT_init(uint32_t frec, int channel, _Bool chained, OS_SEM *PitSem) //Channel 0 a 3.
{
    //uint32_t ticks = PIT_LDVAL_TICKS(frec);
    uint32_t ticks = frec;
    static _Bool firstCall=true;
    if(firstCall)
    {
        // Clock Gating for PIT
        SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
        NVIC_EnableIRQ(PIT0_IRQn);
        NVIC_EnableIRQ(PIT1_IRQn);
        NVIC_EnableIRQ(PIT2_IRQn);
        NVIC_EnableIRQ(PIT3_IRQn);
        firstCall = false;
    }
    // Halt for setup
    PIT->MCR = 0;
    //PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    //PIT->MCR &= ~PIT_MCR_FRZ_MASK;
    PIT->CHANNEL[channel].LDVAL = ticks;
    PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TIE_MASK;  // PIT interrupt enable (not used)
    PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_TEN_MASK;
    if(chained)
    {
        PIT->CHANNEL[channel].TCTRL |= PIT_TCTRL_CHN_MASK;    //RestarÃ¡ solo cuando el anterior termine
    }
    //PIT->MCR |= PIT_MCR_FRZ_MASK;
    //PIT->MCR |= PIT_MCR_MDIS_MASK;

    Sem = PitSem;
}


//ISR of the PIT timer. Clears the PIT interrupt flag and starts DMA channel 0 transfer
void PIT0_IRQHandler(void) {
    // Clear interrupt
    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
    OSSemPost(Sem, OS_OPT_POST_ALL, &pit_err);
}
