#include <board.h>
#include <rtdevice.h>

#define K9F1G_READID                0x90        /* 读器件ID            */
#define K9F1G_RESET                 0xFF        /* 复位芯片            */

#define K9F1G_PGAEREAD1             0x00        /* 页读指令1           */
#define K9F1G_PAGEREAD2             0x30        /* 页读指令2           */

#define K9F1G_RANDOMDATAOUTPUT1     0x05        /* 页内随机读          */
#define K9F1G_RANDOMDATAOUTPUT2     0xE0

#define K9F1G_PAGEPROGRAM1          0x80        /* 页编程指令1         */
#define K9F1G_PAGEPROGRAM2          0x10        /* 页编程指令2         */

#define K9F1G_RANDOMDATAINPUT       0x85        /* 页内随机编程     */

#define K9F1G_BLOCKERASE1           0x60        /* 块擦除指令1         */
#define K9F1G_BLOCKERASE2           0xD0        /* 块擦除指令2         */

#define K9F1G_READSTATUS            0x70        /* 读器件状态          */

#define K9F1G_CACHEPROGRAM1         0x80        /* Cache 编程          */
#define K9F1G_CACHEPROGRAM2         0x15

#define K9F1G_COPYBACKREAD1         0x00        /* 芯片内页拷贝读     */
#define K9F1G_COPYBACKREAD2         0x35

#define K9F1G_COPYBACKPROGRAM1      0x85        /* 芯片内页拷贝编程 */
#define K9F1G_COPYBACKPROGRAM2      0x10

#define K9F1G_BUSY                  (1<<6)      /* 器件忙标志       */
#define K9F1G_OK                    (1<<0)      /* 器件操作成功标志 */

#define MAX_VALIDBLOCK              900


#define SYS_HW_DFBASE    0x70000000

#define M8(adr)  (*((volatile unsigned char  *) (adr)))

#define FLASH_CMD M8(SYS_HW_DFBASE + 0x10000)
#define FLASH_ADR M8(SYS_HW_DFBASE + 0x20000)
#define FLASH_DAT M8(SYS_HW_DFBASE + 0)

#define FSMC_NWE_GPIO_RCC                (RCC_AHB1Periph_GPIOD)
#define FSMC_NWE_GPIO_X                  (GPIOD)
#define FSMC_NWE_GPIO_PIN                (GPIO_PIN_5)
#define FSMC_NWE_GPIO_MODE               (GPIO_Mode_AF)
#define FSMC_NWE_GPIO_SPEED              (GPIO_High_Speed)
#define FSMC_NWE_GPIO_PULL               (GPIO_PuPd_NOPULL)
#define FSMC_NWE_GPIO_OTYPE              (GPIO_OType_PP)
#define FSMC_NWE_GPIO_AF                 (GPIO_AF_FSMC)
#define FSMC_NWE_GPIO_1OUT               (GPIO_1OUT_HIGH)

#define FSMC_NOE_GPIO_RCC                (RCC_AHB1Periph_GPIOD)
#define FSMC_NOE_GPIO_X                  (GPIOD)
#define FSMC_NOE_GPIO_PIN                (GPIO_PIN_4)
#define FSMC_NOE_GPIO_MODE               (GPIO_Mode_AF)
#define FSMC_NOE_GPIO_SPEED              (GPIO_High_Speed)
#define FSMC_NOE_GPIO_PULL               (GPIO_PuPd_NOPULL)
#define FSMC_NOE_GPIO_OTYPE              (GPIO_OType_PP)
#define FSMC_NOE_GPIO_AF                 (GPIO_AF_FSMC)
#define FSMC_NOE_GPIO_1OUT               (GPIO_1OUT_HIGH)

#define FSMC_D0_GPIO_RCC                 (RCC_AHB1Periph_GPIOD)
#define FSMC_D0_GPIO_X                   (GPIOD)
#define FSMC_D0_GPIO_PIN                 (GPIO_PIN_14)
#define FSMC_D0_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D0_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D0_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D0_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D0_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D0_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D1_GPIO_RCC                 (RCC_AHB1Periph_GPIOD)
#define FSMC_D1_GPIO_X                   (GPIOD)
#define FSMC_D1_GPIO_PIN                 (GPIO_PIN_15)
#define FSMC_D1_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D1_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D1_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D1_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D1_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D1_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D2_GPIO_RCC                 (RCC_AHB1Periph_GPIOD)
#define FSMC_D2_GPIO_X                   (GPIOD)
#define FSMC_D2_GPIO_PIN                 (GPIO_PIN_0)
#define FSMC_D2_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D2_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D2_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D2_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D2_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D2_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D3_GPIO_RCC                 (RCC_AHB1Periph_GPIOD)
#define FSMC_D3_GPIO_X                   (GPIOD)
#define FSMC_D3_GPIO_PIN                 (GPIO_PIN_1)
#define FSMC_D3_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D3_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D3_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D3_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D3_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D3_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D4_GPIO_RCC                 (RCC_AHB1Periph_GPIOE)
#define FSMC_D4_GPIO_X                   (GPIOE)
#define FSMC_D4_GPIO_PIN                 (GPIO_PIN_7)
#define FSMC_D4_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D4_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D4_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D4_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D4_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D4_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D5_GPIO_RCC                 (RCC_AHB1Periph_GPIOE)
#define FSMC_D5_GPIO_X                   (GPIOE)
#define FSMC_D5_GPIO_PIN                 (GPIO_PIN_8)
#define FSMC_D5_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D5_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D5_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D5_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D5_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D5_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D6_GPIO_RCC                 (RCC_AHB1Periph_GPIOE)
#define FSMC_D6_GPIO_X                   (GPIOE)
#define FSMC_D6_GPIO_PIN                 (GPIO_PIN_9)
#define FSMC_D6_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D6_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D6_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D6_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D6_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D6_GPIO_1OUT                (GPIO_1OUT_LOW)

#define FSMC_D7_GPIO_RCC                 (RCC_AHB1Periph_GPIOE)
#define FSMC_D7_GPIO_X                   (GPIOE)
#define FSMC_D7_GPIO_PIN                 (GPIO_PIN_10)
#define FSMC_D7_GPIO_MODE                (GPIO_Mode_AF)
#define FSMC_D7_GPIO_SPEED               (GPIO_High_Speed)
#define FSMC_D7_GPIO_PULL                (GPIO_PuPd_NOPULL)
#define FSMC_D7_GPIO_OTYPE               (GPIO_OType_PP)
#define FSMC_D7_GPIO_AF                  (GPIO_AF_FSMC)
#define FSMC_D7_GPIO_1OUT                (GPIO_1OUT_LOW)

rt_inline void nwait (void) 
{
    while(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) == 0);
}

rt_inline uint8_t nstatus(void)
{
    FLASH_CMD = K9F1G_READSTATUS;
	
	return FLASH_DAT;
}

rt_inline void naddr(rt_uint8_t addr)
{
    FLASH_ADR = addr;
}

rt_inline void ncmd(rt_uint8_t cmd)
{
    FLASH_CMD = cmd;
}

rt_inline uint8_t nread8(void)
{
    return FLASH_DAT;
}

rt_inline void nwrite8(uint8_t dat)
{
    FLASH_DAT = dat;
}

rt_inline void io_readbuf(uint8_t *buf, int size)
{
    while (size)
	{
	    *buf = nread8();
		buf ++;
		size --;
	}
}

rt_inline void io_writebuf(const uint8_t *buf, int size)
{
    while (size)
	{
	    nwrite8(*buf);
		buf ++;
		size --;
	}
}

rt_inline int k9f2g08_erase(int page)
{
    ncmd(K9F1G_BLOCKERASE1);

    naddr(page);
    naddr(page >> 8);

    ncmd(K9F1G_BLOCKERASE2);
	nwait();

    return nstatus();
}

static int k9f2g08_read_buf(rt_nand_t *nand, uint8_t *buf, int len)
{
    io_readbuf(buf, len);

    return 0;    
}

static int k9f2g08_write_buf(rt_nand_t *nand, const uint8_t *buf, int len)
{   
    io_writebuf(buf, len);

    return 0;    
}

static int k9f2g08_cmdfunc(rt_nand_t *nand, int cmd, int page, int offset)
{
	int ret = 0;

    switch (cmd)
    {
    case NAND_PAGE_RD:
    {
        ncmd(K9F1G_PGAEREAD1);  

        naddr(offset);     
        naddr(offset >> 8);              
        naddr(page); 
        naddr(page >> 8);

        ncmd(K9F1G_PAGEREAD2);
        nwait();
        ncmd(K9F1G_PGAEREAD1);    
    }break;
    case NAND_PAGE_WR0:
    {
        ncmd(K9F1G_PAGEPROGRAM1);  

        naddr(offset);     
        naddr(offset >> 8);              
        naddr(page); 
        naddr(page >> 8);   
    }break;
    case NAND_PAGE_WR1:
    {
        ncmd(K9F1G_PAGEPROGRAM2);   
        nwait();
		ret = nstatus() & 0x01;
    }break;
    case NAND_BLK_ERASE:
    {
        ret = k9f2g08_erase(page);
    }break;
    case NAND_ECC_WRITE:
    case NAND_ECC_READ:
    {
        FSMC_NANDECCCmd(FSMC_Bank2_NAND, ENABLE);
    }break;
    }
   
    return ret;
}

static rt_nand_t _nand0;
static const struct nand_ops _ops =
{
    k9f2g08_cmdfunc,
    k9f2g08_read_buf,
    k9f2g08_write_buf,
};

static void fsmc_nand_init(void)
{
    FSMC_NAND_PCCARDTimingInitTypeDef  pFSMC_NAND_PCCARDTiming;
    FSMC_NANDInitTypeDef FSMC_NANDInitStructure;

    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    pFSMC_NAND_PCCARDTiming.FSMC_SetupTime = 0x02;
    pFSMC_NAND_PCCARDTiming.FSMC_WaitSetupTime = 0x06;
    pFSMC_NAND_PCCARDTiming.FSMC_HoldSetupTime = 0x04;
    pFSMC_NAND_PCCARDTiming.FSMC_HiZSetupTime = 0x02;

    FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank2_NAND;
    FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Enable;
    FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Disable;
    FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_256Bytes;
    FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &pFSMC_NAND_PCCARDTiming;
    FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &pFSMC_NAND_PCCARDTiming;
    
	FSMC_NANDDeInit(FSMC_Bank2_NAND);
	FSMC_NANDInit(&FSMC_NANDInitStructure);
    FSMC_NANDCmd(FSMC_Bank2_NAND, ENABLE);
}

static void fsmc_gpio_init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
 
    /* Enable GPIOs clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
                                  | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
                                  | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                  | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
                                  | GPIO_Pin_14 | GPIO_Pin_15;
    
	GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	
    /* NAND_R/B: PD6 */
   GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC); 

    /* NCS */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC); 

    /* GPIO Data Line configuration */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC); //D0
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC); //D1
	
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC); //D2
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC); //D3

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC); //D4
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC); //D5
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC); //D6
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC); //D7

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11 , GPIO_AF_FSMC); //A16
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12 , GPIO_AF_FSMC); //A17

    /* NOE configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC); 

    /* NWE configuration */
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC); 
}

unsigned short fs_ReadFlashID (void)  
{
    unsigned char fid[5], i;
    //FLASH_CMD = K9FXX_RESET;                          /* 复位 */

    rt_memset( fid, 0, 5 );

    FLASH_CMD = K9F1G_READID;                         /* 第一周期: 写入命令 */
    FLASH_ADR = 0x00;                                 /* 第二周期: 写入地址 */

    for(i=0; i<5; i++)
    { fid[i] = FLASH_DAT; }

    return (*(unsigned short *)fid);
}

static int stm32_calc_ecc(struct nand_chip *chip, const uint8_t *dat, uint8_t *ecc)
{
	uint32_t e;

    e = FSMC_GetECC(FSMC_Bank2_NAND);
    FSMC_NANDECCCmd(FSMC_Bank2_NAND,DISABLE);			
	
	ecc[0] = e;
	ecc[1] = e >> 8;
	ecc[2] = e >> 24;

	return 0;
}

static int stm32_correct(struct nand_chip *chip, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
    return 0;
}

static const rt_mtdpart_t _parts[2] = 
{
    {"nf0", 2048*64*4, 1024*1024*10},
	{"root", 2048*64*4 + 1024*1024*10, 1024*512}
};

int stm32_nand_init(void)
{
	rt_nand_t *nand;

	fsmc_gpio_init();
    fsmc_nand_init();
	fs_ReadFlashID();
	
	nand = &_nand0;

	nand->pages_pb = 64;
	nand->page_size = 2048;
    nand->oobsize = 64;

	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.bytes = 3;
	nand->ecc.stepsize = 256;

	nand->ecc.calculate = stm32_calc_ecc;
    nand->ecc.correct = stm32_correct;

	nand->ops = &_ops;

	rt_mtd_nand_init(nand, 512, 1);

	mtd_part_add(&nand->mtd, _parts, 2);

	return 0;
}
INIT_DEVICE_EXPORT(stm32_nand_init);
