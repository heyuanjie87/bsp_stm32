#include "proto.h"
#include "relay.h"

uint16_t proto_swap16(uint16_t v)
{
    return ((v <<8) | (v >> 8));
}

uint16_t proto_chksum(uint8_t *d, int size)
{
    uint16_t s = 0;

	while (size > 0)
	{
		s += *d;
		d ++;
		size --;
	}

	return proto_swap16(s);
}

static void rspinit(msgproto_hdr_t *hdr, uint8_t cmd, uint16_t len)
{
    hdr->start = 0x01EF;
	hdr->type = 1;
	hdr->addr = 0xFFFFFFFF;
	hdr->cmd = cmd;
	hdr->len = proto_swap16(len);
}

int proto_drctrl_rsp(uint8_t *buf, char op)
{
    msgproto_drctl_t *rsp = (msgproto_drctl_t *)buf;

	rspinit(&rsp->hdr, 9, 4);

	rsp->op = swv_get(1);
	rsp->sum = proto_chksum(&rsp->hdr.type, sizeof(*rsp) - 8);

	return sizeof(*rsp);
}

static int proto_ltctrl_rsp(uint8_t *buf, char op)
{
    msgproto_drctl_t *rsp = (msgproto_drctl_t *)buf;

	rspinit(&rsp->hdr, MP_LTCTL, 4);

	rsp->op = op;
	rsp->sum = proto_chksum(&rsp->hdr.type, sizeof(*rsp) - 8);

	return sizeof(*rsp);
}

static int proto_swv_rsp(uint8_t *buf, char ch)
{
    mp_swv_rsp *rsp = (mp_swv_rsp*)buf;

	rspinit(&rsp->hdr, MP_SWV, sizeof(*rsp) - 9);
	rsp->ch = ch;
	if (ch > 0 && ch < 5)
	    rsp->ret = swv_get(ch);
	else
		rsp->ret = 2;

	rsp->sum = proto_chksum(&rsp->hdr.type, sizeof(*rsp) - 8);

	return sizeof(*rsp);
}

static int proto_rl_rsp(uint8_t *buf, char ch, char op)
{
    mp_rl_rsp *rsp = (mp_rl_rsp*)buf;

	rspinit(&rsp->hdr, MP_RLCTL, sizeof(*rsp) - 9);
	rsp->ch = ch;
	if (ch > 0 && ch < 3)
	{
	    rsp->ret = 0;
		relay_set(2 + ch, op);
	}
	else
		rsp->ret = 1;

	rsp->sum = proto_chksum(&rsp->hdr.type, sizeof(*rsp) - 8);

	return sizeof(*rsp);
}

int proto_in(uint8_t *in, int size, uint8_t *out)
{
    msgproto_hdr_t *hdr;
	uint16_t len;
    uint16_t *sum;
    int ret = 0;

	hdr = (msgproto_hdr_t *)in;
	if (hdr->start != MP_START)
		return 0;

    len = proto_swap16(hdr->len);
    if (size < (len + 9))
        return 0;
	sum = (uint16_t*)((uint32_t)in + len + 7);
	if (proto_chksum(&hdr->type, len + 1) != *sum)
		return 0;

	switch (hdr->cmd)
	{
	case MP_DRCTL:
	{
		msgproto_drctl_t *ctrl = (msgproto_drctl_t *)hdr;

	    relay_set(1, ctrl->op);
		ret = proto_drctrl_rsp(out, ctrl->op);
	}break;
	case MP_LTCTL:
	{
		msgproto_ltctl_t *ctrl = (msgproto_ltctl_t *)hdr;

	    relay_set(2, ctrl->op);
		ret = proto_ltctrl_rsp(out, ctrl->op);	
	}break;
	case MP_SWV:
	{
	    mp_swv_req *req = (mp_swv_req *)hdr;
		
		ret = proto_swv_rsp(out, req->ch);
	}break;
	case MP_RLCTL:
	{
	    mp_rl_req *req = (mp_rl_req *)hdr;
		
		ret = proto_rl_rsp(out, req->ch, req->op);
	}break;
	}

	return ret;
}
