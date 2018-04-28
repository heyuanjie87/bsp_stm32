#include <rtthread.h>
#include "../coremark.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

int ee_printf(const char *fmt, ...)
{
  char buf[128],*p;
  va_list args;
  int n=0;

  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  p=buf;

  rt_kprintf("%s", p);

  return n;
}
