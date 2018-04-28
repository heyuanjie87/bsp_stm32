#ifndef __WKPARM_H__
#define __WKPARM_H__

#include <stdint.h>
#include "wizchip_conf.h"

struct wkparm
{
    wiz_NetInfo wncfg;
	uint8_t dip[4];
	uint16_t dport;
	uint16_t port;
	uint16_t mode;
	char *domain;
	uint16_t iscon;
	int rst;
};

void wkparm_init(void);
void wkparm_get(struct wkparm *wp);
void make_macstr(uint8_t mac[6], char *str);
void wkparm_default(void);
void wkparm_savedip(uint8_t ip[4]);

#endif
