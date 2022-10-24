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

void CMP_IRQ(bool en, CMP_X_t n);

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




void CMP_init(CMP_X_t n)
{
	CMP_config_t CMP_c;
	CMP_c.CR_0.Filter_CNT = FILTER_SAMPLE_1;
    CMP_c.CR_0.Hyst_CTR = HYSTCTR_3;
	CMP_c.CR_1.Sample_Enable = false;
	CMP_c.CR_1.Windowig_Enable = false;
    CMP_c.CR_1.Power_Mode = true;
    CMP_c.CR_1.Invert = false;
    CMP_c.CR_1.Output = false;
    CMP_c.CR_1.Output_Pin_Enable = true;
    CMP_c.CR_1.Module_Enable = true;

    CMP_c.DAC_CR.DACEN = true;
    CMP_c.DAC_CR.VRSEL= Vin2;
    CMP_c.DAC_CR.VOSEL = VOUT_SELECT;




  //PORT IN
    SIM->SCGC5 |= 1<<(PIN2PORT(CMP0_IN)+ CLK_GATING_OFFSET );	// Prendo el clock del puerto a configurar
    PORTC->PCR[PIN2NUM(CMP0_IN)]=0;
    PORTC->PCR[PIN2NUM(CMP0_IN)] |= PORT_PCR_MUX(0);
  //PORT OUT
    SIM->SCGC5 |= 1<<(PIN2PORT(CMP0_OUT)+ CLK_GATING_OFFSET );	// Prendo el clock del puerto a configurar
    PORTB->PCR[PIN2NUM(CMP0_OUT)]=0;
    PORTB->PCR[PIN2NUM(CMP0_OUT)] |= PORT_PCR_MUX(6);

	//CLK Enable
    SIM->SCGC4 |= SIM_SCGC4_CMP_MASK;

	CMP_ptr[n]->CR0 = CMP_CR0_HYSTCTR(CMP_c.CR_0.Hyst_CTR);
    CMP_ptr[n]->CR0 |= CMP_CR0_FILTER_CNT(FILTER_SAMPLE_1);   

    if( CMP_c.CR_1.Module_Enable)
    {
    	CMP_ptr[n]->CR1 |= CMP_CR1_EN_MASK;
    }

    if(CMP_c.CR_1.Output_Pin_Enable){
        CMP_ptr[n]->CR1 |= CMP_CR1_OPE_MASK; 
    }

    if(CMP_c.CR_1.Invert){
        CMP_ptr[n]->CR1 |= CMP_CR1_INV_MASK;
    }

    if(CMP_c.CR_1.Power_Mode){
        CMP_ptr[n]->CR1 |= CMP_CR1_PMODE_MASK;
    }

    //SCR
    CMP_IRQ(true, n);


    //DACCR
    if(CMP_c.DAC_CR.DACEN){
        CMP_ptr[n]->DACCR = CMP_DACCR_DACEN_MASK;
        CMP_ptr[n]->DACCR |= CMP_DACCR_VRSEL(CMP_c.DAC_CR.VRSEL);
        CMP_ptr[n]->DACCR |= CMP_DACCR_VOSEL(CMP_c.DAC_CR.VOSEL);
    } 

}



void CMP_IRQ(bool en, CMP_X_t n)
{
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

void CMP_set_MUXinput (Mux_input_t p_input, Mux_input_t m_input, CMP_X_t n)
{
    
    CMP_ptr[n]->MUXCR = (CMP_ptr[n]->MUXCR & ~CMP_MUXCR_PSEL_MASK) | CMP_MUXCR_PSEL(p_input);
   
    CMP_ptr[n]->MUXCR = (CMP_ptr[n]->MUXCR & ~CMP_MUXCR_MSEL_MASK) | CMP_MUXCR_MSEL(m_input);
}

void CMP_set_output(CMP_output output)
{
    if(output == FTM1_CH0){
        SIM->SOPT4 &= ~(SIM_SOPT4_FTM1CH0SRC_MASK);
		SIM->SOPT4 |= SIM_SOPT4_FTM1CH0SRC(true);
    }
    else if (output == FTM2_CH0){
        SIM->SOPT4 &= ~(SIM_SOPT4_FTM2CH0SRC_MASK);
		SIM->SOPT4 |= SIM_SOPT4_FTM2CH0SRC(true);
    }
}

void Prueba_CMP(void)
{

    CMP_init(CMP_0);


    CMP_set_MUXinput(IN1,IN7,CMP_0);
    CMP_set_output(FTM1_CH0);


    

}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


