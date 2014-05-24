#ifndef BASIC_IO_H_
#define BASIC_IO_H_

#include <stddef.h>


extern unsigned int send_rtn_val;
extern unsigned int recv_rtn_val;

extern int term_printf(const char *format, ...);
extern int term_gets(char *str, size_t size);
extern int enet_send(const char* str);
extern int enet_recv(char *str, size_t size);
extern int wireless_recv(char *str, size_t size);
extern int wireless_send(const char* str);
extern int read_pow(int gain_ctrl, float *sen, float *mem1, float *mem2, float *core);
extern void panic(const char *format, ...);
#endif
