/*
 * File      : usart.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2010-03-29     Bernard      remove interrupt Tx and DMA Rx mode
 * 2013-05-13     aozima       update for kehong-lingtai.
 * 2015-01-31     armink       make sure the serial transmit complete in putc()
 * 2016-05-13     armink       add DMA Rx mode
 * 2017-01-19     aubr.cool    add interrupt Tx mode
 * 2017-04-13     aubr.cool    correct Rx parity err
 */

#include "usart.h"
#include "board.h"
#include <rtdevice.h>

/* USART1 */
#define UART1_GPIO_TX        GPIO_Pin_9
#define UART1_GPIO_RX        GPIO_Pin_10
#define UART1_GPIO           GPIOA

/* USART2 */
#define UART2_GPIO_TX        GPIO_Pin_2
#define UART2_GPIO_RX        GPIO_Pin_3
#define UART2_GPIO           GPIOA

/* USART3_REMAP[1:0] = 00 */
#define UART3_GPIO_TX        GPIO_Pin_10
#define UART3_GPIO_RX        GPIO_Pin_11
#define UART3_GPIO           GPIOB

/* USART4 */
#define UART4_GPIO_TX        GPIO_Pin_10
#define UART4_GPIO_RX        GPIO_Pin_11
#define UART4_GPIO           GPIOC


/* STM32 uart driver */
struct stm32_uart
{
    USART_TypeDef *uart_device;
    IRQn_Type irq;
    struct stm32_uart_dma
    {
        /* dma channel */
        DMA_Channel_TypeDef *rx_ch;
        /* dma global flag */
        uint32_t rx_gl_flag;
        /* dma irq channel */
        uint8_t rx_irq_ch;
        /* setting receive len */
        rt_size_t setting_recv_len;
        /* last receive index */
        rt_size_t last_recv_index;
    } dma;
};

static void uart_init(USART_TypeDef* dev)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 57600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
    USART_InitStructure.USART_Mode = USART_Mode_Tx;
    USART_Init(dev, &USART_InitStructure);
	USART_Cmd(dev, ENABLE);
}

static int stm32_init(rt_serial_t *serial)
{
    struct stm32_uart* uart;

    uart = (struct stm32_uart *)serial->parent.user_data;
	uart_init(uart->uart_device);

    return 0;
}

static void stm32_deinit(rt_serial_t *serial)
{

}

static int stm32_configure(rt_serial_t *serial, struct termios *cfg)
{
    struct stm32_uart* uart;
    USART_InitTypeDef USART_InitStructure;
    tcflag_t cflag;

    uart = (struct stm32_uart *)serial->parent.user_data;

	cflag = cfg->c_cflag;

    USART_InitStructure.USART_BaudRate = cfg->c_speed;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 
    if (cflag & CSTOPB)
	{
	    USART_InitStructure.USART_StopBits = USART_StopBits_2;
	}
	else
	{
	    USART_InitStructure.USART_StopBits = USART_StopBits_1;
	}
	
	if (cflag & PARENB)
	{
	    if (cflag & PARODD)
		{
		    USART_InitStructure.USART_Parity = USART_Parity_Odd;
		}
		else
		{
		    USART_InitStructure.USART_Parity = USART_Parity_Even;
		}
	}
	else
	{
	    USART_InitStructure.USART_Parity = USART_Parity_No;
	}

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(uart->uart_device, &USART_InitStructure);

    /* Enable USART */
    USART_Cmd(uart->uart_device, ENABLE);

    return RT_EOK;
}

static int stm32_control(rt_serial_t *serial, int cmd, void *arg)
{
    struct stm32_uart* uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case SERIAL_CTRL_RXSTOP:
        /* disable rx irq */
        UART_DISABLE_IRQ(uart->irq);
        /* disable interrupt */
        USART_ITConfig(uart->uart_device, USART_IT_RXNE, DISABLE);
        break;
    case SERIAL_CTRL_RXSTART:
        /* enable rx irq */
        UART_ENABLE_IRQ(uart->irq);
        /* enable interrupt */
        USART_ITConfig(uart->uart_device, USART_IT_RXNE, ENABLE);
        break;
	case SERIAL_CTRL_TXSTART:
		USART_ITConfig(uart->uart_device, USART_IT_TXE, ENABLE);
	    break;
	case SERIAL_CTRL_TXSTOP:
		USART_ITConfig(uart->uart_device, USART_IT_TXE, DISABLE);
	    break;	
    }

    return RT_EOK;
}

static int stm32_putc(rt_serial_t *serial, char c)
{
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    while (!(uart->uart_device->SR & USART_FLAG_TXE));
    uart->uart_device->DR = c;

    return 1;
}

static int stm32_getc(rt_serial_t *serial)
{
    int ch;
    struct stm32_uart *uart;

    RT_ASSERT(serial != RT_NULL);
    uart = (struct stm32_uart *)serial->parent.user_data;

    ch = -1;
    if (uart->uart_device->SR & USART_FLAG_RXNE)
    {
        ch = uart->uart_device->DR & 0xff;
    }

    return ch;
}

/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(rt_serial_t *serial) 
{
    struct stm32_uart *uart = (struct stm32_uart *) serial->parent.user_data;

    RT_ASSERT(uart != RT_NULL);

    if(USART_GetITStatus(uart->uart_device, USART_IT_RXNE) != RESET)
    {
        if(USART_GetFlagStatus(uart->uart_device, USART_FLAG_PE) == RESET)
        {
            rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
        }
        /* clear interrupt */
        USART_ClearITPendingBit(uart->uart_device, USART_IT_RXNE);
    }
    if(USART_GetITStatus(uart->uart_device, USART_IT_TXE) != RESET)
    {
        rt_hw_serial_isr(serial, RT_SERIAL_EVENT_TX_DONE);
        /* clear interrupt */
        //USART_ClearITPendingBit(uart->uart_device, USART_IT_TXE);
    }
    if (USART_GetITStatus(uart->uart_device, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(uart->uart_device, USART_IT_TC);
    }
    if (USART_GetFlagStatus(uart->uart_device, USART_FLAG_ORE) == SET)
    {
        stm32_getc(serial);
    }
}

static const struct rt_uart_ops stm32_uart_ops =
{
	stm32_init,
	stm32_deinit,
    stm32_control,	
    stm32_configure,
    stm32_putc,
    stm32_getc,
};

#if defined(RT_USING_UART1)
/* UART1 device driver structure */
struct stm32_uart uart1 =
{
    USART1,
    USART1_IRQn,
    {
        DMA1_Channel5,
        DMA1_FLAG_GL5,
        DMA1_Channel5_IRQn,
        0,
    },
};
struct rt_serial_device serial1;

void USART1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&serial1);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
/* UART2 device driver structure */
struct stm32_uart uart2 =
{
    USART2,
    USART2_IRQn,
    {
        DMA1_Channel6,
        DMA1_FLAG_GL6,
        DMA1_Channel6_IRQn,
        0,
    },
};
struct rt_serial_device serial2;

void USART2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&serial2);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
/* UART3 device driver structure */
struct stm32_uart uart3 =
{
    USART3,
    USART3_IRQn,
    {
        DMA1_Channel3,
        DMA1_FLAG_GL3,
        DMA1_Channel3_IRQn,
        0,
    },
};
struct rt_serial_device serial3;

void USART3_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&serial3);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
/* UART4 device driver structure */
struct stm32_uart uart4 =
{
    UART4,
    UART4_IRQn,
    {
        DMA2_Channel3,
        DMA2_FLAG_GL3,
        DMA2_Channel3_IRQn,
        0,
    },
};
struct rt_serial_device serial4;

void UART4_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&serial4);

    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* RT_USING_UART4 */

static void RCC_Configuration(void)
{
#if defined(RT_USING_UART1)
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable UART clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
    /* Enable UART GPIO clocks */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    /* Enable UART clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
#endif /* RT_USING_UART4 */
}

static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

#if defined(RT_USING_UART1)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_RX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART1_GPIO_TX;
    GPIO_Init(UART1_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_RX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART2_GPIO_TX;
    GPIO_Init(UART2_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_RX;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART3_GPIO_TX;
    GPIO_Init(UART3_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
    /* Configure USART Rx/tx PIN */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = UART4_GPIO_RX;
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = UART4_GPIO_TX;
    GPIO_Init(UART4_GPIO, &GPIO_InitStructure);
#endif /* RT_USING_UART4 */
}

static void NVIC_Configuration(struct stm32_uart* uart)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = uart->irq;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

int rt_hw_usart_init(void)
{
    struct stm32_uart* uart;

    RCC_Configuration();
    GPIO_Configuration();

#if defined(RT_USING_UART1)
    uart = &uart1;

    serial1.ops = &stm32_uart_ops;

    NVIC_Configuration(uart);

    /* register UART1 device */
    rt_hw_serial_register(&serial1, "uart1",
                          0,
                          uart);
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
    uart = &uart2;

    serial2.ops = &stm32_uart_ops;

    NVIC_Configuration(uart);

    /* register UART2 device */
    rt_hw_serial_register(&serial2, "uart2",
                          0,
                          uart);
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
    uart = &uart3;

    serial3.ops = &stm32_uart_ops;

    NVIC_Configuration(uart);

    /* register UART3 device */
    rt_hw_serial_register(&serial3, "uart3",
                          0,
                          uart);
#endif /* RT_USING_UART3 */

#if defined(RT_USING_UART4)
    uart = &uart4;

    serial4.ops = &stm32_uart_ops;

    NVIC_Configuration(uart);

    /* register UART4 device */
    rt_hw_serial_register(&serial4, "uart4",
                           0,
                          uart);
#endif /* RT_USING_UART4 */

    return 0;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);
