#include "includes.h"

#define          TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */
#define          TASK_START_ID       0                /* Application tasks IDs                         */
#define          TASK_CLK_ID         1
#define          TASK_1_ID           2
#define          TASK_2_ID           3

#define          TASK_START_PRIO    10                /* Application tasks priorities                  */
#define          TASK_CLK_PRIO      11

#define          TASK_1_COMP         1
#define          TASK_2_COMP         3
#define          TASK_3_COMP         4

#define          TASK_1_PRIO        3
#define          TASK_2_PRIO        6
#define          TASK_3_PRIO        9

#define          MSG_QUEUE_SIZE     20                /* Size of message queue used in example         */

        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
        void  Task1();
        void  Task2();
        void  Task3();

OS_STK        TaskStartStk[TASK_STK_SIZE];            /* Startup    task stack                         */
OS_STK        Task1Stk[TASK_STK_SIZE];                /* Task #1    task stack                         */
OS_STK        Task2Stk[TASK_STK_SIZE];                /* Task #2    task stack                         */
OS_STK        Task3Stk[TASK_STK_SIZE];                /* Task #3    task stack                         */


void main (void)
{
    OS_STK *ptos;
    OS_STK *pbos;
    INT32U  size;


    PC_DispClrScr(DISP_FGND_WHITE);                        /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    PC_ElapsedInit();                                      /* Initialized elapsed time measurement     */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);

    OSStart();                                             /* Start multitasking                       */
}

/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */

    // TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */

    OSMsgQueue = OSQCreate(&OSMsgQueueTbl[0], MSG_QUEUE_SIZE); /* Create a message queue                   */
    printf("\nVersion V4");
    TaskStartCreateTasks();
    for (;;) {
        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
            if (key == 0x31) {                             /* Yes, see if it's the ESCAPE key          */
                OS_ENTER_CRITICAL();
                OSQPost(OSMsgQueue, (void *)"Post Msg");
                OS_EXIT_CRITICAL();
            }
        }
        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDly(OS_TICKS_PER_SEC);                       /* Wait one second                          */
    }
}

/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    OSTaskCreate(Task1, (void *)0, &Task1Stk[TASK_STK_SIZE - 1], TASK_1_PRIO);
    OSTaskCreate(Task2, (void *)0, &Task2Stk[TASK_STK_SIZE - 1], TASK_2_PRIO);
    // OSTaskCreate(Task3, (void *)0, &Task3Stk[TASK_STK_SIZE - 1], TASK_3_PRIO);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK 1
*
*********************************************************************************************************
*/
void Task1()
{
    int start;
    int end;
    int toDelay;
    
    char  *msg;
    char  *tempMsg = "";
    INT8U  err;

    start = OSTimeGet();
    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = TASK_1_COMP;
    OS_EXIT_CRITICAL();
    while(1){
        while(OSTCBCur->compTime != 0)
        {
            OS_ENTER_CRITICAL();
            msg = (char *)OSQPend(OSMsgQueue, 0, &err);
            // if (tempMsg != msg) printf("%s", msg);
            // tempMsg = msg;
            printf("%s", msg);
            OS_EXIT_CRITICAL();
        }
        OS_ENTER_CRITICAL();
        end = OSTimeGet();
        toDelay = OSTCBCur->period - (end - start);
        start = start + OSTCBCur->period;
        OSTCBCur->compTime = TASK_1_COMP;
        if (toDelay < 0) printf("\n error toDelay = %d", toDelay);
        OS_EXIT_CRITICAL();
        OSTimeDly(toDelay);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK 2
*
*********************************************************************************************************
*/
void Task2()
{
    int start;
    int end;
    int toDelay;
    INT16S     key;
    char  *msg;
    INT8U  err;

    start = OSTimeGet();
    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = TASK_2_COMP;
    OS_EXIT_CRITICAL();
    while(1){
        while(OSTCBCur->compTime != 0)
        {
        }
        OS_ENTER_CRITICAL();
        end = OSTimeGet();
        toDelay = OSTCBCur->period - (end - start);
        start = start + OSTCBCur->period;
        if (toDelay < 0) printf("\n error toDelay = %d", toDelay);
        OSTCBCur->compTime = TASK_2_COMP;
        OS_EXIT_CRITICAL();
        OSTimeDly(toDelay);
    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                               TASK 3
*
*********************************************************************************************************
*/
void Task3()
{
    int start;
    int end;
    int toDelay;
    
    char  *msg;
    INT8U  err;

    start = OSTimeGet();
    
    OS_ENTER_CRITICAL();
    OSTCBCur->compTime = TASK_3_COMP;
    OS_EXIT_CRITICAL();
    while(1){
        while(OSTCBCur->compTime != 0)
        {
        }
        OS_ENTER_CRITICAL();
        end = OSTimeGet();
        toDelay = OSTCBCur->period - (end - start);
        start = start + OSTCBCur->period;
        OSTCBCur->compTime = TASK_3_COMP;
        OS_EXIT_CRITICAL();
        OSTimeDly(toDelay);
    }
}