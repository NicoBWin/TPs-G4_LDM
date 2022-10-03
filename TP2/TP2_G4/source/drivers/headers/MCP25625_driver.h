/***************************************************************************//**
  @file     MCP25625.h
  @brief    Driver para la placa de conversión SPI-CAN MCP25625
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _MCP25635_DRIVER_H_
#define _MCP25635_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>






/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
void MCP_init();
void MCP_control(char instruction,char address,char txdata);
void MCP_sendMSG(int txdata, _Bool ACSCONT);
void MCP_endTX();

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define TSAMPLE	1	//Cada cuanto espero un pulso en enable, en ms
#define MCP_INST_READ	0b00000011
#define MCP_INST_RESET	0b11000000
#define MCP_INST_WRITE	0b00000010
#define MCP_INST_BITMOD	0b00000101	//Bit modify
#define MCP_INST_READST	0b00000010	//Read Status

#define MCP_READRX_MASK	0b10010000
#define MCP_READRX_NMSHIFT	2
#define MCP_INST_READRX(x)             (((int)(((int)(x)) << MCP_READRX_NMSHIFT)) | MCP_READRX_MASK)
#define MCP_LOADTX_MASK	0b01000000
#define MCP_LOADTX_SHIFT	0
#define MCP_INST_LOADTX(x)             (((int)(((int)(x)) << MCP_LOADTX_SHIFT)) | MCP_LOADTX_MASK)

#define MCP_CNF1_ADDRESS	0x2A
#define MCP_CNF2_ADDRESS	0x29
#define MCP_CNF3_ADDRESS	0x28
#define MCP_CNF1_SJWSHIFT	6
#define MCP_CNF1_SJW(x)             (((char)(((char)(x)) << MCP_CNF1_SJWSHIFT)))
#define MCP_CNF1_BRPSHIFT	0
#define MCP_CNF1_BRP(x)             (((char)(((char)(x)) << MCP_CNF1_BRPSHIFT)))
#define MCP_CNF2_BTLSHIFT	7
#define MCP_CNF2_BTL(x)             (((char)(((char)(x)) << MCP_CNF2_BTLSHIFT)))
#define MCP_CNF2_SAMSHIFT	6
#define MCP_CNF2_SAM(x)             (((char)(((char)(x)) << MCP_CNF2_SAMSHIFT)))
#define MCP_CNF2_PHSEG1SHIFT	3
#define MCP_CNF2_PHSEG1(x)             (((char)(((char)(x)) << MCP_CNF2_PHSEG1SHIFT)))
#define MCP_CNF2_PRSEG2SHIFT	2
#define MCP_CNF2_PRSEG2(x)             (((char)(((char)(x)) << MCP_CNF2_PRSEG2SHIFT)))
#define MCP_CNF2_PRSEG1SHIFT	1
#define MCP_CNF2_PRSEG1(x)             (((char)(((char)(x)) << MCP_CNF2_PRSEG1SHIFT)))
#define MCP_CNF2_PRSEG0SHIFT	0
#define MCP_CNF2_PRSEG0(x)             (((char)(((char)(x)) << MCP_CNF2_PRSEG0SHIFT)))
#define MCP_CNF3_SOFSHIFT	7
#define MCP_CNF3_SOF(x)             (((char)(((char)(x)) << MCP_CNF3_SOFSHIFT)))
#define MCP_CNF3_WAKFILLSHIFT	6
#define MCP_CNF3_WAKFILL(x)             (((char)(((char)(x)) << MCP_CNF3_WAKFILLSHIFT)))
#define MCP_CNF3_PHSEG2SHIFT	0
#define MCP_CNF3_PHSEG2(x)             (((char)(((char)(x)) << MCP_CNF3_PHSEG2SHIFT)))

#define MCP_TXRTSCTRL_ADDRESS 0x0D

#define MCP_RXF0SIDH_ADDRESS 0x00
#define MCP_RXF0SIDL_ADDRESS 0x01

#define MCP_RXM0SIDH_ADDRESS 0x20
#define MCP_RXM0SIDL_ADDRESS 0x21

#define MCP_RXB0CTRL_ADDRESS		0x60
#define MCP_RXB0CTRL_RXMSHIFT	5
#define MCP_RXB0CTRL_RXM(x)             (((char)(((char)(x)) << MCP_RXB0CTRL_RXMSHIFT)))
#define MCP_RXB0CTRL_BUKTSHIFT	2
#define MCP_RXB0CTRL_BUKT(x)             (((char)(((char)(x)) << MCP_RXB0CTRL_BUKTSHIFT)))

#define MCP_RXB1CTRL_ADDRESS		0x70
#define MCP_RXB1CTRL_RXMSHIFT	5
#define MCP_RXB1CTRL_RXM(x)             (((char)(((char)(x)) << MCP_RXB1CTRL_RXMSHIFT)))


#define MCP_CANINTE_ADDRESS		0x2B
#define MCP_CANINTE_MERRESHIFT	7
#define MCP_CANINTE_MERRE(x)             (((char)(((char)(x)) << MCP_CANINTE_MERRESHIFT)))
#define MCP_CANINTE_WAKIESHIFT	6
#define MCP_CANINTE_WAKIE(x)             (((char)(((char)(x)) << MCP_CANINTE_WAKIESHIFT)))
#define MCP_CANINTE_ERRIESHIFT	5
#define MCP_CANINTE_ERRIE(x)             (((char)(((char)(x)) << MCP_CANINTE_ERRIESHIFT)))
#define MCP_CANINTE_TX2IESHIFT	4
#define MCP_CANINTE_TX2IE(x)             (((char)(((char)(x)) << MCP_CANINTE_TX2IESHIFT)))
#define MCP_CANINTE_TX1IESHIFT	3
#define MCP_CANINTE_TX1IE(x)             (((char)(((char)(x)) << MCP_CANINTE_TX1IESHIFT)))
#define MCP_CANINTE_TX0IESHIFT	2
#define MCP_CANINTE_TX0IE(x)             (((char)(((char)(x)) << MCP_CANINTE_TX0IESHIFT)))
#define MCP_CANINTE_RX1IESHIFT	1
#define MCP_CANINTE_RX1IE(x)             (((char)(((char)(x)) << MCP_CANINTE_RX1IESHIFT)))
#define MCP_CANINTE_RX0IESHIFT	0
#define MCP_CANINTE_RX0IE(x)             (((char)(((char)(x)) << MCP_CANINTE_RX0IESHIFT)))

#define MCP_CANINTF_ADDRESS		0x2C
#define MCP_CANINTF_MERRFSHIFT	7
#define MCP_CANINTF_MERRF(x)             (((char)(((char)(x)) << MCP_CANINTF_MERRFSHIFT)))
#define MCP_CANINTF_WAKIFSHIFT	6
#define MCP_CANINTF_WAKIF(x)             (((char)(((char)(x)) << MCP_CANINTF_WAKIFSHIFT)))
#define MCP_CANINTF_ERRIFSHIFT	5
#define MCP_CANINTF_ERRIF(x)             (((char)(((char)(x)) << MCP_CANINTF_ERRIFSHIFT)))
#define MCP_CANINTF_TX2IFSHIFT	4
#define MCP_CANINTF_TX2IF(x)             (((char)(((char)(x)) << MCP_CANINTF_TX2IFSHIFT)))
#define MCP_CANINTF_TX1IFSHIFT	3
#define MCP_CANINTF_TX1IF(x)             (((char)(((char)(x)) << MCP_CANINTF_TX1IFSHIFT)))
#define MCP_CANINTF_TX0IFSHIFT	2
#define MCP_CANINTF_TX0IF(x)             (((char)(((char)(x)) << MCP_CANINTF_TX0IFSHIFT)))
#define MCP_CANINTF_RX1IFSHIFT	1
#define MCP_CANINTF_RX1IF(x)             (((char)(((char)(x)) << MCP_CANINTF_RX1IFSHIFT)))
#define MCP_CANINTF_RX0IFSHIFT	0
#define MCP_CANINTF_RX0IF(x)             (((char)(((char)(x)) << MCP_CANINTF_RX0IFSHIFT)))

#define MCP_CANCTRL_ADDRESS		0x0F
#define MCP_CANCTRL_REQOPSHIFT	5
#define MCP_CANCTRL_REQOP(x)             (((char)(((char)(x)) << MCP_CANCTRL_REQOPSHIFT)))
#define MCP_CANCTRL_ABATSHIFT	4
#define MCP_CANCTRL_ABAT(x)             (((char)(((char)(x)) << MCP_CANCTRL_ABATSHIFT)))
#define MCP_CANCTRL_OSMSHIFT	3
#define MCP_CANCTRL_OSM(x)             (((char)(((char)(x)) << MCP_CANCTRL_OSMSHIFT)))
#define MCP_CANCTRL_CLKENSHIFT	2
#define MCP_CANCTRL_CLKEN(x)             (((char)(((char)(x)) << MCP_CANCTRL_CLKENSHIFT)))
#define MCP_CANCTRL_CLKPRESHIFT	0
#define MCP_CANCTRL_CLKPRE(x)             (((char)(((char)(x)) << MCP_CANCTRL_CLKPRESHIFT)))


/*******************************************************************************
 ******************************************************************************/
 #endif // _CARD_READER_H_
