/***************************************************************************//**
  @file     template.c
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

// +Incluir el header propio (ej: #include "template.h")+
#include "../headers/SPI_driver.h"
#include "../MK64F12.h"



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define PORTde  3


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
#enum typedef{SLAVE,MASTER}SPI_mode_t



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: unsigned int anio_actual;+


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static void falta_envido (int);+


/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void SPI_init(SPI_mode_t mode, uint8 framesize, bool LSBFE, bool CPOL, bool CPHA)
{
  SIM->SCGC5 |= SIM_SCGC5_PORTD(1);	// Prendo el clock del puerto a configurar
  SIM->SCGC3 |= SIM_SCGC3_SPI2(1); //Prendo el clock del periferico SPI2
  port_pointer->PCR[PIN2NUM(pin)] &= ~(1<<PORT_PCR_LK_SHIFT);	// Desactiva el LOCK
	port_pointer->PCR[PIN2NUM(pin)] &= ~(3<< MUX_SHIFT_0);		// Configuro los ceros de mux
	port_pointer->PCR[PIN2NUM(pin)] |=  PORT_PCR_MUX(2);		// Configuro el 1 del mux para hacerlo gpio
  portsetSPI(PORTde, 12);  //Seteo el PCR del clock (PD12)
  portsetSPI(PORTde, 14);  //Seteo el PCR del SIN (PD14)
  portsetSPI(PORTde, 13);  //Seteo el PCR del SOUT (PD13)
  portsetSPI(PORTde, 11);  //Seteo el PCR del ChipSelect0 (PD11)
  portsetSPI(PORTde, 15);  //Seteo el PCR del ChipSelect0 (PD15)
  (SPI_Type*)SPI_ptr = SPI2;
  // inicialmente seteo SPI en INACTIVO para evitar problemas
  SPI2->MCR|=SPI_MCR_HALT(1);
  //1° Configuro el MCR
  SPI2->MCR|= SPI_MCR_MSTR(1); //Configuro en modo MASTER
  SPI2->MCR|= SPI_MCR_CONT_SCKE(1); //Continuous clock enable
  SPI2->MCR&= ~SPI_MCR_DCONF(00); //Configuro el modulo en modo SPI
  SPI2->MCR|= SPI_MCR_PCSIS(1); //Hago que el ChipSelect sea activo bajo.
  SPI2->MCR&= ~SPI_MCR_MDIS(0);    //Activo los clocks del modulo
  SPI2->MCR&= ~SPI_MCR_DIS_TXF(0); //Aseguro de tener habilitadas transferencias
  SPI2->MCR&= ~SPI_MCR_DIS_RXF(0); //y recepciones
  SPI2->MCR&= ~SPI_MCR_FRZ(0); //Desactivo Freeze
  //2° Configuro el CTAR
  SPI2->CTAR&= ~SPI_CTAR_DBR(0); //determino si estoy en modo doble baudrate
  SPI2->CTAR|=SPI_CTAR_FMSZ(framesize-1); //Determino el framesize. Tiene un offset de 1.
  SPI2->CTAR|=SPI_CTAR_CPOL(1); //Configuro clock como activo bajo
  SPI2->CTAR|=SPI_CTAR_CPHA(1); //Data se cambia en Redge, se lee en Fedge.
  SPI2->CTAR|=SPI_CTAR_LSBFE(LSBFE); //determino si primero se manda el LSB (1 si si, 0 si MSB primero)
  //3° Si estoy en modo MASTER, configuro el regitro PUSHR
  SPI2->PUSHR |= SPI_PUSHR_CONT(1);

  ////Desactivo el HALT (inicio run)
  SPI2->MCR&=~SPI_MCR_HALT(0); 
}

void SPI_transfer_enqueue(int txdata)
{
  SPI2->PUSHR |= SPI_PUSHR_TXDATA(txdata); //escribo los datos en el registro para transmitir.
}

uint32_t SPI_read_transfer()
{
  SPI2->SPI2_POPR;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void portsetSPI(PORTn, pin) //Recibe un puerto y un pin y setea el pin en modo SPI;
{
  (PORT_Type*)PORT_ptr=PORT_BASE_PTRS[PORTn];
  PORT_ptr->PCR[pin]|= PORT_PCR_MUX(2); //seteo el MUX del pin en modo SPI
}
