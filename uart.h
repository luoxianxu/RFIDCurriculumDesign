
#ifndef _UART_H
#define _UART_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/input.h>


int uart_init(char *file_name);

#endif/*_UART_H*/
