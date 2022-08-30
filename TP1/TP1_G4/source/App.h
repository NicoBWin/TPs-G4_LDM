/***************************************************************************//**
  @file     App.h
  @brief    Application functions
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

#ifndef _APP_H_
#define _APP_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define CARACTER_NULL 'A' // Caracter que indica que la contraseña es de 4 digitos
#define INCORRECTO 0
#define CORRECTO 1
#define SIZE_ID 8
#define SIZE_PASSWORD 5 
#define SIZE_NAME 10
#define SIZE_DISPLAY_ID 16
#define SIZE_DISPLAY_PW 10
#define SIZE_PW 5
#define LIMITE_IZQ_ID 3
#define LIMITE_DER_ID  SIZE_DISPLAY_ID -1
#define POS_SUBMIT 11
#define POS_CLEAR 12

#define LIMITE_IZQ_PW 3
#define LIMITE_DER_PW  SIZE_DISPLAY_PW -1
#define POS_CHECK LIMITE_DER_ID -1
#define POS_CHECK_PW LIMITE_DER_PW -1

/// Posibles salidas del joystick
#define NOMOVE 0
#define DERECHA 1
#define IZQUIERDA 2 
#define CLICK 3
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Defino estados


typedef struct User
{
    char id[SIZE_ID] ;
    char password[SIZE_PASSWORD];
    char name[SIZE_NAME];
} User;
/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

// +ej: extern unsigned int anio_actual;+

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void App_Init(void);
void App_Run(void);
/*******************************************************************************
 ******************************************************************************/

#endif // _APP_H_