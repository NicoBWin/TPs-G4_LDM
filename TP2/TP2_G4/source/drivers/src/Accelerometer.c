/***************************************************************************//**
  @file     Accelerometer.c
  @brief    Driver Accelerometer
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Main lib
#include "../headers/Accelerometer.h"
#include "../headers/I2C.h"

// Internal libs
#include "../../MCAL/gpio.h"
#include "../board.h"

// MCU libs
#include "MK64F12.h"
#include "hardware.h"


#include <math.h>


// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR	0x1D // with pins SA0=1, SA1=0

// FXOS8700CQ internal register addresses
#define FXOS8700CQ_STATUS		0x00	// Registro donde se lee el acelerometro y el magnetometro
#define FXOS8700CQ_WHOAMI		0x0D	// Indica a que registro le esta hablando
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E	// Pongo 0x0 para estar en escala de +-2G .
#define FXOS8700CQ_CTRL_REG1	0x2A    // device identifier which is set to 0xC4 for preproduction devices and 0xC7 for production devices
#define FXOS8700CQ_M_CTRL_REG1 	0x5B	// Configuración generales  -> en ctrl_reg1 Primero mando 0x0 para poner en standby y luego le saco ese modo
#define FXOS8700CQ_M_CTRL_REG2 	0x5C    // Configuración generales
#define FXOS8700CQ_WHOAMI_VAL 	0xC7



#define FXOS8700CQ_OUT_X_MSB	0x01
#define FXOS8700CQ_OUT_Y_MSB	0x03
#define FXOS8700CQ_OUT_Z_MSB	0x05

#define UINT14_MAX				16383

// number of bytes to be read from the FXOS8700CQ
#define FXOS8700CQ_READ_LEN 13 // status plus 6 channels = 13 bytes

#define SENSIBILITY  0.25 //  mg/LSB
#define pi 3.14159265359

static bool alreadyInit;
static bool alreadyread;

static SRAWDATA Data_Accel_Mag;
//static SRAWDATA pAccelData;

static uint8_t* Buffer;

void initAccelerometer(){

	static int status_config = 0;
	if(status_config == 0)
	{
		alreadyInit = false;
		//I2C_init(i2c_baudrate_111607Hz, 0); //  INICIALIZO EL I2C

	}
	uint8_t writebuffer[5];
	switch(status_config)
	{
	case 0:
		writebuffer[0] = FXOS8700CQ_WHOAMI;
		if( i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 1, writebuffer, 1, 0, initAccelerometer) )
		{
			 status_config++;
		}
	case 1:

		 writebuffer[0] = FXOS8700CQ_CTRL_REG1;
		 writebuffer[1] = 0x0;
		 if( i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 2, writebuffer, 0, 0, initAccelerometer) )
		 {
			 status_config++;
		 }
		 break;

	case 2:

		writebuffer[0] = FXOS8700CQ_XYZ_DATA_CFG;
		writebuffer[1] = 0x0; //rango +-2g  sensibilidad 0.244 mg/LSB
		if( i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 2, writebuffer, 0, 0, initAccelerometer) )
		{
			status_config++;
		}
		break;
	case 3:
		writebuffer[0] = FXOS8700CQ_M_CTRL_REG1;
		writebuffer[1] = 0x1F;			//[0011 1011]  [5] map mag reg to acc regs for burst opp  [4](=1) magnetic min/max detection function DISABLE [1-0] automatic magnetic reset disabled
		if( i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 2, writebuffer, 0, 0, initAccelerometer) )
		{
			status_config++;
		}

	case 4:
		writebuffer[0] = FXOS8700CQ_M_CTRL_REG2;
		writebuffer[1] = 0x20;			//[0011 1011]  [5] map mag reg to acc regs for burst opp  [4](=1) magnetic min/max detection function DISABLE [1-0] automatic magnetic reset disabled
		if( i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 2, writebuffer, 0, 0, initAccelerometer) )
		{
			status_config++;
		}
		break;

	case 5:

		 writebuffer[0] = FXOS8700CQ_CTRL_REG1;
		 writebuffer[1] =  0x0D; // 0x09 puso Ine // 1F puso // 0D dice la datsheet
		 if ( i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 2, writebuffer, 0, 0, initAccelerometer) )
		 {
			 status_config++;
		 }

		 break;
	default:

		 alreadyInit = true;
		 // Dejo el atatus en 3 pq ya se inicializo correctamente todo
		 break;
	}


}

bool get_alreadyInit()
{
	return alreadyInit;
}

// read status and the three channels of accelerometer and magnetometer data from  FXOS8700CQ (13 bytes)

void ReadAccelMagnData()
{
//MQX_FILE_PTR fp; // I2C file pointer
static int flag = 0;
alreadyread = false;
//volatile uint8_t Buffer[FXOS8700CQ_READ_LEN]; // read buffer
// read FXOS8700CQ_READ_LEN=13 bytes (status byte and the six channels of data)
if( flag )
{
Buffer = get_read_buffer(0);
// copy the 14 bit accelerometer byte data into 16 bit words
Data_Accel_Mag.Accel_x = (int16_t)(((Buffer[1] << 8) | Buffer[2]))>> 2;
Data_Accel_Mag.Accel_y = (int16_t)(((Buffer[3] << 8) | Buffer[4]))>> 2;
Data_Accel_Mag.Accel_z = (int16_t)(((Buffer[5] << 8) | Buffer[6]))>> 2;
// copy the magnetometer byte data into 16 bit words
Data_Accel_Mag.Magn_x = (Buffer[7] << 8) | Buffer[8];
Data_Accel_Mag.Magn_y = (Buffer[9] << 8) | Buffer[10];

Data_Accel_Mag.Magn_z = (Buffer[11] << 8) | Buffer[12];
flag = 0;
alreadyread = true;
//printf("recibi data lpm \n");
//printf("%d", Data_Accel_Mag.Accel_x );
}
else
{
	uint8_t writebuffer[1];
	writebuffer[0] = FXOS8700CQ_STATUS;
	if (i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 1, writebuffer, FXOS8700CQ_READ_LEN ,  0 , ReadAccelMagnData))
	{
		flag = 1; //  i2cStartTransaction( 0 ,FXOS8700CQ_SLAVE_ADDR , 2, writebuffer, 0, 0, initAccelerometer)
		alreadyread = 0;
	}

}

}

void proccess_data(SRAWDATA Data_Accel_Mag )
{
	SPROCESSDATA angle;

	// MULTIPLICO POR SENSIBILIDAD
	double argx = Data_Accel_Mag.Accel_x*SENSIBILITY* 0.001 * 0.5; // Multiplico por la sensibilidad, paso a g y divido por 2g para normalizar
	double argy = Data_Accel_Mag.Accel_y*SENSIBILITY* 0.001 * 0.5;
	double argz = Data_Accel_Mag.Accel_z*SENSIBILITY* 0.001 * 0.5;

	Data_Accel_Mag.Magn_x *= SENSIBILITY;
	Data_Accel_Mag.Magn_y *= SENSIBILITY;
	Data_Accel_Mag.Magn_z *= SENSIBILITY;

	// Saco angulo del eje c con el accel_x, asen(accel_x)
	//angle.angle_x = (asin(argx)*180)/pi;
	angle.pitch = (atan2(argx, sqrt(argz*argz + argy*argy)))*180/pi;
	angle.roll = (atan2(argy, sqrt(argz*argz + argx*argx)))*180/pi;
	angle.theta = atan2( sqrt(argx*argx + argy*argy),argz )*180/pi;

	//printf("\n el pitch es: %d", angle.pitch);
	//printf("\n el roll es: %d", angle.roll);
	//printf("\n el theta es: %d", angle.theta);
	//return 1;

}

bool get_alreadyread_AccelMagnData()
{
	return alreadyread;
}
void set_alreadyread_AccelMagnData(bool already)
{
	alreadyread = already;
}

SRAWDATA get_aceleration()
{
	return Data_Accel_Mag;
}
