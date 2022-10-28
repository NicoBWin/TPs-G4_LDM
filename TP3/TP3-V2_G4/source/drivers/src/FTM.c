/***************************************************************************//**
  @file     FTM.c
  @brief    FTM Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main lib
#include "../headers/FTM.h"
#include "../headers/circularbuffer.h"

// Internal libs
#include "../../MCAL/gpio.h"
#include "../board.h"

// MCU libs
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_FTM 4
#define MAX_COUNT	0xFFFF

#define FTM_CH_0 0
#define FTM_CH_1 1
#define FTM_CH_2 2
#define FTM_CH_3 3
#define FTM_CH_4 4
#define FTM_CH_5 5
#define FTM_CH_6 6
#define FTM_CH_7 7

#define FTM_DMA_ON  1
#define FTM_DMA_OFF 0

#define PORT_Alt3	0b011
#define PORT_Alt6	0b110

#define FTM0_CH0	PORTNUM2PIN(PA,3)	//ALT3
#define FTM0_CH1	PORTNUM2PIN(PA,4)	//ALT3
#define FTM0_CH2	PORTNUM2PIN(PA,5)	//ALT3
#define FTM0_CH3	PORTNUM2PIN(PA,6)	//ALT3
#define FTM0_CH4	PORTNUM2PIN(PA,7)	//ALT3
#define FTM0_CH5	PORTNUM2PIN(PA,0)	//ALT3
#define FTM0_CH6	PORTNUM2PIN(PA,1)	//ALT3
#define FTM0_CH7	PORTNUM2PIN(PA,2)	//ALT3

#define FTM1_CH0	PORTNUM2PIN(PA,8)	//ALT3
#define FTM1_CH1	PORTNUM2PIN(PA,9)	//ALT3

#define FTM2_CH0	PORTNUM2PIN(PB,18)	//ALT3
#define FTM2_CH1	PORTNUM2PIN(PB,19)	//ALT3

#define FTM3_CH0	PORTNUM2PIN(PE,5)	//ALT6
#define FTM3_CH1	PORTNUM2PIN(PE,6)	//ALT6
#define FTM3_CH2	PORTNUM2PIN(PE,7)	//ALT6
#define FTM3_CH3	PORTNUM2PIN(PE,8)	//ALT6
#define FTM3_CH4	PORTNUM2PIN(PE,9)	//ALT6
#define FTM3_CH5	PORTNUM2PIN(PE,10)	//ALT6
#define FTM3_CH6	PORTNUM2PIN(PE,11)	//ALT6
#define FTM3_CH7	PORTNUM2PIN(PE,12)	//ALT6


#define READ_BIT(reg, bit_mask)		(((reg) & (bit_mask)) == (bit_mask))
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void FTM_PortConfig(FTM_n FTMn, FTM_Channel_t ch);
void IC_ISR(FTM_n FTMn);
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static FTMConfig_t ftmconfig[MAX_FTM];
static PORT_Type* const PORT_TYPE[4] = {PORTA,PORTA,PORTB,PORTE};
static int const FTM0_PIN[8] = {FTM0_CH0, FTM0_CH1, FTM0_CH2, FTM0_CH3, FTM0_CH4, FTM0_CH5, FTM0_CH6, FTM0_CH7};
static int const FTM1_PIN[2] = {FTM1_CH0, FTM1_CH1};
static int const FTM2_PIN[2] = {FTM2_CH0, FTM2_CH1};
static int const FTM3_PIN[8] = {FTM3_CH0, FTM3_CH1, FTM3_CH2, FTM3_CH3, FTM3_CH4, FTM3_CH5, FTM3_CH6, FTM3_CH7};
/* En ALT3
 * FTM0 -> Todos los CH en PORTA
 *
 * FTM1 -> Solo esta CH0 y CH1 (PTA8-PTA9 o PTA12-PTA13 o PTB0-PTB1)
 *
 * FTM2 -> Solo esta CH0 y CH1 (PTA10-PTA11 o PTB18-PTB19)
 *
 * EN ALT6
 * FTM3 -> Todos los CH en PORTE
 */
static FTM_Type* const FTM_PTRS[] = FTM_BASE_PTRS;


static uint16_t IC_count=0;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void FTM_Init (FTM_n FTMn, FTMConfig_t config) {
	// Save configuration
	ftmconfig[FTMn] = config;

	//Pointer to specific FTM
	FTM_t FTMp;

    // Clock gating for the channel port
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Clock gating and NVIC
    switch(FTMn) {
        case 0:
        	SIM->SCGC6 |= SIM_SCGC6_FTM0_MASK;
        	NVIC_EnableIRQ(FTM0_IRQn);
        	FTM0->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
        	FTMp = FTM0;
            break;
        case 1:
        	SIM->SCGC6 |= SIM_SCGC6_FTM1_MASK;
        	NVIC_EnableIRQ(FTM1_IRQn);
        	FTM1->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
        	FTMp = FTM1;
            break;
        case 2:
        	SIM->SCGC6 |= SIM_SCGC6_FTM2_MASK;
			SIM->SCGC3 |= SIM_SCGC3_FTM2_MASK;
			NVIC_EnableIRQ(FTM2_IRQn);
			FTM2->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
			FTMp = FTM2;
			break;
        case 3:
        	SIM->SCGC3 |= SIM_SCGC3_FTM3_MASK;
        	NVIC_EnableIRQ(FTM3_IRQn);
        	FTM3->PWMLOAD = FTM_PWMLOAD_LDOK_MASK | 0x0F;
        	FTMp = FTM3;
        	break;
    }

    // Enable FTM register write
    FTMp->MODE = FTM_MODE_WPDIS(1);

    //FTMp->MODE |= FTM_MODE_FTMEN_MASK;	// FTM Advanced
    //**************** Init channel: GPIO and polarity ***************************
    switch(config.channel) {
    	case FTM_Channel_0:
    		FTM_PortConfig(FTMn, FTM_Channel_0);
        	FTMp->POL = (FTMp->POL & ~FTM_POL_POL0_MASK) | FTM_POL_POL0(config.active_low);
        break;
    	case FTM_Channel_1:
    		FTM_PortConfig(FTMn, FTM_Channel_1);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL1_MASK) | FTM_POL_POL1(config.active_low);
		break;
    	case FTM_Channel_2:
    		FTM_PortConfig(FTMn, FTM_Channel_2);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL2_MASK) | FTM_POL_POL2(config.active_low);
		break;
    	case FTM_Channel_3:
    		FTM_PortConfig(FTMn, FTM_Channel_3);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL3_MASK) | FTM_POL_POL3(config.active_low);
		break;
    	case FTM_Channel_4:
    		FTM_PortConfig(FTMn, FTM_Channel_4);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL4_MASK) | FTM_POL_POL4(config.active_low);
		break;
    	case FTM_Channel_5:
    		FTM_PortConfig(FTMn, FTM_Channel_5);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL5_MASK) | FTM_POL_POL5(config.active_low);
		break;
    	case FTM_Channel_6:
    		FTM_PortConfig(FTMn, FTM_Channel_6);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL6_MASK) | FTM_POL_POL6(config.active_low);
		break;
    	case FTM_Channel_7:
    		FTM_PortConfig(FTMn, FTM_Channel_7);
			FTMp->POL = (FTMp->POL & ~FTM_POL_POL7_MASK) | FTM_POL_POL7(config.active_low);
		break;
    }

    //**************** FTM Register config ***************************
    // Set Prescaler
    FTMp->SC = (FTMp->SC & ~FTM_SC_PS_MASK) | FTM_SC_PS(config.prescale);

       // Set Modulo and Counter
    FTMp->CNTIN = 0x0;	//Arranca en 0
    FTMp->CNT = 0x0;	//Arranca en 0
    FTMp->MOD = FTM_MOD_MOD(config.modulo); //Defino el módulo para el contador

    // Set Mode
    FTMp->CONTROLS[config.channel].CnSC = (FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_MSB_MASK | FTM_CnSC_MSA_MASK)) | (FTM_CnSC_MSB((config.mode >> 1) & 0X01) | FTM_CnSC_MSA((config.mode >> 0) & 0X01));

    // Set edge on mode
	switch(config.mode) {
		case FTM_mInputCapture:
			FTMp->CONTROLS[config.channel].CnSC = (FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSA_MASK)) | FTM_CnSC_ELSA(1); //(config.IC_edge >> 0) & 0X01
			FTMp->CONTROLS[config.channel].CnSC = (FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSB_MASK)) | FTM_CnSC_ELSB(1); //(config.IC_edge >> 1) & 0X01
		break;
		case FTM_mOutputCompare:
			FTMp->CONTROLS[config.channel].CnSC =
							(FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
							(FTM_CnSC_ELSB((config.OC_effect >> 1) & 0X01) | FTM_CnSC_ELSA((config.OC_effect >> 0) & 0X01));
		break;
		case FTM_mPWM:
			FTMp->CONTROLS[config.channel].CnSC =
							(FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_ELSB_MASK | FTM_CnSC_ELSA_MASK)) |
							(FTM_CnSC_ELSB((config.PWM_logic >> 1) & 0X01) | FTM_CnSC_ELSA((config.PWM_logic >> 0) & 0X01));
		break;
	}

	if(config.mode == FTM_mPWM) {
		// Turn percentage duty cycle to counter value
		config.counter = (uint16_t)((config.PWM_DC / 100.0) * config.modulo);
	}
	FTMp->CONTROLS[config.channel].CnV = FTM_CnV_VAL(config.counter);

    // Set DMA mode
	FTMp->CONTROLS[config.channel].CnSC = (FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_DMA_MASK)) | (FTM_CnSC_DMA(config.DMA_on));

    // Set interrupts
	FTMp->CONTROLS[config.channel].CnSC = (FTMp->CONTROLS[config.channel].CnSC & ~(FTM_CnSC_CHIE_MASK)) |  FTM_CnSC_CHIE(config.interrupt_on);
	FTMp->SC |= FTM_SC_TOIE_MASK;	// Enable Overflow interrupt

	// Set CLK Source
	FTMp->SC = (FTMp->SC & ~FTM_SC_CLKS_MASK) | FTM_SC_CLKS(config.CLK_source);
	if (config.CLK_source == FTM_ExtCLK) {
		// Set external clock gpio
		// Do some day
	}

    // Disable FTM register write
    //FTMp->MODE = (FTMp->MODE & ~FTM_MODE_WPDIS_MASK) | FTM_MODE_WPDIS(0);
}


void FTM_start(FTM_n FTMn) {
	FTM_PTRS[FTMn]->SC = FTM_PTRS[FTMn]->SC | FTM_SC_CLKS(0x01);

}

void FTM_stop(FTM_n FTMn) {
	FTM_PTRS[FTMn]->SC =  FTM_PTRS[FTMn]->SC & ~FTM_SC_CLKS(0x01);
}

void FTM_resetCounter(FTM_n FTMn) {
	// Enable write
	FTM_PTRS[FTMn]->MODE |= FTM_MODE_WPDIS(1);

	// Reset counter
	FTM_PTRS[FTMn]->CNT = 0x0;

	// Disable write
    FTM_PTRS[FTMn]->MODE = (FTM_PTRS[FTMn]->MODE & ~FTM_MODE_WPDIS_MASK) | FTM_MODE_WPDIS(0);
}

uint16_t FTM_getCounter(FTM_n FTMn) {
	//return FTM_PTRS[FTMn]->CONTROLS[ftmconfig[FTMn].channel].CnV = FTM_CnV_VAL(ftmconfig[FTMn].counter);
	return IC_count;
}

void FTM_modifyDC(FTM_n FTMn, uint16_t DC) {
	// Enable FTM register write
	FTM_PTRS[FTMn]->MODE = FTM_MODE_WPDIS(1);

    if(ftmconfig[FTMn].mode == FTM_mPWM) {
    	// Turn percentage duty cycle to counter value
    	ftmconfig[FTMn].counter = (uint16_t)((DC / 100.0) * ftmconfig[FTMn].modulo);
    	FTM_PTRS[FTMn]->CONTROLS[ftmconfig[FTMn].channel].CnV = FTM_CnV_VAL(ftmconfig[FTMn].counter); //ftmconfig[FTMn].counter
    }

    // Disable FTM register write
	//FTM_PTRS[FTMn]->MODE = (FTM_PTRS[FTMn]->MODE & ~FTM_MODE_WPDIS_MASK) | FTM_MODE_WPDIS(0);
}

void FTM_ClearInterruptFlag (FTM_n FTMn) {
    FTM_PTRS[FTMn]->CONTROLS[ftmconfig[FTMn].channel].CnSC &= ~FTM_CnSC_CHF_MASK;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void FTM_PortConfig(FTM_n FTMn, FTM_Channel_t ch){
	switch(FTMn) {
			case FTM_0:
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM0_PIN[ch])] = 0x0; //Clear all bits
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM0_PIN[ch])] |= PORT_PCR_DSE(1);
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM0_PIN[ch])] |= PORT_PCR_MUX(PORT_Alt3); //Set MUX to Alt3
			break;
			case FTM_1:
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM1_PIN[ch])] = 0x0; //Clear all bits
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM1_PIN[ch])] |= PORT_PCR_DSE(1);
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM1_PIN[ch])] |= PORT_PCR_MUX(PORT_Alt3); //Set MUX to Alt3
			break;
			case FTM_2:
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM2_PIN[ch])] = 0x0; //Clear all bits
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM2_PIN[ch])] |= PORT_PCR_DSE(1);
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM2_PIN[ch])] |= PORT_PCR_MUX(PORT_Alt3); //Set MUX to Alt3
			break;
			case FTM_3:
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM3_PIN[ch])] = 0x0; //Clear all bits
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM3_PIN[ch])] |= PORT_PCR_DSE(1);
				PORT_TYPE[FTMn]->PCR[PIN2NUM(FTM3_PIN[ch])] |= PORT_PCR_MUX(PORT_Alt6); //Set MUX to Alt3
			break;
	}
}


__ISR__ FTM0_IRQHandler(void){
	//IC_ISR(FTM_0);
}

__ISR__ FTM1_IRQHandler(void){
	//IC_ISR(FTM_1);
}

__ISR__ FTM2_IRQHandler(void) {
	IC_ISR(FTM_2);
}

__ISR__ FTM3_IRQHandler(void){
	//IC_ISR(FTM_3);
}

void IC_ISR(FTM_n FTMn){
	// Enable FTM register write
	FTM_PTRS[FTMn]->MODE = FTM_MODE_WPDIS(1);

	static uint16_t IC_ovf = 0;
	//FTM2->STATUS = 0;	//Limpio todos los flags
	if (READ_BIT(FTM_PTRS[FTMn]->SC, FTM_SC_TOF_MASK)){
		FTM_PTRS[FTMn]->SC &= ~FTM_SC_TOF_MASK;
		IC_ovf++;
	}
	else if(FTM_PTRS[FTMn]->STATUS){
			FTM_PTRS[FTMn]->STATUS = 0x00;

			IC_count = FTM_PTRS[FTMn]->CONTROLS[ftmconfig[FTMn].channel].CnV & FTM_CnV_VAL_MASK;	//Copy value to internal var

		//Lógica
		static uint16_t med1,med2,med;
		static uint8_t  state=0;
		uint8_t bitRecived;
		static uint8_t Is_cero =0;

		if(state==0) {
			med1=FTM_getCounter(FTM_2); //
			state=1;
		}
		else if(state==1) {
			med2 = FTM_getCounter(FTM_2);
			med = med2 - med1;
			//med += IC_ovf * MAX_COUNT;
			IC_ovf = 0;
			state = 0;                    // Set break point here and watch "med" value
		}
		if(((CLOCK_FREC_PRC/med)>800) & ((CLOCK_FREC_PRC/med)<1600)) {
			bitRecived = 1;
			bitstream_reconTX(bitRecived);
			med1 = 0;
			med2 = 0;
			med = 0;
		}
		else if (((CLOCK_FREC_PRC/med)>2000) & ((CLOCK_FREC_PRC/med)<2600))
		{
			bitRecived = 0;
			if(Is_cero==0) {
				bitstream_reconTX(bitRecived);
				Is_cero=1;
			}
			else {
				Is_cero =0;
			}
			med1 = 0;
			med2 = 0;
			med = 0;
		}
	}
	// Disable FTM register write
	FTM_PTRS[FTMn]->MODE = (FTM_PTRS[FTMn]->MODE & ~FTM_MODE_WPDIS_MASK) | FTM_MODE_WPDIS(0);
}
