#include <rtthread.h>
#include <unistd.h>
#include <poll.h>
#include "zaz.h"
#include <time.h>
#include "led.h"

struct zaz_chardat _chardat;
extern  int _fgservice;
uint32_t zaz_datok = 0;
static int _togle = 0;

static uint16_t swap16(uint16_t v)
{
    return ((v <<8) | (v >> 8));
}

static uint16_t chksum(void *buf, int size)
{
    uint16_t s = 0;
	uint8_t *d = (uint8_t *)buf;
	
	while (size > 0)
	{
		s += *d;
		d ++;
		size --;
	}

	return swap16(s);
}

static int readyread(int fd, int t)
{
    struct pollfd pf;
	
	pf.fd = fd;
	pf.events = POLLIN;
	
	return poll(&pf, 1, t);
}

static int getimg(int fd)
{
    int ret = 0;
    struct zaz_getimg data;

	data.hdr.start = swap16(0xEF01);
    data.hdr.addr = 0xFFFFFFFF;
	data.hdr.type = 1;
	data.hdr.len = swap16(3);
    data.cmd = 1;
	data.sum = chksum(&data.hdr.type, sizeof(data) - 8);

	ioctl(fd, TCFLSH, TCIFLUSH);
	write(fd, &data, sizeof(data));
	readyread(fd, 500);
	rt_thread_msleep(20);
    ret = read(fd, &data, sizeof(data));
    if (ret != sizeof(data))
		return 0;
    if (data.hdr.start != 0x01EF)
		return 0;
    ret = (data.cmd == 0);
    if (!ret)
		rt_thread_msleep(100);

	return ret;
}

static int genchar(int fd, char id)
{
    int ret = 0;
    struct zaz_charreq data;
    struct zaz_charrsp *rsp;

	data.hdr.start = swap16(0xEF01);
    data.hdr.addr = 0xFFFFFFFF;
	data.hdr.type = 1;
	data.hdr.len = swap16(4);
    data.cmd = 2;
	data.bid = id;
	data.sum = chksum(&data.hdr.type, sizeof(data) - 8);

	write(fd, &data, sizeof(data));
	readyread(fd, 500);
	rt_thread_msleep(20);
    ret = read(fd, &data, sizeof(*rsp));
	rsp = (struct zaz_charrsp *)&data;
    if (ret != sizeof(*rsp))
		ret = 0;
    else
		ret = rsp->ret == 0;

	return ret;
}

static int regmodel(int fd)
{
    int ret = 0;
    struct zaz_regmodel data;

	data.hdr.start = swap16(0xEF01);
    data.hdr.addr = 0xFFFFFFFF;
	data.hdr.type = 1;
	data.hdr.len = swap16(3);
    data.cmd = 5;
	data.sum = chksum(&data.hdr.type, sizeof(data) - 8);

	write(fd, &data, sizeof(data));
	readyread(fd, 400);
	rt_thread_msleep(20);
    ret = read(fd, &data, sizeof(data));
    if (ret != sizeof(data))
		ret = 0;
	else
		ret = (data.cmd == 0);

	return ret;
}

static int upchar(int fd)
{
    int ret = 0;
    struct zaz_charreq data;
    struct zaz_charrsp *rsp;

	data.hdr.start = swap16(0xEF01);
    data.hdr.addr = 0xFFFFFFFF;
	data.hdr.type = 1;
	data.hdr.len = swap16(4);
    data.cmd = 8;
	data.bid = 1;
	data.sum = chksum(&data.hdr.type, sizeof(data) - 8);

	write(fd, &data, sizeof(data));
	rt_thread_msleep(20);
    ret = read(fd, &data, sizeof(*rsp));
	rsp = (struct zaz_charrsp *)&data;
    if (ret != sizeof(*rsp))
		ret = 0;
    else
		ret = rsp->ret == 0;

	return ret;
}

static void makechardat(struct zaz_chardat *d)
{
	d->hdr.start = swap16(0xEF01);
    d->hdr.addr = 0xFFFFFFFF;
	d->hdr.type = 2;
	d->hdr.len = swap16(259);
    d->cmd = 8;
	d->sum = chksum(&d->hdr.type, sizeof(struct zaz_chardat) - 8);
}

static int recvchar(int fd)
{
    int size;
	uint8_t *buf;
	int cnt = 0;
    int len;
	int t = 0;
    struct zaz_charsub tmp;
    int pos = 0;

_again:
	t ++;
	cnt = 0;
	buf = (uint8_t *)&tmp;
	rt_memset(buf, 0, sizeof(tmp));
	rt_thread_msleep(5);
    len = read(fd, buf, sizeof(struct zaz_hdr));
    if (len != sizeof(struct zaz_hdr) || tmp.hdr.start != 0x01EF)
		return 0;

	buf += len;
	size = swap16(tmp.hdr.len);

	while (size > 0)
	{
		if (cnt > 5)
			return 0;

	    if (!readyread(fd, 20))
		{
		    cnt ++;
			continue;
		}

		len = read(fd, buf, size);
		size -= len;
		buf += len;
	}

    if (chksum(&tmp.hdr.type, sizeof(tmp) - 8) != tmp.sum)
		return 0;

	rt_memcpy(&_chardat.dat[pos], tmp.dat, 128);
	pos += 128;
	if (t < 2)
	    goto _again;

	makechardat(&_chardat);

	return 1;
}

static int zaz_getchar(int fd)
{
    if (!getimg(fd))
		return 0;
	if (!genchar(fd, 1))
		return 0;
#if 0
    if (!getimg(fd))
		return 0;
	if (!genchar(fd, 2))
		return 0;
	if (!regmodel(fd))
		return 0;
#endif
	if (!upchar(fd))
		return 0;

	if (!recvchar(fd))
		return 0;

	return 1;
}

static void ledtogle(void)
{
	int n = 20;

    while (n--)
	{
        rt_hw_led_on(0);
        rt_thread_msleep(20);

        rt_hw_led_off(0);
        rt_thread_msleep(20);	
	}
}

static void led_thread_entry(void* parameter)
{
    rt_hw_led_init();

    while (1)
    {
		if (_togle == 0)
		{
            rt_hw_led_on(0);
            rt_thread_msleep(500 );

            rt_hw_led_off(0);
            rt_thread_msleep(500 );	
		}
    }
}

static void zaz_worker(void *p)
{
	int fd;

	fd = open("/dev/uart1", O_NONBLOCK, 0);
	if (fd < 0)
		return;
    ioctl(fd, TIFIFOSETS, (void*)256);

    while (1)
	{ 
		if (_fgservice && zaz_datok == 0)
		{
		    if (zaz_getchar(fd))
			{
				_togle = 1;
                zaz_datok = time(0);
				ledtogle();
			}
		}
        
		if (_togle == 0)
		    rt_thread_msleep(200);
		else
			_togle = 0;
	}
}

int zaz_init(void)
{
    rt_thread_t thread;

    thread = rt_thread_create("zaz",
                                   zaz_worker, RT_NULL,
                                   2048, 20, 15);

    if (thread != RT_NULL)
        rt_thread_startup(thread);

    thread = rt_thread_create("led",
                                   led_thread_entry, RT_NULL,
                                   512, 20, 5);

    if (thread != RT_NULL)
        rt_thread_startup(thread);

	return 0;
}
INIT_APP_EXPORT(zaz_init);
