/***************************************************************************//**
  @file     car_reader.c
  @brief    Driver lector de tarjetas de banda magnetica
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../headers/card_reader.h"
#include "../../MCAL/gpio.h"
#include "../../timer/SysTick.h"
#include "../../timer/timer.h"
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



/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
//ENUMS Y DEFINICIONES//
enum {FALSE,TRUE};
typedef enum
{
	PORT_eDisabled				= 0x00,
	PORT_eDMARising				= 0x01,
	PORT_eDMAFalling			= 0x02,
	PORT_eDMAEither				= 0x03,
	PORT_eInterruptDisasserted	= 0x08,
	PORT_eInterruptRising		= 0x09,
	PORT_eInterruptFalling		= 0x0A,
	PORT_eInterruptEither		= 0x0B,
	PORT_eInterruptAsserted		= 0x0C,
} PORTEvent_t;

#define ZERO	0b10000
#define UNO		0b00001
#define DOS		0b00010
#define TRES	0b10011
#define CUATRO	0b00100
#define CINCO	0b10101
#define SEIS	0b10110
#define SIETE	0b00111
#define OCHO	0b01000
#define NUEVE	0b11001
#define STARTS	0b01011
#define FIELDS	0b01101
#define	ENDS	0b11111

#define PORT_mGPIO 1

#define TIMEOUT_SLEEP		1000			//Tiempo de timeout entre llamAdos, tiempo de bloqueo para lectura, en unidades*5ms

//FUNCIONES INTERNAS//
int mag_drv_LIVE();
char lettertochar(char* letter);


//Variables globales//
char mag_word[40];
char word_ready=0;
bool active_flag=TRUE;

//Definicion de funciones//

void mag_drv_INIT()
{
	gpioMode(ENABLE_PIN, INPUT_PULLUP);	//Preparo el pin para el enable.
	gpioMode(CLOCK_PIN,INPUT_PULLUP);	//Preparo el pin para el clock.
	gpioMode(DATA_PIN,INPUT_PULLUP);	//Preparo el pin para data.
	systick_t mag_drv_SYST = {.speed = TSAMPLE, .periodic_Flag =TRUE , .funcallback = &mag_drv_LIVE,.kill_process = &mag_read_end };
  SysTick_Init (mag_drv_SYST);
}

int mag_drv_LIVE()
{
	if((gpioRead(ENABLE_PIN)==0) & (active_flag==TRUE))
	{
		//gpioToggle(PIN_LED_GREEN);		//Aca iria el leer la tarjetra y blabla
		NVIC_EnableIRQ(PORTD_IRQn);			//Seteo el puerto D como activo para recibir interrupciones
		/////////// Set PD3 CLOCK_PIN as input and enable interrupt on this pin (SW3) -> Closest to RGB led on K64 board

		PORTD->PCR[3]=0x0; //Clear
		PORTD->PCR[3]|=PORT_PCR_MUX(PORT_mGPIO); 		       //Set MUX to GPIO
		// Now set pin input properties
		PORTD->PCR[3]|=PORT_PCR_PE(1);          		       //Pull UP/Down  Enable
		PORTD->PCR[3]|=PORT_PCR_PS(1);          		       //Pull UP
		// Enable interrupt on this pin (PTD3)
		PORTD->PCR[3]|=PORT_PCR_IRQC(PORT_eDMAEither);     		//Enable Rising edge interrupts
		return TIMEOUT_SLEEP;
	}
	else
	{
		return -1;											//Si no hubo enable, no hay timeout
	}
}


__ISR__ PORTD_IRQHandler (void)
{
// Clear port IRQ flag
	PORT_ClearInterruptFlag (PORTD, 3);	//Freno la interrupcion del pin 3.
	static char letter[5];
	static char	prev_char = 1, cur_char, half_cycle=1;		//Si al momento de llamar, parity esta en 1 es porque con este llamado termino de definir un bit. (usa curcharyprevchar)
	static int	i=0 ,lenght_counter=0;
	if (!half_cycle)
	{
		prev_char = gpioRead(DATA_PIN);
		half_cycle = 0;
	}
	else if (half_cycle)
	{
		cur_char = gpioRead(DATA_PIN);
		if ((prev_char == cur_char) & (i<5))
		{
			letter[i]=0;
			i++;
		}
		else if ((prev_char != cur_char) & (i<5))
		{
			letter[i]=1;
			i++;
		}
		if (i==5)
		{
			i=0;
			mag_word[lenght_counter]=lettertochar(&letter);	//conversion y procesado de la letra, palabra terminada??
			lenght_counter++;
			if (lenght_counter==40)
			{
				mag_read_end();
				word_ready = TRUE;
				prev_char = 1;
				i=0;
				half_cycle=1;
				lenght_counter=0;
			}
		}
	}
}

char* mag_drv_read()
{
	if(word_ready==1)
	{
		word_ready = FALSE;
		return &(mag_word[0]);
	}
	else
		return null;
}

char lettertochar(char* letter) //WIP//
{
	char returnable;
	return returnable;

}
void mag_read_end()
{
	NVIC_DisableIRQ(PORTD_IRQn);							//
	PORTD->PCR[3]|=PORT_PCR_IRQC(PORT_eDisabled);     		//Disable all interrupts
}

void mag_set_active()
{
	active_flag = TRUE;
}

void mag_clear_active()
{
	active_flag = FALSE;
}