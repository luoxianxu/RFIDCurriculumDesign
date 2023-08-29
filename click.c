
#include "click.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include<string.h>
#include<unistd.h>
#include<linux/input.h>

int get_ClickPos(int fd,int *x,int *y){
	struct input_event et;
	int r = read(fd,&et,sizeof(et));
	if(r==sizeof(et)){
		if(et.type == EV_ABS && et.code == ABS_X){//保存x坐标
			*x = et.value;
		}
		if(et.type == EV_ABS && et.code == ABS_Y){//保存y坐标
			*y = et.value;
		}
		if(et.type == EV_KEY && et.code == BTN_TOUCH && et.value == 0){//判读手指离开屏幕
			//close(fd);
			return 0;
		}
	}
	return -1;
}

