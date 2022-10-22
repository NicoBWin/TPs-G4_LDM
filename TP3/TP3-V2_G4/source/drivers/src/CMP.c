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


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
	bool callback_enabled;
	cmp_callback_t callback;
}edges_interrupts_t;

static edges_interrupts_t interrupts_info[MODS][CMP_AMOUNT_INT_TYPES];

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static CMP_Type * modules[MODS] = {CMP0, CMP1, CMP2};

static void cmp_enable_rising_interrupts(cmp_mods_t module, bool enable_disable);
static void cmp_enable_falling_interrupts(cmp_mods_t module, bool enable_disable);


static void run_interrupt_callback(edges_interrupts_t interrupt);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void cmp_init(cmp_mods_t module){
	
		SIM->SCGC4 |= SIM_SCGC4_CMP_MASK;

		edges_interrupts_t int_info = {.callback_enabled = false, .callback = NULL};
		for (int i =0; i < MODS; i++)
			for (int j = 0; j < CMP_AMOUNT_INT_TYPES; ++j)
				interrupts_info[i][j] = int_info;

		modules[module]->CR1 |= CMP_CR1_EN_MASK;
//		SIM->SOPT4 |= SIM_SOPT4_FTM1CH0SRC(1);

	switch (module) {
	case 0:	NVIC_EnableIRQ(CMP0_IRQn);	break;
	case 1: NVIC_EnableIRQ(CMP1_IRQn);	break;
	case 2: NVIC_EnableIRQ(CMP2_IRQn);	break;
	default:							break;
	}
	//NVIC_EnableIRQ(((uint32_t *) CMP_IRQS)[module]);

}

void cmp_set_mod_conf(cmp_t conf, cmp_dac_t dac_conf){
	CMP_Type* curr_cmp = modules[conf.module];

	//curr_cmp->CR1 |= CMP_CR1_PMODE_MASK;

	if(conf.enable_output_pin)		//TODO: permitir modificar. pone la salida en el pin
		curr_cmp->CR1 |= CMP_CR1_OPE_MASK;
	//		SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;	//Clock Gating

			/* send output to FTM1-CH0 */
	//		SIM->SOPT4 &= ~SIM_SOPT4_FTM1CH0SRC_MASK;
	if(conf.invert_comparison)
		curr_cmp->CR1 |= CMP_CR1_INV_MASK;

	if(conf.comparator_output_unfiltered)
		curr_cmp->CR1 |= CMP_CR1_COS_MASK;

	curr_cmp->CR0 |= CMP_CR0_HYSTCTR(conf.hysteresis);

	curr_cmp->MUXCR = CMP_MUXCR_PSEL(conf.mux_conf.plus_input_mux_control) | CMP_MUXCR_MSEL(conf.mux_conf.minus_input_mux_control);

	cmp_set_dac_conf(dac_conf);

	curr_cmp->CR0 &= ~CMP_CR0_FILTER_CNT_MASK;
	curr_cmp->CR0 |= CMP_CR0_FILTER_CNT(conf.filter_sample_count);
	curr_cmp->FPR = conf.filter_sample_period;
}


bool cmp_get_rising_flag(cmp_mods_t module){
	return modules[module]->SCR & CMP_SCR_CFR_MASK;
}

bool cmp_get_falling_flag(cmp_mods_t module){
	return modules[module]->SCR & CMP_SCR_CFF_MASK;
}

void cmp_enable_interrupt_type(cmp_mods_t module, bool enable_disable, cmp_callback_t callback, cmp_interrupt_types int_type){
	edges_interrupts_t interrupt_conf = {.callback_enabled = enable_disable, .callback = callback};
	interrupts_info[module][int_type] = interrupt_conf;

	if(int_type == CMP_FALLING)
		cmp_enable_falling_interrupts(module, enable_disable);
	else if(int_type == CMP_RISING)
		cmp_enable_rising_interrupts(module, enable_disable);

}

static void cmp_enable_rising_interrupts(cmp_mods_t module, bool enable_disable){
	if(enable_disable)
		modules[module]->SCR |= CMP_SCR_IER_MASK;
	else
		modules[module]->SCR &= ~CMP_SCR_IER_MASK;
}

static void cmp_enable_falling_interrupts(cmp_mods_t module, bool enable_disable){
	if(enable_disable)
		modules[module]->SCR |= CMP_SCR_IEF_MASK;
	else
		modules[module]->SCR &= ~CMP_SCR_IEF_MASK;
}

void cmp_enable_module(cmp_mods_t module, bool enable_disable){
	if(enable_disable)
		modules[module]->CR1 |= CMP_CR1_EN_MASK;
	else
		modules[module]->CR1 &= ~CMP_CR1_EN_MASK;
}

bool cmp_get_output(cmp_mods_t module){
	return modules[module]->SCR & CMP_SCR_COUT_MASK;
}

static void run_interrupt_callback(edges_interrupts_t interrupt){
	if(interrupt.callback_enabled && interrupt.callback != NULL)
		interrupt.callback();
}


void cmp_set_dac_conf(cmp_dac_t conf){
	modules[conf.module]->DACCR = CMP_DACCR_DACEN(1) | CMP_DACCR_VRSEL(conf.digital_input) | CMP_DACCR_VOSEL(conf.reference_voltage_source);
}



void CMP0_IRQHandler(){
	if(modules[MOD_0]->SCR & CMP_SCR_CFR_MASK){						//get flag value
		modules[MOD_0]->SCR &= ~CMP_SCR_CFR_MASK;					//reset flag
		run_interrupt_callback(interrupts_info[MOD_0][CMP_RISING]);		//execute interruption
	}
	else if(modules[MOD_0]->SCR & CMP_SCR_CFF_MASK){					//get flag value
		modules[MOD_0]->SCR &= ~CMP_SCR_CFF_MASK;						//reset flag
		run_interrupt_callback(interrupts_info[MOD_0][CMP_FALLING]);		//execute interruption
	}
}
void CMP1_IRQHandler(){
	if(modules[MOD_1]->SCR & CMP_SCR_CFR_MASK){					//get flag value
		modules[MOD_1]->SCR &= ~CMP_SCR_CFR_MASK;				//reset flag
		run_interrupt_callback(interrupts_info[MOD_1][CMP_RISING]); //execute interruption
	}
	else if(modules[MOD_1]->SCR & CMP_SCR_CFF_MASK){				//get flag value
		modules[MOD_1]->SCR &= ~CMP_SCR_CFF_MASK;					//reset flag
		run_interrupt_callback(interrupts_info[MOD_1][CMP_FALLING]);	//execute interruption
	}
}
void CMP2_IRQHandler(){
	if(modules[MOD_2]->SCR & CMP_SCR_CFR_MASK){		//get flag value
		modules[MOD_2]->SCR &= ~CMP_SCR_CFR_MASK;	//reset flag
		run_interrupt_callback(interrupts_info[MOD_2][CMP_RISING]);	//execute interruption
	}
	else if(modules[MOD_2]->SCR & CMP_SCR_CFF_MASK){	//get flag value
		modules[MOD_2]->SCR &= CMP_SCR_CFF_MASK;		//reset flag
		run_interrupt_callback(interrupts_info[MOD_2][CMP_FALLING]);		//execute interruption
	}
}
