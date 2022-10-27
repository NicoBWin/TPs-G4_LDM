/*
  @file     template.h
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 */
#ifndef _FILTERH
#define _FILTERH

/**
 * INCLUDE HEADER FILES
 **/

#include <stdio.h>
#include <stdint.h>

/***
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 **/


/***
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 **/



/***
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 **/

// +ej: extern unsigned int anio_actual;+


/***
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 **/

/* @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
// +ej: char lcd_goto (int fil, int col);+

double delay_filter(uint16_t sampless);
int comparator( double* floatOutput);

#endif // _TEMPLATEH
