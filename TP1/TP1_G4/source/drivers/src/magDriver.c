#include "stdio.h"
#include "stdbool.h"
#include "../headers/magDriver.h"
#include "../../MCAL/gpio.h"
#include "../../timer/timer.h"
//#include "SysTick.h"

#include "../board.h"
#include "hardware.h"
#include "MK64F12.h"


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
int  mag_drv_LIVE();
bool mag_data2bits();
bool mag_bitscheck();
void mag_bitstochar();
//
typedef struct{
char	PAN[19];
char	additional_data[8];
}magcard_t;

typedef struct{
	char bit0:1;
	char bit1:1;
}single_bit_t;
typedef struct{
	char ch:5;
}single_char_t;




//Variables globales//
static char	final_word[40];
static single_char_t mag_word[40];
	single_bit_t data[200];				//Aqui se guardan los 400 bits recibidos, estructurados de a singlebits ya que 2x1.
static bool active_flag=TRUE,word_ready=FALSE, data_ready=FALSE;
static tim_id_t ID_MAG_DRV_LIVE ;
static tim_id_t ID_MAG_DRV_KILL ;

//Definicion de funciones//

void mag_drv_INIT()
{
	gpioMode(ENABLE_PIN,INPUT_PULLUP);	//Preparo el pin para el enable.
	gpioMode(CLOCK_PIN,INPUT_PULLUP);	//Preparo el pin para el clock.
	gpioMode(DATA_PIN,INPUT_PULLUP);	//Preparo el pin para data.
	ID_MAG_DRV_LIVE = timerGetId();
	ID_MAG_DRV_KILL = timerGetId();
	timerStart(ID_MAG_DRV_LIVE, TIMER_MS2TICKS(1), 1 , &mag_drv_LIVE);
	//timerStart(ID_MAG_DRV_KILL, TIMER_MS2TICKS(1000), 0 , &mag_read_end);

	//systick_t mag_drv_SYST = {.speed = TSAMPLE, .periodic_Flag =TRUE , .funcallback = &mag_drv_LIVE,.kill_process = &mag_read_end };
	//SysTick_Init (mag_drv_SYST);
}

int mag_drv_LIVE()
{
	if((gpioRead(ENABLE_PIN)==0) & (active_flag==TRUE))
	{
		//gpioToggle(PIN_LED_GREEN);		//Aca iria el leer la tarjetra y blabla
		NVIC_EnableIRQ(PORTB_IRQn);			//Seteo el puerto D como activo para recibir interrupciones
		/////////// Set PD3 CLOCK_PIN as input and enable interrupt on this pin (SW3) -> Closest to RGB led on K64 board

		PORTB->PCR[19]=0x0; //Clear
		PORTB->PCR[19]|=PORT_PCR_MUX(PORT_mGPIO); 		       //Set MUX to GPIO
		// Now set pin input properties
		PORTB->PCR[19]|=PORT_PCR_PE(1);          		       //Pull UP/Down  Enable
		PORTB->PCR[19]|=PORT_PCR_PS(1);          		       //Pull UP
		// Enable interrupt on this pin (PTD3)
		PORTB->PCR[19]|=PORT_PCR_IRQC(PORT_eInterruptEither);     		//Enable Rising edge interrupts

		timerStop(ID_MAG_DRV_LIVE);
		return TIMEOUT_SLEEP;
	}
	else
	{
		return -1;											//Si no hubo enable, no hay timeout
	}

}



__ISR__ PORTB_IRQHandler (void)
{

	// IF I= 0 TERMINO INTERRUPCION PERIODICA CON CALLBACK QUE ACTIVE LA mag_drv_LIVE()
	// ARRANCO NUEVI TIMER
	// TIMER == TERMINO
	static int i=0, half_bit=1;
	if ((i == 0) && (half_bit==1))
	{
		data[i].bit0 = 1;
		timerStart(ID_MAG_DRV_KILL, TIMER_MS2TICKS(1000), 0 , &mag_read_end);
		timerStop(ID_MAG_DRV_LIVE);
	}
	PORTB->PCR[19] |= PORT_PCR_ISF_MASK; 	//Freno la interrupcion del pin 3.
	//gpioToggle(PIN_LED_GREEN);
	if(data_ready==TRUE)
	{
		data_ready=FALSE;					//Seteo data_ready en 0, ya que voy a estar escribiendo una nueva palabra
	}
	if (half_bit == 0)
	{
		data[i].bit0 = gpioRead(DATA_PIN);
		half_bit = 1;
	}
	else if (half_bit == 1)
	{
		data[i].bit1 = gpioRead(DATA_PIN);
		half_bit = 0;
		i++;
	}
	if	(i == 200)
	{
		i=0;
		half_bit=1;
		data_ready=TRUE;
		//mag_read_end();
		// REANUDO LA INTERRUPCION PERIODICA


	}
}
bool mag_get_data_ready()
{
	return data_ready;
}

char* mag_drv_read()
{
	if(data_ready==TRUE)
	{
		if(mag_data2bits())				//Data es enmascarado a mag_word.
		{
		//	if(mag_bitscheck())
		//	{
				mag_bitstochar();
		//	}
		}
		word_ready = FALSE;
		return &(final_word[0]);
	}
	else
		return NULL;
}

bool mag_data2bits() //ya debbugeada
{
	static single_char_t	mask = {0b00001};	//Arranca en 00001
	static int index = 0;
	static char prev_char=1;			//
	while (index<200)
	{
		if (data[index].bit0==data[index].bit1)		//En este caso, el dato es un uno
		{


			mask.ch= ~(mask.ch);						//Bit Flipping
			mag_word[index].ch&=mask.ch;					// Hago una AND con un solo cero y el resto unos
			mask.ch= ~(mask.ch);						//Bit Flipping
			mask.ch=mask.ch<<1;								// Shifteo para la proxima iteracion
			if(!(index%5))
			{
				mask.ch = 0b00001;
			}
			index++;
			//data[index].bit0=0;							// De a poco voy borrando data..
			//data[index].bit1=0;

			// Paso el indice al siguiente dato.
		}
		else if (data[index].bit0!=data[index].bit1)//En este caso, el dato es un uno
		{

			mag_word[index].ch|=(mask.ch);
			mask.ch=mask.ch<<1;
			if(!(index%5))
			{
				mask.ch = 0b00001;
			}
			index++;
			//prev_char=1;							// Aviso que recien fue un uno para evitar doble bit flipping
			//data[index].bit0=0;							// De a poco voy borrando data..
			//data[index].bit1=0;
											// Paso el indice al siguiente dato.
		}
	}
	if (index==200)									//Si termine de procesar toda la data-
	{
		index=0;
		prev_char=1;
		mask.ch = 1;								//Reseteo variables locales estáticas
		return	TRUE;								//Devuelvo true
	}
	else
	{
		return FALSE;								//Devuelvo falso en caso de fallas.
	}
}

bool mag_bitscheck()														//Asumo palabra completa en mag_word
{																			//(es decir, previo a esto se leyo en data y se transformo
	static int col[5];			//Guardo cuantos unos hay por columna (para el LRC)	//a mag_word mediante data2bits)
	static int index=0,shifting=0, ones=0;
	static bool LRC_check,parity_check=TRUE, returnvalue;
	static single_char_t temp, mask={1};
	for (index=0,parity_check=TRUE ; (index<39) & (parity_check==TRUE) ; index++)//Itero sobre los primeros 39 chars de mag_word (dejo afuera LRC)
	{
		for (shifting=0, parity_check=FALSE, ones=0, mask.ch=1; shifting<4; shifting++)//Itero sobre los 4 primeros bits de cada char (shifteo y enmascaro de LSB a MSB).
		{
			temp=mag_word[index];											//Creo una copia para no modificar la variable global.
			temp.ch&=mask.ch;														//Y enmascaro para verificar si hay un uno en la posicion (int)shifting del caracter.
			if (temp.ch>0)
			{
				ones++;														//Aumento el contador de unos para este char
				col[shifting]++;											//Ya que estoy, extraigo la cantidad de unos por columna (par el LRC)
			}
			if (shifting == 3)												//Cuando llego al de paridad..
			{
				mask.ch= mask.ch<<1;
				if((!(ones%2))&(mag_word[index].ch&mask.ch))
					parity_check=TRUE;
			}

			mask.ch= mask.ch<<1;
		}
	}
	//LRC CHECK
	for (ones=0,index=39,shifting=0,LRC_check=TRUE, mask.ch=1; (shifting<4)&(LRC_check==TRUE); shifting++)		//Itero sobre los 4 primeros bits del LRC
	{
		if ((col[shifting]%2)&(mag_word[index].ch&mask.ch))									//Si la columna tenia un numero par de unos, y el de paridad es 1
		{
			ones++;																			//Aumento cantidad de unos en el LRC
			LRC_check=TRUE;																	//Y verifico que se cumple por ahora
		}
		else if ((!(col[shifting]%2))&(!(mag_word[index].ch&mask.ch)))
			LRC_check=TRUE;

		else
			LRC_check=FALSE;

		mask.ch= mask.ch<<1;
	}
	//Parity check for LRC
	if ((!(ones%2))&((mag_word[index].ch&mask.ch)))
		LRC_check=TRUE;
	else if((ones%2)&(!(mag_word[index].ch&mask.ch)))
		LRC_check=TRUE;
	else
		LRC_check=FALSE;


	if (parity_check==TRUE)
	{
		if (LRC_check==TRUE)
		{
			returnvalue = TRUE;
		}

		else
		{
			//Print error de LRC
			returnvalue = FALSE;
		}
	}
	else
	{
		//print error de paridad
		returnvalue = FALSE;
	}

	return returnvalue;
}
void mag_bitstochar()				//Se asume que en mag_word hay una palabra ya checkeada.
{
	static single_char_t	temp,mask;
	static int index;
	for (index=0, mask.ch=0b01111; index<39; index++)		//(~16) = 01111
	{
		temp.ch = (mag_word[index].ch & mask.ch);			//Borro los bits de paridad de todos los datos.
		final_word[index]=(char)temp.ch;
	}

}

void mag_read_end()
{
	NVIC_DisableIRQ(PORTD_IRQn);							//
	PORTB->PCR[19]|=PORT_PCR_IRQC(PORT_eDisabled);     		//Disable all interrupts
	timerStart(ID_MAG_DRV_LIVE, TIMER_MS2TICKS(0.5), 1 , &mag_drv_LIVE);
}

void mag_set_active()
{
	active_flag = TRUE;
}

void mag_clear_active()
{
	active_flag = FALSE;
}


