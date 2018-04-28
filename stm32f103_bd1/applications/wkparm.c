#include "wkparm.h"
#include "easyflash.h"
#include <string.h>
#include <stdio.h>
#include "sockutil.h"

void wkparm_init(void)
{
    easyflash_init();

	if (ef_get_env("lip") == 0)
	{
	    ef_set_env("lip", "192.168.1.199");
	}
	
	if (ef_get_env("dip") == 0)
	{
	    ef_set_env("dip", "103.46.128.47");
	}

    if (ef_get_env("gw") == 0)
	{
	    ef_set_env("gw", "192.168.1.1");
	}

	if (ef_get_env("sm") == 0)
	{
	    ef_set_env("sm", "255.255.255.0");
	}

	if (ef_get_env("dns") == 0)
	{
	    ef_set_env("dns", "8.8.8.8");
	}

	if (ef_get_env("dhcp") == 0)
	{
	    ef_set_env("dhcp", "0");
	}

	if (ef_get_env("port") == 0)
	{
	    ef_set_env("port", "12345");
	}

	if (ef_get_env("dport") == 0)
	{
	    ef_set_env("dport", "15331");
	}

	if (ef_get_env("mode") == 0)
	{
	    ef_set_env("mode", "1");
	}

	if (ef_get_env("name") == 0)
	{
	    ef_set_env("name", "QM-NKC44");
	}

	if (ef_get_env("mac") == 0)
	{
		char macstr[20];
        uint8_t mac[6] = {0x00, 0x80, 0xE1};

		mac[3] = *((uint8_t*)0x1ffff7e8 + 0);
		mac[4] = *((uint8_t*)0x1ffff7e8 + 4);
		mac[5] = *((uint8_t*)0x1ffff7e8 + 8);
		make_macstr(mac, macstr);
	    ef_set_env("mac", macstr);
	}
	
	if (ef_get_env("domain") == 0)
	{
	    ef_set_env("domain", "17g42008k9.iok.la");
	}

	ef_save_env();
}

void wkparm_default(void)
{
    ef_set_env("lip", "192.168.1.199");
    ef_set_env("dip", "103.46.128.47");
    ef_set_env("gw", "192.168.1.1");
    ef_set_env("sm", "255.255.255.0");
    ef_set_env("dns", "8.8.8.8");
    ef_set_env("dhcp", "0");
	ef_set_env("port", "12345");
    ef_set_env("dport", "15331");
    ef_set_env("mode", "1");
    ef_set_env("name", "QM-NKC44");
    ef_set_env("domain", "17g42008k9.iok.la");

	{
		char macstr[20];
        uint8_t mac[6] = {0x00, 0x80, 0xE1};

		mac[3] = *((uint8_t*)0x1ffff7e8 + 0);
		mac[4] = *((uint8_t*)0x1ffff7e8 + 4);
		mac[5] = *((uint8_t*)0x1ffff7e8 + 8);
		make_macstr(mac, macstr);
	    ef_set_env("mac", macstr);
	}

    ef_save_env();
}

void make_macstr(uint8_t mac[6], char *str)
{
    int n;
	
	for (n = 0; n < 6; n ++)
	{
	    sprintf(str, "%02X:", mac[n]);
		str += 3;
	}
	str -= 1;
	str[0] = 0;
}

void strtomac(uint8_t mac[6], char *str)
{
    int n;

	for (n = 0; n < 6; n ++)
	{
	    char tmp[3];

		tmp[0] = str[0];
		tmp[1] = str[1];
		tmp[2] = 0;
		mac[n] = (uint8_t)ATOI(tmp, 16);
		str += 3;
	}
}

void wkparm_savedip(uint8_t ip[4])
{
	char buf[20];
	int n;
    
	buf[0] = 0;
	for (n = 0; n < 4; n ++)
	{
		char tmp[5] = {0};

	    sprintf(tmp, ".%d", ip[n]);
		strcat(buf, tmp);
	}

	ef_set_env("dip", &buf[1]);
	ef_save_env();
}

void wkparm_get(struct wkparm *wp)
{
    char *str;

	str = ef_get_env("lip");
	inet_addr_((uint8_t*)str, wp->wncfg.ip);
	str = ef_get_env("sm");
	inet_addr_((uint8_t*)str, wp->wncfg.sn);
	str = ef_get_env("dns");
	inet_addr_((uint8_t*)str, wp->wncfg.dns);	
	str = ef_get_env("gw");
	inet_addr_((uint8_t*)str, wp->wncfg.gw);
	str = ef_get_env("dhcp");
	if (ATOI(str, 10) == 0)
        wp->wncfg.dhcp = 1;
	else
	    wp->wncfg.dhcp = 2;
	str = ef_get_env("dip");
	inet_addr_((uint8_t*)str, wp->dip);
	str = ef_get_env("mode");
	wp->mode = ATOI(str, 10);
	str = ef_get_env("port");
	wp->port = ATOI(str, 10);
	str = ef_get_env("dport");
	wp->dport = ATOI(str, 10);
	str = ef_get_env("mac");
	strtomac(wp->wncfg.mac, str);
	wp->domain = ef_get_env("domain");
	wp->iscon = 0;
	wp->rst = 0;
}
