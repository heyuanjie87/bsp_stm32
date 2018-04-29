/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <unistd.h>

int main(void)
{
    /* user app entry */
    return 0;
}

static int stdio_init(void)
{
    int fd;

	fd = open("/dev/uart1", O_RDWR, 0);
    fd = open("/dev/uart1", O_RDWR, 0);

    return fd;	
}
INIT_ENV_EXPORT(stdio_init);
