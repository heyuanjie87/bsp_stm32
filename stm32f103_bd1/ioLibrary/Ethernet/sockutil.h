#ifndef __SOCKUTIL_H__
#define __SOCKUTIL_H__

#include <stdint.h>

char VerifyIPAddress(char* src, uint8_t * ip);
uint16_t ATOI(char* str, uint16_t base);
void inet_addr_(unsigned char* addr,unsigned char *ip);

#endif
