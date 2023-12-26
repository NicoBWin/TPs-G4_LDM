/***************************************************************************//**
  @file     timer.c
  @brief    Driver del Timer
  @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include <stdbool.h>

#include "../MCAL/gpio.h"
#include "../Pdrivers/board.h"

#include "timer.h"
#include "SysTick.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define TIMER_DEVELOPMENT_MODE    0
#define TIMER_ID_INTERNAL   0 //ID del timer bloqueante reservado dentro del driver

#define TIMERS_MAXCANT 35  // Maxima cantidad de timers en simultaneo

#define TIMER_RUNNING 1
#define TIMER_STOPED 0

#define TIMER_EXPIRED 0
#define TIMER_NOT_EXPIRED 1

#define END_OF_TIMER 0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
    ttick_t             period;             // ticks hasta expiración
	ttick_t             cnt;                // ticks transcurridos
    tim_callback_t      callback;
    uint8_t             mode        : 1;    //TIM_MODE_SINGLESHOT and TIM_MODE_PERIODIC
    uint8_t             running     : 1;    //TIMER_RUNNING and TIMER_STOPED
    uint8_t             expired     : 1;    //TIMER_EXPIRED and TIMER_ALIVE
    uint8_t             unused      : 5;
} timer_t;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
/**
 * @brief Periodic service
 */
static void timer_isr(void);

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
static timer_t timers[TIMERS_MAXCANT];
static tim_id_t timers_cant = TIMER_ID_INTERNAL+1;

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void timerInit(void) {
    gpioMode(PIN_IRQ, OUTPUT);
	gpioWrite(PIN_IRQ, LOW);

    SysTick_Init(timer_isr); // Init SysTick preiferico
}


tim_id_t timerGetId(void) {
    #ifdef TIMER_DEVELOPMENT_MODE
        if (timers_cant >= TIMERS_MAX_CANT)
        {
            return TIMER_INVALID_ID;
        }
        else
    #endif // TIMER_DEVELOPMENT_MODE
        {
            return timers_cant++;
        }
}


void timerStart(tim_id_t id, ttick_t ticks, uint8_t mode, tim_callback_t callback) {
#if TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (mode < CANT_TIM_MODES) && (id >= 0))
#endif // TIMER_DEVELOPMENT_MODE
    {
    	timers[id].running = 0b0;

    	//Timer init default config
        timers[id].period = ticks;
        timers[id].cnt = ticks;
        timers[id].mode = mode;
        timers[id].callback = callback;
        timers[id].expired = 0b0;

        timers[id].running = 0b1;
    }
}


void timerStop(tim_id_t id)
{
    // Apago el timer
    timers[id].running = 0b0;

    // y bajo el flag
    timers[id].expired=0b0;
}

//Set timer on TIMER_RUNNING if it was previously stopped
void timerResume(tim_id_t id)
{
#if TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (id > 0))
#endif
        timers[id].running = TIMER_RUNNING;
}

//Reset ticks count on a timer
void timerReset(tim_id_t id)
{
#if TIMER_DEVELOPMENT_MODE
    if ((id < timers_cant) && (id > 0))
#endif
    {
        timers[id].cnt = timers[id].period;
        timers[id].expired = TIMER_NOT_EXPIRED;
        timers[id].running = TIMER_RUNNING;
    }
}

bool timerExpired(tim_id_t id) {
	return timers[id].expired;
}

//Blocking delay
void timerDelay(ttick_t ticks)
{
    timerStart(TIMER_ID_INTERNAL, ticks, TIM_MODE_SINGLESHOT, NULL);
    while (!timerExpired(TIMER_ID_INTERNAL)) {
        //timers[TIMER_ID_INTERNAL].cnt-=1;
    }
}

uint8_t isTimerPaused(tim_id_t id)
{
	#if TIMER_DEVELOPMENT_MODE
		if ((id < timers_cant) && (id >= 0))
	#endif
		{
			if (timers[id].running == TIMER_RUNNING) {
				return false;
			}
			else {
				return true;
			}
		}
	return false;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
static void timer_isr(void) {
	for(tim_id_t id=TIMER_ID_INTERNAL; id<TIMERS_MAXCANT; id++){

	  // decremento los timers activos y si hubo timeout!
	  if(timers[id].running && !(--timers[id].cnt)){
		// 1) execute action: callback or set flag
		if (timers[id].callback != NULL){
		  (*timers[id].callback)();
		}
		timers[id].expired=0b1;

		// 2) update state
		if(timers[id].mode){
			timers[id].cnt=timers[id].period;
		}
		else{
			timers[id].running=0;
		}
	  }
	}
}
