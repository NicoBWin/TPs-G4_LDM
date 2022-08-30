/***************************************************************************//**
  @file     car_reader.c
  @brief    Driver lector de tarjetas de banda magnetica
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>

#include "../headers/SysTick.h"
#include "../../MCAL/gpio.h"
#include "../board.h"
#include "MK64F12.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SYSTICK_LOAD_INIT ((__CORE_CLOCK__ / SYSTICK_ISR_FREQUENCY_HZ) - 1U)
#if SYSTICK_LOAD_INIT > (1 << 24)
#error Overflow de SysTick! Ajustar  __CORE_CLOCK__ y SYSTICK_ISR_FREQUENCY_HZ!
#endif // SYSTICK_LOAD_INIT > (1<<24)

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static systick_callback_t st_callback;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
bool SysTick_Init(systick_callback_t callback) {
	gpioMode(PIN_DEBUG, OUTPUT);
	gpioWrite(PIN_DEBUG, LOW);

	bool init_status = false;
	NVIC_EnableIRQ(SysTick_IRQn);

	if (callback != NULL) {
		SysTick->CTRL = 0x00;			   //Enable sysT interrupt
		SysTick->LOAD = SYSTICK_LOAD_INIT; //00100000L  - 1;
		SysTick->VAL = 0x00;
		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
		st_callback = callback;
		init_status = true;
	}
	return init_status;
}

__ISR__ SysTick_Handler(void)
{
		st_callback();
}