/***************************************************************************/ /**
   @file     main.c
   @brief    Main μC/OS-III
   @author   Grupo 4 (Bustelo, Mangone, Porras, Terra)
  ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "hardware.h"
#include "App.h"
#include "MCAL/gpio.h"
#include "drivers/board.h"
#include  <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			3u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              2u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/* Task 3 */
#define TASK3_STK_SIZE			256u
#define TASK3_STK_SIZE_LIMIT	(TASK3_STK_SIZE / 10u)
#define TASK3_PRIO              2u
static OS_TCB Task3TCB;
static CPU_STK Task3Stk[TASK3_STK_SIZE];

/* Semaphore */
static OS_SEM MainSem;
static OS_SEM UartSem;

/* Messege Queue */
static OS_Q ComQ;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void TaskStart(void *p_arg);
static void Task2(void *p_arg);
static void Task3(void *p_arg);

/*******************************************************************************
 * MAIN
 ******************************************************************************/
int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();
    OSInit(&err);

    hw_DisableInterrupts();
    /*---------------------------------------------------*/

 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);

    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  TaskStart,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);

    OSQCreate (&ComQ,
    			"Main MQ",
				12,
				&err);		// 12 Bytes de tamaño de queue

    // APP init
    App_Init(&ComQ);

	// UART init
    OSSemCreate(&UartSem, "UARTRx Sem", 0u, &err);
	uart_cfg_t config = {.baudrate = UARTBAUDRATE, .parity = NO_PARITY_UART};
	uartInit(UARTID, config, &UartSem);
    gpioMode(PIN_LED_BLUE, OUTPUT);
    gpioMode(PIN_LED_RED, OUTPUT);
    gpioMode(PIN_LED_GREEN, OUTPUT);


    /*---------------------------------------------------*/
    hw_EnableInterrupts();

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {
    }
}


/*******************************************************************************
 * TASKs
 ******************************************************************************/
static void TaskStart(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    /* Initialize the uC/CPU Services. */
    CPU_Init();

#if OS_CFG_STAT_TASK_EN > 0u
    /* (optional) Compute CPU capacity with no task running */
    OSStatTaskCPUUsageInit(&os_err);
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

    /* Create semaphore */
    OSSemCreate(&MainSem, "Sem Test", 0u, &os_err);

    /* Create Task2 */
    OSTaskCreate(&Task2TCB, 			//tcb
                 "Task 2",				//name
                  Task2,				//func
                  0u,					//arg
                  TASK2_PRIO,			//prio
                  &Task2Stk[0u],		//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    /* Create Task3 */
    OSTaskCreate(&Task3TCB, 			//tcb
                 "Task 3",				//name
                  Task3,				//func
                  0u,					//arg
                  TASK3_PRIO,			//prio
                  &Task3Stk[0u],		//stack
                  TASK3_STK_SIZE_LIMIT,	//stack limit
                  TASK3_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    while (1) {
    	OSSemPost(&MainSem, OS_OPT_POST_1, &os_err);
    	App_Run();
    }
}

// UART Task
static void Task2(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    static void *p_msg;
    static OS_MSG_SIZE msg_size;

    gpioWrite (PIN_LED_BLUE, HIGH);
    gpioWrite (PIN_LED_RED, HIGH);
    gpioWrite (PIN_LED_GREEN, HIGH);

    while (1) {
		static char buffer[12]={0xAA,0x55,0xC3,0x3C,0x07,0x1,0x00,0x00,0x00,0x00,0x01,0x00};
		uartWriteMsg(UARTID, buffer, 12);

		//OSTimeDlyHMSM(0u, 0u, 5u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);

		p_msg = OSQPend(&ComQ, 0, OS_OPT_PEND_BLOCKING, &msg_size, (CPU_TS *)0, &os_err);
		char *msg = (char*)p_msg;
		for(int i=0; i<msg_size; i++){ //Convertir el msg al string
			buffer[i+6]=msg[i];
		}
    }
}

// Keep Alive Task
static void Task3(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    while (1) {

		static char Kbuffer[6]={0xAA,0x55,0xC3,0x3C,0x01,0x2};
		OSTimeDlyHMSM(0u, 0u, 3u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);

		//Send Keep Alive
		uartWriteMsg(UARTID, Kbuffer, 6);

		//Receive Keep Alive
		OSSemPend(&UartSem, 0u, OS_OPT_PEND_BLOCKING, (CPU_TS*)0, &os_err);
		static char Rbuffer[6];
		uartReadMsg(UARTID, Rbuffer, 6);
		if(Rbuffer[5] == 0x82){
			gpioWrite (PIN_LED_RED, HIGH);
			gpioWrite (PIN_LED_BLUE, HIGH);
			gpioWrite (PIN_LED_GREEN, LOW);
		}
		else if(Rbuffer[5] == 0x81){
			gpioWrite (PIN_LED_RED, HIGH);
			gpioWrite (PIN_LED_BLUE, LOW);
			gpioWrite (PIN_LED_GREEN, HIGH);
		}
		else {
			gpioWrite (PIN_LED_RED, LOW);
			gpioWrite (PIN_LED_GREEN, HIGH);
			gpioWrite (PIN_LED_BLUE, HIGH);
		}
    }
}
