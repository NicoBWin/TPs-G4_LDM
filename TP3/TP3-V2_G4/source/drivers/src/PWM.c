/***************************************************************************//**
  @file     template.c
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

// +Incluir el header propio (ej: #include "template.h")+


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/



/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


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
void PWM_Init (void) {

	// PTC 1 as PWM
		PCRstr UserPCR;

		UserPCR.PCR=false;			// Default All false, Set only those needed

		UserPCR.FIELD.DSE=true;
		UserPCR.FIELD.MUX=PORT_mAlt4;
		UserPCR.FIELD.IRQC=PORT_eDisabled;

		PORT_Configure2 (PORTC,1,UserPCR);

	// PTC 8 as GPIO
		UserPCR.PCR=false;			// Default All false, Set only those needed

		UserPCR.FIELD.DSE=true;
		UserPCR.FIELD.MUX=PORT_mGPIO;
		UserPCR.FIELD.IRQC=PORT_eDisabled;

		PORT_Configure2 (PORTC,8,UserPCR);

		GPIO_SetDirection(PTC, 8, GPIO__OUT);





	FTM_SetPrescaler(FTM0, FTM_PSC_x32);
	FTM_SetModulus(FTM0, PWM_modulus);
	FTM_SetOverflowMode(FTM0, true);
	FTM_SetWorkingMode(FTM0, 0, FTM_mPulseWidthModulation);			// MSA  / B
	FTM_SetPulseWidthModulationLogic(FTM0, 0, FTM_lAssertedHigh);   // ELSA / B
	FTM_SetCounter(FTM0, 0, PWM_duty);
	FTM_StartClock(FTM0);
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
