/***************************************************************************//**
  @file     CMP.c
  @brief    CMP
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "..\headers\CMP.h"
#include "MK64F12.h"
#include <stdbool.h>
#include <stddef.h>
#include "../../MCAL/gpio.h"
#include "../board.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/




/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static CMP_Type * CMP_ptr[] = CMP_BASE_PTRS;
static IRQn_Type IRQ_ptr[] = CMP_IRQS;


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/




CMP_config_t* CMP_init(CMP_config_t* CMP, CMP_X_t n)
{
    CMP[n].CR_0.Hyst_CTR = HYSTCTR_3;
    CMP[n].CR_1.Power_Mode = true;
    CMP[n].CR_1.Invert = false;
    CMP[n].CR_1.Output_Pin_Enable = true;
    CMP[n].DAC_CR.DACEN = true;
    CMP[n].DAC_CR.VRSEL= Vin2;
    CMP[n].DAC_CR.VOSEL = VOUT_SELECT;

  gpioMode(CMP0_IN,INPUT);	//Preparo el pin para entrada
	gpioMode(CMP0_OUT,OUTPUT);	//Preparo el pin para salida

	//CLK Enable
    SIM->SCGC4 |= SIM_SCGC4_CMP_MASK;

	  CMP_ptr[n]->CR0 = CMP_CR0_HYSTCTR(CMP[n].CR_0.Hyst_CTR);
    CMP_ptr[n]->CR0 |= CMP_CR0_FILTER_CNT(FILTER_SAMPLE_1);   

    CMP_ptr[n]->CR1 |= CMP_CR1_EN_MASK;

    if(CMP[n].CR_1.Output_Pin_Enable){
        CMP_ptr[n]->CR1 |= CMP_CR1_OPE_MASK; 
    }

    if(CMP[n].CR_1.Invert){
        CMP_ptr[n]->CR1 |= CMP_CR1_INV_MASK;
    }

    if(CMP[n].CR_1.Power_Mode){
        CMP_ptr[n]->CR1 |= CMP_CR1_PMODE_MASK;
    }

    //SCR
    CMP_IRQ(true, n);


    //DACCR
    if(CMP[n].DAC_CR.DACEN){
        CMP_ptr[n]->DACCR = CMP_DACCR_DACEN_MASK;
        CMP_ptr[n]->DACCR |= CMP_DACCR_VRSEL(CMP[n].DAC_CR.VRSEL);
        CMP_ptr[n]->DACCR |= CMP_DACCR_VOSEL(CMP[n].DAC_CR.VOSEL);
    } 
  return CMP;       
}



void CMP_IRQ(bool en, CMP_X_t n){
    if (en){
        CMP_ptr[n]->SCR = (CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK);
        if( (CMP_ptr[n]->SCR  & (CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK)) !=0 ){
            NVIC_EnableIRQ(IRQ_ptr[n]);
        }
			
    }
    else{
        CMP_ptr[n]->SCR &= ~(CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK);
        if( (CMP_ptr[n]->SCR  & (CMP_SCR_IEF_MASK | CMP_SCR_IER_MASK)) ==0 ){
            NVIC_DisableIRQ(IRQ_ptr[n]);
        }
    }
}

void CMP_set_MUXinput (Mux_input_t p_input, Mux_input_t m_input, CMP_X_t n){
    
    CMP_ptr[n]->MUXCR = (CMP_ptr[n]->MUXCR & ~CMP_MUXCR_PSEL_MASK) | CMP_MUXCR_PSEL(p_input);
   
    CMP_ptr[n]->MUXCR = (CMP_ptr[n]->MUXCR & ~CMP_MUXCR_MSEL_MASK) | CMP_MUXCR_MSEL(m_input);
}

void CMP_set_output(CMP_output output){
    if(output == FTM1_CH0){
        SIM->SOPT4 &= ~(SIM_SOPT4_FTM1CH0SRC_MASK);
		SIM->SOPT4 |= SIM_SOPT4_FTM1CH0SRC(true);
    }
    else if (output == FTM2_CH0){
        SIM->SOPT4 &= ~(SIM_SOPT4_FTM2CH0SRC_MASK);
		SIM->SOPT4 |= SIM_SOPT4_FTM2CH0SRC(true);
    }
}

void Prueba (void)
{
    CMP_config_t *ptr_CMP=NULL;
    ptr_CMP = CMP_init(ptr_CMP,CMP_0);


    CMP_set_MUXinput(IN1,IN7,CMP_0);
    CMP_set_output(FTM1_CH0);
    

    

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


