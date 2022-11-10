/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Drivers
#include "drivers/headers/uart.h"
#include "drivers/headers/ADC.h"
#include "drivers/headers/DAC.h"
#include "drivers/headers/FSKd.h"
#include "drivers/headers/circularbuffer.h"

// Timer
#include "timer/timer.h"

// App
#include "App.h"

// Standar libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "drivers/headers/FSKd.h"
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define NUMOFFSET       '0'     // Offset de numero entero a char
#define LENG_SC         4

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void intochar(int16_t num, char chscore[LENG_SC]);
static void send_uart();
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void App_Init(void) {
  timerInit();		// Inicializa timers
  // UART init
  uart_cfg_t config = {.baudrate = UARTBAUDRATE, .parity = EVEN_PARITY_UART};
  uartInit(UARTID_R, config);
  uartInit(UARTID_T, config);
  // Dac init
  DAC_Init(DAC_0);
  // ADC INIT
  ADC_Config_t adcConfig = {.adcN = ADC_0, .resolution = ADC_b16, .cycles = ADC_c16, .divide_select = input_clock, .mux = ADC_mA, .channel = 12}; // Pb2
  ADC_Init (adcConfig);
  ADC_SetInterruptMode (ADC_0, false); // no interruption
  ADC_Start (ADC_0);

  // Initialice seno base
  setup_params(1200, 2200, 50, 0xFFF);

  // Inicializo timers del systick
  tim_id_t DAC_periodicID = timerGetId();
  tim_id_t CHANGE_BIT_periodicID = timerGetId();
  tim_id_t TEST_UART_periodicID = timerGetId();
  tim_id_t ADC_periodicID = timerGetId();
  timerStart(DAC_periodicID, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, buff2DAC_callback); // base de tiempo es de 10u
  timerStart(CHANGE_BIT_periodicID, TIMER_MS2TICKS(83), TIM_MODE_PERIODIC, change_bit_callback);
  timerStart(ADC_periodicID, TIMER_MS2TICKS(6), TIM_MODE_PERIODIC, ADC2BIT_callback); // Frec de sampleo del ADC = 16666,6Hz obteniendo 14 sampples por periodo aprox

  // Test
  timerStart(TEST_UART_periodicID, TIMER_MS2TICKS(830), TIM_MODE_PERIODIC, send_uart);

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {

	  //uint8_t UARTmsg = 0b10110110;


	  //bitstream_modulate(UARTmsg, true);


}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Transforma un entero no signado a un string.
 * @param num Recibe el numero a transformar.
 * @param chscore[] Recibe el string dode transformara el numero a char
 * @return Devulve el string ya transformado.
*/
static void intochar(int16_t num, char chscore[LENG_SC]) {
    unsigned long int a = 0;

    if(num==0) {
        chscore[0]='0';           // Escribo el 0 en el al principio.
    }
    else if (num > 0) {
    	chscore[0]='+';           // Escribo el + si el numero es positivo.
    }
    else {
    	chscore[0]='-';           // Escribo el - si el numero es negativo.
    	num = -num;
    }
	for(int i=LENG_SC-1;i>0;i--) {
		a = num % 10;                   // Tomo un digito a mostrar.
		if(num>0) {
			chscore[i]=a+NUMOFFSET;     // Si sigo teniendo parte del numero disponible para mostrar
										//muestro el nuevo digito.
			num = num / 10;             // Recorto el número para mostrar el nuevo digito.
		}
		else {
			chscore[i]='0';              // Si el numero que queda es = a 0, muestro espacios.
		}
	}
}

static void send_uart()
{
	static char msg = 0b11111111;
	msg = msg<<1;
	if ( msg == 0)
	{
		msg = 0b11111111;
	}
	msg = 11000000;
	uartWriteMsg(UARTID_T, &msg, 1);
}

