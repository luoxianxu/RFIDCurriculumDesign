#include "lcd.h"
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

static int fd = -1;
static int *plcd = NULL;

void lcd_init()
{
	fd = open("/dev/fb0", O_RDWR);
	if (fd == -1)
	{
		perror("open fb0 error");
		exit(1);
	}
	//还可以通过ioctl函数获取lcd的参数   比如像素，RGB的位等信息
	plcd = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);//将内存映射到用户空间
	if (plcd == MAP_FAILED)
	{
		perror("mmap lcd error");
		exit(1);
	}
	printf("lcd init success!\n");
}

void lcd_draw_point(int x, int y, int color)
{
	*(plcd + 800 * y + x) = color;
}

void lcd_draw_rgb888(char *rgb888, int width, int height, int x, int y)
{
	int i, j;
	char r, g, b;
	int color;
	char *p = rgb888;
	for (i = 0; i < height; ++i)
	{
		for (j = 0; j < width; ++j)
		{
			b = *p++;
			g = *p++;
			r = *p++;
			color = r << 16 | g << 8 | b;
			lcd_draw_point(j + x, i + y, color);
		}
	}
}



void LCD_DrawBMP(int x,int y,const char *bmpname){
	unsigned char buf[4];
	int fd = open(bmpname,O_RDONLY);
	if(fd == -1){
		perror("open error");
		return;
	}
	lseek(fd,0,SEEK_SET);
	read(fd,buf,2);
	if(buf[0]!=0x42 || buf[1] != 0x4D){
		printf("this picture is not bmp!\n");
		return;
	}
	
	int bmp_w = 0;
	lseek(fd,0x12,SEEK_SET);
	read(fd,&bmp_w,4);
	int bmp_h = 0;
	lseek(fd,0x16,SEEK_SET);
	read(fd,&bmp_h,4);
	
	int bmp_colordepth = 0;
	lseek(fd,0x1C,SEEK_SET);
	read(fd,&bmp_colordepth,2);
	printf("bmp:%ld*%ld&%ld\n",bmp_w,bmp_h,bmp_colordepth);
	
	lseek(fd,54,SEEK_SET);
	int i,j;
	for(i=0;i<bmp_h;i++){
		for(j=0;j<bmp_w;j++){
			int color = 0;
			read(fd,&color,bmp_colordepth/8);
			lcd_draw_point(x+j,y+(bmp_h>0?(bmp_h-1-i):i),color);
		}
		lseek(fd,(4-bmp_colordepth/8*bmp_w%4)%4,SEEK_CUR);
	}
	close(fd);
}

void lcd_uninit()
{
	munmap(plcd, 800 * 480 * 4);
	close(fd);
	printf("lcd_uninit!\n");
}