/***************************************************************************/ /**
  @file     template.h
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define TBIT 833	// en micro sec
#define SIZE_BITSTREAM 11	// START + 8Bitdata + PAR + STOP

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief TODO: completar descripcion
 * @param param1 Descripcion parametro 1
 * @param param2 Descripcion parametro 2
 * @return Descripcion valor que devuelve
*/
// +ej: char lcd_goto (int fil, int col);+

void setup_params(uint32_t f1_xd, uint32_t f2_xd, uint16_t resolucion, uint16_t scaling_factor);
uint16_t FSK_get_sample(_Bool bit);
void bitstream_modulate(uint8_t data, _Bool uart_received);
/*******************************************************************************
 ******************************************************************************/

#endif // _TEMPLATE_H_

