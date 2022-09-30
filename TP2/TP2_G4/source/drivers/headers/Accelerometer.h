/*
 * Accelerometer.h
 *
 *  Created on: 21 sep. 2022
 *      Author: User
 */

#ifndef DRIVERS_HEADERS_ACCELEROMETER_H_
#define DRIVERS_HEADERS_ACCELEROMETER_H_
typedef struct
{
int16_t Accel_x;
int16_t Accel_y;
int16_t Accel_z;
int16_t Magn_x;
int16_t Magn_y;
int16_t Magn_z;
} SRAWDATA;

typedef struct
{
int16_t angle_x;
int16_t pitch;
int16_t roll;
int16_t theta;
int16_t Magn_y;
int16_t Magn_z;
} SPROCESSDATA;

bool get_alreadyInit();
void initAccelerometer();
void ReadAccelMagnData();
SRAWDATA get_aceleration();
void set_alreadyread_AccelMagnData(bool already);
bool get_alreadyread_AccelMagnData();
void proccess_data(SRAWDATA Data_Accel_Mag );


#define I2C_ERROR 0
#define I2C_OK 1

#endif /* DRIVERS_HEADERS_ACCELEROMETER_H_ */
