/***************************************************************************//**
  @file     magDriver.c
  @brief    Driver de la lectora de banda magnética.
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "stdio.h"
#include "stdbool.h"
#include "../headers/magDriver.h"
#include "../../MCAL/gpio.h"
#include "../../timer/timer.h"
#include "../board.h"
#include "hardware.h"
#include "MK64F12.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
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
#define FIELDS	0b01101						// Tabla de conversion
#define	ENDS	0b11111						// Ref. ISO 7811
#define PORT_mGPIO 1
#define TIMEOUT_SLEEP		1000			//Tiempo de timeout entre llamAdos, tiempo de bloqueo para lectura, en unidades*5ms
#define CLOCK_PIN_NUMBER	11
#define	ENA_ACTIVE	0						//Enable es activo bajo
#define WRITING_MASK	0b00001				//Mascara utilizada para escribir (mag_data2bits)
#define	NP_MASK			0b01111				//Mascara utilizada para borrar bit de paridad (bits2char)
#define ASCII_CONV		48
typedef struct
{
	char PAN[19];
	char additional_data[8];
}magcard_t;

typedef struct
{
	char bit0:1;
	char bit1:1;
}single_bit_t;

typedef struct
{
	char ch:5;
}single_char_t;
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum {FALSE,TRUE};


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
int  mag_drv_LIVE();
bool mag_data2bits();
bool mag_bitscheck();
void mag_bitstochar();

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static char	final_word[40];					//Aqui se guardara la palabra decodificada y convertida a decimal.
static	single_char_t mag_word[40];			//Aqui se guardara la palabra decodificada en el formato recibido.
static	single_bit_t data[200];				//Aqui se guardan los 400 bits recibidos, estructurados de a singlebits ya que 2x1.
static	bool active_flag=TRUE,word_ready=FALSE, data_ready=FALSE;	//Flags
static	tim_id_t ID_MAG_DRV_LIVE ;
static	tim_id_t ID_MAG_DRV_KILL ;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void mag_drv_INIT()						//"Constructor" del driver. Setea los puertos usados en su modo adecuado e inicia un polling en el pin enable
{
	gpioMode(ENABLE_PIN,INPUT_PULLUP);	//Preparo el pin para el enable.
	gpioMode(CLOCK_PIN,INPUT_PULLUP);	//Preparo el pin para el clock.
	gpioMode(DATA_PIN,INPUT_PULLUP);	//Preparo el pin para data.
	ID_MAG_DRV_LIVE = timerGetId();		//Defino Timer para realizar polling en el pin de input.
	ID_MAG_DRV_KILL = timerGetId();
	timerStart(ID_MAG_DRV_LIVE, TIMER_MS2TICKS(1), 1 , &mag_drv_LIVE);	//Inicio el timer.

}

int mag_drv_LIVE()					//Callback, se llama en caso de que se detecte una señal en enable y se prepara para leer los datos.
{
	if((gpioRead(ENABLE_PIN)==ENA_ACTIVE) & (active_flag==TRUE))
	{
		NVIC_EnableIRQ(PORTC_IRQn);			//Seteo el puerto C como activo para recibir interrupciones
		/////////// Seteo el  CLOCK_PIN (PC13) para recibir interrupciones cada flanco de clock
		PORTC->PCR[CLOCK_PIN_NUMBER]=0x0; //Clear
		PORTC->PCR[CLOCK_PIN_NUMBER]|=PORT_PCR_MUX(PORT_mGPIO); 		       //Set MUX to GPIO
		PORTC->PCR[CLOCK_PIN_NUMBER]|=PORT_PCR_PE(TRUE);          		       //Pull UP/Down  Enable
		PORTC->PCR[CLOCK_PIN_NUMBER]|=PORT_PCR_PS(TRUE);          		       //Pull UP
		PORTC->PCR[CLOCK_PIN_NUMBER]|=PORT_PCR_IRQC(PORT_eInterruptEither);    //Enable Rising edge interrupts
		timerStop(ID_MAG_DRV_LIVE);
		return TIMEOUT_SLEEP;
	}
	else
	{
		return -1;											//Si no hubo enable, no hay timeout
	}

}

__ISR__ PORTC_IRQHandler (void)		//Se copian los datos recibidos a partir del primer 1 (inclusive), bit a bit y en el orden recibido.
{
	static int index=0, half_bit=1;
	static bool	start_sentinel=FALSE;
	if ((index == 0) && (half_bit==1)&&(start_sentinel==FALSE))
	{
		data[index].bit0=1;
		timerStart(ID_MAG_DRV_KILL, TIMER_MS2TICKS(1000), 0 , &mag_read_end);
		timerStop(ID_MAG_DRV_LIVE);
	}
	PORTC->PCR[CLOCK_PIN_NUMBER] |= PORT_PCR_ISF_MASK; 	//Freno la interrupcion.
	if(data_ready==TRUE)
	{
		data_ready=FALSE;					//Seteo data_ready en 0, ya que voy a estar escribiendo una nueva palabra
	}
	if (half_bit == 0)
	{
		data[index].bit0 = gpioRead(DATA_PIN);
		half_bit = 1;
	}
	else if (half_bit == 1)
	{
		data[index].bit1 = gpioRead(DATA_PIN);
		half_bit = 0;
		if((start_sentinel==FALSE)&((data[index].bit1)!=(data[index].bit0)))		//Cuando leo el PRIMER uno
		{
			index=0;
			data[0].bit0=1;									//Lo copio en la primera posicion
			data[0].bit1=0;									//Voy a pisar los primeros ceros
			start_sentinel=TRUE;							//Aviso que empieza la data real.
		}
		index++;
	}
	if	(index == 200)
	{
		index=0;
		half_bit=1;
		start_sentinel=FALSE;
		data_ready=TRUE;
		NVIC_DisableIRQ(PORTC_IRQn);							//
		PORTC->PCR[11]|=PORT_PCR_IRQC(PORT_eDisabled);     		//Disable all interrupts


	}
}
bool mag_get_data_ready()			//Getter del flag que se enciende al haber recibido todos los datos mediante un swipe (mag_drv_LIVE + PORTC_IRQHandler)
{
	return data_ready;
}

char* mag_drv_read()				//Funcion que se espera que se llame cuando se recibio la data y se desea convertir y extraerlos.
{
	if(data_ready==TRUE)
	{
		if(mag_data2bits())				//Data es enmascarado a mag_word.
		{
			//if(mag_bitscheck())
			//{
				mag_bitstochar();
			//}
		}
		word_ready = FALSE;
		data_ready=FALSE;
		return &(final_word[0]);
	}
	else
		return NULL;
}

bool mag_data2bits() //Funcion que convierte los bits recibidos en unidades de 5 bits en su orden correspondiente.
{
	static single_char_t	mask = {WRITING_MASK};		//Arranca en 00001
	static int index = 0, bitcounter=0, word_index=0;
	while (index<200)
	{
		if (data[index].bit0==data[index].bit1)			//En este caso, el dato es un cero
		{
			mask.ch= ~(mask.ch);						//Bit Flipping
			mag_word[word_index].ch&=mask.ch;			// Hago una AND con un solo cero y el resto unos
			mask.ch= ~(mask.ch);						//Bit Flipping
			mask.ch=mask.ch<<1;							// Shifteo para la proxima iteracion
			if(bitcounter==4)							//Si termine de escribir de a 5 bits
			{
				word_index++;
				bitcounter=-1;
				mask.ch = WRITING_MASK;
			}
			bitcounter++;
			index++;

		}
		else if (data[index].bit0!=data[index].bit1)	//En este caso, el dato es un uno
		{
			mag_word[word_index].ch|=(mask.ch);
			mask.ch=mask.ch<<1;
			if(bitcounter==4)
			{
				word_index++;
				bitcounter=-1;
				mask.ch = WRITING_MASK;
			}
			bitcounter++;
			index++;

		}
	}
	if (index==200)									//Si termine de procesar toda la data-
	{
		index=0;
		bitcounter=0;
		word_index=0;
		mask.ch = WRITING_MASK;						//Reseteo variables locales estaticas
		return	TRUE;								//Devuelvo true
	}
	else
	{
		return FALSE;								//Devuelvo falso en caso de fallas.
	}
}

bool mag_bitscheck()				//Funcion que verifica las palabras segun paridad y LRC.
{																			//(es decir, previo a esto se leyo en data y se transformo
	static int col[5];														//Guardo cuantos unos hay por columna (para el LRC)	//a mag_word mediante data2bits)
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
void mag_bitstochar()				//Funcion que convierte las palabras de 5 bits codificadas (con paridades) a su valor ASCII correspondiente.
{
	static single_char_t	temp,mask;
	int index;
	for (index=0, mask.ch=NP_MASK; index<39; index++)		//(~16) = 01111
	{
		temp.ch = (mag_word[index].ch & mask.ch);			//Borro los bits de paridad de todos los datos.
		final_word[index]= ((char)temp.ch + ASCII_CONV);
	}
}


void mag_read_end()					//Funcion que frena las interrupciones de lectura, se llama desde ell timer 1 segundo despues de detectado el enable
{
	NVIC_DisableIRQ(PORTC_IRQn);							//
	PORTC->PCR[11]|=PORT_PCR_IRQC(PORT_eDisabled);     		//Disable all interrupts
	timerStart(ID_MAG_DRV_LIVE, TIMER_MS2TICKS(1), 1 , &mag_drv_LIVE);
}

void mag_set_active()				//Llave de control que habilita el driver. Encendido por default.
{
	active_flag = TRUE;
}

void mag_clear_active()				//Llave de control que deshabilita el driver
{
	active_flag = FALSE;
}
