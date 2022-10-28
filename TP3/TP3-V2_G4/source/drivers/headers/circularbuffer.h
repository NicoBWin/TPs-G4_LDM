/*
  @file     DAC.h
  @brief    DAC Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 **/
#ifndef _CIRCULARBUFFERH
#define _CIRCULARBUFFERH

/**
 * INCLUDE HEADER FILES
 **/
#include "hardware.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/***
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 **/
#define BITS_PER_SAMPLE 13
#define MAX 2 * BITS_PER_SAMPLE
/***
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 **/
void insert(uint16_t item, int arr);

uint16_t obtain(int arr);

void display();
void buff2DAC_callback ();
void ADC2BIT_callback ();
void change_bit_callback ();
/**
 **/

#endif / void _DACH */
