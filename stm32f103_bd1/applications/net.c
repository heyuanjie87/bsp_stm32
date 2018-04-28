#include "spiHandler.h"
#include "socket.h"
#include <rtthread.h>
#include "easyflash.h"
#include "wkparm.h"
#include "dhcp.h"
#include "dns.h"
#include <time.h>
#include "zaz.h"
#include "proto.h"
#include "sockutil.h"
#include "atcmd.h"

/*
  sock0 udp 参数配置/查询
  sock1 tcp 继电器
  sock2 tcp 指纹
  sock3 dhcp
  sock4 dns
*/

#define SOCK_DHCP    3
uint8_t buffer[1024];

 int _fgservice = 0;
 extern uint32_t zaz_datok;
 extern struct zaz_chardat _chardat;
 static struct rt_mutex _lock;
 
static void w5500_dhcp_assign(void)
{
	wiz_NetInfo gWIZNETINFO;
    struct wkparm wp;

	wkparm_get(&wp);

	gWIZNETINFO = wp.wncfg;

	getIPfromDHCP(gWIZNETINFO.ip);
	getGWfromDHCP(gWIZNETINFO.gw);
	getSNfromDHCP(gWIZNETINFO.sn);
	getDNSfromDHCP(gWIZNETINFO.dns);

	ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
}

static void w5500_dhcp_conflict()
{

}

static void handle_configreq(struct wkparm *wp)
{
    uint16_t len;
    uint8_t rIP[4];
    uint16_t rPort;
    char rsp[32];
    char macstr[20];
    uint8_t buf[64];

    switch (getSn_SR(0))
    {
	case SOCK_UDP:
    {
		uint8_t *atstr;

        len = getSn_RX_RSR(0);
		if (len > 0)
		{
			if (len > 64)
				len = 64;
		    recvfrom(0, buf, len, rIP,&rPort);
			if (len < 4)
				return;

			macstr[0] = '<';
			make_macstr(wp->wncfg.mac, &macstr[1]);
			macstr[18] = '>';
			macstr[19] = ',';

			if (rt_memcmp(buf, macstr, 20) == 0 || 
				rt_memcmp(buf, "<00:00:00:00:00:00>,", 20) == 0)
			{
				atstr = buf + 40;
				len -= 40;

			    len = atcmd_config((char*)atstr, len, rsp, &wp->rst);
			    if (len > 0)
			    {
                    rt_memcpy(buf, macstr, 20);
				    rt_memcpy(&buf[20], rsp, len);
			        sendto(0, buf, len + 20, rIP, 5000);
			    }		
			}
			else if (rt_memcmp(buf, "AT", 2) == 0)
			{
				atstr = buf;
			    len = atcmd_config((char*)atstr, len, rsp, &wp->rst);
			    if (len > 0)
			    {
			        sendto(0, (uint8_t*)rsp, len, rIP, 5000);
			    }
			}
		}
	}break;
    case SOCK_CLOSED:
    {
		socket(0, Sn_MR_UDP, 5000, 0);
	}break;			
	}
}

static void handle_relay(struct wkparm *wp)
{
	uint8_t state;

	state = getSn_SR(1);
    switch (state)
    {
	case SOCK_ESTABLISHED:
	{
		uint16_t len;

        _fgservice = 1;
        len=getSn_RX_RSR(1);
        if(len > 0)
        {
			if (len > 1024)
				len = 1024;
            recv(1, buffer, len);
			len = proto_in(buffer, len, buffer);
			if (len > 0)
			{
			    send(1, buffer, len);
			}
        }

		if (zaz_datok == 0)
			break;

		if ((time(0) - zaz_datok) < 5)
		{
		    send(1, (uint8_t*)&_chardat, sizeof(_chardat));
		}
		zaz_datok = 0;
	}break;
	case SOCK_INIT:
    {
		if (connect(1, wp->dip, wp->dport) == SOCK_OK)
		{
            wp->iscon = 1;
		}
	}break;
	case SOCK_CLOSED:
	{
		uint8_t ka = 2;

	    socket(1, Sn_MR_TCP, wp->port, SF_TCP_NODELAY);
		setsockopt(1, SO_KEEPALIVEAUTO, &ka);
	}break;
	case SOCK_CLOSE_WAIT:
	{
        disconnect(1);
		closesock(1);
		wp->iscon = 0;
	}break;
	default:
	{
	
	}break;
	}
}

int getipbyname(uint8_t dns[4], char *name, uint8_t ip[4])
{
    if (VerifyIPAddress(name, ip) == 1)
		return 1;

	return DNS_run(dns, (uint8_t*)name, ip);
}

static void w5500_lock(void)
{
    rt_mutex_take(&_lock, -1);
}

static void w5500_unlock(void)
{
    rt_mutex_release(&_lock);
}

static void net_config_worker(void *p)
{
    struct wkparm wp;

	rt_thread_msleep(4000);
    wkparm_get(&wp);

	while (1)
	{
        /* udp作为参数配置/查询通道 */
		handle_configreq(&wp);
		rt_thread_msleep(50);
		if (wp.rst)
		{
		    NVIC_SystemReset();
		}
	}
}

static void net_io_worker(void *p)
{
    struct wkparm wp;
    time_t dns_update;

    W5500_SPI_Init();
	W5500_Init();
    reg_wizchip_cris_cbfunc(w5500_lock, w5500_unlock);

	wkparm_get(&wp);

	if (wp.wncfg.dhcp == 2)
	{
		uint32_t ret;
		uint8_t dhcp_retry = 0;

        setSHAR(wp.wncfg.mac);
	    DHCP_init(SOCK_DHCP, buffer);
	    reg_dhcp_cbfunc(w5500_dhcp_assign, w5500_dhcp_assign, w5500_dhcp_conflict);

		while (1) 
		{
			ret = DHCP_run();

			if(ret == DHCP_IP_LEASED)
				break;

			dhcp_retry++;

			if(dhcp_retry > 10) 
			{
				ctlnetwork(CN_SET_NETINFO, &wp.wncfg);
				break;
			}
			rt_thread_msleep(50);
		}
	}
    else
	{
	    ctlnetwork(CN_SET_NETINFO, &wp.wncfg);
	}
    wizchip_sw_reset();
	DNS_init(4, buffer);

    while (1)
	{
		if (wp.wncfg.dhcp == 2)
		{
		    DHCP_run();
		}

		if ((time(0) - dns_update > 60) && !wp.iscon)
		{
			uint8_t dip[4];

		    dns_update = time(0);
			rt_memcpy(dip, wp.dip, 4);
			if (getipbyname(wp.wncfg.dns, wp.domain, wp.dip))
			{
				if (rt_memcmp(dip, wp.dip, 4) != 0)
			        wkparm_savedip(wp.dip);
			}
		}

        /* 继电器控制 */
		handle_relay(&wp);

		rt_thread_msleep(50);
	}
}

int net_io_init(void)
{
    rt_thread_t thread;

	wkparm_init();
    rt_mutex_init(&_lock, "net", 0);

    thread = rt_thread_create("net",
                                   net_io_worker, RT_NULL,
                                   2048, 20, 5);

    if (thread != RT_NULL)
        rt_thread_startup(thread);

    thread = rt_thread_create("cfg",
                                   net_config_worker, RT_NULL,
                                   1024, 20, 5);

    if (thread != RT_NULL)
        rt_thread_startup(thread);

    return 0;
}
INIT_APP_EXPORT(net_io_init);
