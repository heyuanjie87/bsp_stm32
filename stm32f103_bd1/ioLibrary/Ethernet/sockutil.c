/*
*
@file		sockutil.c
@brief	Implementation of useful function of iinChip
*
*/

#include <stdio.h>
#include <string.h>
#include "sockutil.h"

void inet_addr_(unsigned char* addr,unsigned char *ip)
{
	int i;
//	u_long inetaddr = 0;
	char taddr[30];
	char * nexttok;
	char num;
	strcpy(taddr,(char *)addr);
	
	nexttok = taddr;
	for(i = 0; i < 4 ; i++)
	{
		nexttok = strtok(nexttok,".");
		if(nexttok[0] == '0' && nexttok[1] == 'x') num = ATOI(nexttok+2,0x10);
		else num = ATOI(nexttok,10);
		
		ip[i] = num;
		nexttok = NULL;
	}
}	

char VerifyIPAddress(char* src, uint8_t * ip)
{
	int i;
	int tnum;
	char tsrc[50];
	char* tok = tsrc;
	uint8_t tmp[4];

	if (strlen(src) > 15)
		return 0;

	strcpy(tsrc,src);

	for(i = 0; i < 4; i++)
	{
		tok = strtok(tok,".");
		if ( !tok ) 
			return 0;
		if(tok[0] == '0' && tok[1] == 'x')
		{
			if(!ValidATOI(tok+2,0x10,&tnum)) 
				return 0;
		}
		else if(!ValidATOI(tok,10,&tnum)) 
			return 0;

		if(tnum < 0 || tnum > 255) 
			return 0;
		tmp[i] = tnum;
		tok = NULL;
	}

	memcpy(ip, tmp, 4);

	return 1;	
}

char C2D(uint8_t c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';

	return (char)c;
}

uint16_t ATOI(char* str, uint16_t base)
{
  unsigned int num = 0;
  while (*str !=0)
          num = num * base + C2D(*str++);
  return num;
}

int ValidATOI(
	char* str, 	/**< is a pointer to string to be converted */
	int base, 	/**< is a base value (must be in the range 2 - 16) */
	int* ret		/**<  is a integer pointer to return */
	)
{
  int c;
  char* tstr = str;
  if(str == 0 || *str == '\0') return 0;
  while(*tstr != '\0')
  {
    c = C2D(*tstr);
    if( c >= 0 && c < base) tstr++;
    else    return 0;
  }
  
  *ret = ATOI(str,base);
  return 1;
}
