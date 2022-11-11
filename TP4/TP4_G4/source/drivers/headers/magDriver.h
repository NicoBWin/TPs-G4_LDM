/***************************************************************************//**
  @file     leds.h
  @brief    Header LEDs
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _MAGDR_H_
#define _MAGDR_H_

//OS
#include <os.h>


#define TSAMPLE	1	//Cada cuanto espero un pulso en enable, en ms

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void mag_drv_INIT(OS_SEM *MagSem);
int mag_set_LIVE();
void mag_read_end();	//Se llama desde la IRQ cuando se termina de leer una palabra, desactiva las interrupciones. (Kill_process)
char* mag_drv_read();	//Devuelve un strinc de todos los campos de datos.
bool mag_get_data_ready();	//Devuelve verdadero si tiene data para procesar, y falso si no.
char* mag_get_ID();		//Devuelve un string del campo de datos PAN
char* mag_get_DATE();	//Devuelve un string del campo de datos adicionales
void mag_clear_active();
void mag_set_active();

/*******************************************************************************
 ******************************************************************************/

#endif // _MAGD_H_
