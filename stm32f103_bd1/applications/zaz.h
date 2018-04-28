#ifndef __ZAZ_H__
#define __ZAZ_H__

#include <stdint.h>

#pragma pack(1)
struct zaz_hdr
{
    uint16_t start;
	uint32_t addr;
	uint8_t type;
	uint16_t len;
};

struct zaz_getimg
{
    struct zaz_hdr hdr;
	uint8_t cmd;
	uint16_t sum;
};

struct zaz_charreq
{
    struct zaz_hdr hdr;
	uint8_t cmd;
	uint8_t bid;
	uint16_t sum;
};

struct zaz_charrsp
{
    struct zaz_hdr hdr;
	uint8_t ret;
	uint16_t sum;
};

struct zaz_chardat
{
    struct zaz_hdr hdr;
	uint8_t cmd;
	uint8_t dat[256];
	uint16_t sum;
};

struct zaz_charsub
{
    struct zaz_hdr hdr;
	uint8_t dat[128];
	uint16_t sum;
};

struct zaz_regmodel
{
    struct zaz_hdr hdr;
	uint8_t cmd;
	uint16_t sum;
};
#pragma pack()

#endif
