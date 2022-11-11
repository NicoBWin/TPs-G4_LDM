/***************************************************************************//**
  @file     encoder.c
  @brief    Driver encoder
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "../../timer/timer.h"
#include "../headers/encoder.h"
#include "../../MCAL/gpio.h"
#include "../board.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
//El switch tiene un pullup externo
#define SWACTIVE       LOW

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
//Estados de la Encoder FSM
enum states {START, CW, CW1, CW2, CCW, CCW1, CCW2};
//Son 3 estados porque analizamos 3 flancos

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void encoCallback(void);
static void readPins(void);
static encResult_t encStatus(bool A, bool B, bool SW);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
// Pin Status
static bool PINA;
static bool PINB;
static bool PINSW;

//Data de encoder
static bool status;
static encResult_t encEvent;

//Timer para el encoder
static tim_id_t encTimer;

//Semaphore
static OS_SEM *Sem;
static OS_ERR enc_err;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
/**
 * @brief Initialize encoder
 */
void encInit(OS_SEM *EncSem) {
  //Inicializo Data
  encEvent = ENC_NONE;
  status = false;

  //Seteo los Pines
  gpioMode(PIN_RCHA, INPUT);
  gpioMode(PIN_RCHB, INPUT);
  gpioMode(PIN_SW, INPUT);

  //Seteo el puntero del semáforo
  Sem = EncSem;

  //Seteo el timer para que llame periodicamente a la callback con 1ms
  encTimer = timerGetId();
  timerStart(encTimer, TIMER_MS2TICKS(1), TIM_MODE_PERIODIC, &encoCallback);
}

/**
 * @brief Check for new encoder events
 * @return true if there is an encoder event
 */
/*bool encGetStatus() {
	if(status) {
		status = false;
		return true;
	}
	else {
		return false;
	}
}*/ //NO SE USARÍA CON SEMAFOROS

/**
 * @brief Get event type
 * @return event
 */
encResult_t encGetEvent() {
  return encEvent;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void encoCallback(void){
  readPins();
  encEvent = encStatus(PINA, PINB, PINSW);
  if(status){
	  status = false;
	  OSSemPost(Sem, OS_OPT_POST_ALL, &enc_err);
  }
}

// Lee todos los pins
static void readPins(void) {
  PINA = gpioRead(PIN_RCHA);
  PINB = gpioRead(PIN_RCHB);
  PINSW = gpioRead(PIN_SW);
}

// FSM para reconocer el giro del encoder y el SW
static encResult_t encStatus(bool A, bool B, bool SW){
  encResult_t result = ENC_NONE;
  static enum states encFSM = START;

  //SWITCH
  static bool lastSW = false;
  bool currentSW;
  currentSW = SW == SWACTIVE;
  if(!lastSW && currentSW) {
   	result = ENC_CLICK;
   	status = true;
  }
  lastSW = currentSW;

  //ENCODER FSM
  switch(encFSM) {
    //INICIO
    case START:
      if (A && !B) {
        encFSM = CW;
      }
      if (!A && B) {
        encFSM = CCW;
      }
    break;
    case CW:
      if(!A && !B){
          encFSM = CW1;   //Sigo girando
      }
      else if(A && !B){   //"Me quedo quieto"
      }
      else{
          encFSM = START; //Volvi para atras
      }
    break;
    case CW1:
      if(!A && B){
        encFSM = CW2;     //Sigo girando
      }
      else if(!A && !B){  //"Me quedo quieto"
      }
      else{
          encFSM = START; //Volvi para atras
      }
    break;
    case CW2:
      if(A && B){         //Termine de girar ClockWise
          encFSM = START;
          result = ENC_RIGHT;
          status = true;
      } 
      else if(!A && B){   //"Me quedo quieto"
      } 
      else{
          encFSM = START; //Volvi para atras
      }
    break;
    case CCW:
      if(!A && !B){       //Sigo girando
          encFSM = CCW1;
      }
      else if(!A && B){ 	//"Me quedo quieto"
      }
      else{
    	 encFSM = START;   //Volvi para atras
      }
    break;
    case CCW1:
      if(A && !B){
        encFSM = CCW2;    //Sigo girando
      }
      else if(!A && !B){  //"Me quedo quieto"
      }
      else{
        encFSM = START;   //Volvi para atras
      }
    break;
    case CCW2:
      if(A && B){         //Termine de girar Counter ClockWise
        encFSM = START;
        result = ENC_LEFT;
        status = true;
      } 
      else if(A && !B){   //"Me quedo quieto"
      }
      else {
        encFSM = START;   //Volvi para atras
      }
    break;
    default: break;
  }
  return result;
}
