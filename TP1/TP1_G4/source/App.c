/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
//#include "drivers/headers/card_reader.h"
//#include "drivers/headers/display.h"
//#include "drivers/headers/encoder.h"
#include "App.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/
User *ptr_user;
int cant_user = 4;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static int compare_array(char arr1[], char arr2[], int size);
static void write_array( char arr[], char arr_copy[], int size);
static int user_verify(char id[] , char password[] );
static void user_init(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static int temperaturas_actuales[4];+

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init(void)
{
    user_init();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	//MAIN PROGRAM
	//dispInit();
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void user_init(void) // inicializa primeros usuarios con memoria dinamica
{
  ptr_user = malloc( cant_user * sizeof(User));

  write_array(ptr_user[0].id, "00000000", SIZE_ID);
  write_array(ptr_user[0].password, "00000", SIZE_PASSWORD);
  write_array(ptr_user[0].name, "PORRAS", SIZE_NAME);

}

static int user_verify(char id[] , char password[] )
{
   // verifico ID y password
   int i;
   for (i = 0; i < cant_user; i++)
   {
     if( compare_array( id, ptr_user[i].id, SIZE_ID))
     {
        if (compare_array( password, ptr_user[i].password, SIZE_PASSWORD ) )
        {
          return CORRECTO;  
        }
        else
        {
          return INCORRECTO;
        }
     }
   }
   return INCORRECTO;
}


//compara 2 arreglos y devuelve 1 si son iguales y 0 sin son distintos
static int compare_array(char arr1[], char arr2[], int size) 
{
  int i;
	for(i=0;i<size;i++)
  {
		if(arr1[i]!=arr2[i])
      return INCORRECTO;
  }
  return CORRECTO;
}

static void write_array( char arr[], char arr_copy[], int size)
{
  int i;
  for (i = 0; i<size;i++)
  {
    arr[i] = arr_copy[i];
  }
}