
#ifndef _RFID_H
#define _RFID_H

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

long Get_Rfid_ID(int fd);

#endif /*_RFID_H*/

