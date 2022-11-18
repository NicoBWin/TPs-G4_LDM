/***************************************************************************//**
  @file     uart.c
  @brief    UART Driver
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main lib
#include "../headers/uart.h"

// Internal libs
#include "../../MCAL/gpio.h"
#include "../board.h"

// MCU libs
#include "MK64F12.h"
#include "hardware.h"

// Standar libs
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define SCGC_CONFIG(id)	((id==0)?(SIM_SCGC4_UART0(1)): \
						((id==1)?(SIM_SCGC4_UART1(1)): \
						((id==2)?(SIM_SCGC4_UART2(1)): \
						((id==3)?(SIM_SCGC4_UART3(1)): \
						((id==4)?(SIM_SCGC1_UART4(1)): \
						((id==5)?(SIM_SCGC1_UART5(1)):0))))))

#define PORT_Alt3	0b11

#define ISR_t         void __attribute__ ((interrupt))
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct {
	char buffer[BUFFER_SIZE];
	uint8_t readIndex;
	uint8_t writeIndex;
	uint8_t size;
	bool done;
	bool read;
} buffer_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Sets baudrate for transmition
static void UART_SetBaudRate(UART_Type *uart, uint32_t baudrate);

// Copy Msg to buffer
void copyMsg_buffer(uint8_t id, const char * msg, uint32_t len);

//Handler for IQRs
void UART_IQRHandler(uint8_t id);
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static UART_Type* const UART_NUM[6] = UART_BASE_PTRS;
static PORT_Type* const PORT_TYPE[6] = {PORTB,PORTC,PORTD,PORTC,PORTE,PORTE};

static int const UART_TX_PIN[6] = {UART0_TX, UART1_TX, UART2_TX, UART3_TX, UART4_TX, UART5_TX};
static int const UART_RX_PIN[6] = {UART0_RX, UART1_RX, UART2_RX, UART3_RX, UART4_RX, UART5_RX};

static const IRQn_Type IRQnUART[6] = {UART0_RX_TX_IRQn, UART1_RX_TX_IRQn, UART2_RX_TX_IRQn, UART3_RX_TX_IRQn, UART4_RX_TX_IRQn, UART5_RX_TX_IRQn};

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static buffer_t Tx[6];
static buffer_t Rx[6];

//Semaphore
static OS_SEM *Sem;
static OS_ERR enc_err;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Initialize UART driver
 * @param id UART's number
 * @param config UART's configuration (baudrate, parity, etc.)
*/
void uartInit (uint8_t id, uart_cfg_t config, OS_SEM *UartSem){
	//Seteo el puntero del semáforo
	Sem = UartSem;

	UART_Type* const UARTN = UART_NUM[id];

	switch(id) {
	case 0: case 1: case 2: case 3:
		SIM->SCGC4 |= SCGC_CONFIG(id);
		break;
	case 4: case 5:
		SIM->SCGC1 |= SCGC_CONFIG(id);
	}

	PORT_TYPE[id]->PCR[PIN2NUM(UART_TX_PIN[id])] = 0x0; //Clear all bits
	PORT_TYPE[id]->PCR[PIN2NUM(UART_TX_PIN[id])] = PORT_PCR_MUX(PORT_Alt3); //Set MUX to Alt3 in UART4
	//----------------------------------
	PORT_TYPE[id]->PCR[PIN2NUM(UART_RX_PIN[id])] = 0x0; //Clear all bits
	PORT_TYPE[id]->PCR[PIN2NUM(UART_RX_PIN[id])] = PORT_PCR_MUX(PORT_Alt3); //Set MUX to Alt3 in UART4

	UARTN->C5 &= ~UART_C5_TDMAS_MASK;
	UARTN->C2 = UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK;

	switch (config.parity) {
		case ODD_PARITY_UART:
			UARTN->C1 |= UART_C1_PE_MASK;
			UARTN->C1 |= UART_C1_PT_MASK;
			UARTN->C1 |= UART_C1_M_MASK; //parity
			break;
		case EVEN_PARITY_UART:
			UARTN->C1 |= UART_C1_PE_MASK;
			UARTN->C1 &= (~UART_C1_PT_MASK);
			UARTN->C1 |= UART_C1_M_MASK;
			break;
		case NO_PARITY_UART:
			UARTN->C1 &= ~UART_C1_M_MASK;
			UARTN->C1 &= (~UART_C1_PE_MASK);
			break;
	}

	UART_SetBaudRate(UARTN, config.baudrate);

	NVIC_EnableIRQ(IRQnUART[id]);

	//BUFFER INIT
	// Init buffers
	Rx[id].size = 0;
	Rx[id].readIndex = 0;
	Rx[id].writeIndex = 0;

	Tx[id].size = 0;
	Tx[id].readIndex = 0;
	Tx[id].writeIndex = 0;
	Tx[id].done = true;
}

/**
 * @brief Check if a new byte was received
 * @param id UART's number
 * @return A new byte has being received
*/
uint8_t uartIsRxMsg(uint8_t id){
	return Rx[id].read;
}

/**
 * @brief Check how many bytes were received
 * @param id UART's number
 * @return Quantity of received bytes
*/
uint8_t uartGetRxMsgLength(uint8_t id){
	return Rx[id].size;
}

/**
 * @brief Read a received message. Non-Blocking
 * @param id UART's number
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @return Real quantity of pasted bytes
*/
uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant){
	if (Rx[id].read == false) {
		return 0;
	}
	uint8_t size = Rx[id].size;

	for(int i=0 ; i<cant && i<size; i++) {
		*(msg+i) = Rx[id].buffer[Rx[id].readIndex];
		Rx[id].readIndex = ((Rx[id].readIndex)+1 == BUFFER_SIZE) ? 0 : (Rx[id].readIndex)+1;
		Rx[id].size--;
		if(Rx[id].readIndex == Rx[id].writeIndex) {
			Rx[id].size = 0;
		}
	}
	return (cant < size) ? cant : size;
}

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param id UART's number
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
 * @return Real quantity of bytes to be transfered
*/
uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant){
	if(cant > BUFFER_SIZE - Tx[id].size) {
		return 1;
	}
	Tx[id].done = false;

	//	Copy the message to buffer
	copyMsg_buffer(id, msg, cant);

	// Enable transmission
	UART_NUM[id]->C2 |= UART_C2_TIE_MASK;
	return 0;
}

/**
 * @brief Check if all bytes were transfered
 * @param id UART's number
 * @return All bytes were transfered
*/
uint8_t uartIsTxMsgComplete(uint8_t id){
	return Tx[id].done;
}
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void UART_SetBaudRate(UART_Type *uart, uint32_t baudrate){
	uint16_t sbr, brfa;
	uint32_t clock;

	clock = ((uart == UART0) || (uart == UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);

	sbr = clock / (baudrate << 4);
	brfa = (clock << 1) / baudrate - (sbr << 5);

	uart->BDH = UART_BDH_SBR(sbr>>8);
	uart->BDL = UART_BDL_SBR(sbr);
	uart->C4 = (uart->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
}


void copyMsg_buffer(uint8_t id, const char * msg, uint32_t len) {
	uint32_t i;
	for (i = 0; i < len; i++) {
		Tx[id].buffer[Tx[id].writeIndex] = msg[i];

		Tx[id].writeIndex = (Tx[id].writeIndex+1 == BUFFER_SIZE) ? 0 : Tx[id].writeIndex+1;
		Tx[id].size++;
	}
}

void UART_IQRHandler(uint8_t id){
	UART_Type* const UARTN = UART_NUM[id];

	// Interrupt by transmitter
	if(UARTN->S1 & UART_S1_TDRE_MASK){
		if(Tx[id].size > 0){
			UARTN->D = Tx[id].buffer[Tx[id].readIndex]; // Send data
			Tx[id].readIndex = (Tx[id].readIndex+1 == BUFFER_SIZE) ? 0 : Tx[id].readIndex+1;
			Tx[id].size--;
			if(Tx[id].writeIndex == Tx[id].readIndex) {	// Cola empty
				Tx[id].size = 0; // Just in case
			}
		}
		else {
			Tx[id].done = true;
			UARTN->C2 &= ~UART_C2_TIE_MASK;	// disable transmission
		}
	}

	// Interrupt by receiver
	if (UARTN->S1 & UART_S1_RDRF_MASK) {
		if(Rx[id].size < BUFFER_SIZE) {	//Si se lleno el buffer no guardo nada
			Rx[id].buffer[Rx[id].writeIndex] = UARTN->D; 	// Guardo lo recibido
			Rx[id].writeIndex = (Rx[id].writeIndex+1 == BUFFER_SIZE) ? 0 : Rx[id].writeIndex+1;
			Rx[id].size++;
		}
		Rx[id].read = true;
		OSSemPost(Sem, OS_OPT_POST_ALL, &enc_err);
	}
}

ISR_t UART0_RX_TX_IRQHandler(void) {UART_IQRHandler(0);}
ISR_t UART1_RX_TX_IRQHandler(void) {UART_IQRHandler(1);}
ISR_t UART2_RX_TX_IRQHandler(void) {UART_IQRHandler(2);}
ISR_t UART3_RX_TX_IRQHandler(void) {UART_IQRHandler(3);}
ISR_t UART4_RX_TX_IRQHandler(void) {UART_IQRHandler(4);}
ISR_t UART5_RX_TX_IRQHandler(void) {UART_IQRHandler(5);}
