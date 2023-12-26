/***************************************************************************//**
  @file     RGBMatrix.h
  @brief    RGB Matrix header. (CJMCU-8*8)
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _RGBMATRIX_H_
#define _RGBMATRIX_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define HEIGHT	8
#define WIDTH	8

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

typedef color_t LEDMatrix_t[HEIGHT][WIDTH];

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void RGBMatrix_Init();

//col or row -> 0-7
//led RGB -> 0-255
void RGBMatrix_UpdateLED(color_t led, uint8_t col, uint8_t row);

void RGBMatrix_SetBrightness(float brightness);

void RGBMatrix_Clear(void);

void RGBMatrix_Test(void);

/*
 * col -> 0-7
 * percentage -> 0-100
 * linecolor must be a color_t
 */
void VUmeter(uint8_t col, uint8_t percentage, color_t lineColor);
/*******************************************************************************
 ******************************************************************************/

#endif // _RGBMATRIX_H_
