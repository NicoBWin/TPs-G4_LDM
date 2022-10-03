/***************************************************************************//**
  @file     SPI_driver.h
  @brief    Driver protocolo SPI
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
typedef enum {nPORTA, nPORTB,nPORTC,nPORTD,nPORTE}nPORTs;
typedef enum{MASTER,SLAVE}SPImodes_t;
typedef struct{
	nPORTs usePort;
	int	CLKpin;
	int	SINpin;
	int SOUTpin;
	int CS0pin;
}SPI_port_setting;

typedef struct {
	SPImodes_t mode;	//Definira el modo de operación.
	int	framesize;		//Definira el tamaño inicial del frame.
	_Bool	LSBFE;		//Definira si el LSB se envia primero o ultimo.
	_Bool	CPHA;		//Definira el orden de escritura/lectura de datos segun los flancos del clock.
	_Bool	CKPOL;		//Definira la polaridad del clock.
	_Bool	CSPOL;
	_Bool	CONTCLK;	//Definira si el chip select es continuo, entre transferencias
	_Bool	CONTCS;		//Definira si el chip select es continuo, entre transferencias
	SPI_port_setting	PORT_settings;
	int		CTARn;


}SPI_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void SPI_MSTR_init(SPI_t mySPI);//framesize a uint8
void SPI_transfer_enqueue(int txdata, _Bool ACSCONT);
char SPI_read_transfer();
void SPI_halt();
void SPI_resumeHalt();
void SPI_PCS_dis();
/*******************************************************************************
 ******************************************************************************/
 #endif // _CARD_READER_H_
