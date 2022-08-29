/***************************************************************************/ /**
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
static void write_array(char arr[], char arr_copy[], int size);
static int user_verify(char id[], char password[]);
static void user_init(void);
static char encoder_control(char number, int joystick_input, int *status);
static void print_display(char first, char second, char third, char fourth);
static char *ID_scroll(char array_id[], char *ptr_id, int joystick_input, int *status);
static char *mag_get_ID(void); // devuelve un string del campo de datos PAN
static int simulacion(void);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

// +ej: static const int temperaturas_medias[4] = {23, 26, 24, 29};+

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
enum numbers
{
  CERO,
  ONE,
  TWO,
  THREE,
  FOUR,
  FIVE,
  SIX,
  SEVEN,
  EIGHT,
  NINE
};
enum status
{
  CHANGE_BRIGHT,
  ID,
  CHANGE_ID,
  SUBMIT,
  CLEAR,
  PASSWORD,
  OPEN,
  BLOCK
};
// +ej: static int temperaturas_actuales[4];+

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
int main(void)
{
  App_Init();
  return 1;
}

void App_Init(void)
{
  user_init();

  App_Run();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void)
{
  // MAIN PROGRAM
  // dispInit();
  int status = ID;
  // int status_number = CERO;
  // int status_bright = NINE;
  char array_id[SIZE_DISPLAY_ID] = "ID=00000000SCB=0";
  char *ptr_id = array_id + 3;
  int counter = 0;
  while (counter <= 10)
  {
    counter++;
    int joystick_input = simulacion();
    // printf("%d \n", joystick_input);
    switch (status)
    {

    case CHANGE_BRIGHT:
      *ptr_id = encoder_control(*ptr_id, joystick_input, &status);
      printf(" Llamar funcion de Nico");
      break;

    case ID:
      ptr_id = ID_scroll(array_id, ptr_id, joystick_input, &status);

      break;

    case CHANGE_ID:
      *ptr_id = encoder_control(*ptr_id, joystick_input, &status);
      break;

    case SUBMIT:
      status = PASSWORD;
      break;

    case CLEAR:
      break;

    default:
      break;
    }
    print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0]);
  }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void print_display(char first, char second, char third, char fourth)
{
  printf("%c %c %c %c", first, second, third, fourth);
  printf("\n");
}

static char encoder_control(char number, int joystick_input, int *status)
{

  if (joystick_input == DERECHA)
    if (number == '9')
      number = '0';
    else
      number++;
  else if (joystick_input == IZQUIERDA)
    if (number != '0')
    {
      number--;
    }
    else
      number = '9';
  else if (joystick_input == CLICK)
  {
    *status = ID;
  }
  return number;
}

static void user_init(void) // inicializa primeros usuarios con memoria dinamica
{
  ptr_user = malloc(cant_user * sizeof(User));

  write_array(ptr_user[0].id, "00000000", SIZE_ID);
  write_array(ptr_user[0].password, "00000", SIZE_PASSWORD);
  write_array(ptr_user[0].name, "PORRAS", SIZE_NAME);
}

static int user_verify(char id[], char password[])
{
  // verifico ID y password
  int i;
  for (i = 0; i < cant_user; i++)
  {
    if (compare_array(id, ptr_user[i].id, SIZE_ID))
    {
      if (compare_array(password, ptr_user[i].password, SIZE_PASSWORD))
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

// compara 2 arreglos y devuelve 1 si son iguales y 0 sin son distintos
static int compare_array(char arr1[], char arr2[], int size)
{
  int i;
  for (i = 0; i < size; i++)
  {
    if (arr1[i] != arr2[i])
      return INCORRECTO;
  }
  return CORRECTO;
}

static void write_array(char arr[], char arr_copy[], int size)
{
  int i;
  for (i = 0; i < size; i++)
  {
    arr[i] = arr_copy[i];
  }
}

static char *ID_scroll(char array_id[], char *ptr_id, int joystick_input, int *status)
{

  if (joystick_input == DERECHA)
  {

    if (ptr_id == (array_id + LIMITE_DER_ID))
    {
      ptr_id = array_id + LIMITE_IZQ_ID;
    }
    else if (ptr_id == (array_id + POS_CLEAR)) // salta de la C al numero despues del = (se saltea B=)
    {
      ptr_id = array_id + LIMITE_DER_ID;
    }
    else
    {
      ptr_id++;
    }
  }
  else if (joystick_input == IZQUIERDA)
  {
    if (ptr_id == (array_id + LIMITE_IZQ_ID))
    {
      ptr_id = array_id + LIMITE_DER_ID;
    }
    else if (ptr_id == (array_id + LIMITE_DER_ID))
    {
      ptr_id = array_id + POS_CLEAR;
    }
    else
    {
      ptr_id--;
    }
  }
  else if (joystick_input == CLICK)
  {
    if ((ptr_id >= array_id + LIMITE_IZQ_ID) && (ptr_id <= array_id + LIMITE_IZQ_ID + 7))
    {
      *status = CHANGE_ID;
    }
    else if (ptr_id == array_id + POS_SUBMIT)
    {
      *status = SUBMIT;
    }
    else if (ptr_id == array_id + POS_CLEAR)
    {
      *status = CLEAR;
    }
    else if (ptr_id == array_id + LIMITE_DER_ID)
    {
      *status = CHANGE_BRIGHT;
    }
  }
  return ptr_id;
}

static char *mag_get_ID(void)
{
  return "12345678";
}

int simulacion(void)
{
  static int counter = -1;
  counter++;
  // printf("%d \n ", counter);
  switch (counter)
  {
  case 0:
    return CLICK;
    break;
  case 1:
    return DERECHA;
    break;
  case 2:
    return DERECHA;
    break;
  case 3:
    return CLICK;
    break;
  case 4:
    return DERECHA;
    break;
  case 5:
    return DERECHA;
    break;
  case 6:
    return CLICK;
    break;
  case 7:
    return IZQUIERDA;
    break;
  case 8:
    return CLICK;
    break;
  default:
    return CLICK;
    break;
  }
}