#pragma once

void uart_init(void);
char uart_recv(void);
void uart_send(char c);
void putc(void* p, char c);
void uart_send_string(char * str);
