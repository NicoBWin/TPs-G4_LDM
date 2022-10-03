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

	// 2° Configuro el tiempo de bit	CHK
	char CNF1=0,CNF2=0,CNF3=0;
	CNF1 = (MCP_CNF1_SJW(0b00)|MCP_CNF1_BRP(0b00111));
	MCP_control(MCP_INST_WRITE, MCP_CNF1_ADDRESS, CNF1);

	CNF2 = (MCP_CNF2_BTL(1)|MCP_CNF2_SAM(1)|MCP_CNF2_PHSEG1(0b100)|MCP_CNF2_PRSEG2(0b111));
	MCP_control(MCP_INST_WRITE, MCP_CNF2_ADDRESS, CNF2);

	CNF3 = (MCP_CNF3_SOF(1)|MCP_CNF3_WAKFILL(1)|MCP_CNF3_PHSEG2(0b100));
	MCP_control(MCP_INST_WRITE, MCP_CNF3_ADDRESS, CNF3);


	// 3° Configuro los filtros de recepcion
	MCP_control(MCP_INST_WRITE, MCP_TXRTSCTRL_ADDRESS, 0x01); // Configurar para utilizar uno solo de los 3 buffers de TX

	// Aparte de configurar los filtros
	MCP_control(MCP_INST_WRITE, MCP_RXF0SIDH_ADDRESS, 0x20);	// Seteo el filtro (H)
	MCP_control(MCP_INST_WRITE, MCP_RXF0SIDL_ADDRESS, 0x00);	// Seteo el filtro (L)
	MCP_control(MCP_INST_WRITE, MCP_RXM0SIDH_ADDRESS, 0xFF);	// Seteo la mask (H)
	MCP_control(MCP_INST_WRITE, MCP_RXM0SIDL_ADDRESS, 0x00);	// Seteo la mask (L)


	// 4° Configurar modo de recepción (RXB0CTRL y RXB1CTRL, chapter 4.2) CHWCK
	char RXB0CTRL=0, RXB1CTRL=0;
	RXB0CTRL = (MCP_RXB0CTRL_RXM(0b00)|MCP_RXB0CTRL_BUKT(1));
	MCP_control(MCP_INST_WRITE, MCP_RXB0CTRL_ADDRESS, RXB0CTRL);
	RXB1CTRL = (MCP_RXB1CTRL_RXM(0b11));
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
	CANCTRL = (MCP_CANCTRL_REQOP(0b000)|MCP_CANCTRL_ABAT(1)|MCP_CANCTRL_OSM(1)|MCP_CANCTRL_CLKEN(1)|MCP_CANCTRL_CLKPRE(0b00));
	MCP_control(MCP_INST_WRITE, MCP_CANCTRL_ADDRESS, CANCTRL);
}



void MCP_sendMSG(int txdata, _Bool ACSCONT){
	SPI_transfer_enqueue(txdata, ACSCONT);
}

void MCP_control(char instruction,char address,char txdata) 
{
	SPI_transfer_enqueue(instruction, true);
	SPI_transfer_enqueue(address, true);
	SPI_transfer_enqueue(txdata, false);
}

void MCP_SEND_MESSAGE(int myID,char dataNUM, char data[4])
{
	int TXn=0;

	//Solicito un buffer de TX
	//MCP_polltxbuffer(); ESTA PREGUNTARIA SI HAY UNO LIBRE.
	
	//Lleno los datos de ID
	MCP_fillID(myID);
	//Lleno la cantidad de bytes a enviar
	MCP_control(MCP_INST_WRITE,MCP_TXB0DLC_ADDRESS,dataNUM);
	//Lleno los datos a enviar
	if(dataNUM<2)
	{
	MCP_control(MCP_INST_WRITE,MCP_TXB0D0_ADDRESS,data[TXn]);
	TXn++;
		if(dataNUM<3)
		{
			MCP_control(MCP_INST_WRITE,MCP_TXB0D1_ADDRESS,data[TXn]);
			TXn++;
		}
		if(dataNUM<4)
		{
			MCP_control(MCP_INST_WRITE,MCP_TXB0D2_ADDRESS,data[TXn]);
			TXn++;
		}
		if(dataNUM<5)
		{
			MCP_control(MCP_INST_WRITE,MCP_TXB0D3_ADDRESS,data[TXn]);
			TXn++;
		}
	}
	//Solicito el envío del mensaje	
	MCP_control(MCP_INST_WRITE,MCP_TXB0CTRL_ADDRESS,0b00001000);
}

char* MCP_RECEIVE_MESSAGE()
{
	static _Bool hitRx=false;
	static int datanum=0;
	static char	returnvalues[5];
	//Verifico que llego un mensaje en los flags
	//hitRX = MCP_control(MCP_INST_READ,MCP_CANINTF_ADDRESS,0xFF);

	//Leo la informacion de los buffers
	MCP_control(MCP_INST_READ,MCP_RXB0DLC_ADDRESS,0xFF);
	datanum = MCP_reqread();
	returnvalues[2]=datanum;
	MCP_control(MCP_INST_READ,MCP_RXB0D0_ADDRESS,0xFF);
	rawdata = MCP_reqread();
	returnvalues[3]=rawdata;
	
	//Borro el flag de rx lleno
	MCP_control(MCP_INST_WRITE,MCP_CANINTF_ADDRESS,0x01);
}
/*int MCP_polltxbuffer() //WIP
{
	int i=0;
	for(i=0; i<3; i++)
	{
		(MCP_INST_READ,MCP_TXREQ0_ADDRESS,0);
		//SPI POPR
		if((MCP_INST_READ,MCP_TXREQ0_ADDRESS,0));
	}
}*/
char MCP_reqread()
{
	char rval=0;
	rval = SPI_read_transfer();
	return rval;
}
void MCP_fillID(int myID) {
	char idH, idL;
	idL = (char)myID;
	idH = (char)((myID<<4));
	MCP_control(MCP_INST_WRITE,MCP_TXB0SIDH_ADDRESS,idH);
	MCP_control(MCP_INST_WRITE,MCP_TXB0SIDL_ADDRESS,idL);

}

// void MCP_transferdata(int bytecount, char* d0) {
// 	static int i=0;
// 	if(bytecount<9)	{
// 		if(i=0)	{
// 			MCP_control(MCP_INST_WRITE , MCP_TXB0CTRL_TXB0DLC_ADDRESS ,MCP_TXB0CTRL_TXB0DLC);
// 			MCP_control(MCP_INST_WRITE , MCP_TXB0CTRL_ADDRESS,);
// 		}
// 	}
// }
void MCP_endTX() {
	SPI_PCS_dis();
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
