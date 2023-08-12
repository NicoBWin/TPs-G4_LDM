/***************************************************************************/ /**
@file     App.c
@brief    Application functions
@author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Drivers
#include "drivers/headers/FTM.h"
#include "drivers/headers/RGBMatrix.h"
#include "drivers/headers/encoder.h"
#include "drivers/headers/switches.h"
#include "drivers/headers/LCD1602.h"

// Timer
#include "timer/timer.h"

// App
#include "App.h"
#include <os.h>

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
	ONOFF,
	VOLUME
};

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
/* Semaphores */
static OS_SEM EncSem;
static OS_SEM SWSem;

/* Messege Queue */
static OS_Q *AppMQ;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static encResult_t 	encoderState;
static swResult_t 	swState;

static color_t VUColor = {.r=255,.b=0,.g=0};

static const char menu[5]={'M', 'S', 'E', 'O', 'V'};

static OS_ERR app_err;

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

/*******************************************************************************
 * USEFUL FUNCTION PROTOTYPES (FILE LEVEL SCOPE)
 ******************************************************************************/
static void intochar(int16_t num, char chscore[LENG_SC]);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* Todos los init necesarios */
void App_Init(OS_Q *ComQ) {

	timerInit();		// Inicializa timers

	OSSemCreate(&EncSem, "Enc Sem", 0u, &app_err);
	ENC_Init(&EncSem);		// Inicializa encoder

	OSSemCreate(&SWSem, "SW Sem", 0u, &app_err);
	SW_Init(&SWSem);		// Inicializa encoder

	RGBMatrix_Init();
	RGBMatrix_Test();

	LCD1602_Init();

	AppMQ = ComQ;
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
	RGBMatrix_Clear();
	RGBMatrix_SetBrightness(50.0);
	LCD1602_Clear();

	encResult_t joystick_input = ENC_NONE; // Variable que recibe los estados del encoder
	swResult_t switches_input = SW_NONE; // Variable que recibe los estados del encoder

	static int status = MENU;
	static int next_status = MENU;

	static uint8_t vol = 10;

	static uint8_t index;

	//For multi Pend
	OS_PEND_DATA pend_data_tbl[2];

	while (1) {
		// Maquina de estados avanzado
		do {
			status = next_status;
			switch (status) {
				case MENU:
					LCD1602_Clear();
					encoder_control(&index, joystick_input, &next_status);
					switch_control(switches_input, &next_status);

					if(index == 5)
						index = 0;
					if(index == 255) //If it is -1
						index = 4;

					printMenuLCD(index);
				break;

				case SONGS:
					// Ir al menu de canciones
					switch_control(switches_input, &next_status);
					printSongsLCD();
				break;

				case EQUALIZER:
					// Ecualizar las bandas
					switch_control(switches_input, &next_status);
					printEqLCD();
				break;

				case ONOFF:
					// Apagar los display y poner el modo bajo consumo
					switch_control(switches_input, &next_status);
					printOnOffLCD();
				break;

				case VOLUME:
					encoder_control(&vol, joystick_input, &next_status);
					switch_control(switches_input, &next_status);

					if(vol == 21)
						vol = 20;
					if(vol == 255) //If it is -1
						vol = 0;
					printVolLCD(vol*5);
				break;

				default:
				  break;
			}
			switches_input = SW_NONE;
			joystick_input = ENC_NONE;
		}
		while( status != next_status );

	    pend_data_tbl[0].PendObjPtr = (OS_PEND_OBJ *) &EncSem;
	    pend_data_tbl[1].PendObjPtr = (OS_PEND_OBJ *) &SWSem;
	    OSPendMulti(&pend_data_tbl[0], 2, 0, OS_OPT_PEND_BLOCKING, &app_err);

		// Se comunica con el encoder para saber si se acciono y que es lo que se acciono
		if(encGetStatus())
			encoderState = encGetEvent();	// Cambio el encoder
		else
			encoderState = ENC_NONE;        // El usuario no realizó movimiento

		if(swGetStatus())
			swState = swGetEvent();
		else
			swState = SW_NONE;

		joystick_input = encoderState;
		switches_input = swState;
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
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
		else if(menu[(*index)] == 'O') {
			*status = ONOFF;
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
	else if (switches_input == SW_ONOFF)
		*status = ONOFF;
	else if (switches_input == SW_VOL)
		*status = VOLUME;
	else if (switches_input == SW_PLAY) {
		*status = SONGS;
		// PONER PLAY O PAUSAR LA MUSICA
	}
	else if (switches_input == SW_LEFT) {
		*status = SONGS;
		// PONER LA CANCIÓN ANTERIOR
	}
	else if (switches_input == SW_RIGHT) {
		*status = SONGS;
		// PONER LA CANCIÓN SIGUIENTE
	}
}

static void printMenuLCD(uint8_t index) {
	const unsigned char  menu_text[] = 	 "      MENU      ";
	const unsigned char  songs_text[] =  "     SONGS      ";
	const unsigned char  eq_text[] = 	 "   EQUALIZER    ";
	const unsigned char  onoff_text[] =  "    ON / OFF    ";
	const unsigned char  volume_text[] = "     VOLUME     ";
	const unsigned char  arrows_text[] = "<              >";
	//const unsigned char  TEST[] =		 "________________";

	if(menu[index] == 'M')
		LCD1602_W1L(&menu_text);
	else if (menu[index] == 'S')
		LCD1602_W1L(&songs_text);
	else if (menu[index] == 'E')
		LCD1602_W1L(&eq_text);
	else if (menu[index] == 'O')
		LCD1602_W1L(&onoff_text);
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


static void printOnOffLCD(){
	const unsigned char text1[] = "  BAJO CONSUMO";
	LCD1602_Clear();
	LCD1602_W1L(&text1);
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
