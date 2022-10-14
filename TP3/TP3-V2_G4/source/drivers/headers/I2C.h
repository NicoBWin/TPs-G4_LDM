/***************************************************************************//**
  @file     I2C.h
  @brief    Driver I2C header
  @author   Nicolás Magliola
 ******************************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include "../../MCAL/gpio.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
// USARE EL I2C0
//#define I2C0_DATA PORTNUM2PIN(PB,1) // ALT2
//#define I2C0_CLK PORTNUM2PIN(PB, 2) // ALT2

//#define i2c1_DATA PORTNUM2PIN(PD, 11) // ALT2
//#define i2c1_CLK PORTNUM2PIN(PC, 10)  // ALT2

//#define i2c2_DATA PORTNUM2PIN(PA, 11) // ALT5
//#define i2c2_CLK PORTNUM2PIN(PB, 12)  // ALT2

// Posibles velocidades del clock: Se pueden incorporar mas
#define CLOCK_MAQUINA 12500000; // cada ICR divide al clk por su equivalente de la tabla 51-2 del reference manual
enum
{
  i2c_baudrate_260416Hz = 0x10,
  i2c_baudrate_156250Hz = 0x14,
  i2c_baudrate_111607Hz = 0x1A, // Usaremos esta usualmente 100Khz --> Normal mode del accelerometer
  i2c_baudrate_52083Hz = 0x1F,
};

#define WRITE_MODE 0
#define READ_MODE 1
#define NOT_BUSY 0
#define BUSY 1

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void I2C_init(uint16_t i2c_baudrate, int ic2num); 


bool i2cStartTransaction(int i2cnum, uint8_t address, int cantwrite, uint8_t *writebuffer, int cantread, uint8_t *readbuffer, void (*callback_fn)(void));

int get_ready(); // Devuelve 0 si todavia no esta listo, devuelve 1 si esta listo, devuelve 2 si hay error
/*******************************************************************************
 ******************************************************************************/
uint8_t* get_read_buffer();

#endif // _UART_H_
