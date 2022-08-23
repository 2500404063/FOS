#include "tasks.h"

#include "main.h"

/****************************************************************
 * Definitions: Task, Stack, Node
 ****************************************************************/
FOS_Task task0;
stack_t task0_stack[32];
FOS_QueueNode task0_node;

FOS_Task task1;
stack_t task1_stack[256];
FOS_QueueNode task1_node;

FOS_Task task2;
stack_t task2_stack[256];
FOS_QueueNode task2_node;

FOS_Mutex mutex;

/****************************************************************
 * Definitions: Task Function
 ****************************************************************/

void Task0(void *parameter) {
	while (1) {

	}
}

void Task1(void *parameter) {
	uint8_t data = 0x05;
	while (1) {
		FOS_MutexLock(&mutex);
		HAL_UART_Transmit(&huart1, &data, 1, 1000);
		FOS_TaskDelay(0, 100 * TIME_MS);
		FOS_MutexUnlock(&mutex);
	}
}

void Task2(void *parameter) {
	uint8_t data = 0x15;
	while (1) {
		FOS_MutexLock(&mutex);
		HAL_UART_Transmit(&huart1, &data, 1, 1000);
		FOS_TaskDelay(0, 100 * TIME_MS);
		FOS_MutexUnlock(&mutex);
	}
//	uint8_t flag = 0;
//	while (1) {
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//		FOS_TaskDelay(0, 100);
//		if (flag == 0) {
//			flag = 1;
//			FOS_TaskSuspend(&task1_node);
//		} else {
//			flag = 0;
//			FOS_TaskResume(&task1_node);
//		}
//	}
}

void FOS_TaskQueueInit() {
	FOS_TaskCreate(&task0_node, &task0, Task0, NULL, PRIORITY_MAX, FOS_GetStackTop(task0_stack));
	FOS_TaskCreate(&task1_node, &task1, Task1, NULL, 0, FOS_GetStackTop(task1_stack));
	FOS_TaskCreate(&task2_node, &task2, Task2, NULL, 0, FOS_GetStackTop(task2_stack));
	FOS_MutexInit(&mutex, 1);
}
