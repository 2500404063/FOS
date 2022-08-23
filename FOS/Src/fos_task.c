/*
 * fos_task.c
 *
 *  Created on: Aug 23, 2022
 *      Author: Felix
 */

#include "fos_task.h"
#include "tasks.h"

//These variables are for scheduling.
uint8_t ScheduleEnable = 0;
uint8_t last_priority = PRIORITY_MAX + 1;
FOS_Task *cur_task = 0;
FOS_QueueNode *cur_task_node = 0;
FOS_Task *next_task = 0;

FOS_QueueNode _task_queue_ready_priority[PRIORITY_MAX];
FOS_QueueNode _task_queue_sleep;
FOS_QueueNode _task_queue_suspend;

void FOS_TaskInit() {
	for (int i = 0; i < PRIORITY_MAX; ++i) {
		FOS_QueueNodeInit(&_task_queue_ready_priority[i], 0);
	}
	FOS_QueueNodeInit(&_task_queue_sleep, 0);
	FOS_QueueNodeInit(&_task_queue_suspend, 0);
	__set_PSP(0);
	//Set PendSV the lowest priority.
	HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);

	/*
	 * This is the way to alter the Frequency.
	 */
//	__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
//	{
//	  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
//	  {
//	    return (1UL);                                                   /* Reload value impossible */
//	  }
//
//	  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
//	  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
//	  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
//	  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
//	                   SysTick_CTRL_TICKINT_Msk   |
//	                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
//	  return (0UL);                                                     /* Function successful */
//	}
	SysTick_Config(TICK_TIME);
}

void FOS_TaskCreate(FOS_QueueNode *node, FOS_Task *task, TaskFun entry, addr_t param, uint8_t priority, addr_t stack) {
	if (priority > PRIORITY_MAX) {
//TODO:The priority is over the maximum.
		return;
	}
	FOS_QueueNodeInit(node, task);
	task->entry = entry;
	task->param = param;
	task->priority_state = (priority << 2) | TaskState_Ready;
	task->timeout = 0;

//Init the initial stack state
	*(--stack) = (unsigned int) 0x01000000; // XPSR
	*(--stack) = (unsigned int) entry;      // r15(pc)
	*(--stack) = (unsigned int) 0x14;       // R14(LR)
	*(--stack) = (unsigned int) 0x12;       // R12
	*(--stack) = (unsigned int) 0x3;        // R3
	*(--stack) = (unsigned int) 0x2;        // R2
	*(--stack) = (unsigned int) 0x1;        // R1
	*(--stack) = (unsigned int) param;      // R0 = param
//User save
	*(--stack) = (unsigned int) 0x11111111;        // R11
	*(--stack) = (unsigned int) 0x10101010;        // R10
	*(--stack) = (unsigned int) 0x99999999;        // R9
	*(--stack) = (unsigned int) 0x88888888;        // R8
	*(--stack) = (unsigned int) 0x77777777;        // R7
	*(--stack) = (unsigned int) 0x66666666;        // R6
	*(--stack) = (unsigned int) 0x55555555;        // R5
	*(--stack) = (unsigned int) 0x44444444;        // R4
	task->sp = stack;
	FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[priority];
	if (_cur_priority_node->value == 0) {
		_cur_priority_node->value = node;
	} else {
		FOS_QueueAddRoundTail(_cur_priority_node->value, node);
	}
}

void FOS_TaskDelay(FOS_QueueNode *node, uint16_t timeout) {
	FOS_DISABLE_IRQ();
	if (node == 0) {
		node = cur_task_node;
	}
	FOS_Task *_task = (FOS_Task*) node->value;
	uint8_t _priority = _task->priority_state >> 2;
	FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[_priority];
	_task->priority_state = (_priority << 2) + TaskState_Sleep;
	_task->timeout = timeout;
	/*
	 * We need to check the entry of the task cycle of the same priority.
	 * If no more tasks, the entry will be set Zero.
	 */
	if (node->nextNode == node) {
		_cur_priority_node->value = 0;
	} else {
		_cur_priority_node->value = node->nextNode;
	}
//These are simple removing and adding.
	FOS_QueueRemoveNode(node);
	FOS_QueueAddNext(&_task_queue_sleep, node);
	/*
	 * If the entry is set to the next task, we have to use this to break the cycle,
	 * but it is still on the right order to call the next task.
	 */
	last_priority = PRIORITY_MAX + 1;
	FOS_TaskSchedule();
}

void FOS_TaskSuspend(FOS_QueueNode *node) {
	FOS_DISABLE_IRQ();
	if (node == 0) {
		node = cur_task_node;
	}
	FOS_Task *_task = (FOS_Task*) node->value;
	uint8_t _priority = _task->priority_state >> 2;
	FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[_priority];
	_task->priority_state = (_priority << 2) + TaskState_Suspend;
	/*
	 * If the task is in Ready, we need to take it out of the Ready Queue,
	 * and sustain the Ready Queue.
	 * If the task is in Sleep, we just need to take it out of the Sleep Queue,
	 * and put it in Suspend Queue.
	 */
	if (_task->timeout == 0) {
		if (node->nextNode == node) {
			_cur_priority_node->value = 0;
		} else {
			_cur_priority_node->value = node->nextNode;
		}
	}
	FOS_QueueRemoveNode(node);
	FOS_QueueAddNext(&_task_queue_suspend, node);
	//The reason why last_priority = PRIORITY_MAX + 1; is same to FOS_TaskDelay().
	last_priority = PRIORITY_MAX + 1;
	FOS_TaskSchedule();
}

void FOS_TaskResume(FOS_QueueNode *node) {
	FOS_DISABLE_IRQ();
	if (node == 0) {
		node = cur_task_node;
	}
	FOS_Task *_task = (FOS_Task*) node->value;
	uint8_t _priority = _task->priority_state >> 2;
	FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[_priority];
	FOS_QueueRemoveNode(node);
	/*
	 * Check which state the task is before, resume to the old state.
	 */
	if (_task->timeout == 0) {
		_task->priority_state = (_priority << 2) + TaskState_Ready;
		if (_cur_priority_node->value == 0) {
			_cur_priority_node->value = node;
		} else {
			FOS_QueueAddRoundTail(_cur_priority_node->value, node);
		}
	} else {
		_task->priority_state = (_priority << 2) + TaskState_Sleep;
		FOS_QueueAddNext(&_task_queue_sleep, node);
	}
	last_priority = PRIORITY_MAX + 1;
	FOS_TaskSchedule();
}

void FOS_TaskEnableSchedule() {
	ScheduleEnable = 1;
}

void FOS_TaskDisableSchedule() {
	ScheduleEnable = 0;
}

#define TriggerSwitchTo() SCB->ICSR |= 1 << 28

void FOS_TaskSchedule() {
	FOS_DISABLE_IRQ();
	uint8_t TriggerSwitchToEn = 0;
	if (ScheduleEnable) {
		/*
		 * Schedule by priority
		 * When the value of a priority is not Zero, it means that under the priority has ready tasks.
		 */
		for (int i = 0; i < PRIORITY_MAX + 1; ++i) {
			FOS_QueueNode *_cur_priority = &_task_queue_ready_priority[i];
			if (_cur_priority->value != 0) {
				if (last_priority == i) {
					//Start at the following task.
					FOS_Task *_temp_next_task = (FOS_Task*) cur_task_node->nextNode->value;
					if (_temp_next_task != cur_task) {
						next_task = _temp_next_task;
						cur_task_node = cur_task_node->nextNode;
						TriggerSwitchToEn = 1;
					}
				} else {
					//Start at the first task.
					last_priority = i;
					next_task = ((FOS_QueueNode*) _cur_priority->value)->value;
					cur_task_node = (FOS_QueueNode*) _cur_priority->value;
					TriggerSwitchToEn = 1;
				}
				break;
			}
		}

		/*
		 * Sleeping Tasks count down
		 */
		FOS_QueueNode *_temp_sleep_node = _task_queue_sleep.nextNode;
		if (_temp_sleep_node != &_task_queue_sleep) {
			while (1) {
				FOS_Task *_temp_sleep_task = (FOS_Task*) _temp_sleep_node->value;
				//pre-read the next node, because _temp_sleep_task may be modified at line+6
				FOS_QueueNode *_next_sleep_node = _temp_sleep_node->nextNode;

				/*
				 * Timeout decreasement should be slower than SysTick.
				 * If Timeout decresement is as fast as Systick,
				 * Systick will not have the time to switch to the tasks as the same priority.
				 */
				_temp_sleep_task->timeout--;
//				if (--_timeout_trigger <= 0) {
//					_timeout_trigger = 10;
//				}
				if (_temp_sleep_task->timeout == 0) {
					uint8_t _temp_sleep_priority = _temp_sleep_task->priority_state >> 2;
					_temp_sleep_task->priority_state = (_temp_sleep_priority << 2) + TaskState_Ready;
					FOS_QueueNode *_cur_priority_node = &_task_queue_ready_priority[_temp_sleep_priority];
					FOS_QueueRemoveNode(_temp_sleep_node);
					/*
					 * This place is special.
					 * If all tasks were suspended, there will be no root.
					 * So the first one must be the root.
					 */
					if (_cur_priority_node->value == 0) {
						_cur_priority_node->value = _temp_sleep_node;
					} else {
						FOS_QueueAddRoundTail(_cur_priority_node->value, _temp_sleep_node);
					}
				}
				if (_next_sleep_node != _temp_sleep_node) {
					_temp_sleep_node = _next_sleep_node;
				} else {
					break;
				}
			}
		}
		if (TriggerSwitchToEn) {
			TriggerSwitchTo();
		}
	}
	FOS_ENABLE_IRQ();
}
