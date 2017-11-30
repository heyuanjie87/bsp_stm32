
#include <board.h>
#include <rtdevice.h>

static int get_time(rt_rtc_t *rtc, struct rtc_time *tm)
{
	RTC_TimeTypeDef rtctm;
	RTC_DateTypeDef rtcdt;

	RTC_GetTime(RTC_Format_BIN, &rtctm);
	RTC_GetDate(RTC_Format_BIN, &rtcdt);

    tm->tm_sec = rtctm.RTC_Seconds;
	tm->tm_min = rtctm.RTC_Minutes;
	tm->tm_hour = rtctm.RTC_Hours;

	tm->tm_year = rtcdt.RTC_Year + 100;
	tm->tm_mon = rtcdt.RTC_Month;
	tm->tm_mday = rtcdt.RTC_Date;

    return 0;
}

static int set_time(rt_rtc_t *rtc, struct rtc_time *tm)
{
	RTC_TimeTypeDef rtctm;
	RTC_DateTypeDef rtcdt;
    
	rtctm.RTC_H12 = 0;
    rtctm.RTC_Seconds = tm->tm_sec;
	rtctm.RTC_Minutes = tm->tm_min;
	rtctm.RTC_Hours = tm->tm_hour;
	
	rtcdt.RTC_WeekDay = 1;
	rtcdt.RTC_Year = tm->tm_year % 100;
	rtcdt.RTC_Month = tm->tm_mon;
	rtcdt.RTC_Date = tm->tm_mday;

	RTC_SetTime(RTC_Format_BIN, &rtctm);
	RTC_SetDate(RTC_Format_BIN, &rtcdt);
	
    return 0;
}

static int control(rt_rtc_t *rtc, int cmd, void *args)
{
    return 0;
}

static rt_rtc_t stm32_rtc;
static const struct rt_rtc_ops rtcops =
{
    get_time,
	set_time,
	control,
};

static void stm32_rtc_init(void)
{
	RTC_InitTypeDef RTC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}

     /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);    //??RTC??


    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24????

    RTC_Init(&RTC_InitStructure);
}

int rt_hw_rtc_init(void)
{
	stm32_rtc.ops = &rtcops;

	rt_hw_rtc_register(&stm32_rtc, "rtc", RTC);
    
	stm32_rtc_init();
	
	return 0;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_init);
