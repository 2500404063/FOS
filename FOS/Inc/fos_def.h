/*
 * fos_def.h
 *
 *  Created on: Aug 23, 2022
 *      Author: Felix
 */

#ifndef INC_FOS_DEF_H_
#define INC_FOS_DEF_H_

#include "stm32f4xx_hal.h"

/*
 * User Configration
 */
#define SYSFREQ 84000000                 //System Frequency

#define TICK_1MS SYSFREQ / 1000          //Normally, don't need to change
#define TICK_10MS SYSFREQ / 100          //Normally, don't need to change
#define TICK_100MS SYSFREQ / 10          //Normally, don't need to change

#define PRIORITY_MAX 5                   //The maximum value of priority, range in [0,62]
#define TICK_TIME TICK_10MS              //The Schedule Tick Time

#if (TICK_TIME == TICK_1MS)              //Minimum Delay: 1MS
#define TIME_MS 1
#elif (TICK_TIME == TICK_10MS)           //Minimum Delay: 10MS
#define TIME_MS 0.1
#elif (TICK_TIME == TICK_100MS)          //Minimum Delay: 100MS
#define TIME_MS 0.01
#endif

#define FOS_DISABLE_IRQ() __disable_irq()
#define FOS_ENABLE_IRQ() __enable_irq()

/*
 * Pre-defined types compatible to GCC
 */
//typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned long *addr_t;
typedef __attribute__ ((aligned (4))) uint32_t stack_t;

typedef void (*TaskFun)(void *params);

#endif //INC_FOS_DEF_H_
