#include "hardware.h"
#include  <os.h>

/* LEDs */
#define LED_R_PORT            PORTB
#define LED_R_GPIO            GPIOB
#define LED_G_PORT            PORTE
#define LED_G_GPIO            GPIOE
#define LED_B_PORT            PORTB
#define LED_B_GPIO            GPIOB
#define LED_R_PIN             22
#define LED_G_PIN             26
#define LED_B_PIN             21
#define LED_B_ON()           (LED_B_GPIO->PCOR |= (1 << LED_B_PIN))
#define LED_B_OFF()          (LED_B_GPIO->PSOR |= (1 << LED_B_PIN))
#define LED_B_TOGGLE()       (LED_B_GPIO->PTOR |= (1 << LED_B_PIN))
#define LED_G_ON()           (LED_G_GPIO->PCOR |= (1 << LED_G_PIN))
#define LED_G_OFF()          (LED_G_GPIO->PSOR |= (1 << LED_G_PIN))
#define LED_G_TOGGLE()       (LED_G_GPIO->PTOR |= (1 << LED_G_PIN))
#define LED_R_ON()           (LED_R_GPIO->PCOR |= (1 << LED_R_PIN))
#define LED_R_OFF()          (LED_R_GPIO->PSOR |= (1 << LED_R_PIN))
#define LED_R_TOGGLE()       (LED_R_GPIO->PTOR |= (1 << LED_R_PIN))

/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];

/* Task 2 */
#define TASK2_STK_SIZE			256u
#define TASK2_STK_SIZE_LIMIT	(TASK2_STK_SIZE / 10u)
#define TASK2_PRIO              3u
#define TASK3_PRIO              3u
static OS_TCB Task2TCB;
static OS_TCB Task3TCB;
static CPU_STK Task2Stk[TASK2_STK_SIZE];
static CPU_STK Task3Stk[TASK2_STK_SIZE];

/* Example semaphore */
static OS_SEM semTest;
static OS_SEM semExample;

/* Messege Queue */
static OS_Q ComQ;

static void Task2(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;
    char ComQBuff[3] = {'B', 'G', 'R'};
    int i = 0;
    while (1) {
        OSSemPost(&semTest, OS_OPT_POST_1, &os_err);
        OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        //OSSemPost(&semExample, OS_OPT_POST_1, &os_err); //Sets or unsets the sem to use un Task3
        //OSTimeDly(1000, OS_OPT_TIME_DLY, &os_err);	// Cuenta ticks de uC3, la frec. es de 1000Hz
		OSQPost(&ComQ, &ComQBuff[i], sizeof(ComQBuff), OS_OPT_POST_FIFO + OS_OPT_POST_ALL, &os_err);
		i++;
        if(i>2){
        	i=0;
        }
    }
}

static void Task3(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    static void *p_msg;
	static OS_MSG_SIZE msg_size;


    while (1) {
        OSSemPost(&semTest, OS_OPT_POST_1, &os_err);
        //OSTimeDlyHMSM(0u, 0u, 1u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
    	//OSSemPend(&semExample, 0u, OS_OPT_PEND_BLOCKING, (CPU_TS*)0, &os_err);	//Wait for release made in Task2
    	p_msg = OSQPend(&ComQ, 0, OS_OPT_PEND_BLOCKING, &msg_size, (CPU_TS *)0, &os_err);
    	char msg = *(char*)p_msg;
    	if(msg == 'B'){
    		LED_B_TOGGLE();
    	}
    	else if (msg == 'G') {
    		LED_G_TOGGLE();
    	}
    	else if (msg == 'R'){
    		LED_R_TOGGLE();
    	}
    }
}


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
    OSSemCreate(&semExample, "Sem Example", 0u, &os_err);


    /* Create Task2 */
    OSTaskCreate(&Task2TCB, 			//tcb
                 "Task 2",				//name
                  Task2,				//func
                  0u,					//arg
                  TASK2_PRIO,			//prio
                 &Task2Stk[0u],			//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    OSTaskCreate(&Task3TCB, 			//tcb
                 "Task 3",				//name
                  Task3,				//func
                  0u,					//arg
                  TASK3_PRIO,			//prio
                 &Task3Stk[0u],			//stack
                  TASK2_STK_SIZE_LIMIT,	//stack limit
                  TASK2_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    while (1) {
        OSTimeDlyHMSM(0u, 0u, 10u, 0u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        //LED_G_TOGGLE();
    }
}

int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    /* RGB LED */
    SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTE_MASK);
    LED_B_PORT->PCR[LED_B_PIN] = PORT_PCR_MUX(1);
    LED_G_PORT->PCR[LED_G_PIN] = PORT_PCR_MUX(1);
    LED_R_PORT->PCR[LED_R_PIN] = PORT_PCR_MUX(1);
    LED_B_GPIO->PDDR |= (1 << LED_B_PIN);
    LED_G_GPIO->PDDR |= (1 << LED_G_PIN);
    LED_R_GPIO->PDDR |= (1 << LED_R_PIN);
    LED_B_GPIO->PSOR |= (1 << LED_B_PIN);
    LED_G_GPIO->PSOR |= (1 << LED_G_PIN);
    LED_R_GPIO->PSOR |= (1 << LED_R_PIN);

    OSInit(&err);
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
				10,
				&err);

    OSStart(&err);

	/* Should Never Get Here */
    while (1) {

    }
}
