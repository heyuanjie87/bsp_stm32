#include <finsh.h>
#include <ymodem.h>
#include <unistd.h>

static int ym_cb(long *usrdat, enum rym_event evt, uint8_t *buf, int size)
{
	int ret = 0;

    switch (evt)
	{
	case RYM_EVT_BEGIN:
	{
	    char fp[64] = {'/', 0};

		strcat(fp, buf);
		ret = open(fp, O_TRUNC | O_CREAT | O_APPEND, 0);
		if (ret >= 0)
		{
		    *usrdat = ret;
			ret = 0;
		}
	}break;
    case RYM_EVT_DATA:
    {
	    ret = write((int)*usrdat, buf, size);
		if (ret == size)
			ret = 0;
	}break;
    case RYM_EVT_EOT:
    {
	    close(*usrdat);
	}break;
    case RYM_EVT_ERR:
    {
	    close(*usrdat);
	}break;		
	}

	return ret;
}

static void yloadf(void)
{
    struct rym_ctx ctx;

	ctx.cb = ym_cb;

	rym_recv(&ctx, 10, "/dev/uart3");
}
FINSH_FUNCTION_EXPORT(yloadf, ymodem load to flash);
