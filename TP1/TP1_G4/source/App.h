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
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
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