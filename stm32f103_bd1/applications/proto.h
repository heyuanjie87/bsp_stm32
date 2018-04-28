#ifndef __PROTO_H__
#define __PROTO_H__

#include <stdint.h>

#define MP_START    0x01EF
#define MP_DRCTL    0x09
#define MP_LTCTL    0x10
#define MP_SWV       0x11
#define MP_RLCTL    0x12

#pragma pack(1)
typedef struct
{
    uint16_t start;
	uint32_t addr;
	uint8_t type;
	uint16_t len;
	uint8_t cmd;
}msgproto_hdr_t;

typedef struct
{
    msgproto_hdr_t hdr;
	uint8_t op;
	uint16_t sum;
}msgproto_drctl_t;

typedef msgproto_drctl_t msgproto_ltctl_t;

typedef struct
{
    msgproto_hdr_t hdr;
	uint8_t ch;
	uint16_t sum;
}mp_swv_req;

typedef struct
{
    msgproto_hdr_t hdr;
	uint8_t ch;
	uint8_t ret;
	uint16_t sum;
}mp_swv_rsp;

typedef struct
{
    msgproto_hdr_t hdr;
	uint8_t ch;
	uint8_t op;
	uint16_t sum;
}mp_rl_req;

typedef struct
{
    msgproto_hdr_t hdr;
	uint8_t ch;
	uint8_t ret;
	uint16_t sum;
}mp_rl_rsp;
#pragma pack()

int proto_in(uint8_t *in, int size, uint8_t *out);

#endif
