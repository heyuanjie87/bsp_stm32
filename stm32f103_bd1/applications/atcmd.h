#ifndef __ATCMD_H__
#define __ATCMD_H__

int atcmd_config(char *in, int ilen, char *out, int *rst);
int atcmd_relay(char *in, int ilen, int sock);

#endif
