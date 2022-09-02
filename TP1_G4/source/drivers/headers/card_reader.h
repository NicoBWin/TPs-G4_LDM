/***************************************************************************//**
  @file     card_reader.h
  @brief    Driver lector de tarjetas de banda magnetica
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _CARD_READER_H_
#define _CARD_READER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define TSAMPLE	1	//Cada cuanto espero un pulso en enable, en ms

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void mag_drv_INIT();
int mag_set_LIVE();
char* mag_drv_read();	//Devuelve un strinc de todos los campos de datos.
char* mag_get_ID();		//Devuelve un string del campo de datos PAN
char* mag_get_DATE();	//Devuelve un string del campo de datos adicionales
void mag_clear_active();
void mag_set_active();
void mag_read_end();	//Se llama desde la IRQ cuando se termina de leer una palabra, desactiva las interrupciones. (Kill_process)

/*******************************************************************************
 ******************************************************************************/
 #endif // _CARD_READER_H_