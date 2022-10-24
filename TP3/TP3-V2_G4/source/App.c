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

// Timer
#include "timer/timer.h"

// App
#include "App.h"

// Standar libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
/* Todos los init necesarios */
void App_Init(void) {
 // timerInit();		// Inicializa timers

  // UART init
  //int id = UARTID;
  //uart_cfg_t config = {.baudrate = UARTBAUDRATE};
  //uartInit(id, config);
	Prueba_CMP();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {

	// DO ALGO

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
