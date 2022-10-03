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
#include "MK64F12.h"



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
//enum {SLAVE,MASTER}SPI_mode_t



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
static  SPI_Type* SPI_ptr;
static PORT_Type SPI_PORTS[5];

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void portsetSPI(int PORTn,int pin);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
void SPI_MSTR_init(SPI_t mySPI);//framesize a uint8
//void SPI_transfer_enqueue(int txdata);
//char SPI_read_transfer();
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void SPI_MSTR_init(SPI_t mySPI)
{
	  SIM->SCGC5 |= SIM_SCGC5_PORTD(1);	// Prendo el clock del puerto a configurar
	  SIM->SCGC5 |= SIM_SCGC5_PORTC(1);	// Prendo el clock del puerto a configurar
	  SIM->SCGC6 |= SIM_SCGC6_SPI0(1); //Prendo el clock del periferico SPI2
	  portsetSPI(mySPI.PORT_settings.usePort, mySPI.PORT_settings.CLKpin);  //Seteo el PCR del clock (PD1)
	  portsetSPI(mySPI.PORT_settings.usePort, mySPI.PORT_settings.SINpin);  //Seteo el PCR del SIN (PD3)
	  portsetSPI(mySPI.PORT_settings.usePort, mySPI.PORT_settings.SOUTpin);  //Seteo el PCR del SOUT (PD2)
	  portsetSPI(mySPI.PORT_settings.usePort, mySPI.PORT_settings.CS0pin);  //Seteo el PCR del ChipSelect0 (PD0)
	  SPI_ptr = SPI0;
	  uint32_t* MCR = &SPI_ptr->MCR;
	  uint32_t* CTAR = &SPI_ptr->CTAR[mySPI.CTARn];
	  // inicialmente seteo SPI en INACTIVO para evitar problemas
	  *MCR = 0;
	  *MCR |=SPI_MCR_HALT(1);
	  if(mySPI.mode==MASTER)
	  {
	  //1° Configuro el MCR
	  *MCR |= SPI_MCR_MSTR(1); //Configuro en modo MASTER
	  *MCR |= SPI_MCR_CONT_SCKE(mySPI.CONTCLK); //Continuous clock enable
	  *MCR |= SPI_MCR_PCSIS(mySPI.CSPOL); //Hago que el ChipSelect sea activo bajo.
	  //*MCR &= ~SPI_MCR_DCONF(0b11); //Configuro el modulo en modo SPI
	  //*MCR &= ~SPI_MCR_MDIS(1);    //Activo los clocks del modulo
	  //*MCR &= ~SPI_MCR_DIS_TXF(1); //Aseguro de tener habilitadas transferencias
	  //*MCR &= ~SPI_MCR_DIS_RXF(1); //y recepciones
	  //*MCR &= ~SPI_MCR_FRZ(1); //Desactivo Freeze
  	  //2° Configuro el CTAR

  	  //reg = (reg & ~SPI_CTAR_SLAVE_FMSZ_MASK) | SPI_CTAR_SLAVE_FMSZ(0b1101);

  	  *CTAR = 0;
  	  //*CTAR &= ~SPI_CTAR_DBR(1); //determino si estoy en modo doble baudrate
  	  *CTAR |= SPI_CTAR_FMSZ(mySPI.framesize-1); //Determino el framesize. Tiene un offset de 1.
	  *CTAR |= SPI_CTAR_CPOL(mySPI.CKPOL); //Configuro clock como activo bajo
	  *CTAR |= SPI_CTAR_CPHA(mySPI.CPHA); //Data se cambia en Redge, se lee en Fedge.
	  *CTAR |= SPI_CTAR_LSBFE(mySPI.LSBFE); //determino si primero se manda el LSB (1 si si, 0 si MSB primero)
	//  *CTAR |= SPI_CTAR_PBR(0b11);
	  *CTAR |= SPI_CTAR_BR(0b0111);
	  *CTAR |= SPI_CTAR_ASC(0b0110);
	//  *CTAR |= SPI_CTAR_PASC(0b01);
	  *CTAR |= SPI_CTAR_PCSSCK(0b0000);			//Configuro el desfazaje entre PCS y CLK
	  *CTAR |= SPI_CTAR_CSSCK(0b0110);

  	  //3° Si estoy en modo MASTER, configuro el regitro PUSHR
  	  //SPI_ptr->PUSHR=0;
  	  SPI_ptr->PUSHR |= SPI_PUSHR_CONT(mySPI.CONTCS);
  	  if(mySPI.CTARn==0)
  	  {
  		  SPI_ptr->PUSHR &= ~SPI_PUSHR_CTAS(0b111);
  	  }
  	  else if(mySPI.CTARn==1)
  	  {
  		  SPI_ptr->PUSHR |=SPI_PUSHR_CTAS(0b001);
  	  }
  }

	  else if (mySPI.mode==SLAVE)
	  {

	  }
  	  ////Desactivo el HALT (inicio run)
  	  *MCR &= ~SPI_MCR_HALT(1);
}

void SPI_transfer_enqueue(int txdata, _Bool ACSCONT )	// Recibe los datos a enviar,
//y ademas un flag que indica si el usuario quiere que una vez enviado el mensaje si el CS queda ACtivado o no
{
	SPI_ptr->MCR |=SPI_MCR_HALT(1);
	SPI_ptr->PUSHR = (SPI_ptr->PUSHR | SPI_PUSHR_CONT(ACSCONT) | SPI_PUSHR_PCS(1)) & ~SPI_PUSHR_TXDATA(0b1111111111111111); //Mantengo el CSCONT, pero borro los datos previos del registro
	SPI_ptr->MCR |= SPI_MCR_CLR_TXF(1);
	SPI_ptr->PUSHR = (SPI_ptr->PUSHR | SPI_PUSHR_CONT(ACSCONT) | SPI_PUSHR_PCS(1)) | SPI_PUSHR_TXDATA(txdata);	//escribo los datos en el registro para transmitir.
	SPI_ptr->MCR  &= ~SPI_MCR_HALT(1);
}

void SPI_PCS_dis()
{
	SPI_ptr->MCR |=SPI_MCR_HALT(1);
	SPI_ptr->PUSHR &= ~SPI_PUSHR_PCS(1);
	SPI_ptr->MCR &= ~SPI_MCR_HALT(1);
}
//char SPI_read_transfer() {
//  receivedMSG=0;
//  receivedMSG = SPI_ptr->POPR;
//  return receivedMSG;
//}

void SPI_halt(){

}

void SPI_resumeHalt(){

}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void portsetSPI(int PORTn,int pin) //Recibe un puerto y un pin y setea el pin en modo SPI;
{
	PORT_Type* PORT_ptr;
	if(PORTn==nPORTD)
	{
		PORT_ptr =PORTD;
	}
	if(PORTn==nPORTC)
	{
		PORT_ptr =PORTC;
	}
	PORT_ptr->PCR[pin] &= ~PORT_PCR_LK(0);
	PORT_ptr->PCR[pin] |= PORT_PCR_PE(1);
	PORT_ptr->PCR[pin] |= PORT_PCR_PS(1);
	PORT_ptr->PCR[pin] |= PORT_PCR_MUX(2); //seteo el MUX del pin en modo SPI
}
