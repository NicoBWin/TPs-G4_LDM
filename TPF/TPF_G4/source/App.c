/***************************************************************************/ /**
@file     App.c NO RTOS
@brief    Application functions
@author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// User Drivers
#include "UI/Pdrivers/headers/encoder.h"
#include "UI/Pdrivers/headers/switches.h"
#include "UI/Pdrivers/headers/RGBMatrix.h"
#include "UI/Pdrivers/headers/LCD1602.h"

// Peripheral Drivers
#include "UI/Pdrivers/headers/DMA.h"
#include "UI/Pdrivers/headers/FTM.h"
#include "UI/Pdrivers/headers/PIT.h"
#include "UI/Pdrivers/headers/DAC.h"

// Timer
#include "sound.h"

// Sound
#include "UI/timer/timer.h"

// Standar lib
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define NUMOFFSET       '0'     // Offset de numero entero a char
#define LENG_SC         4

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum status {  //estados de la interfaz principal
	MENU,
	SONGS,
	EQUALIZER,
	PLAY,
	PAUSE,
	VOLUME
};

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static encResult_t 	encoderState;
static swResult_t 	swState;

static color_t VUColor = {.r=255,.b=0,.g=0};

static const char menu[4]={'M', 'S', 'E', 'V'};

static uint8_t	mp3_files[50][15];    //to save file names -> 50 songs + 15 letter name
static uint8_t 	mp3_total_files;
static int mp3_file_index = 0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void encoder_control(uint8_t *index, encResult_t joystick_input, int *status);
static void switch_control(swResult_t switches_input, int *status);

static void printMenuLCD(uint8_t index);
static void printVolLCD(uint8_t volume);

static void printOnOffLCD();
static void printEqLCD();
static void printSongsLCD();
static void printPauseLCD();

static int equalizer_control(encResult_t joystick_input);
static void printEQLCD(int N_Frequency, char Attenuation, int equalizer_status);

/*******************************************************************************
 * USEFUL FUNCTION PROTOTYPES (FILE LEVEL SCOPE)
 ******************************************************************************/
static void INIT(void);

static void intochar(int16_t num, char chscore[LENG_SC]);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* Todos los init necesarios */
void App_Init() {

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {

	INIT();
	mp3_total_files = SD_ReadSongs(mp3_files);

	RGBMatrix_SetBrightness(40.0);


	encResult_t joystick_input = ENC_NONE; // Variable que recibe los estados del encoder
	swResult_t switches_input = SW_NONE; // Variable que recibe los estados del encoder

	static int status = MENU;
	static int next_status = MENU;

	static uint8_t vol = 10;

	static uint8_t index;



	// Maquina de estados NO RTOS
	while (1) {
		// Se comunica con el encoder para saber si se acciono y que es lo que se acciono
		if(encGetStatus())
			encoderState = encGetEvent();	// Cambio el encoder
		else
			encoderState = ENC_NONE;        // El usuario no realizó movimiento
		joystick_input= encoderState;

		// Se comunica con la botonera para saber si se acciono y que es lo que se acciono
		if(swGetStatus())
			swState = swGetEvent();
		else
			swState = SW_NONE;
		switches_input = swState;

		encoder_control(&index, joystick_input, &next_status);
		switch_control(switches_input, &next_status);
		status = next_status;
		if(status!=PAUSE){
			resumeSound();
			play_file(mp3_files[mp3_file_index], vol);
		}


		// Maquina de estados avanzado
		if(switches_input != SW_NONE || joystick_input != ENC_NONE) {
			switch (status) {
				case MENU:
					LCD1602_Clear();

					if(index < 0)
						index = 0;
					if(index > 3) //If it is -1
						index = 3;

					RGBMatrix_Clear();


					printMenuLCD(index);
				break;

				case SONGS:
					// Ir al menu de canciones
					printSongsLCD();
				break;

				case PLAY:
					// Mostrar la cancion que está sonando
					RGBMatrix_Clear();
					VUmeter(5, 70, VUColor);
					RGBMatrix_UpdateLED(VUColor, 4, 3);
					RGBMatrix_UpdateLED(VUColor, 4, 1);
					RGBMatrix_UpdateLED(VUColor, 3, 2);

					printSongsLCD();
				break;

				case PAUSE:
					// Indicar que está en pausa la canción
					RGBMatrix_Clear();
					VUmeter(2, 100, VUColor);
					VUmeter(5, 100, VUColor);

					pauseSound();
					printPauseLCD();
				break;

				case EQUALIZER:
					// Ecualizar las bandas
					next_status = equalizer_control(joystick_input);
					//printEqLCD();
				break;

				case VOLUME:
					if(encoderState == ENC_RIGHT && vol <= 19)
						vol++;
					if(encoderState == ENC_LEFT && vol >= 1)
						vol--;
					printVolLCD(vol*5);
				break;

				default:
				  break;
			}
			switches_input = SW_NONE;
			joystick_input = ENC_NONE;
		}
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void INIT(void){
	// Init de Sound
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	SYSMPU_Enable(SYSMPU, false);
	LED_BLUE_INIT(1);

	timerInit();		// Inicializa timers

	encInit();		// Inicializa encoder

	SW_Init();		// Inicializa encoder

    RGBMatrix_Init();
	RGBMatrix_Clear();

	LCD1602_Init();
}

static void encoder_control(uint8_t *index, encResult_t joystick_input, int *status) {
	if (joystick_input == ENC_NONE)
		return;
	else if(joystick_input == ENC_CLICK){
		if(menu[(*index)] == 'S') {
			*status = SONGS;
		}
		else if(menu[(*index)] == 'E') {
			*status = EQUALIZER;
		}
		else if(menu[(*index)] == 'V') {
			*status = VOLUME;
		}
		else if(*status == VOLUME) {
			*status = MENU;
		}
	}
	else if(joystick_input == ENC_RIGHT)
		(*index)++;
	else if(joystick_input == ENC_LEFT)
		(*index)--;
}

static void switch_control(swResult_t switches_input, int *status){
	if (switches_input == SW_NONE)
		return;
	else if (switches_input == SW_MENU)
		*status = MENU;
	else if (switches_input == SW_PAUSE)
		*status = PAUSE;
	else if (switches_input == SW_VOL)
		*status = VOLUME;
	else if (switches_input == SW_PLAY) {
		// PONER PLAY O PAUSAR LA MUSICA
		*status = PLAY;
	}
	else if (switches_input == SW_LEFT) {
		//*status = SONGS;

		mp3_file_index--;
		if(mp3_file_index < 0 ) {
			mp3_file_index = mp3_total_files - 1;
		}
		// PONER LA CANCIÓN ANTERIOR O LA ULTIMA
	}
	else if (switches_input == SW_RIGHT) {
		//*status = SONGS;
		mp3_file_index++;
		if(mp3_file_index >= mp3_total_files ) {
			mp3_file_index = 0;
		  }
		// PONER LA CANCIÓN SIGUIENTE O LA PRIMERA
	}
}

static void printMenuLCD(uint8_t index) {
	const unsigned char  menu_text[] = 	 "      MENU      ";
	const unsigned char  songs_text[] =  "     SONGS      ";
	const unsigned char  eq_text[] = 	 "   EQUALIZER    ";
	const unsigned char  volume_text[] = "     VOLUME     ";
	const unsigned char  arrows_text[] = "<              >";
	//const unsigned char  TEST[] =		 "________________";

	if(menu[index] == 'M')
		LCD1602_W1L(&menu_text);
	else if (menu[index] == 'S')
		LCD1602_W1L(&songs_text);
	else if (menu[index] == 'E')
		LCD1602_W1L(&eq_text);
	else if (menu[index] == 'V')
		LCD1602_W1L(&volume_text);

	LCD1602_W2L(&arrows_text);
}

static void printVolLCD(uint8_t vol){
	LCD1602_Clear();
	const unsigned char  volume_text[] = "     VOLUME     ";
	unsigned char  volume_val[] =  		 "      100%      ";
	//const unsigned char  TEST[] =		 "________________";

	char charVol[4];
	intochar(vol, charVol);

	for(int i=1; i<4; i++){
		volume_val[i+5] = charVol[i];
	}

	LCD1602_W1L(&volume_text);
	LCD1602_W2L(&volume_val);
}

static void printEqLCD(){
	const unsigned char text2[] = "ECUALIZANDO...";
	LCD1602_Clear();
	LCD1602_W1L(&text2);
}

static void printSongsLCD(){
	const unsigned char text3[] = "CANCIONES...";
	LCD1602_Clear();
	LCD1602_W1L(&text3);
}

static void printPauseLCD(){
	const unsigned char text3[] = "PAUSA...";
	LCD1602_Clear();
	LCD1602_W1L(&text3);
}

/*********************** ECUALIZADOR **********************/
static int equalizer_control(encResult_t joystick_input){
	static int first_click = 0;
	static int equalizer_status = MENU;
	static uint8_t index_Equalizer = 0;
	static char menu_equalizer[]={'0','0','0','0','S'};
	if(first_click == 1)
	{
		if (joystick_input == ENC_NONE) // SI no clickea que siga en la funcion
			return EQUALIZER;
		else if(joystick_input == ENC_CLICK)
		{// CLICK
			if ( equalizer_status == MENU) // Si esta en el MENU del equalizar, moverse entre frecuencias
			{
				if (index_Equalizer == 4) // Si Seteo e equalizer
				{
					if ( menu_equalizer[0] == '0' && menu_equalizer[1] == '0' && menu_equalizer[2] == '0' && menu_equalizer[3] == '0')
					{
						// Desactivar equalizador
					}
					else
					{
						// Activar Equalizador
					}
					return MENU;
				}
				else // Click para modificar la atenuación de una frecuencia
				{
					equalizer_status = EQUALIZER;

				}

			}
			else if(equalizer_status == EQUALIZER) // Si ya estaba modificando una atenuación, que vuelva al menu del equalizer
			{
				equalizer_status = MENU;
			}
		}
		else if(joystick_input == ENC_RIGHT)
		{
			if(equalizer_status == MENU)
			{
				if (index_Equalizer == 4 )
					index_Equalizer = 0;
				else
					index_Equalizer++;
			}
			else if (equalizer_status == EQUALIZER)
			{
				if(menu_equalizer[index_Equalizer] == '9')
				{
					menu_equalizer[index_Equalizer] = '0';
				}
				else
				{
					menu_equalizer[index_Equalizer]+= 3;
				}
			}
		}
		else if(joystick_input == ENC_LEFT)
		{
			if(equalizer_status == MENU)
			{
				if (index_Equalizer == 0 )
					index_Equalizer = 4;
				else
					index_Equalizer--;
			}
			else if (equalizer_status == EQUALIZER)
			{
				if(menu_equalizer[index_Equalizer] == '0')
				{
					menu_equalizer[index_Equalizer] = '4';
				}
				else
				{
					menu_equalizer[index_Equalizer]--;
				}
			}
		}
	}
	else
	{
		first_click = 1;

	}
	printEQLCD(index_Equalizer, menu_equalizer[index_Equalizer], equalizer_status);
	return EQUALIZER;
}

static void printEQLCD(int N_Frequency, char Attenuation, int equalizer_status){
	LCD1602_Clear();

	unsigned char  Set_text[] = "<    SET EQ    >";
	unsigned char  Att_val[] =  		 "      0dB       ";
	unsigned char  arrows_text[] = "<              >";
	unsigned char  Blank[] =		 "                ";
	if (N_Frequency == 4 )
	{
		LCD1602_W1L(&Set_text);
		LCD1602_W2L(&Blank);
	}
	else
	{
		if (N_Frequency == 3)
			if(equalizer_status == MENU)
				LCD1602_W1L("<   F=4000Hz   >");
			else
				LCD1602_W1L("    F=4000Hz    ");
		else if (N_Frequency == 1)
			if(equalizer_status == MENU)
				LCD1602_W1L("<   F=500Hz    >");
			else
				LCD1602_W1L("    F=500Hz     ");
		else if (N_Frequency == 2)
			if(equalizer_status == MENU)
				LCD1602_W1L("<   F=1500Hz   >");
			else
				LCD1602_W1L("    F=1500Hz    ");
		else if (N_Frequency == 0)
			if(equalizer_status == MENU)
				LCD1602_W1L("<   F=150Hz    >");
			else
				LCD1602_W1L("    F=150Hz     ");
		Att_val[6] = Attenuation;
		if(equalizer_status == EQUALIZER)
		{
			Att_val[0] = '<';
			Att_val[15] = '>';
		}
		LCD1602_W2L(&Att_val);
	}
}

/*******************************************************************************
 *******************************************************************************
                        USEFUL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Transforma un entero no signado a un string.
 * @param num Recibe el numero a transformar.
 * @param chscore[] Recibe el string dode transformara el numero a char 157 -> [+157]
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
