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
#include  <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
static OS_TCB Task2TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];

/* Example semaphore */
static OS_SEM semTest;

/* Messege Queue */
static OS_Q ComQ;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void TaskStart(void *p_arg);
static void Task2(void *p_arg);

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

    App_Init();

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
    OSSemCreate(&semTest, "Sem Test", 0u, &os_err);

    /* Create Task2 */
    /*OSTaskCreate(&Task2TCB, 			//tcb
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
                 &os_err);*/

    while (1) {
    	App_Run();
    }
}

// UART Task
/*static void Task2(void *p_arg) {
    (void)p_arg;
    //OS_ERR os_err;
    while (1) {

    }
}*/
