#include <string.h>
#include "easyflash.h"
#include "relay.h"
#include "socket.h"
#include "atcmd.h"

int atcmd_config(char *in, int ilen, char *out, int *rst)
{
	int ret = 0;
    int pos;

	out[0] = 0;
    if (strncasecmp(in, "AT\r\n", 4) == 0)
	{
	    strcat(out, "OK");
		goto _out;
	}
    
	pos = strchr(in, '\r') - in;
    in[pos] = 0;

	if (strncasecmp(in, "AT+GATEWAY=?", 12) == 0)
	{
	    strcat(out, "+GATEWAY:");
		strcat(out, ef_get_env("gw"));
		goto _out;
	}

	if (strncasecmp(in, "AT+GATEWAY=", 11) == 0)
	{
		ef_set_env("gw", &in[11]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+MASK=?", 9) == 0)
	{
	    strcat(out, "+MASK:");
		strcat(out, ef_get_env("sm"));
		goto _out;
	}

	if (strncasecmp(in, "AT+MASK=", 8) == 0)
	{
		ef_set_env("sm", &in[8]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+IP=?", 7) == 0)
	{
	    strcat(out, "+IP:");
		strcat(out, ef_get_env("lip"));
		goto _out;
	}

	if (strncasecmp(in, "AT+IP=", 6) == 0)
	{
		ef_set_env("lip", &in[6]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+DIP=?", 8) == 0)
	{
	    strcat(out, "+DIP:");
		strcat(out, ef_get_env("dip"));
		goto _out;
	}

	if (strncasecmp(in, "AT+DIP=", 7) == 0)
	{
		ef_set_env("dip", &in[7]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+DPORT=?", 10) == 0)
	{
	    strcat(out, "+DPORT:");
		strcat(out, ef_get_env("dport"));
		goto _out;
	}

	if (strncasecmp(in, "AT+DPORT=", 9) == 0)
	{
		ef_set_env("dport", &in[9]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+PORT=?", 9) == 0)
	{
	    strcat(out, "+PORT:");
		strcat(out, ef_get_env("port"));
		goto _out;
	}

	if (strncasecmp(in, "AT+PORT=", 8) == 0)
	{
		ef_set_env("port", &in[8]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+MODEL=?", 10) == 0)
	{
	    strcat(out, "+MODEL:");
		strcat(out, ef_get_env("mode"));
		goto _out;
	}

	if (strncasecmp(in, "AT+MODEL=", 9) == 0)
	{
		ef_set_env("mode", &in[9]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+NAME=?", 9) == 0)
	{
	    strcat(out, "+NAME:");
		strcat(out, ef_get_env("name"));
		goto _out;
	}

	if (strncasecmp(in, "AT+NAME=", 8) == 0)
	{
		ef_set_env("name", &in[8]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+DCHP=?", 9) == 0)
	{
	    strcat(out, "+DCHP:");
		strcat(out, ef_get_env("dhcp"));
		goto _out;
	}

	if (strncasecmp(in, "AT+DCHP=", 8) == 0)
	{
		ef_set_env("dhcp", &in[8]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+MAC=?", 8) == 0)
	{
	    strcat(out, "+MAC:");
		strcat(out, ef_get_env("mac"));
		goto _out;
	}

	if (strncasecmp(in, "AT+MAC=", 7) == 0)
	{
		ef_set_env("mac", &in[7]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+VER=?", 8) == 0)
	{
	    strcat(out, "+VER:");
		strcat(out, "QM1.0.0");
		goto _out;
	}

	if (strncasecmp(in, "AT+DEFAULT", 10) == 0)
	{
		wkparm_default();
		strcat(out, "OK");
		goto _out;
	}

	if (strncasecmp(in, "AT+DOMAIN=?", 11) == 0)
	{
	    strcat(out, "+DOMAIN:");
		strcat(out, ef_get_env("domain"));
		goto _out;
	}

	if (strncasecmp(in, "AT+DOMAIN=", 10) == 0)
	{
		ef_set_env("domain", &in[10]);
		ef_save_env();
	    strcat(out, "OK");
		goto _out;
	}
	
	if (strncasecmp(in, "AT+RST", 6) == 0)
	{
		*rst = 1;
	    strcat(out, "OK");
		goto _out;
	}

_out:
	ret = strlen(out);
    if (ret > 0)
	{
        strcpy(&out[ret], "\r\n");
        ret += 2;	
	}

	return ret;
}

