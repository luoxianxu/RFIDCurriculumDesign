#ifndef LCD_H
#define LCD_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
#include<string.h>

void lcd_init();
void lcd_draw_point(int x, int y, int color);
void lcd_draw_rgb888(char *rgb888, int width, int height, int x, int y);
void LCD_DrawBMP(int x,int y,const char *bmpname);
void lcd_uninit();

#endif
