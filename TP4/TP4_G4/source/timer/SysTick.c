/***************************************************************************//**
  @file     SysTick.c
  @brief    Systyck configurator
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>

#include "SysTick.h"
#include "../MCAL/gpio.h"
#include "../drivers/board.h"
#include "MK64F12.h"

#include  <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SYSTICK_DEVELOPMENT_MODE    1

#define SYSTICK_LOAD_INIT ((__CORE_CLOCK__ / SYSTICK_ISR_FREQUENCY_HZ) - 1U)
#if SYSTICK_LOAD_INIT > (1 << 24)
#error Overflow de SysTick! Ajustar  __CORE_CLOCK__ y SYSTICK_ISR_FREQUENCY_HZ!
#endif // SYSTICK_LOAD_INIT > (1<<24)

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static systick_callback_t st_callback;
#define ISR_t         void __attribute__ ((interrupt))
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
bool SysTick_Init(systick_callback_t callback) {
	//CPU_CRITICAL_ENTER();
	OS_AppTimeTickHookPtr = G4_SysTick_Handler;
	st_callback = callback;
	//CPU_CRITICAL_EXIT(); //SEEE MANUAL
	return true;
}

void G4_SysTick_Handler(void) {
	#ifdef SYSTICK_DEVELOPMENT_MODE
		gpioWrite(PIN_IRQ, HIGH);
	#endif //SYSTICK_DEVELOPMENT_MODE
	st_callback();
	#ifdef SYSTICK_DEVELOPMENT_MODE
		gpioWrite(PIN_IRQ, LOW);
	#endif //SYSTICK_DEVELOPMENT_MODE
}
