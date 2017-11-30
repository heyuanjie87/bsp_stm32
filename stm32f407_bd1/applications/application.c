/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2014-04-27     Bernard      make code cleanup. 
 */
 
#include <rtthread.h>
#include <unistd.h>

static void rt_init_thread_entry(void* parameter)
{
    rt_components_init();
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

static int stdio_init(void)
{
    int fd;

	fd = open("/dev/uart3", O_RDWR, 0);
    dup(fd);

    return fd;	
}
INIT_ENV_EXPORT(stdio_init);

/*@}*/
