/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 * 2013-11-15     bright       add RCC initial and print RCC freq function
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif __ICCARM__
#pragma section="HEAP"
#else
extern int __bss_end;
#endif

/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/**
 * This RCC initial for system.
 * use HSE clock source
 * HSE = 20MHZ; sysclk = 20MHZ
 * sysclk source is HSE
 * AHB prescaler is 1, HCLK = SYSCKL = SystemCoreClock = 20MHZ
 */
static void RCC_Configuration(void)
{
    RCC_ClkInitTypeDef ClkInit = {0};
    RCC_OscInitTypeDef OscInit = {0};
	
	HAL_RCC_DeInit();
	
    /* Enable HSI Oscillator and Activate PLL with HSI as source */
    OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    OscInit.HSIState = RCC_HSI_ON;
	OscInit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    OscInit.PLL.PLLState = RCC_PLL_ON;
    OscInit.PLL.PLLDIV = RCC_PLLDIV_2;
    OscInit.PLL.PLLMUL = RCC_PLLMUL_4;
    OscInit.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    if (HAL_RCC_OscConfig(&OscInit) != HAL_OK)
    {
        RT_ASSERT(RT_NULL);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
    clocks dividers */
    ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK |
                        RCC_CLOCKTYPE_HCLK |
                        RCC_CLOCKTYPE_PCLK1 |
                        RCC_CLOCKTYPE_PCLK2;

    ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
    ClkInit.APB1CLKDivider = RCC_HCLK_DIV1;
    ClkInit.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&ClkInit, FLASH_LATENCY_1) != HAL_OK)
    {
        RT_ASSERT(RT_NULL);
    }
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	/* NVIC Configuration */
	NVIC_Configuration();

	/* Configure the SysTick */
	RCC_Configuration();
	SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

#ifdef __CC_ARM
    rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)STM32_SRAM_END);
#elif __ICCARM__
    rt_system_heap_init(__segment_end("HEAP"), (void*)STM32_SRAM_END);
#else
    /* init memory system */
    rt_system_heap_init((void*)&__bss_end, (void*)STM32_SRAM_END);
#endif

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

/*@}*/
