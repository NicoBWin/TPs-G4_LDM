/***************************************************************************/ /**
@file     App.c
@brief    Application functions
@author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
// Drivers
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
enum status   //estados de la interfaz principal 
{
  CHANGE_BRIGHT, //cambia el indicador del brillo de 0 a 9
  ID,             // menu principal donde se ingresa el ID 
  CHANGE_ID,      // cambia de a un numero del ID
  SUBMIT,         // submit despues de cargar el ID, pasa a PASSWORD
  CANCEL,         // vuelve al estado ID y cancela la entrada de datos
  PASSWORD,       // menu de ingreso del PIN una vez que se ingreso el ID
  OPEN,           // si el ID y el PIN son correctos abre la puerta por 5 segundos
  BLOCK,          // si el PIN es erroneo bloquea el ingreso exponencialmente
  CHANGE_PW,       // cambia cada digito del PIN
  CHECK_ID_PW,      // chequea que el ID y el PIN si es correcto o no
  ADMIN,            // menu de administrador para poder agregar o borrar usuarios
  ADD,              // agrega usuarios
  DELETE            // borra usuarios
};

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static int compare_array(char arr1[], char arr2[], int size); // Compara dos arreglos
static void write_array(char arr[], char arr_copy[], int size);		// Copia un arreglo en otro
static int user_verify(char id[], char password[], User *ptr_user, int cant_user); // Compara el ID y PASSWORD ingresadas con las de la base de datos existente de usuarios
static int admin_verify(char id[], char password[], User* ptr_admin); // Compara el ID y PASSWORD ingresadas con las de la base de datos existente de administradores
static User* user_init(int cant_user, User* ptr_user); // Inicializa la base de datos de usuarios 
static User* admin_init( User* ptr_admin); // Inicializa base de datos de administradores 
static char encoder_control(char number, int joystick_input, int *status); // Scrollea los numeros que se seleccionan tanto para el ID como la PASSWORD
static void print_display(char first, char second, char third, char fourth,encResult_t joystick_input ); // Imprime los datos en los display de 7 segmentos
static char *ID_scroll(char array_id[], char *ptr_id, int joystick_input, int *status); // Navega en el menu del ingreso de ID 
//static char *mag_get_ID(void); // devuelve un string del campo de datos 

static char *PW_scroll(char array_pw[], char *ptr_pw, int joystick_input, int *status); // Navega en el menu del ingreso de PASSWORD

static char *ADMIN_scroll(char array_admin[], char *ptr_admin, int joystick_input, int *status); // Navega en el menu del administrador
static User* user_add(int cant_user, User* ptr_user, char id[], char password[]); // Agrega el ID y contraseña ingresada a la base de datos dinámica de usuarios 

static char cant_piso1 = 0;
static char cant_piso2 = 0;
static char cant_piso3 = 0;
static char buffer_piso[6] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
/* Semaphores */
static OS_SEM EncSem;
static OS_SEM MagSem;
static OS_SEM TimerSem;

/* Messege Queue */
static OS_Q *AppMQ;

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
// +ej: static int temperaturas_actuales[4];+
static encResult_t encoderState;	// 
static int flag_add = 0;	// Indica que el administrador esta añadiendo un usuario a la base de datos
static int cant_admin = 2;	// Indica la cantidad de administradores 

static OS_ERR app_err;
static OS_ERR enc_err;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/* Todos los init necesarios */
void App_Init(OS_Q *ComQ) {

	OSSemCreate(&TimerSem, "Timer Sem", 0u, &app_err);
	timerInit();		// Inicializa timers

	/* Create semaphore */
	OSSemCreate(&EncSem, "Enc Sem", 0u, &app_err);
	encInit(&EncSem);		// Inicializa encoder
	dispInit();		// Inicializa Display
	ledsInit();		// Inicializa Leds

	/* Create semaphore */
	// Hay que pasarle el puntero al semaforo para que mag lo pueda usar cuando tenga info lista
	// El semaforo reemplazaría a mag_get_data_ready(); VER IMPLEMENTACION SIMILAR EN ENCODER
	OSSemCreate(&MagSem, "Mag Sem", 0u, &app_err);
	mag_drv_INIT(&MagSem);	// Inicializa lector de tarjeta magnetica

	AppMQ = ComQ;
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run(void) {
  User *ptr_user;	// Apunta a la base de datos de usuarios
  User *ptr_administradores;   // Apunta a la base de datos de administradores
  int cant_user = 6;		// Cantidad de usuarios

  //incializa usuarios y administradores
  ptr_user = user_init(cant_user, ptr_user);
  ptr_administradores = admin_init(ptr_administradores);
  SetdispBrightness(7);
  
  // Inicializa sistema con valores por defecto
  // dispInit();
  static int status = ID;
  static int next_status = ID;
  static int prev_status = ID;
  // int status_number = CERO;
  // int status_bright = NINE;
  char array_id[SIZE_DISPLAY_ID] = "ID=00000000SCB=7"; //arreglo del menu del ID
  char array_pw[SIZE_DISPLAY_PW] = "PD=_____SC"; // arreglo del menu del PIN
  char array_pw_number[SIZE_PW] = "0000_"; //arreglo solo con el PIN sin el menu y teniendo en cuenta el ultimo caracter si es de 4 o 5
  char array_admin[SIZE_DISPLAY_ADMIN] = "   ADO"; // ADD, DELETE, OUT
  

  char *ptr_id = array_id + LIMITE_IZQ_ID;	// Este puntero actuará como el cursor que el usuario utilizará. Este puntero siempre estará en el 4to display de 7 seg
  char *ptr_pw = array_pw + LIMITE_IZQ_PW;	// Este puntero actuará como el cursor que el usuario utilizará. Este puntero siempre estará en el 4to display de 7 seg
  char *ptr_admin = array_admin + LIMITE_DER_ADMIN; // Este puntero actuará como el cursor que el usuario utilizará. Este puntero siempre estará en el 4to display de 7 seg
  int prueba = 0;
  int cant_try = 0;	// Variable que guarda la cantidad de errores seguidos que comete el usuario al ingresar la contraseña
  int temporal=0;
  encResult_t joystick_input = ENC_NONE; // Variable que recibe los estados del encoder 
  tim_id_t ID_LED = timerGetId(); // ID que se usará para el tiempo que estan prendidos los leds

  static OS_SEM_CTR EncSt;
  int counter = 0;


  //For multi Pend
  OS_PEND_DATA pend_data_tbl[3];

  while (1)
  {
    counter++;

    // Maquina de estados
    do
        {
        	status = next_status;
        	switch (status)
        	    {

        	    case CHANGE_BRIGHT:		// El usuario clickeo el cambio de brillo
        	      *ptr_id = encoder_control(*ptr_id, joystick_input, &next_status);	// Modifico el numero del brillo y si clickea vuelvo a estado ID
        	      SetdispBrightness((int)*ptr_id - 48);	// Setea brillo
        	      print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0],joystick_input );
        	      break;

        	    case ID:
        	      ptr_id = ID_scroll(array_id, ptr_id, joystick_input, &next_status);
        	      print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0],joystick_input);

        	      break;

        	    case CHANGE_ID:
        	      *ptr_id = encoder_control(*ptr_id, joystick_input, &next_status);	// Cambio numero de ID
        	      print_display(ptr_id[-3], ptr_id[-2], ptr_id[-1], ptr_id[0],joystick_input);
        	      break;

        	    case SUBMIT:				// Estado que al final no se utiliza ( no se lo elimina pues por el feriado no podemos testear )
        	      status = PASSWORD;
        	      print_display(ptr_pw[-3], ptr_pw[-2], ptr_pw[-1], ptr_pw[0],joystick_input);
        	      //dispSendWord(char* ch); //"ingresar pin"
        	      break;
        	    case PASSWORD:				// Ingresa contraseña
        	      ptr_pw= PW_scroll(array_pw, ptr_pw,joystick_input, &next_status);
        	      print_display(ptr_pw[-3], ptr_pw[-2], ptr_pw[-1], ptr_pw[0],joystick_input);
        	      break;
        	    case CHECK_ID_PW:				// Se llama al ingresar un ID y Password

        	      if (user_verify(array_id + LIMITE_IZQ_ID, array_pw_number, ptr_user, cant_user))	// Se ingreso un ID y PW perteneciente a un usuario
        	      {
        	    	timerStart(ID_LED, TIMER_MS2TICKS(TIME_LED_ON), 0 , NULL);
        	    	ledSet(0);
        	    	ledSet(1);		// Prende los LEDS
        	    	ledSet(2);
        	    	while ( !timerExpired(ID_LED) ) // bloquea el programa por 5 segundos por la abertura de la puerta
        	    	{
        	    		print_display('O','P','E','N',joystick_input);
        	    	}
        	    	ledClear(0);
        	    	ledClear(1);		// Apaga LEDS
        	    	ledClear(2);
        			next_status = CANCEL;	// Se reinicia y bloquea puerta de vuelta
        	        cant_try = 0;		// COmo hacerto se reinicia la variable de cantidad de intentos fallidos consecutivos
        	      }
        	      else if(admin_verify(array_id + LIMITE_IZQ_ID, array_pw_number, ptr_administradores)) // Se ingreso un ID y PW perteneciente a un administrador
        	      {
        	      	timerStart(ID_LED, TIMER_MS2TICKS(TIME_GOD), 0 , NULL);
        	      	ledSet(0);
        	      	ledSet(1); 		// Prende los LEDS
        	      	ledSet(2);
        	      	while ( !timerExpired(ID_LED) )
        	      	{
        	      		print_display('G','O','D',' ',joystick_input);	// Indica que entro al menú del adminitrador
        	      	}
        	    	ledClear(0);
        	    	ledClear(1);		// Apaga los LEDS
        	        ledClear(2);
        	        next_status = ADMIN;		// Ingresa al modo administrador
        	    	// Llamar a user add
        	      }
        	      else			// No se a ingresado un ID y PASSWORD correspondiente a ningun usuario ni administrador
        	      {
        	        cant_try++;		// Aumenta contador de intentos fallidos
        	        if (cant_try <= CANT_TRY_BLOCK)		// Si no superó una cantidad de intentos determinados ( Se eligió CANT_TRY_BLOCK = 2 para poder testear rapidamente )
        	        {
        	          timerStart(ID_LED, TIMER_MS2TICKS(TIME_LED_ON), 0 , NULL); //
        	          next_status = CANCEL;
        	          ledSet(1);
        	          while ( !timerExpired(ID_LED) )
        	          {
        	        	  print_display('F','A','I','L',joystick_input);
        	          }
        	          ledClear(1);
        	        }
        	        else
        	        {
        		// Si supero una cantidad de intentos determinados, blockea el ingreso por una cantidad de tiempo que aumento cuanto mas cantidad de veces falle
        	            timerStart(ID_LED, TIMER_MS2TICKS(cant_try*TIME_LED_BLOCK), 0 , NULL);
        	            next_status = CANCEL;
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
        	    case CHANGE_PW:	// Cambio el numero de la contraseña
        	      prueba = (int)(ptr_pw - (array_pw + LIMITE_IZQ_PW));

        	      *ptr_pw = encoder_control(*(array_pw_number + prueba), joystick_input, &next_status);
        	      *(array_pw_number + prueba) = *ptr_pw;
        	      if (next_status == PASSWORD)
        	      {
        	        *ptr_pw = '_';  // Vuelve a poner un guion si se hizo click, asi no se ve la contraseña ingresada
        	      }
        	      print_display(ptr_pw[-3], ptr_pw[-2], ptr_pw[-1], ptr_pw[0],joystick_input);
        	      break;
        	    case CANCEL:	// Reinicia todo y vuelve al estado inicial
        	      next_status = ID;
        	      temporal = array_id[SIZE_DISPLAY_ID - 1];
        	      write_array(array_id, "ID=00000000SCB=0", SIZE_DISPLAY_ID);
        	      array_id[SIZE_DISPLAY_ID - 1] = temporal;
        	      write_array(array_pw_number, "0000_", SIZE_PASSWORD);
        	      ptr_id = array_id + LIMITE_IZQ_ID;
        	      ptr_pw = array_pw + LIMITE_IZQ_PW;
        	      break;
        	    case OPEN:		// Se abrio la puerta
        	    	CleardispDP();
        	    	timerStart(ID_LED, TIMER_MS2TICKS(TIME_LED_ON), 0 , NULL);	// prende LEDs por 5 segundo
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
        	    	next_status = CANCEL;

        	    	SetdispDP();

        	    	break;
        	    case ADMIN:		// Entra al modo Admin
        	    	ptr_admin = ADMIN_scroll( array_admin, ptr_admin, joystick_input, &next_status);
        	    	print_display(ptr_admin[-3], ptr_admin[-2], ptr_admin[-1], ptr_admin[0],joystick_input);
        	    	break;

        	    case ADD:		// Añado el id y password ingresado
        	  	  cant_user++;
        	  	  ptr_user = user_add( cant_user,  ptr_user, array_id + LIMITE_IZQ_ID, array_pw_number);
        	  	  timerStart(ID_LED, TIMER_MS2TICKS(TIME_GOD), 0 , NULL);
        	  	  print_display('U','A','D','D',joystick_input); // user add
        	  	  while ( !timerExpired(ID_LED) )
        	    	{

        	    	}
        	  	  next_status = CANCEL;
        	  	  flag_add = 0;

        	    	break;
        	    case DELETE:	// Elimina usuario
        	    	if(cant_user>0)
        	    		cant_user--; // Decremente la cantidad de usuarios
        	    	next_status = CANCEL;
        	    default:
        	      break;
        	    }
        		joystick_input = ENC_NONE;
        }
        while( status != next_status );

    pend_data_tbl[0].PendObjPtr = (OS_PEND_OBJ *) &EncSem;
    pend_data_tbl[1].PendObjPtr = (OS_PEND_OBJ *) &MagSem;
    pend_data_tbl[2].PendObjPtr = (OS_PEND_OBJ *) &TimerSem;
    OSPendMulti(&pend_data_tbl[0], 2, 0, OS_OPT_PEND_BLOCKING, &app_err);

    // se comunica con el encoder para saber si se acciono y que es lo que se acciono
    if(encGetStatus()) {
    	encoderState = encGetEvent();	// Cambio el encoder
    }
    else{
    	encoderState = ENC_NONE;        // El usuario no realizó movimiento
    }
    //Se comunica con la lectora de tarjetas para saber si se paso una tarjeta y levantar los numeros de la misma
    if( mag_get_data_ready() && status == ID) {	// Si el usuario paso la tarjeta relleno el usuario con los numeros de la tarjeta
    	write_array(array_id + LIMITE_IZQ_ID , mag_drv_read()+1+8 , SIZE_ID);
    	status = PASSWORD;	// EL usuario debe ingresar la password
    }

    joystick_input= encoderState;
  }
  free(ptr_user);
  free(ptr_administradores);	// Libero la memoria dinamica
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void print_display(char first, char second, char third, char fourth,encResult_t joystick_input )	
{
  dispSendChar(first, 0);
  dispSendChar(second, 1);
  dispSendChar(third, 2);
  dispSendChar(fourth, 3);
}

static char encoder_control(char number, int joystick_input, int *status) // Modifica los numeros del ID, PW
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
  // Usuarios del piso 1
  write_array(ptr_user[0].id, "10000000", SIZE_ID);
  write_array(ptr_user[0].password, "1000_", SIZE_PASSWORD);
  ptr_user[0].piso = 0x1;
  ptr_user[0].inside = 0;

  write_array(ptr_user[1].id, "53450411", SIZE_ID);
  write_array(ptr_user[1].password, "1000_", SIZE_PASSWORD);
  ptr_user[1].piso = 0x1;
  ptr_user[1].inside = 0;
  // Usuarios del piso 2
  write_array(ptr_user[2].id, "30782716", SIZE_ID);
  write_array(ptr_user[2].password, "1000_", SIZE_PASSWORD);
  ptr_user[2].piso = 0x2;
  ptr_user[2].inside = 0;

  write_array(ptr_user[3].id, "40000000", SIZE_ID);
  write_array(ptr_user[3].password, "1000_", SIZE_PASSWORD);
  ptr_user[3].piso = 0x2;
  ptr_user[3].inside = 0;
  // Usuarios del piso 3
  write_array(ptr_user[4].id, "50000000", SIZE_ID);
  write_array(ptr_user[4].password, "1000_", SIZE_PASSWORD);
  ptr_user[4].piso = 0x3;
  ptr_user[4].inside = 0;

  write_array(ptr_user[5].id, "34950962", SIZE_ID);
  write_array(ptr_user[5].password, "1000_", SIZE_PASSWORD);
  ptr_user[5].piso = 0x3;
  ptr_user[5].inside = 0;
  //write_array(ptr_user[0].name, "PORRAS", SIZE_NAME);
  //printf("USER: %s",ptr_user[0].password );
  return ptr_user;
}


static User* user_add(int cant_user, User* ptr_user, char id[], char password[]) // inicializa primeros usuarios con memoria dinamica
{
  ptr_user = realloc(ptr_user, cant_user * sizeof(User));

  write_array(ptr_user[cant_user-1].id, id , SIZE_ID);
  write_array(ptr_user[cant_user-1].password, password, SIZE_PASSWORD);
  ptr_user[cant_user-1].piso = 0x1;
  // write_array(ptr_user[cant_user-1].piso, 0x0 , SIZE_NAME); // Fijo a que piso corresponde el usuario
  //printf("USER: %s",ptr_user[0].password );
  return ptr_user;
}


static User* admin_init( User* ptr_admin)    // Inicializa los primeros administradores con memoria dinamica
{
    // El administración pertenece al piso 0
    ptr_admin = malloc(2*sizeof(User));
    write_array(ptr_admin[0].id, "94950962", SIZE_ID);
    write_array(ptr_admin[0].password, "1959_", SIZE_PASSWORD);
    ptr_admin[0].piso = 0x0;
    //write_array(ptr_admin[0].piso, 0x1, SIZE_NAME);

    write_array(ptr_admin[1].id, "44546438", SIZE_ID);
    write_array(ptr_admin[1].password, "1111_", SIZE_PASSWORD);
    ptr_admin[1].piso = 0x0;
    //write_array(ptr_admin[1].piso, 0x2 , SIZE_NAME);
    return ptr_admin;
}


static int admin_verify(char id[], char password[], User* ptr_admin)	// Verifico que sea administrador
{
	int size_pw = SIZE_PASSWORD;
	for (int i = 0; i < cant_admin; i++)
	  {
		if (compare_array(id, ptr_admin[i].id, SIZE_ID))
		{
		  //printf("user correct \n");
		  if (password[SIZE_PASSWORD-1] == '_')
			 {
			  //printf("es de 4 \n");
				size_pw--;
				if(ptr_admin[i].password[SIZE_PASSWORD-1] != '_' )
				{
					return INCORRECTO;
				}
			 }
		  if (compare_array(password, ptr_admin[i].password, size_pw))
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

static int user_verify(char id[], char password[], User* ptr_user, int cant_user)    // Verifico que sea usuario
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
        if(ptr_user[i].password[SIZE_PASSWORD-1] != '_' )
        {
            return INCORRECTO;
        }
      } 
      if (compare_array(password, ptr_user[i].password, size_pw))
      {

        if ( ptr_user[i].piso == 0x1 )
        {
            if(ptr_user[i].inside)
            {
                cant_piso1--;
                ptr_user[i].inside = 0;
            }
            else
            {
                cant_piso1++;
                ptr_user[i].inside = 1;
            }
            buffer_piso[0] = cant_piso1;
        }
        else if ( ptr_user[i].piso == 0x2 )
        {
            if(ptr_user[i].inside)
            {
                cant_piso2--;
                ptr_user[i].inside = 0;
            }
            else
            {
                cant_piso2++;
                ptr_user[i].inside = 1;
            }
            buffer_piso[2] = cant_piso2;
        }
        else if ( ptr_user[i].piso == 0x3 )
        {
            if(ptr_user[i].inside)
            {
                cant_piso3--;
                ptr_user[i].inside = 0;
            }
            else
            {
                cant_piso3++;
                ptr_user[i].inside = 1;
            }
            buffer_piso[4] = cant_piso3;
        }
        // buffer_piso = {cant_piso1, 0x0, cant_piso2, 0x0, cant_piso3, 0x0};

        OSQPost(AppMQ, &buffer_piso, sizeof(buffer_piso), OS_OPT_POST_FIFO + OS_OPT_POST_ALL, &app_err);

        return CORRECTO;
      }
      else {
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
static char *ADMIN_scroll(char array_admin[], char *ptr_admin, int joystick_input, int *status)	// Menú del Admin 
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


static char *ID_scroll(char array_id[], char *ptr_id, int joystick_input, int *status)	// Menú del ID
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



static char *PW_scroll(char array_pw[], char *ptr_pw, int joystick_input, int *status)	// Menu de password
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
