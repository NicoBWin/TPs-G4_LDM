/***************************************************************************//**
  @file     template.c
  @brief    template
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

// +Incluir el header propio (ej: #include "template.h")+
#include "../headers/MCP25625_driver.h"
#include "../headers/SPI_driver.h"
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

	//ACA defino la secuencia de inicializacion del MCP
	// 1° RESETEO el controlador
	SPI_transfer_enqueue(MCP_INST_RESET, false);

	// 2° Configuro el tiempo de bit
	char CNF1=0,CNF2=0,CNF3=0;
	CNF1 = (MCP_CNF1_SJW(0b01)|MCP_CNF1_BRP(0b10001));
	MCP_control(MCP_INST_WRITE, MCP_CNF1_ADDRESS, CNF1);

	CNF2 = (MCP_CNF2_BTL(1)|MCP_CNF2_SAM(1)|MCP_CNF2_PHSEG1(0b010)|MCP_CNF2_PRSEG2(0)|MCP_CNF2_PRSEG1(0)|MCP_CNF2_PRSEG0(0));
	MCP_control(MCP_INST_WRITE, MCP_CNF2_ADDRESS, CNF2);

	CNF3 = (MCP_CNF3_SOF(1)|MCP_CNF3_WAKFILL(1)|MCP_CNF3_PHSEG2(0b111));
	MCP_control(MCP_INST_WRITE, MCP_CNF3_ADDRESS, CNF3);


	// 3° Configuro los filtros de recepcion
	//MCP_TXRTSCTRL_ADDRESS // Configurar para utilizar uno solo de los 3 buffers de TX
// Aparte de configurar los filtros
	/*
	setear -> MCP_RXF0SIDH_ADDRESS, 0x20 // Seteo el filtro (H)
	setear -> MCP_RXF0SIDL_ADDRESS, 0x00 // Seteo el filtro (L)
	setear -> MCP_RXM0SIDH_ADDRESS, 0xFF // Seteo la mask (H)
	setear -> MCP_RXM0SIDL_ADDRESS, 0x00 // Seteo la mask (L)
	 */

	// 4° Configurar modo de recepción (RXB0CTRL y RXB1CTRL, chapter 4.2)
	char RXB0CTRL=0, RXB1CTRL=0;
	RXB0CTRL = (MCP_RXB0CTRL_RXM(0b00)|MCP_RXB0CTRL_BUKT(0));
	MCP_control(MCP_INST_WRITE, MCP_RXB0CTRL_ADDRESS, RXB0CTRL);
	RXB1CTRL = (MCP_RXB1CTRL_RXM(0b00));
	MCP_control(MCP_INST_WRITE, MCP_RXB1CTRL_ADDRESS, RXB1CTRL);

	// 5° Borrar flags y habilitar interrupciones (CANINTF y CANINTE, chapter 4.7)
	char CANINTF=0, CANINTE=0;
	CANINTE = (MCP_CANINTE_MERRE(0)|MCP_CANINTE_WAKIE(0)|MCP_CANINTE_ERRIE(0)|MCP_CANINTE_TX2IE(0)
			   |MCP_CANINTE_TX1IE(0)|MCP_CANINTE_TX0IE(0)|MCP_CANINTE_RX1IE(0)|MCP_CANINTE_RX0IE(0));
	MCP_control(MCP_INST_WRITE, MCP_CANINTE_ADDRESS, CANINTE);

	CANINTF = (MCP_CANINTF_MERRF(0)|MCP_CANINTF_WAKIF(0)|MCP_CANINTF_ERRIF(0)|MCP_CANINTF_TX2IF(0)
			   |MCP_CANINTF_TX1IF(0)|MCP_CANINTF_TX0IF(0)|MCP_CANINTF_RX1IF(0)|MCP_CANINTF_RX0IF(0));
	MCP_control(MCP_INST_WRITE, MCP_CANINTF_ADDRESS, CANINTF);

	// 6° Ponerlo en Normal Mode o Loopback
	char CANCTRL=0;
	CANCTRL = (MCP_CANCTRL_REQOP(0b000)|MCP_CANCTRL_ABAT(0)|MCP_CANCTRL_OSM(0)|MCP_CANCTRL_CLKEN(0)|MCP_CANCTRL_CLKPRE(0b00));
	MCP_control(MCP_INST_WRITE, MCP_CANCTRL_ADDRESS, CANCTRL);
}



void MCP_sendMSG(int txdata, _Bool ACSCONT){
	SPI_transfer_enqueue(txdata, ACSCONT);
}

void MCP_control(char instruction,char address,char txdata){
	SPI_transfer_enqueue(instruction, true);
	SPI_transfer_enqueue(address, true);
	SPI_transfer_enqueue(txdata, false);
}

void MCP_endTX(){
	SPI_PCS_dis();
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
