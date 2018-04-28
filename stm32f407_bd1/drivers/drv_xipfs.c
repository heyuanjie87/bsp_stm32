#include <rtthread.h>
#include <rtdevice.h>
#include "InFlash.h"
#include <xipfs.h>

struct stmflash
{
    struct rt_device parent;
	struct xipfs_dev xip;
};
 
static struct stmflash _inflash;

static int flash_erase(uint32_t addr, uint32_t len)
{
	InflashInit();
    FLASH_Unlock();
	InflashErase(addr, addr + len - 1);
	FLASH_Lock();
	
	return len;
}

static int flash_write(uint32_t addr, uint8_t *buf, uint32_t size)
{
	InflashInit();
    FLASH_Unlock();
	InflashWrite(addr, (char*)buf, size);
	FLASH_Lock();
	
	return size;
}

static int flash_mutex(void *userdata, int lock)
{
    return 0;
}

const struct xipfs_nfops _ops =
{
    flash_erase,
	flash_write,
	flash_mutex
};

int drv_xipfs_init(void)
{
    _inflash.xip.start = 0x8040000;
	_inflash.xip.blksize = 128*1024;
	_inflash.xip.nblk = 3;
	_inflash.xip.ops = &_ops;
	_inflash.parent.user_data = &_inflash.xip;
    _inflash.xip.writer = 0;

	return rt_device_register(&_inflash.parent, "nor0", 0);
}
INIT_DEVICE_EXPORT(drv_xipfs_init);
