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
void MCP_control(int instruction,int address,int txdata);
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



/*******************************************************************************
 ******************************************************************************/
 #endif // _CARD_READER_H_
