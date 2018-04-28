#ifndef __RELAY_H__
#define __RELAY_H__

int relay_init(void);
char relay_get(char ch);
void relay_set(char ch, char s);
char swv_get(char ch);

#endif
