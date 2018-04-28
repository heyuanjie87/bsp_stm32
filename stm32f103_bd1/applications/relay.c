#include "relay.h"
#include <rtthread.h>
#include <stm32f10x.h>

int relay_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_1 | GPIO_Pin_0;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	return 0;
}
INIT_DEVICE_EXPORT(relay_init);

int swv_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

     return 0;
}
INIT_DEVICE_EXPORT(swv_init);

char swv_get(char ch)
{
    char v;
	
	switch (ch)
	{
	case 1:
		v = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_3);
	    break;
    case 2:
		v = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2);
        break;
    case 3:
		v = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
        break;
    case 4:
		v = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0);
        break;		
	}
}

char relay_get(char ch)
{
	char v;

	switch (ch)
	{
	case 1:
		v = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_11);
        break;
	case 2:
		v = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10);
        break;
	case 3:
		v = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_1);
        break;
	case 4:
		v = GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_0);
        break;
	}

	v += '0';

    return v;
}

void relay_set(char ch, char s)
{
    switch (ch)
	{
	case 1:
		GPIO_WriteBit(GPIOB, GPIO_Pin_11, s);
		break;
	case 2:
		GPIO_WriteBit(GPIOB, GPIO_Pin_10, s);
		break;
	case 3:
		GPIO_WriteBit(GPIOB, GPIO_Pin_1, s);
		break;
	case 4:
		GPIO_WriteBit(GPIOB, GPIO_Pin_0, s);
		break;
	}
}
