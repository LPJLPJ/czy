////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Top.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "publicDefine.h"

#ifdef AHMI_CORE




/* This demo uses heap_5.c, and these constants define the sizes of the regions
that make up the total heap.  This is only done to provide an example of heap_5
being used as this demo could easily create one large heap region instead of
multiple smaller heap regions - in which case heap_4.c would be the more
appropriate choice. */
#define mainREGION_1_SIZE	2001
#define mainREGION_2_SIZE	18005
#define mainREGION_3_SIZE	1007

/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */

/*
 * Some of the RTOS hook (callback) functions only need special processing when
 * the full demo is being used.  The simply blinky demo has no special
 * requirements, so these functions are called from the hook functions defined
 * in this file, but are defined in main_full.c.
 */
void vFullDemoTickHookFunction( void );
void vFullDemoIdleFunction( void );

/*
 * This demo uses heap_5.c, so start by defining some heap regions.  This is
 * only done to provide an example as this demo could easily create one large
 * heap region instead of multiple smaller heap regions - in which case heap_4.c
 * would be the more appropriate choice.  No initialisation is required when
 * heap_4.c is used.
 */
static void  prvInitialiseHeap( void );

/*
 * Prototypes for the standard FreeRTOS callback/hook functions implemented
 * within this file.
 */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/*
 * Writes trace data to a disk file when the trace recording is stopped.
 * This function will simply overwrite any trace files that already exist.
 */
static void prvSaveTraceFile( void );

/* The user trace event posted to the trace recording on each tick interrupt.
Note tick events will not appear in the trace recording with regular period
because this project runs in a Windows simulator, and does not therefore
exhibit deterministic behaviour. */

static portBASE_TYPE xTraceRunning = pdTRUE;

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */

	/* Uncomment the following code to allow the trace to be stopped with any
	key press.  The code is commented out by default as the kbhit() function
	interferes with the run time behaviour. */
	/*
		if( _kbhit() != pdFALSE )
		{
			if( xTraceRunning == pdTRUE )
			{
				vTraceStop();
				prvSaveTraceFile();
				xTraceRunning = pdFALSE;
			}
		}
	*/

}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/



void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
static portBASE_TYPE xPrinted = pdFALSE;
volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;


}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
//FILE* pxOutputFile;

}
/*-----------------------------------------------------------*/

static void  prvInitialiseHeap( void )
{
/* This demo uses heap_5.c, so start by defining some heap regions.  This is
only done to provide an example as this demo could easily create one large heap
region instead of multiple smaller heap regions - in which case heap_4.c would
be the more appropriate choice.  No initialisation is required when heap_4.c is
used.  The xHeapRegions structure requires the regions to be defined in order,
so this just creates one big array, then populates the structure with offsets
into the array - with gaps in between and messy alignment just for test
purposes. */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
volatile uint32_t ulAdditionalOffset = 19; /* Just to prevent 'condition is always true' warnings in configASSERT(). */
const HeapRegion_t xHeapRegions[] =
{
	/* Start address with dummy offsets						Size */
	{ ucHeap + 1,											mainREGION_1_SIZE },
	{ ucHeap + 15 + mainREGION_1_SIZE,						mainREGION_2_SIZE },
	{ ucHeap + 19 + mainREGION_1_SIZE + mainREGION_2_SIZE,	mainREGION_3_SIZE },
	{ NULL, 0 }
};

	/* Sanity check that the sizes and offsets defined actually fit into the
	array. */
	configASSERT( ( ulAdditionalOffset + mainREGION_1_SIZE + mainREGION_2_SIZE + mainREGION_3_SIZE ) < configTOTAL_HEAP_SIZE );

	vPortDefineHeapRegions( xHeapRegions );
}

void vTask1(void *pvParameters)
{
	const char *pcTaskName = "number 1";
	volatile unsigned long u1;
	for(;;)
	{
		printf("%s",pcTaskName);
		
		for(u1 = 0; u1<10000; u1++)
		{
		}
	}
}

void vTask2(void *pvParameters)
{
	const char *pcTaskName = "number 2";
	volatile unsigned long u1;
	for(;;)
	{
		for(u1 = 0; u1<10000; u1++)
		{
		}
	}
}


void vTaskFunction(void *pvParameters)
 {
	 char *pcTaskName;
	 portTickType xLastWakeTime;

	 pcTaskName = (char *) pvParameters;

	 xLastWakeTime = xTaskGetTickCount();

	 for(;;)
	 {
		 printf("%s",pcTaskName);

		 vTaskDelayUntil(&xLastWakeTime,
			             (250/portTICK_PERIOD_MS));
	 }
}
		  


static const char *pcTextForTask1="Task 1 is running\r\n";

int top(void)// main funtion of application
{
	xTaskCreate(vTaskFunction, "Task 1", 1000, (void*)pcTextForTask1, 1, NULL);
	xTaskCreate(vTask2, "Task 2", 1000, NULL, 2, NULL);
	vTaskStartScheduler();
	//for(;;);

	return 0;
}

#endif