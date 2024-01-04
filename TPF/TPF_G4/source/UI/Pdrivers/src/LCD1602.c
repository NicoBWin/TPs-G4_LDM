/***************************************************************************//**
  @file     LCD1602.c
  @brief    LCD 1602A driver. (16*2 LCD)
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
  @Link de ayuda: https://community.nxp.com/t5/Kinetis-Software-Development-Kit/Driving-16x2-LCD-using-KSDK-drivers/ta-p/1098903
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main libs
#include <UI/Pdrivers/pines.h>
#include "../headers/PIT.h"

#include "../../MCAL/gpio.h"
#include "../../timer/timer.h"

#include "../headers/LCD1602.h"

// MCU libs
#include "MK64F12.h"
#include "hardware.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define 	BOARD_PIT_INSTANCE  	0

#define LCD_D7_ON	gpioWrite(LCD_D7, HIGH)
#define LCD_D7_OFF	gpioWrite(LCD_D7, LOW)
#define LCD_D6_ON	gpioWrite(LCD_D6, HIGH)
#define LCD_D6_OFF	gpioWrite(LCD_D6, LOW)
#define LCD_D5_ON	gpioWrite(LCD_D5, HIGH)
#define LCD_D5_OFF	gpioWrite(LCD_D5, LOW)
#define LCD_D4_ON	gpioWrite(LCD_D4, HIGH)
#define LCD_D4_OFF	gpioWrite(LCD_D4, LOW)

#define LCD_EN_ON	gpioWrite(LCD_EN, HIGH)
#define LCD_EN_OFF	gpioWrite(LCD_EN, LOW)
#define LCD_RS_ON	gpioWrite(LCD_RS, HIGH)
#define LCD_RS_OFF	gpioWrite(LCD_RS, LOW)
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
const unsigned char  upper_line[] = "   MP3 PLAYER";
const unsigned char  lower_line[] = "   LDM TPF G4";

const unsigned char  initLCD[8]={0x02, 0x28, 0x0C, 0x06, 0x01, 0x00};
// Set 4bit interface -> 2 line mode -> Display On ->   0x06?  -> Display Clear -> No decrement + no shift

volatile long count;

extern volatile long count;
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void delay(unsigned int time);
void enable(void);
void enable2(void);
void lcd_init();
void SetUp ();
void instructionLong(unsigned char x);
void instruction (unsigned char x);
void lcd_data(unsigned char x);
void text (unsigned char *b);
void info(unsigned char x);
void LCD_Pin_Enable(void);


void PITdelayCallback();
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void LCD1602_Init() {
	LCD_Pin_Enable(); 	// Enable pins

	delay(10000);

	SetUp();

	instruction(0x80); //First line
	text((unsigned char *)&upper_line[0]);

	instruction(0xC0); //Second line
	text((unsigned char *)&lower_line[0]);
}

void LCD1602_Clear() {
	instructionLong(0x01); //Clear display
	//instruction(0x0F); //Cursor on blinking
}

// Write text on First line
void LCD1602_W1L(unsigned char* texto1) {
	instruction(0x80); //First line
	text(texto1);
}

// Write text on Second line
void LCD1602_W2L(unsigned char* texto2) {
	instruction(0xC0); //Second line
	text(texto2);
}

void LCD1602_ShiftR() {
	instruction(0x1C);
}

void LCD1602_ShiftL() {
	instruction(0x18);
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void enable(void){
	LCD_EN_ON;
	delay(6000);
	LCD_EN_OFF;
}

void enable2(void){
	LCD_EN_ON;
	delay(3000);
	LCD_EN_OFF;
}

//-----------------------------------------------------------------------
void SetUp(){
	unsigned char a = 0;

	while(initLCD[a]) {
		instructionLong(initLCD[a]);
		a++;
	}
}

//-----------------------------------------------------------------------
void instructionLong(unsigned char x){
	LCD_RS_OFF;

	lcd_data(x&0xF0);
	enable2();

	lcd_data((x<<4)&0xF0);
	enable2();
}

//-----------------------------------------------------------------------
void instruction(unsigned char x){
	LCD_RS_OFF;

	lcd_data(x&0xF0);
	enable();

	lcd_data((x<<4)&0xF0);
	enable();
}

//-----------------------------------------------------------------------
void lcd_data(unsigned char x) {
	//Bit 7
	if (x&0x80)
		LCD_D7_ON;
	else
		LCD_D7_OFF;

	//Bit 6
	if (x&0x40)
	  LCD_D6_ON;
	else
	  LCD_D6_OFF;

	//Bit 5
	if (x&0x20)
	  LCD_D5_ON;
	else
	  LCD_D5_OFF;

	//Bit 4
	if (x&0x10)
	  LCD_D4_ON;
	else
	  LCD_D4_OFF;
}

//-----------------------------------------------------------------------
void text (unsigned char *b){
	while(*b) {
		info(*b);
		b++;
	}
}

//-----------------------------------------------------------------------
void info(unsigned char x) {
	LCD_RS_ON;

	lcd_data( x&0xF0 );
	enable();

	lcd_data( (x<<4)&0xF0 );
	enable();
}

//-----------------------------------------------------------------------
void LCD_Pin_Enable(void) {
	gpioMode(LCD_EN, OUTPUT);
	gpioMode(LCD_RS, OUTPUT);
	gpioMode(LCD_D7, OUTPUT);
	gpioMode(LCD_D6, OUTPUT);
	gpioMode(LCD_D5, OUTPUT);
	gpioMode(LCD_D4, OUTPUT);

	LCD_D7_OFF;
	LCD_D6_OFF;
	LCD_D5_ON;
	LCD_D4_ON;

	LCD_RS_OFF;
	LCD_EN_OFF;


}

//-----------------------------------------------------------------------
static void delay(unsigned int time){
	volatile int i;
	for(i = 0; i < time; i++);

}


// Could not implement with PIT
/*
 	PIT_Init(1, PIT_CH1, false);
	PIT_Stop(PIT_CH1);
	Pit_SetCallback(PIT_CH1, PITdelayCallback);

void PITdelayCallback(){
    count++;
}
 */
