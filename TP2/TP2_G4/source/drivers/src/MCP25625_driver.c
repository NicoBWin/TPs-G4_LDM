/***************************************************************************//**
  @file     template.c
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

// +Incluir el header propio (ej: #include "template.h")+
#include "MCP25625_driver.h"
#include "SPI_driver.h"
#include "MK64F12.h"
#include <stdbool.h>



/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void MCP_init(){
SPI_t mySPI = {.mode=MASTER, .framesize=8,	.LSBFE=false, .CPHA=false,
		.CKPOL=false, .CSPOL=true, .CONTCLK=false, .CONTCS=true, .CTARn=0};
mySPI.PORT_settings.usePort=nPORTD;
mySPI.PORT_settings.CS0pin=0;
mySPI.PORT_settings.CLKpin=1;
mySPI.PORT_settings.SOUTpin=2;
mySPI.PORT_settings.SINpin=3;

//Definidos mis parametros,, llamo a SPI INIT
SPI_MSTR_init(mySPI);
//ACA defino la secuencia de inicializacion del MCP segun ppt
// 1° RESETEO el controlador
SPI_transfer_enqueue(MCP_INST_RESET, false);
// 2° Configuro el tiempo de bit
char CNF1=0,CNF2=0,CNF3=0;
CNF1=(MCP_CNF1_SJW(0b01)|MCP_CNF1_BRP(0b10001));
CNF2=(MCP_CNF2_BTL(1)|MCP_CNF2_SAM(1)|MCP_CNF2_PHSEG1(0b010)|MCP_CNF2_PRSEG2(0)|MCP_CNF2_PRSEG1(0)|MCP_CNF2_PRSEG0(0));
CNF3=(MCP_CNF3_SOF(1)|MCP_CNF3_WAKFILL(1)|MCP_CNF3_PHSEG2(0b111));
SPI_transfer_enqueue(MCP_INST_WRITE, true);
SPI_transfer_enqueue(MCP_CNF1_ADDRESS, true);
SPI_transfer_enqueue(CNF1, false);
SPI_transfer_enqueue(MCP_INST_WRITE, true);
SPI_transfer_enqueue(MCP_CNF2_ADDRESS, true);
SPI_transfer_enqueue(CNF2, false);
SPI_transfer_enqueue(MCP_INST_WRITE, true);
SPI_transfer_enqueue(MCP_CNF3_ADDRESS, true);
SPI_transfer_enqueue(CNF3, false);


// 3° Configuro los filtros de recepcion

// 4° Configurar modo de recepción (RXB0CTRL y RXB1CTRL, chapter 4.2)

// 5° Borrar flags y habilitar interrupciones (CANINTF y CANINTE, chapter 4.7)

// 6° Ponerlo en Normal Mode
char CANCTRL=0;

}



void MCP_sendMSG(int txdata, _Bool ACSCONT){
	SPI_transfer_enqueue(txdata, ACSCONT);
}

void MCP_control(int instruction,int address,int txdata){
//	SPI_transfer_enqueue(instruction);
//	SPI_transfer_enqueue(address);
//	SPI_transfer_enqueue(txdata);
//	SPI_PCS_dis();
}

void MCP_endTX(){
	SPI_PCS_dis();
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
