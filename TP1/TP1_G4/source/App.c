/***************************************************************************/ /**
   @file     App.c
   @brief    Application functions
   @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Drivers
#include "drivers/headers/card_reader.h"
#include "drivers/headers/display.h"
#include "drivers/headers/encoder.h"
#include "drivers/headers/leds.h"
#include "drivers/headers/magDriver.h"

// Timer
#include "timer/timer.h"

// App
#include "App.h"

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum status
{
  CHANGE_BRIGHT,
  ID,
  CHANGE_ID,
  SUBMIT,
  CANCEL,
  PASSWORD,
  OPEN,
  BLOCK,
  CHANGE_PW,
  CHECK_ID_PW,
  ADMIN,
  ADD,
  DELETE
};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static int compare_array(char arr1[], char arr2[], int size);
static void write_array(char arr[], char arr_copy[], int size);
static int user_verify(char id[], char password[], User *ptr_user, int cant_user);
static int admin_verify(char id[], char password[], User* ptr_admin);
static User* user_init(int cant_user, User* ptr_user);
static User* admin_init( User* ptr_admin);
static char encoder_control(char number, int joystick_input, int *status);
static void print_display(char first, char second, char third, char fourth,encResult_t joystick_input );
static char *ID_scroll(char array_id[], char *ptr_id, int joystick_input, int *status);
//static char *mag_get_ID(void); // devuelve un string del campo de datos PAN
static int simulacion(void);
static char *PW_scroll(char array_pw[], char *ptr_pw, int joystick_input, int *status);

static char *ADMIN_scroll(char array_admin[], char *ptr_admin, int joystick_input, int *status);
static User* user_add(int cant_user, User* ptr_user, char id[], char password[]);
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static encResult_t encoderState;
static int flag_add = 0;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
// +ej: static int temperaturas_actuales[4];+

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* Todos los init necesarios */
void App_Init(void) {
  timerInit();
  encInit();
  dispInit();
  ledsInit();
  mag_drv_INIT();
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
  User *ptr_user;
  User *ptr_administradores;
  int cant_user = 1;
  ptr_user = user_init(cant_user, ptr_user);
  ptr_administradores = admin_init(ptr_administradores);
  SetdispBrightness(7);
  
  // MAIN PROGRAM
  // dispInit();
  static int status = ID;
  // int status_number = CERO;
  // int status_bright = NINE;
  char array_id[SIZE_DISPLAY_ID] = "ID=00000000SCB=7";
  char array_pw[SIZE_DISPLAY_PW] = "PD=_____SC";
  char array_pw_number[SIZE_PW] = "0000_";
  char array_admin[SIZE_DISPLAY_ADMIN] = "   ADO"; // ADD, DELETE, OUT
  

  char *ptr_id = array_id + LIMITE_IZQ_ID;
  char *ptr_pw = array_pw + LIMITE_IZQ_PW;
  char *ptr_admin = array_admin + LIMITE_DER_ADMIN;
  int prueba = 0;
  int cant_try = 0;
  int temporal=0;
  encResult_t joystick_input = ENC_NONE;
  tim_id_t ID_LED = timerGetId();

  //SetdispDP();


  //printf("%c", *(array_pw_number + prueba));

  int counter = 0;
  while (1)
  {
    counter++;
    if(encGetStatus()) {
    	encoderState = encGetEvent();
    	//printf("Cambie algo crack    : ");
    	//printf("%d\n",encoderState);
    }
    else{
    	encoderState = ENC_NONE;
    }

    if(encoderState == ENC_CLICK){
    	//printf("CLICK REY");
    }
    joystick_input= encoderState;

    if( mag_get_data_ready())
    {
    	write_array(array_id + LIMITE_IZQ_ID , mag_drv_read()+1 , SIZE_ID);
    	status = PASSWORD;
    	printf("%s", mag_drv_read()+1);
    }
    switch (status)
    {

    case CHANGE_BRIGHT:
      *ptr_id = encoder_control(*ptr_id, joystick_input, &status);
      //printf(" Llamar funcion de Nico");
      SetdispBrightness((int)*ptr_id - 48);
      print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0],joystick_input );
      break;

    case ID:
      ptr_id = ID_scroll(array_id, ptr_id, joystick_input, &status);
      print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0],joystick_input);

      break;

    case CHANGE_ID:
      *ptr_id = encoder_control(*ptr_id, joystick_input, &status);
      print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0],joystick_input);
      break;

    case SUBMIT:
      status = PASSWORD;
      print_display(ptr_pw[-3], ptr_pw[-2], ptr_pw[-1], ptr_pw[0],joystick_input);
      //dispSendWord(char* ch); //"ingresar pin"
      break;
    case PASSWORD:
      ptr_pw= PW_scroll(array_pw, ptr_pw,joystick_input, &status);
      print_display(ptr_pw[-3], ptr_pw[-2], ptr_pw[-1], ptr_pw[0],joystick_input);
      break;
    case CHECK_ID_PW:
      //printf("entro a chech id \n");
      //printf("password: %s \n", array_pw_number);
      //CleardispDP();
      if (user_verify(array_id + LIMITE_IZQ_ID, array_pw_number, ptr_user, cant_user))
      {
    	timerStart(ID_LED, TIMER_MS2TICKS(TIME_LED_ON), 0 , NULL);
    	ledSet(0);
    	ledSet(1);
    	ledSet(2);
    	while ( !timerExpired(ID_LED) )
    	{
    		print_display('O','P','E','N',joystick_input);
    	}
    	ledClear(0);
    	ledClear(1);
		ledClear(2);
    	status = CANCEL;
        cant_try = 0;
      }
      else if(admin_verify(array_id + LIMITE_IZQ_ID, array_pw_number, ptr_user))
      {
    	//status = ADMIN;
    	// Llamar a user add
      }
      else
      {
        cant_try++;
        if (cant_try <= CANT_TRY_BLOCK)
        {
          timerStart(ID_LED, TIMER_MS2TICKS(TIME_LED_ON), 0 , NULL);
          status = CANCEL;
          ledSet(1);
          while ( !timerExpired(ID_LED) )
          {
        	  print_display('F','A','I','L',joystick_input);
          }
          ledClear(1);
        }
        else
        {

            timerStart(ID_LED, TIMER_MS2TICKS(cant_try*TIME_LED_BLOCK), 0 , NULL);
            status = CANCEL;
            ledSet(0);
            ledSet(2);
            while ( !timerExpired(ID_LED) )
            {
          	  print_display('B','L','O','C',joystick_input);
            }
            ledClear(0);
            ledClear(2);

        }
      }
      //SetdispDP();
      break;
    case CHANGE_PW:
      prueba = (int)(ptr_pw - (array_pw + LIMITE_IZQ_PW));
      //printf("%c\n", *(array_pw_number + prueba) );
      *ptr_pw = encoder_control(*(array_pw_number + prueba), joystick_input, &status);
      *(array_pw_number + prueba) = *ptr_pw;
      if (status == PASSWORD)
      {
        *ptr_pw = '_';
      }
      print_display(ptr_pw[-3], ptr_pw[-2], ptr_pw[-1], ptr_pw[0],joystick_input);
      break;
    case CANCEL:
      status = ID;
      temporal = array_id[SIZE_DISPLAY_ID - 1];
      write_array(array_id, "ID=00000000SCB=0", SIZE_DISPLAY_ID);
      array_id[SIZE_DISPLAY_ID - 1] = temporal;
      write_array(array_pw_number, "0000_", SIZE_PASSWORD);
      ptr_id = array_id + LIMITE_IZQ_ID;
      ptr_pw = array_pw + LIMITE_IZQ_PW;
      break;
    case OPEN:
    	CleardispDP();
    	timerStart(ID_LED, TIMER_MS2TICKS(TIME_LED_ON), 0 , NULL);
    	printf("PRENDER LED\n");
    	ledSet(0);
    	ledSet(1);
    	ledSet(2);
    	while ( !timerExpired(ID_LED) )
    	{
    		print_display('O','P','E','N',joystick_input);
    	}

    	ledClear(0);
    	ledClear(1);
		ledClear(2);
    	printf("APAGAR LED\n");
    	status = CANCEL;
    	SetdispDP();
    	break;
    case ADMIN:
    	ptr_admin = ADMIN_scroll( array_admin, ptr_admin, joystick_input, &status);
    	print_display(ptr_admin[-3], ptr_admin[-2], ptr_admin[-1], ptr_admin[0],joystick_input);
    	break;
    default:
      break;
    }
    //print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0]);
  }
  free(ptr_user);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void print_display(char first, char second, char third, char fourth,encResult_t joystick_input )
{
  if (joystick_input != ENC_NONE)
  {
	  printf("%c %c %c %c", first, second, third, fourth);
	  printf("\n");
  }
  dispSendChar(first, 0);
  dispSendChar(second, 1);
  dispSendChar(third, 2);
  dispSendChar(fourth, 3);
}

static char encoder_control(char number, int joystick_input, int *status)
{

  if (joystick_input == ENC_RIGHT){

    if(number== '_')
      number = '1';
    else if (number == '9')
      number = '0';
    else
      number++;
  }
  else if (joystick_input == ENC_LEFT)
  {
    if (number == '_')
    {
      number = '9';
    }
    else if (number != '0')
    {
      number--;
    }
    else
      number = '9';
  }
  else if (joystick_input == ENC_CLICK)
  {
    if(*status==CHANGE_ID || *status==CHANGE_BRIGHT )
    {
      *status = ID;
    }
    else if (*status==CHANGE_PW)
    {
      *status = PASSWORD;
    }  
  }
  return number;
}

static User* user_init(int cant_user, User* ptr_user) // inicializa primeros usuarios con memoria dinamica
{
  ptr_user = malloc(cant_user * sizeof(User));

  write_array(ptr_user[0].id, "00000000", SIZE_ID);
  write_array(ptr_user[0].password, "1000", SIZE_PASSWORD);
  write_array(ptr_user[0].name, "PORRAS", SIZE_NAME);
  //printf("USER: %s",ptr_user[0].password );
  return ptr_user;
}
static User* user_add(int cant_user, User* ptr_user, char id[], char password[]) // inicializa primeros usuarios con memoria dinamica
{
  ptr_user = realloc(ptr_user, cant_user * sizeof(User));

  write_array(ptr_user[cant_user-1].id, id , SIZE_ID);
  write_array(ptr_user[cant_user-1].password, password, SIZE_PASSWORD);
  write_array(ptr_user[cant_user-1].name, "PORRAS", SIZE_NAME);
  //printf("USER: %s",ptr_user[0].password );
  return ptr_user;
}

static User* admin_init( User* ptr_admin)
{
	ptr_admin = malloc(sizeof(User));
    write_array(ptr_admin[0].id, "10000000", SIZE_ID);
	write_array(ptr_admin[0].password, "1200", SIZE_PASSWORD);
	write_array(ptr_admin[0].name, "PORRAS", SIZE_NAME);
	return ptr_admin;
}
static int admin_verify(char id[], char password[], User* ptr_admin)
{
	int size_pw = SIZE_PASSWORD;
    if (compare_array(id, ptr_admin[0].id, SIZE_ID))
    {
      //printf("user correct \n");
      if (password[SIZE_PASSWORD-1] == '_')
      {
    	  return INCORRECTO;
      }
      if (compare_array(password, ptr_admin[0].password, size_pw))
      {
        return CORRECTO;
      }
      else
      {
        return INCORRECTO;
      }
    }
    return INCORRECTO;
}
static int user_verify(char id[], char password[], User* ptr_user, int cant_user)
{
  //printf("ID userver: %s\n", id);
  //printf("Paddword userveri: %s\n", password);
  //printf("USER: %s \n", ptr_user[0].id);
  // verifico ID y password
  int i;
  int size_pw = SIZE_PASSWORD;
  for (i = 0; i < cant_user; i++)
  {
    if (compare_array(id, ptr_user[i].id, SIZE_ID))
    {
      //printf("user correct \n");
      if (password[SIZE_PASSWORD-1] == '_')
      {
        //printf("es de 4 \n");
        size_pw--;
      } 
      if (compare_array(password, ptr_user[i].password, size_pw))
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
  //printf("arr1:%s \n", arr1);
  //printf("arr2:%s \n", arr2);
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
static char *ADMIN_scroll(char array_admin[], char *ptr_admin, int joystick_input, int *status)
{
	if (joystick_input == ENC_RIGHT)
	  {
	    if (ptr_admin == (array_admin + LIMITE_DER_ADMIN))
	    {
	      ptr_admin = array_admin + LIMITE_IZQ_ADMIN;
	    }
	    else
	    {
	      ptr_admin++;
	    }
	  }
	else if (joystick_input == ENC_LEFT)
	  {
	    if (ptr_admin == (array_admin + LIMITE_IZQ_ADMIN))
	    {
	      ptr_admin = array_admin + LIMITE_DER_ADMIN;
	    }
	    else
	    {
	      ptr_admin--;
	    }
	  }
	else if (joystick_input == ENC_CLICK)	// " ADO"
	  {
		if (ptr_admin == array_admin + LIMITE_IZQ_ADMIN)
		{
			*status = CANCEL;
			flag_add = 1;

		}
		if (ptr_admin == array_admin + LIMITE_IZQ_ADMIN + 1)
		{
			*status = DELETE;
		}
		if (ptr_admin == array_admin+ LIMITE_DER_ADMIN)
		{
			*status = CANCEL;
		}

	  }
	  //printf("%c", ptr_id[0]);
	  return ptr_admin;
}


static char *ID_scroll(char array_id[], char *ptr_id, int joystick_input, int *status)
{

  if (joystick_input == ENC_RIGHT)
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
  else if (joystick_input == ENC_LEFT)
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
  else if (joystick_input == ENC_CLICK)
  {
    if ((ptr_id >= array_id + LIMITE_IZQ_ID) && (ptr_id <= array_id + LIMITE_IZQ_ID + 7))
    {
      *status = CHANGE_ID;
    }
    else if (ptr_id == array_id + POS_SUBMIT)
    {
      *status = PASSWORD;
    }
    else if (ptr_id == array_id + POS_CLEAR)
    {
      *status = CANCEL;
    }
    else if (ptr_id == array_id + LIMITE_DER_ID)
    {
      *status = CHANGE_BRIGHT;
    }

  }
  //printf("%c", ptr_id[0]);
  return ptr_id;
}

//static char *mag_get_ID(void)
//{
// return "12345678";
//}

int simulacion(void)
{
  static int counter = -1;
  counter++;
  // printf("%d \n ", counter);
  switch (counter)
  {
     case 0:
        return CLICK; // 0
        break;
     case 1:
       return CLICK; // CAMBIO NUM0
       break;
     case 2:
       return DERECHA; // paso a num1
       break;
     case 3:
       return CLICK; // quiero cambiar primer numero
       break;
     case 4:
       return DERECHA; // 1
       break;
     case 5:
       return CLICK; // CAMBIO NUM1
       break;
     case 6:
       return DERECHA; // paso a num2
       break;
     case 7:
       return CLICK; // quiero cambiar primer numero
       break;
     case 8:
       return DERECHA; //1
       break;
     case 9:
       return DERECHA;//2
       break;
     case 10:
       return CLICK; // MODIFICO NUM2
       break;
     case 11:
       return DERECHA; // paso a num3
       break;
     case 12:
       return CLICK; // quiero cambiar num3
       break;
     case 13:
       return DERECHA; // 1
       break;
     case 14:
       return DERECHA; // 2
       break;
     case 15:
       return DERECHA; // 3
       break;
     case 16:
       return CLICK; // cambio num3
       break;
     case 17:
       return DERECHA; // muevo al NUM4
       break;
     case 18:
       return DERECHA; // muevo a NUM5
       break;
     case 19:
       return DERECHA; // MUEVO A NUM6
       break;
     case 20:
       return DERECHA; // muevo a NUM7
       break;
     case 21:
       return DERECHA; // MUEVO A SUBMIT
       break;
     case 22:
       return CLICK; // paso a password
       break;
     case 23:
       return CLICK; // MODIFICO NUM1
       break;
     case 24:
       return DERECHA; // 1
       break;
     case 25:
       return CLICK; // MODIFICO NUM1
       break;
     case 26:
       return DERECHA; // muevo al segundo numero
       break;
     case 27:
       return DERECHA; // muevo al 3ER numero
       break;
      case 28:
        return DERECHA; // muevo al 4TO numero
        break;
      case 29:
          return DERECHA; // muevo al 5TO numero
          break;
      case 30:
          return CLICK;   // modifico el 5to num
          break;
      case 31:
          return DERECHA; // 1
          break;
      case 32:
        return CLICK; // modifico el 5num
        break;
      case 33:
          return DERECHA; // muevo al SUBMIY numero
          break;
      case 34:
          return CLICK; // chequeo
          break;
      case 35:
          return DERECHA; // XD
          break;
  }
  /*switch (counter)
  {
  case 0:
    return CLICK; //quiero cambiar primer numero
    break;
  case 1:
    return DERECHA; 
    break;
  case 2:
    return DERECHA;
    break;
  case 3:
    return CLICK; //cambie primer numero
    break;
  case 4:
    return DERECHA; // muevo al segundo numero
    break;
  case 5:
    return DERECHA; // muevo al tercer numero
    break;
  case 6:
    return CLICK; // quiero cambiar tercer numero
    break;
  case 7:
    return IZQUIERDA; // paso del 0 al 9
    break;
  case 8:
    return CLICK; // cambio tercer numero
    break;
  case 9:
    return DERECHA; // paso al 4to
    break;
  case 10:
    return DERECHA; // paso al 5to
    break;
  case 11:
    return DERECHA; // paso al 6to
    break;
  case 12:
    return DERECHA; //paso al 7
    break;
  case 13:
    return DERECHA; //paso al 8
    break;
  case 14:
    return DERECHA; // paso al S
    break;
  case 15:
    return DERECHA; // paso al C
    break;
  case 16:
    return DERECHA; //paso al brillo (salteo B=)
    break;
  case 17:
    return CLICK; //quiero cambiar brillo
    break;
  case 18:
    return DERECHA;//
    break;
  case 19:
    return DERECHA;//
    break;
  case 20:
    return CLICK; // cambio brillo incremmentado en 2
    break;
  case 21:
    return IZQUIERDA; // vuelvo a C
    break;
  case 22:
    return IZQUIERDA; // vuelvo a S
    break;
  case 23:
    return CLICK; // paso a password 
    break;

  case 24:
    return CLICK; // quiero cambiar primer numero
    break;
  case 25:
    return DERECHA;//1
    break;
  case 26:
    return DERECHA;//2
    break;
  case 27:
    return DERECHA;//3
    break;
  case 28:
    return CLICK;//lo cambio a 3
    break;
  case 29:
    return DERECHA; // paso al numero 2
    break;
  case 30:
    return DERECHA;//paso al numero 3
    break;
  case 31:
    return CLICK; //queiro cambiar el num 3
    break;
  case 32:
    return IZQUIERDA;// 9
    break;
  case 33:
    return CLICK; //cambio num3 a 9
    break;
  case 34:
    return DERECHA; // paso al 4
    break;
  case 35:
    return DERECHA;// paso al 5
    break;
  case 36:
    return DERECHA;// paso a S
    break;
  case 37:
    return DERECHA;//paso a C
    break;
  default:
    return CLICK; //hago un clear y vuelvo a ID
    break;
  }*/
  return 0;
}

static char *PW_scroll(char array_pw[], char *ptr_pw, int joystick_input, int *status)
{

  if (joystick_input == ENC_RIGHT)
  {

    if (ptr_pw == (array_pw + LIMITE_DER_PW))
    {
      ptr_pw = array_pw + LIMITE_IZQ_PW;
    }
    else
    {
      ptr_pw++;
    }
  }
  else if (joystick_input == ENC_LEFT)
  {
    if (ptr_pw== (array_pw + LIMITE_IZQ_PW))
    {
      ptr_pw = array_pw + LIMITE_DER_PW;
    }
    else
    {
      ptr_pw--;
    }
  }
  else if (joystick_input == ENC_CLICK)
  {
   // printf("click \n");
    if ((ptr_pw >= array_pw + LIMITE_IZQ_PW) && (ptr_pw <= array_pw + LIMITE_IZQ_PW + 4))
    {
      *status = CHANGE_PW;
      //printf("cambio \n ");
    }
    else if (ptr_pw == array_pw + POS_CHECK_PW)
    {
      printf("cambio estado \n");
      if ( !flag_add )				// si no esta en modo admin
      {
    	  *status = CHECK_ID_PW;
      }
      else
      {
    	  *status = ADD;
      }

    }
    else if (ptr_pw == array_pw + LIMITE_DER_PW)
    {
      *status = CANCEL;
    }
    //printf("%d \n", *status);
  }
  return ptr_pw;
}
