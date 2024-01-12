/***************************************************************************//**
  @file     RGBMatrix.c
  @brief    RGB Matrix driver. (CJMCU-8*8)
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main libs
#include "../headers/DMA.h"
#include "../headers/FTM.h"
#include "../headers/PIT.h"

#include "../../timer/timer.h"
#include "../headers/RGBMatrix.h"

// MCU libs
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define LED_BITS 24
#define PMW_ARRAY_LEN	HEIGHT*WIDTH*LED_BITS
#define PMW_ARRAY_LEN_PLUSRES	HEIGHT*WIDTH*LED_BITS+48

#define MAX_BRIGHTNESS	(float)100.0

/*#define TrFREC	(uint16_t) 61	// 125ns periodo @ 50Mhz
#define PWM0	(uint16_t) 20	// 33% de DC
#define PWM1 	(uint16_t) 40	// 65% de DC */

#define TrFREC	(uint16_t) 74	// 125ns periodo @ 60Mhz
#define PWM0	(uint16_t) 25	// 33% de DC
#define PWM1 	(uint16_t) 49	// 65% de DC

#define GREEN_BITS 0
#define RED_BITS	1
#define BLUE_BITS	2
#define BIT_SYMBOL(brightness, i)	(((brightness >> (7 - i%8) & 0x01) == 0) ? PWM0 : PWM1)

#define RESET_TICKS		TIMER_MS2TICKS(10)
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef uint16_t PWM_Array_t[PMW_ARRAY_LEN_PLUSRES];

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static PWM_Array_t pwmMatrix;
static LEDMatrix_t rgbMatrix;

static tim_id_t ResetCode_Tim;

static float matrix_brightness = 100.0;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static void Matrix_RGB2PWM();

//Callbaks
static void RGBMatrix_Reset();
static void RGBMatrix_Restart();

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void RGBMatrix_Init() {
	//ResetCode_Tim = timerGetId(); // If you want to use timers

	// Matrix array init
	RGBMatrix_Clear();
	for(int i=PMW_ARRAY_LEN; i<PMW_ARRAY_LEN_PLUSRES; i++){
		pwmMatrix[i]=1;
	}

	// PWM Config -> DO NOT USE FTM0 & CH5!
	FTMConfig_t FTMConfigPWM = {.channel=FTM_Channel_0, .mode=FTM_mPWM, .prescale=FTM_PSC_x1, .CLK_source=FTM_SysCLK,
						  .PWM_logic=FTM_High, .modulo=TrFREC, .PWM_DC=PWM0, .active_low=false, .DMA_on=true, .interrupt_on=true};
	FTM_Init (FTM_0, FTMConfigPWM); // PTC,1

	// DMA Config
	DMA_config_t DMAConfigOutput = {.source_buffer = (uint16_t*)pwmMatrix, .destination_buffer = &(PWM0_DC_MEM),
			 	 	 	 	 	 .request_source = FTM0CH0, .source_offset = sizeof(uint16_t), .destination_offset = 0x00,
								 .transfer_bytes = sizeof(uint16_t), .major_cycles = (PMW_ARRAY_LEN_PLUSRES), .wrap_around = sizeof(pwmMatrix)};

	DMA_Init(DMA_0, DMAConfigOutput);
	DMA_SetCallback(DMA_0, RGBMatrix_Reset);

	PIT_Init(600000, PIT_CH0, false); // Refresh each 10us
	//PIT_Stop(PIT_CH0);
	PIT_TIEen(PIT_CH0);
	Pit_SetCallback(PIT_CH0, RGBMatrix_Restart);

	FTM_start(FTM_0);
}

void RGBMatrix_UpdateLED(color_t led, uint8_t col, uint8_t row) {
	rgbMatrix[row][col] = led;
	Matrix_RGB2PWM();
}

void RGBMatrix_SetBrightness(float brightness) {
	matrix_brightness = (brightness > MAX_BRIGHTNESS) ? MAX_BRIGHTNESS : ((brightness < 0) ? 0 : brightness);
	Matrix_RGB2PWM();
}

void RGBMatrix_Clear(void) {
	color_t ledOff = {.r=0,.b=0,.g=0};
	for(int i=0;i<8;i++){
		for(int j=0;j<8;j++){
			rgbMatrix[i][j] = ledOff;
		}
	}
	Matrix_RGB2PWM();
}

void RGBMatrix_Test(void){
	color_t ledOn = {.r=255,.b=255,.g=255};
	RGBMatrix_SetBrightness(50.0);	// MAX CURRENT TESTED 1.25Amps @ 100.0 Brightness
	for(int i=0;i<HEIGHT;i++){
		for(int j=0;j<WIDTH;j++){
			rgbMatrix[i][j] = ledOn;
		}
	}
	Matrix_RGB2PWM();
}

void VUmeter(uint8_t col, uint8_t percentage, color_t lineColor) {
	color_t ledOFF = {.r=0,.b=0,.g=0};

	if(col > 7)
		col = 7;	// Just in case

	uint8_t max =(uint8_t) ((percentage * HEIGHT) / 100);

	for(int j=0; j<max; j++){
		rgbMatrix[j][col] = lineColor;
	}
	for(int i=max; i<HEIGHT; i++){
		rgbMatrix[i][col] = ledOFF;
	}
	Matrix_RGB2PWM();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void Matrix_RGB2PWM() {
	uint8_t color_checker = GREEN_BITS;

	uint8_t LEDCounter = 0;

	for(uint8_t x = 0; x < HEIGHT; x++){
		for(uint8_t y = 0; y < WIDTH; y++){
			uint8_t red_brightness = (uint8_t)((float)rgbMatrix[x][y].r * (matrix_brightness / MAX_BRIGHTNESS));
			uint8_t green_brightness = (uint8_t)((float)rgbMatrix[x][y].g * (matrix_brightness / MAX_BRIGHTNESS));
			uint8_t blue_brightness = (uint8_t)((float)rgbMatrix[x][y].b * (matrix_brightness / MAX_BRIGHTNESS));

			// Sets pwm for each RGB bit of each LED
			for(uint8_t i = 0; i < LED_BITS; i++){
				switch(color_checker){
					case GREEN_BITS:
						pwmMatrix[(LEDCounter * LED_BITS) + i] = BIT_SYMBOL(green_brightness, i);
						break;
					case RED_BITS:
						pwmMatrix[(LEDCounter * LED_BITS) + i] = BIT_SYMBOL(red_brightness, i);
						break;
					default:
						pwmMatrix[(LEDCounter * LED_BITS) + i] = BIT_SYMBOL(blue_brightness, i);
						break;
				}
				if(((i + 1) % 8 == 0) && (i != 0)){
					color_checker++;
					if(color_checker > BLUE_BITS){
						color_checker = GREEN_BITS;
					}
				}
			}
			LEDCounter++;
		}
	}
}

// Future improvement -> USE PIT!
static void RGBMatrix_Reset() {
	FTM_stop(FTM_0);
	PIT_Start(PIT_CH0);
	//timerStart(ResetCode_Tim, RESET_TICKS, TIM_MODE_SINGLESHOT, RGBMatrix_Restart); // If you want to use timers
}

static void RGBMatrix_Restart() {
	FTM_startWDuty(FTM_0, *((uint16_t*)pwmMatrix));
	//FTM_start(FTM_0);
}
